//================================================================================
//
//		Filename:	TiledMap.h
//		Created:	04/03/12
//		Author:	    Andrew Copland
//
//================================================================================

#ifndef TILEDMAP_H
#define TILEDMAP_H

#include <vector>
#include "glm\vec2.hpp"

// forward declarations
class CSpriteSheet;
struct SDL_Window;

struct TMapTile
{
	int		imageID;
	bool	flipV;
	bool	flipH;
	bool	drawn;
};
typedef TMapTile TMapTile;

static const unsigned int IMAGENAME_MAX_LEN = 64;
class TileImage
{
public:
	TileImage();
	~TileImage();
	CSpriteSheet*	pSpriteSheet;
	char			imageName[IMAGENAME_MAX_LEN];
	int				imageW;		//	image size in pixels
	int				imageH;
	int				tileW;		//	tile size in pixels
	int				tileH;
	int				width;		//	number of tiles tiles
	int				height;
	//bool			required;
};

// class: 
class TiledMap
{
public:
	struct TSurfaceDesc {
		unsigned char walkableType;
	};

	TiledMap(const char* pathname, const char* filename, const int screenWide, const int screenHigh);
	~TiledMap();

	void Update(const bool mouseDown, const int xx, const int yy);
	void Render(SDL_Window* renderWindow, const int iSingleLayer = -1);

	int getNumLayers() const { return numLayers; }
	bool doesMapHaveErrors() const { return mapHadErrors; }
	const std::string& getMapFilename() const { return mapFilename; }

	glm::vec2 pixelToTileCoords(const float xIn, const float yIn, int &layerOut) const;
	glm::vec2 pixelToTileCoords(float x, float y) const;
	glm::vec2 tileToPixelCoords(float x, float y) const;
#if 0
	void drawTileLayers(SDL_Window* renderWindow, const sf::FloatRect &exposed) const;
#endif

	void setHighlight(const int tilex, const int tiley, const int layer) {
		hiliteX = tilex;
		hiliteY = tiley;
		hiliteLayer = layer;
	}

	TSurfaceDesc& getWalkableTile(const int tilex, const int tiley, const int layer) const {
		TSurfaceDesc** pTileLayer = _walkableLayer[layer];
		return pTileLayer[tilex][tiley];
	}

private:
	int numLayers;				//	How many map layers
	int	numTilesets;			//	How many tilesets
	int numObjects;				//	Now many sprite objects

	int	screenWidth;			//	Screen size
	int	screenHeight;

	int	mapWidth;				//	Size of map in tiles
	int	mapHeight;

	int tileWidth;				//	Size of tiles in pixels
	int tileHeight;

	int halftileW;				//	half a tiles width
	int halftileH;				//	...	use these a lot, so keep handy!

	int mapPixelW;				//	size of full map in pixels
	int mapPixelH;

	int startOffsetX;			//	X offset to draw tile at 0,0

	int mapDrawX;				//	Where to draw the map
	int mapDrawY;

	int hiliteX;
	int hiliteY;
	int hiliteLayer;

	bool mapHadErrors;

	std::string mapFilename;

	std::vector<TileImage> _imageList;
	std::vector<int >		_gids;
	std::vector<TMapTile**> _mapLayer;
	std::vector<TSurfaceDesc**> _walkableLayer;
	TSurfaceDesc** CreateWalkableLayer(const int w, const int h);
	void DeleteWalkableLayer(TSurfaceDesc** map);

	TMapTile** CreateMapLayer(const int w, const int h);
	void DeleteMapLayer(TMapTile** map);

	int FindTileset(int gid);

	void RenderMap(SDL_Window* renderWindow, const int drawX, const int drawY, const int iSingleLayer);
};


#endif	// TILEDMAP_H