#include <SDL2/SDL.h>

class Timer 
{ 
	public:  
		Timer(); 
 
		void start(); 
		void stop(); 
		void pause(); 
		void resume(); 

		// Need float for fps calculation
		// How long is this timer running for?
		float getTime(); 			// Could use Uint32 for higher values?

		// Timer status
		bool paused;
		bool active;

       	private: 
		 // Clock time when the timer was started
		 int startTime;

		 // Time when the timer was paused
		 int pausedTime;
};

//-----------------------------------------------
// Methods

Timer::Timer()
{ 
	// Initialize variables
	startTime = 0;
       	pausedTime = 0;
       	paused = false;
       	active = false;
}

void Timer::start() 
{
       	// Start timer
	active = true;
       	// Unpause timer
	paused = false;
       	// Get clock time 
	startTime = SDL_GetTicks();
       	pausedTime = 0;
}

void Timer::stop() 
{ 
	// Stop the timer
	active = false;
       	// Unpause the timer
	paused = false;
       	// Clear time variables
	startTime = 0;
       	pausedTime = 0;
} 

void Timer::pause()
{ 
	// If the timer is active and isn't paused 
	if( active && !paused ) 
	{ 
		// Pause the timer 
		paused = true; 
		// Get the time at the pause 
		pausedTime = SDL_GetTicks() - startTime; 
		startTime = 0;
       	} 
} 

void Timer::resume() 
{ 
	// If the timer is active and paused 
	if( active && paused ) 
	{ 
		// Unpause the timer 
		paused = false; 
		// Reset the starting ticks 
		startTime = SDL_GetTicks() - pausedTime; 
		// Reset the paused ticks 
		pausedTime = 0; 
	} 
}

float Timer::getTime() 
{ 
	float time = 0; 
	// If the timer is active 
	if( active ) 
	{ 
		// If the timer is paused 
		if( paused ) 
		{ 
			// Return the number of ticks when the timer was paused 
			time = pausedTime; 
		} 
		else 
		{ 
			// Return the time running 
			time = SDL_GetTicks() - startTime; 
		} 
	} 
	return time; 
}


