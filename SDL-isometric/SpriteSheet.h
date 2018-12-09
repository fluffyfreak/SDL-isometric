#pragma once

#ifndef __SPRITESHEET_H__
#define __SPRITESHEET_H__

#include <vector>
#include "SDL.h"
#include "glm\vec2.hpp"

// forward declarations

// class declarations
class CSprite
{
public:
	CSprite(SDL_Surface* imageSheet, int16_t Width, int16_t Height, int16_t XOffset, int16_t YOffset)
		: _imageSheet(imageSheet), _width(Width), _height(Height), _xoffset(XOffset), _yoffset(YOffset)
	{
	}
	~CSprite() {}

	glm::vec2 GetTopLeft() const { return glm::vec2(_xoffset, _yoffset); }
	glm::vec2 GetWidthHeight() const { return glm::vec2(_width, _height); }

	SDL_Surface* GetImageSheet() const { return _imageSheet; }
private:
	SDL_Surface* _imageSheet;
	int16_t _width, _height;
	int16_t _xoffset, _yoffset;
};

// SpriteSheet class declaration
class CSpriteSheet
{
public:
	// public ctor & dtor
	CSpriteSheet(char* filename, const unsigned int tile_width, const unsigned int tile_height);
	~CSpriteSheet(void);

	unsigned int spriteWidth() const	{ return m_tile_width; }
	unsigned int spriteHeight() const	{ return m_tile_height; }
	unsigned int numTilesWide() const	{ return m_num_wide; }
	unsigned int numTilesHigh() const	{ return m_num_high; }

	CSprite& getSprite( const unsigned int idx );

private:
	// private copy and default ctor
	CSpriteSheet(const CSpriteSheet& r) : m_spritesheet(r.m_spritesheet) {/*assert(false);*/}
	CSpriteSheet & operator= (const CSpriteSheet & other);// {other.m_spritesheet = m_spritesheet;}

	// private methods
	void buildTileSprites();

	// private members
	SDL_Surface* m_spritesheet;
	SDL_Texture* m_spriteTexture;
	unsigned int m_tile_width;
	unsigned int m_tile_height;
	unsigned int m_num_wide;
	unsigned int m_num_high;

	std::vector<CSprite> m_tiles;
};

#endif // __SPRITESHEET_H__