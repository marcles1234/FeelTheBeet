#pragma once
#include <list>

using namespace std;
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"

class Beatgame {
public:
	Beatgame();
	void InitialiseSDL();
	void InitialiseGame();
	void Cleanup();
	void HandleInput();
	void Update();
	void Render();
	void DrawMap();
	void CheckWin();
	bool IsRunning() const;
private:
	const int screenWidth = 1200;
	const int screenHeight = 1000;
	const int fontSize = 72;
	SDL_Window* window;
	SDL_Renderer* renderer;
	TTF_Font* font;
	bool isRunning;
	bool gameOver;
	SDL_Color textColor = { 255, 0, 0, 255 };
};

class Note {
public:
	int lane;
	int noteID;
	float lingerTime;
	float speed;
	void Move();
	int Hit();
	void Turnback();
private:
	float timeAlive;
};

class Lane {
public:
	int laneID;
	Lane(int typeIn) {
		laneID = typeIn;
	}
	void getID() {
		cout << laneID << endl;
	}
	list<Note*> notes;
	void HitNote();
};
