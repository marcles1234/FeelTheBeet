// Test song bpm = 106 -> 1.766 bps -> 0.566 seconds per beat

/*---TODO---
- Add player in the middle
- Add entry screen
- Input/sound offset
- New song
*/

#include "main.h"

bool KEYS[322];
Lane* lanes[7];

static SDL_AudioStream* stream = NULL;
static Uint8* wav_data = NULL;
static Uint32 wav_data_len = 0;

int main(int argc, char* argv[])
{
    Beatgame game;
	game.InitialiseSDL();
    game.InitialiseGame();

    Beatcounter beatcounter;
	beatcounter.bpm = 106.0f;
	beatcounter.timePerBeat = 60.0f / beatcounter.bpm;
	beatcounter.beatTracker = 0;
	beatcounter.running = true;

	thread beatThread(&Beatcounter::StartBeatCounter, &beatcounter, &beatcounter);

    Uint64 lastTime = SDL_GetPerformanceCounter();
	while(game.IsRunning()) {
		Uint64 currentTime = SDL_GetPerformanceCounter();
        game.deltaTime = (float)(currentTime - lastTime) / (float)SDL_GetPerformanceFrequency();
        lastTime = currentTime;

		game.HandleInput();
		game.Update();
		game.Render();
	}
    beatcounter.running = false;
	beatThread.join();
	game.Cleanup();
	return 0;
}

Beatgame::Beatgame() : isRunning(true), gameOver(false), window(nullptr), renderer(nullptr), font(nullptr) {};

void Beatgame::InitialiseSDL() { //Import SDL and TTF libraries, create window and renderer, load font and .wav file, set the metadata
    SDL_SetAppMetadata("Beet Game", "1.0", "com.example.beetgame");
    
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

	SDL_SetRenderVSync(renderer, 1); //Enble VSync to limit framerate to monitor refresh rate

    font = TTF_OpenFont("BagelFatOne-Regular.ttf", fontSize);
    if (font == nullptr) {
        cerr << "Font cannot be loaded! TTF_ERROR: " << SDL_GetError() << endl;
        isRunning = false;
        return;
    }

    // Audio setup sourced from https://github.com/libsdl-org/SDL/blob/main/examples/audio/03-load-wav/load-wav.c
    SDL_AudioSpec spec;
    char* wav_path = NULL;
    SDL_asprintf(&wav_path, "test.wav", SDL_GetBasePath());
    if (!SDL_LoadWAV(wav_path, &spec, &wav_data, &wav_data_len)) {
        cerr << "Couldn't load WAV file: " << SDL_GetError() << endl;
        return;
    }
	
    stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
	SDL_free(wav_path); //deallocate the string after use
    if (!stream) {
        cerr << "Couldn't create audio stream: " << SDL_GetError() << endl;
        return;
    }

    if (!SDL_PutAudioStreamData(stream, wav_data, wav_data_len)) { // Put the WAV data into the stream's queue
        cerr << "Couldn't queue WAV data: " << SDL_GetError() << endl;
        return;
    }

    if (!SDL_ResumeAudioStreamDevice(stream)) { // Start audio playback
        cerr << "Couldn't start audio playback: " << SDL_GetError() << endl;
        return;
    }

	SDL_Surface* surface = IMG_Load("Bee-pollen.png");
    if (!surface) {
        cerr << "Couldn't load image: " << SDL_GetError() << endl;
        return;
	}
	beatTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!beatTexture) {
        cerr << "Couldn't create texture: " << SDL_GetError() << endl;
        return;
    }
    SDL_DestroySurface(surface);

    SDL_Surface* surface1 = IMG_Load("Bee-nopollen.png");
    if (!surface1) {
        cerr << "Couldn't load image: " << SDL_GetError() << endl;
        return;
    }
    beatTextureEmpty = SDL_CreateTextureFromSurface(renderer, surface1);
    if (!beatTextureEmpty) {
        cerr << "Couldn't create texture: " << SDL_GetError() << endl;
        return;
    }
    SDL_DestroySurface(surface1);

    SDL_Surface* surface2 = IMG_Load("Queen bee.png");
    if (!surface2) {
        cerr << "Couldn't load image: " << SDL_GetError() << endl;
        return;
    }
    queenBee = SDL_CreateTextureFromSurface(renderer, surface2);
    if (!beatTextureEmpty) {
        cerr << "Couldn't create texture: " << SDL_GetError() << endl;
        return;
    }
    SDL_DestroySurface(surface2);

    SDL_Surface* backSurface = IMG_Load("Honeycomb.png");
    if (!backSurface) {
        cerr << "Couldn't load image: " << SDL_GetError() << endl;
        return;
    }
    backgroundTexture = SDL_CreateTextureFromSurface(renderer, backSurface);
    if (!beatTexture) {
        cerr << "Couldn't create texture: " << SDL_GetError() << endl;
        return;
    }
    SDL_DestroySurface(backSurface);

    textSurface = TTF_RenderText_Shaded(font, "Score: 0", 0, foregroundColor, backgroundColor);
    if (!textSurface) {
        cerr << "Couldn't load text: " << SDL_GetError() << endl;
        return;
    }
    textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!beatTexture) {
        cerr << "Couldn't create texture: " << SDL_GetError() << endl;
        return;
    }
    SDL_DestroySurface(textSurface);

    //Set up key presses
    for (int i = 0; i < 322; i++) { // init them all to false
        KEYS[i] = false;
    }
}

void Beatgame::InitialiseGame() { //Create lanes
    constexpr float PI = 3.14159265358979323846f;
	float heightComp = sin(60.0 * PI / 180.0) * noteDistance;
    for (int i = 1; i < sizeof(lanes) / sizeof(lanes[0]); i++) {
		Lane* lane = new Lane(i);
		lanes[i] = lane;
        if (i == 1) {
			lane->spawnX = halfScreenWidth - noteDistance / 2;
			lane->spawnY = halfScreenHeight - heightComp;
            lane->angle = 150.0;
        }
        else if (i == 2) {
            lane->spawnX = halfScreenWidth + noteDistance / 2;
            lane->spawnY = halfScreenHeight - heightComp;
            lane->angle = 210.0;
        }
        else if (i == 3) {
            lane->spawnX = halfScreenWidth - noteDistance;
            lane->spawnY = halfScreenHeight;
            lane->angle = 90.0;
        }
        else if (i == 4) {
            lane->spawnX = halfScreenWidth + noteDistance;
            lane->spawnY = halfScreenHeight;
            lane->angle = 270.0;
        }
        else if (i == 5) {
            lane->spawnX = halfScreenWidth - noteDistance / 2;
            lane->spawnY = halfScreenHeight + heightComp;
            lane->angle = 30.0;
        }
        else if (i == 6) {
            lane->spawnX = halfScreenWidth + noteDistance / 2;
            lane->spawnY = halfScreenHeight + heightComp;
            lane->angle = 330.0;
        }
	}
}

void Beatcounter::StartBeatCounter(Beatcounter* beatcounter) {
    beatTracker = 1;
    auto beatDuration = chrono::duration_cast<chrono::steady_clock::duration>(
        chrono::duration<double>(timePerBeat)
    );

    auto nextBeat = chrono::steady_clock::now() + beatDuration;

    float overflow = 0.0f;

	// Variable to generate random numbers for note creation
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dist(1, 6);

    int randomLane = 1;
    int lastNote = 0;

    while (running) {
        this_thread::sleep_until(nextBeat);
        
        beatTracker ++;

        nextBeat += beatDuration;

		// Randomly create next note
        do {
            randomLane = dist(gen);
        } while (randomLane == lastNote);
        lanes[randomLane]->CreateNote(nextBeat + beatDuration);
		lastNote = randomLane;
    }
}

void Beatgame::HandleInput() { //Check for input events
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
                            score += lanes[1]->HitNote();
					        break;
                        case SDLK_E:
                            score += lanes[2]->HitNote();
                            break;
                        case SDLK_A:
                            score += lanes[3]->HitNote();
                            break;
                        case SDLK_D:
                            score += lanes[4]->HitNote();
                            break;
                        case SDLK_Z:
                            score += lanes[5]->HitNote();
                            break;
                        case SDLK_X:
                            score += lanes[6]->HitNote();
                            break;
                    }
                    UpdateScore();
                }
                break;
	        case SDL_EVENT_KEY_UP:
                KEYS[event.key.key] = false;
		        break;
        }   
    }
}

float Lane::HitNote() { //Check if a note is in the lane and remove it if it is
    if (!notes.empty()) {
        float score = notes.front()->Hit();
		delete notes.front(); // Delete the note object to free memory
		notes.pop_front(); // Remove the note from the lanes list
        return score;
    }
}

float Note::Hit() {
	chrono::steady_clock::time_point timeOfHit = chrono::steady_clock::now();
    auto timeDifference = chrono::duration_cast<chrono::milliseconds>(timeOfHit - hitTime).count();
    if (timeDifference < -200) { //EARLY
        cout << "Note hit early by " << timeDifference << " milliseconds" << endl;
        if (timeDifference > -300 ) {
			timeDifference = (timeDifference * -1) - 200;
            return 10.0f - (timeDifference / 10);
		}
    } else if (timeDifference > 200) { //LATE
        cout << "Note hit late by " << timeDifference << " milliseconds" << endl;
        if (timeDifference < 300) {
			timeDifference = timeDifference - 200;
            return 10.0f - (timeDifference / 10);
        }
	} else { //WITHIN 200ms OF HIT TIME
        cout << "Note hit perfectly on time!" << endl;
        return 10.0f;
	}
    return 0.0f;
}

void Lane::CreateNote(chrono::steady_clock::time_point hitTime) { //Create a note in the lane
    Note* note = new Note();
	note->lane = laneID;
    note->hitTime = hitTime;
	note->noteX = spawnX; // Set the X position based on the lane ID
    note->noteY = spawnY;
	note->angle = angle;
    note->speedStorage = note->speed;
	notes.push_back(note);
}

void Lane::CheckNotes() { //Check all notes in a lane
	cout << "There are " << notes.size() << " notes in lane " << laneID << endl;
}

void Beatgame::UpdateScore() { //Update the score text
    scoreText = "Score: " + to_string(score);
    textSurface = TTF_RenderText_Shaded(font, scoreText.c_str(), 0, foregroundColor, backgroundColor);
	textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_DestroySurface(textSurface);
}

void Beatgame::Render() { //Create the window and draw the game over text if the game is over
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

	SDL_RenderTexture(renderer, backgroundTexture, NULL, NULL);

	SDL_FRect queenBeeRect{ halfScreenWidth-35, halfScreenHeight-35, 220.0f, 220.0f };
    SDL_RenderTexture(renderer, queenBee, NULL, &queenBeeRect);
    
    for (int i = 1; i <= 6 ; i++) {
        lanes[i]->Render(renderer, beatTexture, beatTextureEmpty, deltaTime);
	}
    
    if (textTexture) {
        SDL_FRect textRect{ 50, 50, 200, fontSize };
        SDL_RenderTexture(renderer, textTexture, NULL, &textRect);
	}

    if (gameOver) {
        SDL_Surface* surface = TTF_RenderText_Solid(font, "GameOver", 0, foregroundColor);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FRect textRect{ screenHeight / 2, screenWidth / 2, fontSize, fontSize };
        SDL_RenderTexture(renderer, texture, NULL, &textRect);
        SDL_DestroySurface(surface);
        SDL_DestroyTexture(texture);
    }
    SDL_RenderPresent(renderer);
}

void Lane::Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Texture* textureEmpty, float deltaTime) { //Render the notes in the lane
    for (Note* note : notes) {
        SDL_FRect destination = { note->noteX, note->noteY, note->noteWidth, note->noteHeight }; //Set the destination
        
        if (note->temp() == 0) { //Determine note colour
            SDL_RenderTextureRotated(renderer, textureEmpty, NULL, &destination, note->angle, NULL, SDL_FLIP_NONE);
		}
		else if (note->temp() == 1) {
            SDL_RenderTextureRotated(renderer, texture, NULL, &destination, note->angle, NULL, SDL_FLIP_NONE);
            note->speed = 0.0f;
        }
        else if (note->temp() == 2) {
            if (!note->turnBack) {
                note->turnBack = true;
                note->angle -= 180.0f;
                note->speed = -note->speedStorage;
            }
            SDL_RenderTextureRotated(renderer, textureEmpty, NULL, &destination, note->angle, NULL, SDL_FLIP_NONE);
        }
        else {
            SDL_RenderTextureRotated(renderer, texture, NULL, &destination, note->angle, NULL, SDL_FLIP_NONE);
        }

		float dx = 525.0f - note->noteX; //Calculate the angle of travel towards the center of the screen
		float dy = 425.0f - note->noteY;
		float angleOfTravel = atan2(dy, dx);

        note->noteX += cos(angleOfTravel) * note->speed * deltaTime;
        note->noteY += sin(angleOfTravel) * note->speed * deltaTime;
    }
}

bool Note::isExpired() { //Check if the note has expired and remove it if it has
    auto now = chrono::steady_clock::now();
    auto elapsed = chrono::duration_cast<chrono::milliseconds>(now - hitTime).count();
    return elapsed > lingerTime;
}

int Note::temp() { //Used for debugging, returns the time since the note was created
    chrono::steady_clock::time_point timeOfHit = chrono::steady_clock::now();
	auto timeDifference = chrono::duration_cast<chrono::milliseconds>(timeOfHit - hitTime).count();
    if (timeDifference < -200) { //EARLY
        return 0;
    }
    else if (timeDifference > 200) { //LATE
        return 2;
    }
    else { //WITHIN 200ms OF HIT TIME
        return 1;
    }
    return 3;
}

bool Beatgame::IsRunning() const { //Used in main() to check if the game is running
    return isRunning;
}

void Beatgame::Update() { //Update the game state
    for (int i = 1; i <= 6; i++) {
		lanes[i]->Update();
    }
}

void Lane::Update() { //Called from Beatgame::Update(), checks if notes have expired and removes them if they have
    for (auto it = notes.begin(); it != notes.end();) { //List through notes
		if ((*it)->isExpired()) { //Check if the note has expired
            delete* it;
            it = notes.erase(it);
        } else {
			++it; //Move to the next note if not expired
        }
    }
}

void Beatgame::Cleanup() { //Called when game is over, cleans up SDL and TTF libraries, destroys window and renderer
    TTF_CloseFont(font);
    if (beatTexture) {
        SDL_DestroyTexture(beatTexture);
        beatTexture = nullptr;
    }
    if (textTexture) {
        SDL_DestroyTexture(textTexture);
        textTexture = nullptr;
    }
    if (backgroundTexture) {
        SDL_DestroyTexture(backgroundTexture);
        backgroundTexture = nullptr;
    }
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_free(wav_data);
    SDL_Quit();
}
