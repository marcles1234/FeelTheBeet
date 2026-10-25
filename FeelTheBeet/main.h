#pragma once
#include <list>
#include <chrono>

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
	void CreateNote();
	void CheckNotes();
};

class Beatcounter {
public:
	float bpm;
	float timePerBeat;
	int beatTracker;
	void StartBeatCounter(Beatcounter* beatcounter);
	void Start();
	void Stop();
	double elapsed() const;
private:
	bool running;
	chrono::steady_clock::time_point start_time, end_time;
}; 