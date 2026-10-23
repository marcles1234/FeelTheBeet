#include <iostream>
#include "main.h"

bool KEYS[322];
int main(int argc, char* argv[])
{
    Beatgame game;
	game.Initialise();
	while(game.IsRunning()) {
		game.HandleInput();
		game.Update();
		game.Render();
	}
	game.Cleanup();
	return 0;
}

Beatgame::Beatgame() : isRunning(true), gameOver(false), window(nullptr), renderer(nullptr), font(nullptr) {};

void Beatgame::Initialise() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
        isRunning = false;
        return;
    }
    if (TTF_Init() == -1) {
        cerr << "SDL_ttf could not initialize! TTF_Error: " << SDL_GetError() << endl;
        isRunning = false;
        return;
    }
    window = SDL_CreateWindow("Feel the Beet", screenWidth, screenHeight, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        cerr << "Window cannot be created! SDL_ERROR: " << SDL_GetError() << endl;
        isRunning = false;
        return;
    }
    renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == nullptr) {
        cerr << "Renderer cannot be created! SDL_ERROR: " << SDL_GetError() << endl;
        isRunning = false;
        return;
    }
    font = TTF_OpenFont("Freedom-10eM.ttf", fontSize);
    if (font == nullptr) {
        cerr << "Font cannot be loaded! TTF_ERROR: " << SDL_GetError() << endl;
        isRunning = false;
        return;
    }

    //Set up key presses
    for (int i = 0; i < 322; i++) { // init them all to false
        KEYS[i] = false;
    }

}

#pragma region "Input handling"
void Beatgame::HandleInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
		switch (event.type) { //Input if button is pressed or released
            case SDL_EVENT_QUIT:
                isRunning = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                KEYS[event.key.key] = true;
                if (!gameOver) {
					switch (event.key.key) { //Individual key presses
                        case SDLK_W:
                            cout << "W key was pressed" << endl;
					        break;
                        case SDLK_E:
                            cout << "E key was pressed" << endl;
                            break;
                        case SDLK_A:
                            cout << "A key was pressed" << endl;
                            break;
                        case SDLK_D:
                            cout << "D key was pressed" << endl;
                            break;
                        case SDLK_Z:
                            cout << "Z key was pressed" << endl;
                            break;
                        case SDLK_X:
                            cout << "X key was pressed" << endl;
                            break;
                    }
                }
                break;
	        case SDL_EVENT_KEY_UP:
                KEYS[event.key.key] = false;
		        break;
        }   
    }
}
#pragma endregion "Input handling"

#pragma region "Board"
void Beatgame::Render() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    DrawMap();
    if (gameOver) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, "GameOver", 0, textColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FRect textRect{ screenHeight / 2, screenWidth / 2, fontSize, fontSize };
        SDL_RenderTexture(renderer, texture, NULL, &textRect);
        SDL_DestroySurface(surface);
        SDL_DestroyTexture(texture);
    }
    SDL_RenderPresent(renderer);
}

void Beatgame::DrawMap() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}
#pragma endregion "Board"

bool Beatgame::IsRunning() const {
    return isRunning;
}

void Beatgame::Update() {
    CheckWin();
}

void Beatgame::CheckWin() {
}

void Beatgame::Cleanup() {
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}