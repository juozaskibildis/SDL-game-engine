#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

// Include data files
#include "data/structs.cpp"
#include "data/keybinds.cpp"
#include "data/display.cpp"
#include "data/fps.cpp""
#include "data/input.cpp"

// Include class files
#include "classes/Timer.cpp"
#include "classes/Texture.cpp"


//-------------------------------------------------------
// Character class

class LCharacter
{
	public: 					// Consider attaching texture to the class
		double x; 				// Seperate player character
		double y;
		
		double currentSpeedX;
		double currentSpeedY;
		double postSpeedX; 			// Post processed speed( impacted by steps )
		double postSpeedY;
		double maxSpeed;
		double acceleration;
		double deceleration;
		double baseSpeed;

		float step;

		// Initialization	
		LCharacter();
	
		// Deallocate
		~LCharacter();

		// Deallocate
		void free();

	private:

};

//-------------------------------------------------------
// Character Methods

// Constructor
LCharacter::LCharacter() 			// TODO: Add values as arguments
{
	// Start position
	x = SCREEN_WIDTH / 2;
	y = SCREEN_WIDTH / 2;

	currentSpeedX = 0;
	currentSpeedY = 0;
	postSpeedX = 0;
	postSpeedY = 0;

	step = 1;
					// TODO: Find correct values
	acceleration = 0.0011;
	deceleration = 0.005;
	maxSpeed = 1; 
	baseSpeed = 0.1;

	void free();
}

// Deallocate
LCharacter::~LCharacter()
{
	free();
}

void LCharacter::free()
{
}


//-------------------------------------------------------------------------------------------
// Functions

// Start SDL and create window
bool init();

// Load media
bool loadMedia();

// Frees media and shuts down SDL
void close();

// Returns coordinate of one axis based on speed
double updateLocation( double location, double speed );

// Calculates speed with acceleration
double accelerate( double currentSpeed, double maxSpeed, double acceleration );

// Calculates speed reduction over time
double decelerate( double currentSpeed, double maxSpeed, double deceleration );

// Returns starting speed
double moveDownTheAxis( double currentSpeed, double baseSpeed = 0.2 );

double moveToward( double currentSpeed, double maxSpeed, double acceleration, double locationX, double locationY, double toX, double toY );

// Gets frames per second from given timer
int getFps( Timer );

//----------------------------------------------------------------------------------------------
// Global Variables

// The window that will be rendered to
SDL_Window* gWindow = NULL;

// The window renderer
SDL_Renderer* gRenderer = NULL;

// Font
TTF_Font *gFont = NULL;

// Text texture
LTexture gTextTexture;

// Fps counter texture
LTexture gFpsCounter;

// Background texture
LTexture gPromptTexture;

// Character texture
LTexture gCharacterTexture;
LCharacter gPlayer;

// Music
Mix_Music *gMusic = NULL;

// Sound effects
Mix_Chunk *gScratch = NULL;

//--------------------------------------------------------
// Color variables
const SDL_Color WHITE = { 255, 255, 255 };
const SDL_Color BLACK = { 0, 0, 0 };
const SDL_Color RED = { 255, 0, 0 };
const SDL_Color BLUE = { 0, 0, 255 };
const SDL_Color GREEN = { 0, 255, 0 };
const SDL_Color ORANGE = { 255, 127, 0 };
const SDL_Color GREY = { 50, 50, 50 };

// Frames for calculating fps
int frames = 0;

//----------------------------------------------------------
// Initialization Function

bool init()
{
	bool success = true;

	// Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO | SDL_INIT_AUDIO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		// Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) ) 
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}
	
		/*
		// Get display resolution
		SDL_DisplayMode DM;
		SDL_GetCurrentDisplayMode( 0, &DM ); 				// Use this for automatic resolution for first time setup
		screen_width = DM.w;
		screen_height = DM.h;
		*/

		// Create window
		gWindow = SDL_CreateWindow( 
				WINDOWNAME, 
			       	SDL_WINDOWPOS_UNDEFINED, 
				SDL_WINDOWPOS_UNDEFINED, 
				SCREEN_WIDTH, 
				SCREEN_HEIGHT,
				SDL_WINDOW_SHOWN /*SDL_WINDOW_INPUT_GRABBED*/ ); 			// TODO: Change window type

		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			// Create renderer for window
			// Use VSync for now
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED /* | SDL_RENDERER_PRESENTVSYNC */ );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				// Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

				// Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if( !( IMG_Init( imgFlags ) & imgFlags ) )
				{
					printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
					success = false;
				}

				// Initialize fonts
				if( TTF_Init() == -1 )
				{
					printf("SDL_ttf couldn`t be initialised! SDL_ttf Error: %s\n", TTF_GetError() );
					success = false;
				}
				// Initialize mixer
				// TODO: Auto set best possible
				if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) 	// 44100 sample rate, bit depth, stereo channels, max number of sounds
				{
					printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
					success = false;
				}
			}
		}
	}

	return success;
}

//-----------------------------------------------------------------------------------------
// Load Media

bool loadMedia()
{
	bool success = true;

	// Load prompt texture
	if( !gPromptTexture.loadFromFile( "res/prompt.png" ) )
	{
		printf( "Failed to load prompt texture!\n" );
		success = false;
	}

	// Load character texture
	if( !gCharacterTexture.loadFromFile( "res/character.png" ) )
	{
		printf( "Failed to load character texture!\n" );
		success = false;
	}

	// Load font
	gFont = TTF_OpenFont( "res/Roboto-Regular.ttf", 28 ); 					// TODO: Change font
	if( gFont == NULL )
	{
		printf( "Failed to load font! SDL_Font Error: %s\n", TTF_GetError() );
		success = false;
	}
	else
	{
		SDL_Color textColor = ORANGE;
		if( !gTextTexture.loadFromRenderedText( "Sample text", textColor ) ) 		// Text texture should be renamed
		{
			printf( "Failed to render text texture!\n" );
			success = false;
		}
	}

	// Load music
	gMusic = Mix_LoadMUS( "res/bgm.flac" );
	if( gMusic == NULL )
	{
		printf( "Failed to load music! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	// Load sound effects
	gScratch = Mix_LoadWAV( "res/scratch.wav" );
	if( gScratch == NULL )
	{
		printf( "Failed to load scratch sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}
	
	return success;
}

//--------------------------------------------------------
// Texture Methods

LTexture::LTexture()
{
	// Init
	mTexture = NULL;
	mWidth = 0;
	mHeight = 0;
}

LTexture::~LTexture()
{
	// Deallocate
	free();
}

//--------------------------------------------------------------------------------------------------------
// Load texture from file

bool LTexture::loadFromFile(std::string path)
{
	// Clear existing texture
	free();

	SDL_Texture* newTexture = NULL;

	// Load image
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());

	if(loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		// Color key image
		SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );

		// Create texture from surface pixels
       		newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

		if( newTexture == NULL )
		{
			printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
		}
		else
		{
			// Get dimensions
			mWidth = loadedSurface->w;
			mHeight = loadedSurface->h;
		}

		// Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	// Return success
	mTexture = newTexture;
	return mTexture != NULL;
}

//--------------------------------------------------------------------------------------
// Text surface

bool LTexture::loadFromRenderedText( std::string textureText, SDL_Color textColor )
{
	// Get rid of preexisting texture
	free();

	// Render text surface
	// SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, textureText.c_str(), textColor );
	SDL_Surface* textSurface = TTF_RenderUTF8_Blended( gFont, textureText.c_str(), textColor );

	if( textSurface != NULL )
	{
		// Create texture from surface pixels
       		mTexture = SDL_CreateTextureFromSurface( gRenderer, textSurface );

		if( mTexture == NULL )
		{
			printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
		}
		else
		{
			// Get image dimensions
			mWidth = textSurface->w;
			mHeight = textSurface->h;
		}

		// Get rid of old surface
		SDL_FreeSurface( textSurface );
	}
	else
	{
		printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
	}

	
	// Return success
	return mTexture != NULL;
}

void LTexture::free()
{
	// Free texture if it exists
	if( mTexture != NULL )
	{
		SDL_DestroyTexture( mTexture );
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
	}
}

void LTexture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
	// Change texture rgb
	SDL_SetTextureColorMod( mTexture, red, green, blue );
}

void LTexture::setBlendMode( SDL_BlendMode blending )
{
	// Set blending function
	SDL_SetTextureBlendMode( mTexture, blending );
}
		
void LTexture::setAlpha( Uint8 alpha )
{
	// Change texture alpha
	SDL_SetTextureAlphaMod( mTexture, alpha );
}

void LTexture::render( int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip )
{
	// Set rendering space and render to screen
	SDL_Rect renderQuad = { x, y, mWidth, mHeight };

	// Set clip rendering dimensions
	if( clip != NULL )
	{
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	// Render to screen
	SDL_RenderCopyEx( gRenderer, mTexture, clip, &renderQuad, angle, center, flip );
}

int LTexture::getWidth()
{
	return mWidth;
}

int LTexture::getHeight()
{
	return mHeight;
}

//---------------------------------------------------------
// Logic Functions

double updateLocation( double location, double speed )
{
	return location + speed;
}

// Get average fps through given timer`s runtime
int getFps( Timer timer ) 
{
	float fps = frames / (double)( timer.getTime() / 1000 );
	// Don`t show huge values
	if( fps > 10000000 )
	{
		fps = 0;
	}
	return (int)fps;	
}

//-------------------------------------------------------
// Speed Calculation

double getDistance( double location, double to )
{
	return to - location;
}

double getRatio( double distance1, double distance2 )
{
	// Avoid negatives since its distance ratio
	if( distance1 < 0 )
	{	
		distance1 = -distance1;
	}

	if( distance2 < 0 )
	{
		distance2 = -distance2;
	}

	
	// Avoid deviding by zero
	if( distance2 == 0 )
	{
		return 0; 
	}
	
	return distance1 / distance2;
}

// Returns ratio of axis 1 compared to axis 2
double rationalize( double distance1, double distance2 )
{
	// If different axis is 0 then current axis is full speed
	if( distance2 == 0 )
	{
		return 1;
	}
	if( distance1 == 0 )
	{
		return 1;
	}

	// Divide smaller distance with the greater
	double ratio = getRatio( distance1, distance2 );

	// Check for higher values than 1
	if( ratio > 1 )
	{
		// Flip it
		ratio = 1 / ratio;
	}
	
	if( ratio < 0.5 )
	{
		return 1;
	}

	return ratio;
}

// TODO: Move up/down the axis
// TODO: Implement 8 directional movement
double moveToward( double currentSpeed, double maxSpeed, double acceleration, double location1, double to1, double location2, double to2 )
{
	if( getDistance( location1, to1 ) == 0 )
	{
		return currentSpeed;
	}

	// If distance is positive
	if( getDistance( location1, to1 ) > 0 )
	{
		currentSpeed = -moveDownTheAxis( -currentSpeed );
		currentSpeed = accelerate( currentSpeed, maxSpeed, acceleration );
		/*
		if( getDistance( location2, to2 ) != 0 )
		{
			currentSpeed = currentSpeed * rationalize( getDistance( location1, to1 ), getDistance( location2, to2 ) );
		}
		*/
	}
	// If distance is negative
	else if( getDistance( location1, to1 ) < 0 )
	{
		currentSpeed = moveDownTheAxis( currentSpeed );
		currentSpeed = accelerate( currentSpeed, maxSpeed, acceleration );
		/*
		if( getDistance( location2, to2 ) != 0 )
		{
			currentSpeed = currentSpeed * rationalize( getDistance( location2, to2 ), getDistance( location1, to1 ) );
		}
		*/
	}
}

// TODO: Fix instant acceleration
double accelerate( double currentSpeed, double maxSpeed, double acceleration ) 			// Gets one dimensional speed (x or y)
{	
	// If negative
	if( currentSpeed < 0 )
	{
		acceleration = -acceleration;
		maxSpeed = -maxSpeed;
		// If speed is less than max
		if( currentSpeed > maxSpeed - acceleration )
		{
			// Increase speed
			return currentSpeed + acceleration;
		}
		else
		{
			// return max
			return maxSpeed;
		}
	}
	// If positive
	else if( currentSpeed > 0 )
	{
		// If speed is less than max
		if( currentSpeed < maxSpeed - acceleration )
		{
			// Increase speed
			return currentSpeed + acceleration;
		}
		else
		{
			// return max
			return maxSpeed;
		}
	}
}

// TODO: Remove maxspeed
double decelerate( double currentSpeed, double deceleration )
{
	float stopThreshold = 0.00001; 				// TODO: Consider making threshold a passable character quality
	// If negative
	if( currentSpeed < 0 )
	{
		// If speed is more than 0 + deceleration
		if( currentSpeed < -stopThreshold )
		{
			// Decrease speed
			return currentSpeed + deceleration;
		}
		else
		{
			return 0;
		}
	}
	else if( currentSpeed == 0 )
	{
		return 0;
	}
	// If positive
	else
	{
		deceleration = -deceleration;
		// If speed is more than 0 + deceleration
		if( currentSpeed > stopThreshold )
		{
			// Decrease speed
			return currentSpeed + deceleration;
		}
		else
		{
			return 0;
		}
	}
}

// TODO: Unhardcode
// TODO: Find a way around abrupt stop
// TODO: Make everything more responsive
double moveDownTheAxis( double currentSpeed, double baseSpeed )
{
	if( currentSpeed > -baseSpeed && currentSpeed < baseSpeed )
	{
		return -baseSpeed;
	}
}

// Return speed after step
// TODO: Play a certain sound
// TODO: Don`t pass in speed seperately
double step( double currentSpeed, LCharacter character )
{
	// Speed multiplier amount for one iteration
	float stepSize = 0.1;
	if( currentSpeed != 0 ) 				// If check to avoid unnecessery calculations
	{
		if( character.step <= 0 ) 			// Apparently there are skipped frames
		{
			character.step = 1;
		}

		currentSpeed = currentSpeed * character.step; // TODO: Make more sophisticated formula
		character.step -= stepSize;
	}
	return currentSpeed;
}

//-----------------------------------------------------------------------------------------------------
// Cleanup

void close()
{
	// Free loaded images
	gPromptTexture.free();
	gCharacterTexture.free();
	gFpsCounter.free();

	// Free fonts
	gTextTexture.free();
	TTF_CloseFont( gFont );
	gFont = NULL;

	// Free music
	Mix_FreeMusic( gMusic );
	gMusic = NULL;

	// Destroy window	
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	// Quit SDL
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

//----------------------------------------------------------------------------------
// Main

int main( int argc, char* args[] )
{
	// Start SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		// Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{	
			bool quit = false;
			SDL_Event event;

			Timer runTime;
			runTime.start();

			// Step stands for time step
			Timer stepTimer;
			float timeStep = 0;
			stepTimer.start();

			Timer fpsTimer;
			std::stringstream timeText;
			fpsTimer.start();

			while( !quit )
			{

//--------------------------------------------------------------------------------------
// Event Handling

				while( SDL_PollEvent( &event ) != 0 ) 			// Events go down until zero
				{
					if( event.type == SDL_QUIT )
					{
						quit = true;
					}
					else if( event.type == SDL_KEYDOWN ) 		// Key is being pressed down
					{
						switch( event.key.keysym.sym )
						{
							case SDLK_p:
							if( runTime.active == true ) 
							{ 
								runTime.resume(); 
							} 
							else 
							{ 
								runTime.pause(); 
							}
							break;
							
							// Only for testing
							case SDLK_COMMA:
							gPlayer.currentSpeedX = moveToward( 
									gPlayer.currentSpeedX, 
									gPlayer.maxSpeed, 
									gPlayer.acceleration, 
									gPlayer.x, 
									mouseX,
								        gPlayer.y,
									mouseY	);

							gPlayer.currentSpeedY = moveToward( 
									gPlayer.currentSpeedY, 
									gPlayer.maxSpeed, 
									gPlayer.acceleration, 
									gPlayer.y, 
									mouseY,
								        gPlayer.x,
									mouseX
									);
							
						
							break;
//-----------------------------------------------------------------------------------------------------------------------
// Directional movement


							if( runTime.paused == false )
							{
							case UP:
							gPlayer.currentSpeedY = moveDownTheAxis( gPlayer.currentSpeedY, gPlayer.maxSpeed );
							gPlayer.currentSpeedY = accelerate( gPlayer.currentSpeedY, gPlayer.maxSpeed, gPlayer.acceleration );
							break;
		
							case LEFT:
							gPlayer.currentSpeedX = moveDownTheAxis( gPlayer.currentSpeedX, gPlayer.maxSpeed );
							gPlayer.currentSpeedX = accelerate( gPlayer.currentSpeedX, gPlayer.maxSpeed, gPlayer.acceleration );
							break;

							case RIGHT:
							gPlayer.currentSpeedX = -moveDownTheAxis( -gPlayer.currentSpeedX, gPlayer.maxSpeed );
							gPlayer.currentSpeedX = accelerate( gPlayer.currentSpeedX, gPlayer.maxSpeed, gPlayer.acceleration );
							break;	
							
							case DOWN:
							gPlayer.currentSpeedY = -moveDownTheAxis( -gPlayer.currentSpeedY, gPlayer.maxSpeed );
							gPlayer.currentSpeedY = accelerate( gPlayer.currentSpeedY, gPlayer.maxSpeed, gPlayer.acceleration );
							break;
							}

//------------------------------------------------------------------------------------------------------------------------
// Sound Control


							case SDLK_9:
							if( Mix_PlayingMusic() == 0 )
							{
								// Play music
								Mix_PlayMusic( gMusic, -1 );
							}
							else
							{
								// Playing or paused
								if( Mix_PausedMusic() == 1 )
								{
									Mix_ResumeMusic();
								}
								else
								{
									Mix_PauseMusic();
								}
							}
							break;
							
							case SDLK_0:
							Mix_HaltMusic(); 		// Stop command
							break;
						}
					}
					else if( event.type == SDL_MOUSEMOTION ) 
					{
						SDL_GetMouseState( &mouseX, &mouseY );
						// Get mirrored mouse coordinates( assuming that getmousestate can`t return negative )
						mouseMirroredX = SCREEN_WIDTH - mouseX;
						mouseMirroredY = SCREEN_HEIGHT - mouseY;
					}
				}

				// Set renderer color
				SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0xFF );
				// Clear screen
				SDL_RenderClear( gRenderer );

//----------------------------------------------------------------------------------------------------------
// Text Rendering


				// TODO: Consider showing time running
				// Set text to be rendered 
				timeText.str( "" );	
				timeText <<  fps; 
				// Render text 
				if( !gFpsCounter.loadFromRenderedText( timeText.str().c_str(), GREEN ) ) 
				{ 
					printf( "Unable to render fps texture!\n" ); 
				}

//---------------------------------------------------------------------------------------------------------
// Independant From Framerate

				// Calculate the time step in seconds 
				float timeStep = stepTimer.getTime() / 1000;
			
				fps = getFps( fpsTimer );
				
				// Magical calculations to keep a constant time
				if( fps != 0 )
				{
					if( fpsArrayFull == true )
					{
						// Use average of last frames
						// Calculate average fps of last several seconds
						int sum = 0;
						int avg = 0;
						for( int i = 0; FPSSIZE > i; i++ )
						{
							sum += lastFps[ i ]; 
						}
						avg = sum / FPSSIZE;

						fpsMultiplier = ( 1 / LOGIC_PERIOD ) / avg;
					}
					else
					{
						
						// Calculate average fps of last several seconds
						int sum = 0;
						int avg = 0;
						for( int i = 0; fpsIterator > i; i++ )
						{
							sum += lastFps[ i ]; 
						}
						
						avg = sum / FPSSIZE;
						
						if( avg == 0 )
						{
							avg = fps;
						}

						// Use current fps
						fpsMultiplier = ( 1 / LOGIC_PERIOD ) / avg;
					}
				}
				
				// If more than second has passed
				if( fpsTimer.getTime() / 1000 >= 1 )
				{
					// If frames array has been filled
					if( fpsIterator == FPSSIZE )
					{
						// Reset the iterator
						fpsIterator = 0;
						fpsArrayFull = true;
					}
	
					lastFps[ fpsIterator ] = fps;
					fpsIterator++;

					// Reset counted number of frames
					frames  = 0;
					// Reset the frame counter
					//fpsTimer.stop();
					fpsTimer.start();
				}


				// If unpaused
				if( runTime.paused == false )
				{
					// Decelerate both axis
					gPlayer.currentSpeedX = decelerate(
								gPlayer.currentSpeedX,
								gPlayer.deceleration );

					gPlayer.currentSpeedY = decelerate(
								gPlayer.currentSpeedY,
								gPlayer.deceleration );

					// Steps			
					gPlayer.postSpeedX = step( gPlayer.currentSpeedX, gPlayer );
					gPlayer.postSpeedY = step( gPlayer.currentSpeedY, gPlayer );
			
					// Compensate for fps
					// There is a problem with doing it this way
					gPlayer.postSpeedX *= fpsMultiplier;
					gPlayer.postSpeedY *= fpsMultiplier;

					// Update location
					gPlayer.x = updateLocation( gPlayer.x, gPlayer.postSpeedX );
					gPlayer.y = updateLocation( gPlayer.y, gPlayer.postSpeedY );

				}
//----------------------------------------------------------------------------------------------------------
// Rendering

/* Test image rendering
				gPromptTexture.render( 0, 0 ); 				// Render background
*/
				gCharacterTexture.render( 
						(int)gPlayer.x - gCharacterTexture.getWidth() / 2, 
						(int)gPlayer.y - gCharacterTexture.getHeight() / 2
					       	);
/* Test text rendering
				gTextTexture.render( 
						( screen_width - gTextTexture.getWidth() ) / 2,
						( screen_height - gTextTexture.getHeight() ) / 2 );
*/
				gFpsCounter.render( 
						SCREEN_WIDTH - gFpsCounter.getWidth() - 4, 	// 4 pixels offset
						SCREEN_HEIGHT - gFpsCounter.getHeight() );

				// Update screen
				SDL_RenderPresent( gRenderer );
				++frames;  
			}
		}
	}

	// Free resources and close SDL
	close();
	return 0;
}
