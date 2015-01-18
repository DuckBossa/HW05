
#include <iostream>
#include <SDL.h>
#include <SDL2_gfxPrimitives.h>
#include <sstream>
//#include <cmath>
#include <vector>

const int WIDTH = 480;
const int HEIGHT = 640;
const int FPS = 60;
const int NUM_BALLS = 50;
const int RADIUS = 7;

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
	int dt;
	particle(){};
	particle( int tx, int ty, int tvx, int tvy, int trad, Uint32 tcolor): x(tx), y(ty), vx(tvx), vy(tvy), rad(trad), color(tcolor) {}

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

void collideWalls(particle& ball){
	if(ball.x - ball.rad  < 0 && (ball.y - ball.rad > 0 && ball.y + ball.rad < HEIGHT)){ // make x go right
		if(ball.vx < 0)
			ball.vx*=-1;
	}	
	else if(ball.x + ball.rad  > WIDTH && (ball.y - ball.rad > 0 && ball.y + ball.rad < HEIGHT)){//make x go left
		if(ball.vx > 0)
			ball.vx*=-1;
	}
	else if( (ball.y - ball.rad < 0) && (ball.x - ball.rad > 0 && ball.x + ball.rad < WIDTH)){// make y go down
		if(ball.vy < 0)
			ball.vy*=-1;
	}
	else if( (ball.y + ball.rad > HEIGHT) && (ball.x - ball.rad > 0 && ball.x + ball.rad < WIDTH)){//make y go up
		if(ball.vy > 0)
			ball.vy*=-1;
	}
	else if( (ball.x - ball.rad < 0) && (ball.y - ball.rad < 0) ){ // make y go down and x go left
		if(ball.vy < 0)
			ball.vy*=-1;
		if(ball.vx < 0)
			ball.vx*=-1;
	}
	else if( (ball.x - ball.rad < 0) && (ball.y + ball.rad > HEIGHT) ){ // make y go up and x go right
		if(ball.vy > 0)
			ball.vy*=-1;
		if(ball.vx < 0)
			ball.vx*=-1;
	}
	else if( (ball.x + ball.rad > WIDTH) && (ball.y - ball.rad < HEIGHT) ){// make y go down and x go left
		if(ball.vy < 0)
			ball.vy*=-1;
		if(ball.vx < 0)
			ball.vx*=-1;
	}
	else if( (ball.x + ball.rad > WIDTH) && (ball.y + ball.rad > HEIGHT) ){//make y go up and x go left
		if(ball.vy > 0)
			ball.vy*=-1;
		if(ball.vx < 0)
			ball.vx*=-1;
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
	SDL_ShowCursor(SDL_DISABLE);
	return run;
}

void setup(){
	for(int i = 0; i < NUM_BALLS; i++){
		//particle( int tx, int ty, int tvx, int tvy, int trad, Uint32 tcolor): x(tx), y(ty), vx(tvx), vy(tvy), rad(trad), color(tcolor) {}
		int ranX = rand() % (WIDTH - 10)+ 10;
		int ranY = rand() % (HEIGHT - 10) + 10;
		int ranVX = rand() % 6 + 2;
		int ranVY = rand() % 6 + 2;
		px.push_back(*new particle(ranX,ranY,ranVX,ranVY,RADIUS,0xFF000000));
	}
}


void physics(){
	for(int i = 0; i < px.size(); i++){
		collideWalls(px[i]);
		//check for collision with balls here
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
