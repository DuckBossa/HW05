
#include <iostream>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <sstream>
//#include <cmath>
#include <vector>

const int WIDTH = 800;
const int HEIGHT = 600;
const int FPS = 60;
const int NUM_BALLS = 50;
const int RADIUS = 10;
const int MASS = 7;

bool running = false;
int score = 0;
bool hit = false;
int time = 0;
int ptime = 0;

SDL_Window *win = NULL;
SDL_Renderer *ren = NULL;

using namespace std;

struct particle{
	double x, y;
	double vx, vy;
	double rad;
	Uint32 color;
	int mass;
	particle(){};
	particle( double tx, double ty, double tvx, double tvy, double trad, int tmass, Uint32 tcolor): x(tx), y(ty), vx(tvx), vy(tvy), rad(trad), color(tcolor), mass(tmass) {}

};

vector<particle> px;

bool collide(particle& ball1, particle& ball2){ // feeling ko kailangan natin sundan and elastic motion sa pagpalit ng mga velocities
	double dist = sqrt((ball1.x - ball2.x) * (ball1.x - ball2.x) + (ball1.y - ball2.y) * (ball1.y - ball2.y));
	if(dist > ball1.rad + ball2.rad)
		return false;
	else
		return true;
}

void resolveCollision(particle& ball1, particle& ball2){

	//dvx, dvy = push vector between ball1 & ball2
	double dvx = ball1.x - ball2.x;
	double dvy = ball1.y - ball2.y;
	//magnitude of dv
	double dl = sqrt(1.0*dvx*dvx+dvy*dvy);
	//min pushback
	double dpx = 1.0*dvx*((ball1.rad + ball2.rad-dl)/dl);
	double dpy = 1.0*dvy*((ball1.rad + ball2.rad-dl)/dl);
	//inverted ball mass
	double im1 = 1.0/ball1.rad;
	double im2 = 1.0/ball2.rad;
	//reposition
	ball1.x += dpx*im1/(im1+im2);
	ball1.y += dpy*im1/(im1+im2);
	ball2.x -= dpx*im2/(im1+im2);
	ball2.y -= dpy*im2/(im1+im2);



	double unX = ball1.x - ball2.x;
	double unY = ball1.y - ball2.y;
	double magN = sqrt(1.0*unX*unX + unY*unY);
	unX /= magN;
	unY /= magN;
	//double dot = unX*unX + unY*unY;
	//if (dot < 0.0) return;
	//#2
	double tanx = -1.0*unY;
	double tany = unX;
	//getting the velocity vector is done for us.

	double v1n = ball1.vx * unX + ball1.vy * unY;
	double v1t = ball1.vx*(tanx) + ball1.vy*tany;
	double v2n = ball2.vx*unX + ball2.vy *unY;
	double v2t = ball2.vx*(tanx) + ball2.vy*tany;

	double v1pn = (v1n * (ball1.rad - ball2.rad) + ( 2*ball2.rad* v2n))/(ball1.rad + ball2.rad);
	double v2pn= (v2n * (ball2.rad - ball1.rad) + ( 2*ball1.rad * v1n))/(ball1.rad + ball2.rad);

	//double vlnux1 = v1pn *unX;
	//double vlnuy1 = v1pn *unY;
	//double vltux1 = v1t * unX;
	//double vltuy1 = v1t * unY;

	//double vlnux2 = v2pn *unX;
	//double vlnuy2 = v2pn *unY;
	//double vltux2 = v2t * unX;
	//double vltuy2 = v2t * unY;


	//double fin1x = vlnux1 + vltux1;
	//double fin1y = vlnuy1 + vltuy1;
	//double fin2x = vlnux2 + vltux2;
	//double fin2y = vlnuy2 + vltuy2;


	ball1.vx = v1pn*unX + v1t*unX;
	ball1.vy = v1pn*unY + v1t*unY;
	ball2.vx = v2pn*unX + v2t*unX;
	ball2.vy = v2pn*unY + v2t*unY;

	//ball1.x += ball1.vx;
	//ball2.y += ball2.vy;
	//ball1.x += ball1.vx;
	//ball2.y += ball2.vy;
}

bool collideWalls(particle& ball){
	bool coll = false;
	if(ball.x - ball.rad <= 0){
		if(ball.vx <= 0) ball.vx *= -1;
		//ball.x = ball.rad;
		coll = true;
	}else if(ball.x + ball.rad >= WIDTH){
		if(ball.vx >= 0) ball.vx *= -1;
		//ball.x = WIDTH - ball.rad;
		coll = true;
	}
	if(ball.y - ball.rad <= 0){
		if(ball.vy <= 0) ball.vy *= -1;
		//ball.y = ball.rad;
		coll = true;
	}else if(ball.y + ball.rad >= HEIGHT){
		if(ball.vy >= 0) ball.vy *= -1;
		//ball.y = HEIGHT - ball.rad;
		coll = true;
	}
	return coll;
}

bool init(){
	bool run = true;
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		run = false;
	}
	win = SDL_CreateWindow("Bullets all over", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	if (win == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		run = false;
	}
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
	if (ren == nullptr){
		SDL_DestroyWindow(win);
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		SDL_Quit();
		run = false;
	}
	//SDL_ShowCursor(SDL_DISABLE);
	return run;
}

void setup(){
	for(int i = 0; i < NUM_BALLS; i++){
		//particle( int tx, int ty, int tvx, int tvy, int trad, Uint32 tcolor): x(tx), y(ty), vx(tvx), vy(tvy), rad(trad), color(tcolor) {}
		double ranX = rand() % (WIDTH - 10)+ 10;
		double ranY = rand() % (HEIGHT - 10) + 10;
		double ranVX = rand()*1.0/RAND_MAX * 20 - 10;
		double ranVY = rand()*1.0/RAND_MAX * 20 - 10;
		double varrad = rand()*1.0/RAND_MAX * 10 + 5;
		Uint32 color = 0xFF000000 + 0x00FF0000 *rand()*1.0/RAND_MAX  + 0x0000FF00 *rand()*1.0/RAND_MAX + 0x00000FF *rand()*1.0/RAND_MAX; 
		px.push_back(*new particle(ranX,ranY,ranVX,ranVY,RADIUS,MASS,color));
	}
}


void physics(){
	for(int i = 0; i < px.size(); i++){
		
		
		//check for collision with balls here
		for(int j = 0; j < px.size(); j++){
			if(i == j) continue;
			if(collide(px[i],px[j])){
				swap(px[i].color,px[j].color);
				resolveCollision(px[i],px[j]);
				//cout << "COLLISION between " << i << " and " << j << endl;
			}
		}
		if(collideWalls(px[i])) px[i].color = 0xFF000000 + 0x00FF0000 *rand()*1.0/RAND_MAX  + 0x0000FF00 *rand()*1.0/RAND_MAX + 0x00000FF *rand()*1.0/RAND_MAX;
		px[i].x += px[i].vx;
		px[i].y += px[i].vy;
		
	}
}

void render(){
	SDL_SetRenderDrawColor(ren, 255, 255, 255, 255);
	SDL_RenderClear(ren);
	for(int i = 0; i < px.size(); i++){
		filledCircleColor(ren, px[i].x, px[i].y, px[i].rad, px[i].color);
	}
	SDL_RenderPresent(ren);
}

int main( int argc, char* args[] ){
	if(!init()) return 1;

	running = true;
	Uint32 start;
	SDL_Event ev;

	setup();

	while (running) {
		// time here
		start = SDL_GetTicks();
		physics();
		render();
		if(1000/FPS > (SDL_GetTicks() - start)){
			SDL_Delay(1000/FPS - (SDL_GetTicks() - start));
		}
		
		time++;
	}
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit();
}
