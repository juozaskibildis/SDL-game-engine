#include <SDL2/SDL.h>
#include <string>


class LTexture
{
	public:
		// Initialization
		LTexture();

		// Deallocates memory
		~LTexture();

		// Loads image from specified path
		bool loadFromFile( std::string path ); 		// consider using namespace
		
		// Creates image from font string
		bool loadFromRenderedText( std::string textureText, SDL_Color textColor );

		// Deallocates texture
		void free();

		// Set color filter ( negative values )
		void setColor( Uint8 red, Uint8 green, Uint8 blue );

		// Set blending
		void setBlendMode( SDL_BlendMode blending );

		// Set alpha modulation
		void setAlpha( Uint8 alpha );
		
		// Renders texture at given point
		void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

		int getWidth();
		int getHeight();

	private:
		// Actual texture
		SDL_Texture* mTexture;

		int mWidth;
		int mHeight;
};




