#pragma once
#include <iostream>
#include <algorithm>
#include <list>
#include <chrono>
#include <thread>
#include <random>
#include <string>
#include <iomanip>

using namespace std;
#include "SDL3/SDL.h"
#include "SDL3_ttf/SDL_ttf.h"
#include <SDL3_image/SDL_image.h>

class Beatgame {
public:
	Beatgame();
	void InitialiseSDL();
	void InitialiseGame();
	void Cleanup();
	void HandleInput();
	void Update();
	void Render();
	bool IsRunning() const;
	void UpdateScore();
	SDL_Texture* beatTexture;
	SDL_Texture* beatTextureEmpty;
	SDL_Texture* textTexture;
	SDL_Texture* backgroundTexture;
	SDL_Surface* textSurface;
	SDL_FRect textLocation;
	string scoreText;
	int score = 0;
	float deltaTime = 0.0f;
private:
	const int screenWidth = 1200;
	const int screenHeight = 1000;
	const int halfScreenWidth = (screenWidth / 2) - 75.0f;
	const int halfScreenHeight = (screenHeight / 2) - 75.0f;
	const int noteDistance = 450;
	const int fontSize = 72;
	SDL_Window* window;
	SDL_Renderer* renderer;
	TTF_Font* font;
	bool isRunning;
	bool gameOver;
	SDL_Color backgroundColor = { 255, 255, 255, 0 };
	SDL_Color foregroundColor = { 73, 53, 22, 255 };
};

class Note {
public:
	int lane;
	float lingerTime = 1200.0f;
	float speed = 200.0f;
	float speedStorage = 0.0f;
	chrono::steady_clock::time_point hitTime;
	float noteX;
	float noteY;
	float noteWidth = 150.0f;
	float noteHeight = 150.0f;
	bool turnBack = false;
	double angle;
	void Move();
	float Hit();
	bool isExpired();
	int temp();
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
	float spawnX;
	float spawnY;
	double angle;
	float HitNote();
	void CreateNote(chrono::steady_clock::time_point hitTime);
	void CheckNotes();
	void Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Texture* textureEmpty, float deltaTime);
	void Update();
};

class Beatcounter {
public:
	float bpm;
	float timePerBeat;
	int beatTracker;
	bool running;
	void StartBeatCounter(Beatcounter* beatcounter);
private:
	chrono::steady_clock::time_point start_time, end_time;
}; 