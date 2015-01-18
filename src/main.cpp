
#include <iostream>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <sstream>
//#include <cmath>
#include <vector>

const int WIDTH = 480;
const int HEIGHT = 640;
const int FPS = 120;
const int NUM_BALLS = 10;
const int RADIUS = 25;
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
	int x, y;
	int vx, vy;
	int rad;
	Uint32 color;
	int mass;
	particle(){};
	particle( int tx, int ty, int tvx, int tvy, int trad, int tmass, Uint32 tcolor): x(tx), y(ty), vx(tvx), vy(tvy), rad(trad), color(tcolor), mass(tmass) {}

};

vector<particle> px;

bool collide(particle& ball1, particle& ball2){ // feeling ko kailangan natin sundan and elastic motion sa pagpalit ng mga velocities
	double radSum = ball1.rad + ball2.rad + 0.0;
	double xVal = (ball1.x - ball2.x) * (ball1.x - ball2.x) * 1.0;
	double yVal = (ball1.y - ball2.y) * (ball1.y - ball2.y) * 1.0;
	double dist = sqrt(xVal + yVal);
	if(dist > radSum)
		return false;
	else
		return true;
}

void resolveCollision(particle& ball1, particle& ball2){
	//dvx, dvy = push vector between ball1 & ball2
	int dvx = ball1.x - ball2.x;
	int dvy = ball1.y - ball2.y;
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

	//get velocity vector
	int vvx = ball1.vx - ball2.vx;
	int vvy = ball1.vy - ball2.vy;
	//get magnitude
	double dvv = sqrt(1.0*vvx*vvx+vvy*vvy);
	//normalize
	double nvx = vvx/dvv;
	double nvy = vvy/dvv;
	//dot product
	double dotvv = nvx*nvx + nvy*nvy;
	if(dotvv > 0.0) return;

	double imp = dotvv/(im1+im2);
	double ivx = dpx*imp;
	double ivy = dpy*imp;

	ball1.vx += ivx*im1;
	ball1.vy += ivy*im1;
	ball2.vx -= ivx*im2;
	ball2.vy -= ivy*im2;
}

void resolveCollision2(particle& ball1, particle& ball2){
	ball1.vx = (ball1.vx * (ball1.rad - ball2.rad ) + ( 2*ball2.rad * ball1.vx))/(ball1.rad + ball2.rad);
	ball1.vy = (ball1.vy * (ball1.rad - ball2.rad ) + ( 2*ball2.rad * ball1.vy))/(ball1.rad + ball2.rad);
	ball2.vx = (ball2.vx * (ball2.rad - ball1.rad ) + ( 2*ball1.rad * ball2.vx))/(ball1.rad + ball2.rad);
	ball2.vy = (ball2.vy * (ball2.rad - ball1.rad ) + ( 2*ball1.rad * ball2.vy))/(ball1.rad + ball2.rad);

	ball1.x += ball1.vx;
	ball2.y += ball2.vy;
	ball1.x += ball1.vx;
	ball2.y += ball2.vy;
}

void collideWalls(particle& ball){
	if(ball.x - ball.rad  < 0 && (ball.y - ball.rad > 0 && ball.y + ball.rad < HEIGHT)){ // make x go right
		if(ball.vx < 0){
			ball.vx*=-1;
			ball.x = ball.rad;
		}
	}	
	else if(ball.x + ball.rad  > WIDTH && (ball.y - ball.rad > 0 && ball.y + ball.rad < HEIGHT)){//make x go left
		if(ball.vx > 0){
			ball.vx*=-1;
			ball.x = WIDTH - ball.rad;
		}
	}
	else if( (ball.y - ball.rad < 0) && (ball.x - ball.rad > 0 && ball.x + ball.rad < WIDTH)){// make y go down
		if(ball.vy < 0){
			ball.vy*=-1;
			ball.y = ball.rad;
		}
	}
	else if( (ball.y + ball.rad > HEIGHT) && (ball.x - ball.rad > 0 && ball.x + ball.rad < WIDTH)){//make y go up
		if(ball.vy > 0){
			ball.vy*=-1;
			ball.y = HEIGHT - ball.rad;
		}
	}
	else if( (ball.x - ball.rad < 0) && (ball.y - ball.rad < 0) ){ // make y go down and x go left
		if(ball.vy < 0){
			ball.vy*=-1;
			ball.y = ball.rad;
		}
		if(ball.vx < 0){
			ball.vx*=-1;
			ball.x = ball.rad;
		}
	}
	else if( (ball.x - ball.rad < 0) && (ball.y + ball.rad > HEIGHT) ){ // make y go up and x go right
		if(ball.vy > 0){
			ball.vy*=-1;
			ball.y = HEIGHT - ball.rad;
		}
		if(ball.vx < 0){
			ball.vx*=-1;
			ball.x = 0;
		}
	}
	else if( (ball.x + ball.rad > WIDTH) && (ball.y - ball.rad < 0 )){// make y go down and x go left
		if(ball.vy < 0){
			ball.vy*=-1;
			ball.y = ball.rad;
		}
		if(ball.vx < 0){
			ball.vx*=-1;
			ball.x = WIDTH - ball.rad;
		}
	}
	else if( (ball.x + ball.rad > WIDTH) && (ball.y + ball.rad > HEIGHT) ){//make y go up and x go left
		if(ball.vy > 0){
			ball.vy*=-1;
			ball.y = HEIGHT - ball.rad;
		}
		if(ball.vx < 0){
			ball.vx*=-1;
			ball.x = WIDTH - ball.rad;
		}
	}
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
	ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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
		int ranX = rand() % (WIDTH - 10)+ 10;
		int ranY = rand() % (HEIGHT - 10) + 10;
		int ranVX = rand() % 6 + 2;
		int ranVY = rand() % 6 + 2;
		px.push_back(*new particle(ranX,ranY,ranVX,ranVY,RADIUS,MASS,0xFF000000));
	}
}


void physics(){
	for(int i = 0; i < px.size(); i++){
		px[i].x += px[i].vx;
		px[i].y += px[i].vy;
		collideWalls(px[i]);
		//check for collision with balls here
		for(int j = 0; j < px.size(); j++){
			if(i == j) continue;
			if(collide(px[i],px[j])){
				resolveCollision(px[i],px[j]);
				//cout << "COLLISION between " << i << " and " << j << endl;
			}
		}
		
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
