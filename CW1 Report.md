3016 CW1 Report � Feel the Beet ??
PDF OF THIS REPORT CAN BE FOUND ON SUBMISSIONS ZIP FILE UNDER 'DOCCUMENTS+LINKS'

Overview
	Feel the Beet is a rhythm game about bees. You play as the queen be at the centre of a hive, collecting nectar from your worker bees. In this case, the worker bees are the notes you must hit. There are six lanes and six corresponding keys on your keyboard you must hit in time to the music to build your score!

Image 1 � Screenshot of gameplay
Inspiration
	Due to a very limiting project deadline, this game was made in the span of two weeks (alongside other projects and responsibilities) so isn�t the most innovative and unique game on the market. However, I attempted to create something that I hadn�t seen before from the mechanics and ideas of other games in the genre. There isn�t one definitive inspiration here but a combination of many. Listed are a few references:
1.
Osu!: Often seen as �the� game of the genre. Features multiple different modes to play but the focus here is osu!mania � where notes fall from the top of the screen and you must hit them at the correct timings.

2.
Friday Night Funkin�: This game strictly follows a four key structure, utilising the arrow keys. It has a unique charm and almost comic like art style. If I had more time to develop this program, I would aim to have a similar style in its animations and visual effects.

3.
Maimai: This game takes a slightly different approach to most rhythm games. Instead of having notes scroll from one side of the screen to another, they originate from the centre of the circular screen and you must hit them as they reach the circumference of the screen using surrounding buttons (this is an arcade cabinet).

4.
Cuphead: Being a bit of an outlier in this list, Cuphead is a platformer/boss-rush game with no rhythmical elements. However, it features a level dedicated to bees! Whilst this level features an evil queen bee trying to stop you, I found it a useful reference for art and sound design. The games soundtrack is mostly comprised of jazz-like music which was my biggest reference for choosing my own song for my game.  


Game mechanics
	The game revolves around keeping on time with the music via collecting the nectar from your fellow bees.
	Bees will spawn along the six edges leading up to the perimeter of your cell (the central one). They will then traverse along their given path and rest on the corner, offering their nectar to you. But you must be quick! The bee only rests there for 400ms total. If you do not collect from this bee on time, it will turn around and return to its origin. 
	

Sprite 1 - Bee
Sprite 2 � Bee with nectar ready

	Points are awarded in line with how well you can keep to the beat. There are three categories of awarded points: early, on time and late. Hitting the beat within the 400ms window results in a +10-score reward. Early and late hits can award points but only for a very brief (100ms) window, either side of the respective side of on time. Score here is calculated on exactly how close you are. You can be awarded anywhere from +1 to +9, calculated via: 10 � (time difference between hit and on time) / 10.
	There is also an option to set the offset of the beat timer in relation to the music. This is featured on the menu screen � accessed upon launching the game. You have the option to set it to any positive value of your choosing. This will delay the programs beat clock from starting to that it will better align with the audio output. You are not required to input a value if you choose not to. A small tutorial image and control scheme is displayed here also

Image 2 � Screenshot of menu
Programming
	In line with C++ programming principles, I have followed an implementation of Object Orientated Programming. I have used a series of classes to create objects. Each class has a series of relevant methods and attributes attached to them. These are as follows:
Class
Public
Private
Beatgame
Beatgame();
void InitialiseSDL();
void InitialiseGame();
void Cleanup();
void HandleInput();
void Update();
void Render();
bool IsRunning() const;
void StopRunning();
void UpdateScore();
SDL_Window* window;
SDL_Renderer* renderer;
TTF_Font* font;
SDL_Texture* beatTexture;
SDL_Texture* beatTextureEmpty;
SDL_Texture* queenBee;
SDL_Texture* textTexture;
SDL_Texture* backgroundTexture;
SDL_Texture* tutorialTexture;
SDL_Surface* textSurface;
SDL_FRect textLocation;
string scoreText;
int score = 0;
float deltaTime = 0.0f;
bool isRunning;
const int screenWidth = 1200;
const int screenHeight = 1000;
const int halfScreenWidth = (screenWidth / 2) - 75.0f;
const int halfScreenHeight = (screenHeight / 2) - 75.0f;
const int noteDistance = 450;
const int fontSize = 72;
bool gameOver;
SDL_Color backgroundColor = { 255, 255, 255, 0 };
SDL_Color foregroundColor = { 73, 53, 22, 255 };
Lane
Lane(int typeIn) {
	laneID = typeIn;
}
void getID() {
	cout << laneID << endl;
}
float HitNote();
void CreateNote(chrono::steady_clock::time_point hitTime);
void CheckNotes();
void Render(SDL_Renderer* renderer, SDL_Texture* texture, SDL_Texture* textureEmpty, float deltaTime);
void Update();
int laneID;
list<Note*> notes;
float spawnX;
float spawnY;
double angle;

Note
int lane;
float lingerTime = 1200.0f;
float speed = 250.0f;
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
float timeAlive;
Beatcounter
float bpm;
float timePerBeat;
int beatTracker;
bool running;
void StartBeatCounter(Beatcounter* beatcounter, const string& offsetText);
chrono::steady_clock::time_point start_time, end_time;

Menu
Menu();
void InitialiseMenu(SDL_Window* gameWindow);
void HandleInput();
void Render(Beatgame* game);
SDL_Window* window = nullptr;
string offsetText = "";
bool textInputActive = false;
int offset = 0;

Table 1 � All classes, methods and attributes

Diagram 1 � Class access system
	These classes all have a distinct purpose in the game; the most important being �beatgame�  this is the overarching class that ties everything together. It is responsible for running the whole game, initialising SDL (along with its textures, fonts and music) and is where the most important game loop resides (in main()). 

Code 1 � Initialising SDL

Code 2 � Core gameplay loop
	Inside the main while loop, we can see that this game object also is responsible for handling the user inputs and rendering the game itself. All of this is completed every frame that the game exists for. Outside of this loop, the game is finished and some cleaning takes place to properly exit the program.
	Beatcounter is ran as a thread when the game is initialised. It runs alongside the main game loop. It takes the bpm value of the song and calculates the time between each individual beat. It then runs on a loop, iterating once to spawn a note in one of the given six lanes, and then sleeping for the duration of one beat (only activating once for every beat in the song). 

Code 3 � Beatcounter loop
	From this, we can see that it passes values into a method of a �lanes� object via a pointer to an array. These lanes are created upon launching the game. Six of them to be exact. These are what are created in beatgame�s InitaliseGame(). 

Code 4 � InitialiseGame() (rest of fuction sets spawn and angle for i = 2-> 5)
	So, beatcounter points to one of these six lanes created in code 4 and from there, a note is created in the respective lane.

Code 5 � CreateNote()
	Each lane contains a list of objects (of type �note�). When this method is called from a specific object, it creates a new note object and adds it to the bottom of that lanes list. Similarly, when a note is hit or expires it is removed from the top. 
      The primary method of removing notes is executed through the user�s inputs. This stems from the beatgame�s input handling 

Code 6 � Input handling
	Pressing any of the playable keys calls to hit a note in its respective lane if that lane is not empty. If there are any notes held in the lane, it accesses the hit function from the one at the top of the lanes list.

Code 7 � Lane hit
	That individual note then checks to see where it is in relation to when it has been destroyed (how late/early). It then awards the player with score if appropriate and prints a console message of how on time the hit was.

Code 8 � Note hit
Flowchart

Diagram 2 � Game flowchart
	This flowchart was make using https://www.canva.com. No generative AI was used.

Testing
	I undergone testing myself throughout development. Over the two-week development period, I have slowly been implementing features one at a time. I have tested the feature by itself though methods like testing in an empty project and console logging. The method of console logging is evident through my GitHub commits. You can see from different commits that I have implemented a feature, tested it, committed and then usually remove unused/unneeded logging in future commits.

Image 3 � GitHub commit log
	I did have one of my housemates playtest my game. However, this was not done under proper testing conditions as I gave her the game to play in our kitchen. As such, I did not gather any evidence of this nor officially log the outcomes. However, it is worth noting that she did enjoy the game but found it difficult to play. She has minimal experience with this type of game so should I continue to develop it, I would add options to make the experience easier through slower songs or less notes spawning.
      Had the project scope have been larger, this would have undergone more diverse testing, including getting play testers to use the program � setting them tasks to complete and documenting outcomes.

GitHub Contribution

Image 4 � GitHub commit timeline
	As shown here, I started development shortly after our week 4 lab on the 22nd October. My first commit was on the 23rd October � the day after this. Whilst I have made most of my commits in the last week, many of these were small changes to the program and many included changes to my .gitignore file and uploading of SDL files into the project to allow building on other devices (I developed with these installed in a directory outside the project). It took a fair few attempts to get it ported correctly.

Evaluation
	Given the very tight timeframe and lack of experience (primarily in SDL) I feel like this project was a success. I had a very solid idea on what I would like my product to do that was outlined in the proposal and I stuck to it very well. All core mechanics were featured in my game to a in a somewhat robust way with serviceable coding practices.
	I understand that this game isn�t the most engaging to play � having very little replay value but it serves its purpose as a good first time play. With this being said, provided I had more time to implement more features, it has a decent amount of potential. It is a very limited experience at the moment but through methods of having different songs, implementing SFX and VFX, new sprites and increased game mechanics (i.e. hold notes or bees swapping lanes), it could turn out to be much more interesting.
	I feel like I somewhat delivered in the art department as well as everything you see (besides text) what digitally hand drawn by me. As mentioned in my AI declaration, I did attempt to get AI to do it for me but couldn�t get a feasible product.
	If I were to complete this project again, I would have done some more research into beat tracking specifically. I did do some research into �BTrack� which is a C++ program that can automatically detect beats from an audio stream. But after testing it out, I found that it would be more efficient (given the project scope and timeline) to use the method I implemented.

Links
GitHub repo: https://github.com/marcles1234/FeelTheBeet/tree/master

Video link: https://youtu.be/Ah5bqGolT7U



