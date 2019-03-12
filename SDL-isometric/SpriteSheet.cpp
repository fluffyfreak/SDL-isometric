
#include "SpriteSheet.h"
#include "SDL_image.h"
#include "MacrosAndWrappers.h"

CSpriteSheet::CSpriteSheet(SDL_Renderer *pRenderer, const char* filename, const unsigned int tile_width, const unsigned int tile_height) :
	m_spritesheet(NULL),
	m_tile_width(tile_width),
	m_tile_height(tile_height)
{
	m_spritesheet = IMG_Load(filename);
	//IMG_SavePNG(m_spritesheet, "bodger.png");
	const char *errorMsg = IMG_GetError();
	if (errorMsg != nullptr)
	{
		printf("%s\n", errorMsg);
	}
	AA_ASSERT(m_spritesheet != nullptr);

	// set the color key, this is ... supposed to work
	//SDL_SetColorKey(m_spritesheet, SDL_TRUE, SDL_MapRGBA(m_spritesheet->format, 0xFF, 0xFF, 0xFF, 0xFF));
	//SDL_SetColorKey(m_spritesheet, SDL_TRUE, SDL_MapRGB(m_spritesheet->format, 0xFF, 0xFF, 0xFF));

	if(m_spritesheet->format->format != SDL_PIXELFORMAT_RGBA32)
		SDL_ConvertSurfaceFormat(m_spritesheet, SDL_PIXELFORMAT_RGBA32, 0);

	// so that when we build the texture for it things should be transparent
	m_spriteTexture = SDL_CreateTextureFromSurface(pRenderer, m_spritesheet);

	// set the blend mode
	const int res = SDL_SetTextureBlendMode(m_spriteTexture, SDL_BLENDMODE_BLEND);
	AA_ASSERT(res == 0);

	// TODO
	// check the tile dimensions fit within
	m_num_wide = m_spritesheet->w / m_tile_width;
	m_num_high = m_spritesheet->h / m_tile_height;

	buildTileSprites();
}

CSpriteSheet::~CSpriteSheet(void)
{
	SDL_FreeSurface(m_spritesheet);
}

// private copy operator
CSpriteSheet & CSpriteSheet::operator= (const CSpriteSheet & other) 
{
	m_spritesheet = other.m_spritesheet;
	m_tile_width = other.m_tile_width;
	m_tile_height = other.m_tile_height;
	m_num_wide = other.m_num_wide;
	m_num_high = other.m_num_high;
	return *this;
}

void CSpriteSheet::buildTileSprites()
{
	AA_ASSERT(m_tiles.size()==0);

	for( unsigned int y=0; y<m_num_high; y++ )
	{
		for( unsigned int x=0; x<m_num_wide; x++ )
		{
			const unsigned int left = m_tile_width * x;
			const unsigned int top = m_tile_height * y;
			CSprite spr(m_spritesheet, m_tile_width, m_tile_height, left, top);
			m_tiles.push_back(spr);
		}
	}
	AA_ASSERT(m_tiles.size()==(m_num_wide*m_num_high));
}

CSprite& CSpriteSheet::getSprite( const unsigned int idx )
{
	AA_ASSERT( idx < m_tiles.size() );
	return m_tiles[idx];
}
