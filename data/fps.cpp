/* This file contains all data that is dependant on time
 * ( fps, game speed and so on )
 * 
 */

// Average fps
int fps = 0;
// Multiplier for fps to keep a constant game speed
float fpsMultiplier = 1;
// Used for counting only last few frames for fps
int fpsIterator = 0;
// Amount of frames to count in fps
const int FPSSIZE = 60;
// Holds last few frames
int lastFps[ FPSSIZE ];
// Is lastFps filled?
bool fpsArrayFull = false;

// Preferable amount of time between logic loops
// Basically game speed
// Figure out best time
// 0.01 = 100hz
// Preferable time should be dividable by refresh rate?
//const float LOGIC_PERIOD = 0.00135; 
const float LOGIC_PERIOD = 0.01; 
