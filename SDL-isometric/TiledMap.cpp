//================================================================================
//
//		Filename:	TiledMap.cpp
//		Created:	04/03/12
//		Author:	    Andrew Copland
//
//================================================================================

#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>
#include <cassert>

//#include <SFML/Graphics.hpp>

//#include "common.h"

#include "tinyxml2/tinyxml2.h"
#include "tiled/Tmx.h"
#include "TiledMap.h"
#include "SpriteSheet.h"

#include "SDL.h"

/*
#include "astar/stlastar.h" // See header for copyright and usage information
// Definitions
namespace AStar
{
	class MapSearchNode
	{
	private:
		unsigned int x;	 // the (x,y,z) positions of the node
		unsigned int y;
		unsigned int z;
		TiledMap* map;
		unsigned int numTiles;
	public:
		MapSearchNode() { x = y = z = 0; }
		MapSearchNode(const unsigned int px, const unsigned int py, const unsigned int pz) { x = px; y = py; z = pz; }

		float GoalDistanceEstimate(const MapSearchNode &nodeGoal) const;
		bool IsGoal(const MapSearchNode &nodeGoal) const;
		bool GetSuccessors(AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node);
		float GetCost(const MapSearchNode &successor) const;
		bool IsSameState(const MapSearchNode &rhs) const;
		TiledMap::TSurfaceDesc& GetMap(int x, int y, int z) const;
	};

	TiledMap::TSurfaceDesc& MapSearchNode::GetMap(int x, int y, int z) const
	{
		return map->getWalkableTile(x, y, z);
	}

	bool MapSearchNode::IsSameState(const MapSearchNode &rhs) const
	{
		// same state in a maze search is simply when (x,y) are the same
		if ((x == rhs.x) && (y == rhs.y) && (z == rhs.z)) {
			return true;
		}
		else {
			return false;
		}
	}

	// Here's the heuristic function that estimates the distance from a Node
	// to the Goal.
	float MapSearchNode::GoalDistanceEstimate(const MapSearchNode &nodeGoal) const
	{
		const float xd = fabs(float(((float)x - (float)nodeGoal.x)));
		const float yd = fabs(float(((float)y - (float)nodeGoal.y)));
		const float zd = fabs(float(((float)z - (float)nodeGoal.z)));

		return xd + yd + zd;
	}

	bool MapSearchNode::IsGoal(const MapSearchNode &nodeGoal) const
	{
		if ((x == nodeGoal.x) && (y == nodeGoal.y) && (z == nodeGoal.z)) {
			return true;
		}

		return false;
	}

	// This generates the successors to the given Node. It uses a helper function called
	// AddSuccessor to give the successors to the AStar class. The A* specific initialisation
	// is done for each node internally, so here you just set the state information that
	// is specific to the application
	bool MapSearchNode::GetSuccessors(AStarSearch<MapSearchNode> *astarsearch, MapSearchNode *parent_node)
	{
		int parent_x = -1;
		int parent_y = -1;
		int parent_z = -1;

		if (parent_node)
		{
			parent_x = parent_node->x;
			parent_y = parent_node->y;
			parent_z = parent_node->z;
		}
#if 1
		const int offsets[2][4] = {
			{	-1,	 0,	 1,	 0	},	// x
			{	 0,	-1,	 0,	 1	}	// y
		};

		const int minz = (parent_z == 0) ? 0 : -1;
		const int maxz = (parent_z == (numTiles - 1)) ? 0 : 1;

		MapSearchNode NewNode;
		for (int z = minz; z <= maxz; z++) {
			for (int i = 0; i < 4; i++) {
				if ((GetMap((x + offsets[0][i]), (y + offsets[1][i]), parent_z + z).walkableType < 9) &&
					!((parent_x == (x + offsets[0][i])) && (parent_y == (y + offsets[1][i])) && (parent_z == z)))
				{
					NewNode = MapSearchNode((x + offsets[0][i]), (y + offsets[1][i]), parent_z + z);
					astarsearch->AddSuccessor(NewNode);
				}
			}
		}
#else
		MapSearchNode NewNode;

		// push each possible move except allowing the search to go backwards

		if ((GetMap(x - 1, y) < 9) && !((parent_x == x - 1) && (parent_y == y)))
		{
			NewNode = MapSearchNode(x - 1, y);
			astarsearch->AddSuccessor(NewNode);
		}

		if ((GetMap(x, y - 1) < 9) && !((parent_x == x) && (parent_y == y - 1)))
		{
			NewNode = MapSearchNode(x, y - 1);
			astarsearch->AddSuccessor(NewNode);
		}

		if ((GetMap(x + 1, y) < 9) && !((parent_x == x + 1) && (parent_y == y)))
		{
			NewNode = MapSearchNode(x + 1, y);
			astarsearch->AddSuccessor(NewNode);
		}

		if ((GetMap(x, y + 1) < 9) && !((parent_x == x) && (parent_y == y + 1)))
		{
			NewNode = MapSearchNode(x, y + 1);
			astarsearch->AddSuccessor(NewNode);
		}
#endif
		return true;
	}

	// given this node, what does it cost to move to successor. In the case
	// of our map the answer is the map terrain value at this node since that is
	// conceptually where we're moving
	float MapSearchNode::GetCost(const MapSearchNode &successor) const
	{
		return 1.0f;// GetMap( x, y, z ).walkableType;
	}
};*/

#include "MacrosAndWrappers.h"

TileImage::TileImage() : pSpriteSheet(nullptr)
{
}

TileImage::~TileImage()
{
	if (nullptr != pSpriteSheet) {
		delete pSpriteSheet;
	}
}

TiledMap::TiledMap(SDL_Renderer *pRenderer, const char* pathname, const char* filename, const int screenWide, const int screenHigh)
	: numTilesets(0),
	numLayers(0),
	screenWidth(screenWide),
	screenHeight(screenHigh),
	hiliteX(0),
	hiliteY(0),
	hiliteLayer(0),
	mapHadErrors(false),
	mapFilename(filename)
{
	//	Load in the map xml
	char filepath[256];
	_snprintf(filepath, 256, "%s/%s", pathname, filename);

	//IwTrace( MAP, ("Loading TiledMap: %s", filepath) );

	Tmx::Map *map = new Tmx::Map();
	map->ParseFile(filepath);

	if (map->HasError())
	{
		//IwTrace( MAP, ("error code: %d\n", map->GetErrorCode()) );
		//IwTrace( MAP, ("error text: %d\n", map->GetErrorText().c_str() ) );
		mapHadErrors = true;
		return;
	}

	//	So what have we just loaded?
	numLayers = map->GetNumLayers();
	numTilesets = map->GetNumTilesets();

	mapWidth = map->GetWidth();
	mapHeight = map->GetHeight();

	tileWidth = map->GetTileWidth();
	tileHeight = map->GetTileHeight();

	//IwTrace( MAP, ("TiledMap Loaded OK: %d layers, %d tilesets, map size (%d,%d), tile size (%d,%d)", 
	//				numLayers, numTilesets,
	//				mapWidth, mapHeight,
	//				tileWidth, tileHeight) );

	//	Calculate some useful numbers
	halftileW = (tileWidth + 1) >> 1;	//	Half tile sizes
	halftileH = (tileHeight + 1) >> 1;	//	...allowing for odd size tiles

	mapPixelW = (mapWidth + mapHeight) * halftileW;	//	The BIG rect
	mapPixelH = (mapWidth + mapHeight) * halftileH;

	startOffsetX = mapHeight * halftileW;

	//	Find draw position to centre the map
	mapDrawX = (mapPixelW >> 1) - (screenWidth >> 1) - startOffsetX;
	mapDrawY = (mapPixelH - screenHeight) >> 1;

	//		Load in the tileset info
	_imageList.reserve(numTilesets);
	_gids.reserve(numTilesets);

	for (int i = 0; i < numTilesets; i++)
	{
		TileImage tileImage;

		// Get a tileset.
		const Tmx::Tileset *tileset = map->GetTileset(i);
		_gids.push_back(tileset->GetFirstGid());

		const Tmx::Image *image = tileset->GetImage();

		_snprintf(tileImage.imageName, IMAGENAME_MAX_LEN, "%s%s", pathname, image->GetSource().c_str());

		tileImage.imageW = image->GetWidth();				//	image size in pixels
		tileImage.imageH = image->GetHeight();

		tileImage.tileW = tileset->GetTileWidth();			//	tile size in pixels
		tileImage.tileH = tileset->GetTileHeight();

		tileImage.width = tileImage.imageW / tileImage.tileW;		//	size in tiles
		tileImage.height = tileImage.imageH / tileImage.tileH;

		tileImage.pSpriteSheet = NULL;

		_imageList.push_back(tileImage);
	}

	//		Load in the map layers
	for (int i = 0; i < numLayers; i++)
	{
		// Get a layer
		const Tmx::Layer *layer = map->GetLayer(i);
		std::string layerName = layer->GetName().c_str();

		//	Drawable layer
		TMapTile** thisLayer = CreateMapLayer(mapWidth, mapHeight);
		_mapLayer.push_back(thisLayer);

		// blah blah
		for (int x = 0; x < mapWidth; x++)
		{
			for (int y = 0; y < mapHeight; y++)
			{
				// Get a tile global id
				Tmx::MapTile tile = layer->GetTile(x, y);
				thisLayer[x][y].imageID = tile.gid - 1;
				thisLayer[x][y].flipH = tile.flippedHorizontally;
				thisLayer[x][y].flipV = tile.flippedVertically;
				thisLayer[x][y].drawn = tile.gid >= 6;
			}
		}
	} // end of for loop

	//		Load in the images
	for (size_t i = 0; i < _imageList.size(); i++)
	{
		_imageList[i].pSpriteSheet = new CSpriteSheet(pRenderer, _imageList[i].imageName, 64, 48);
	}

	mapHadErrors = (numLayers != _mapLayer.size());

	// NB: WORK-IN-PROGRESS!!!
	//fail-compile-here!!!
	unsigned char walkableMapping[256];
	double version = 0.0f;
	int entries = 0;
	{
		char* fileText;
		int fileSize;

		// Open the file for reading.
		FILE *file = fopen("data/maps/collision/collision.xml", "rb");

		// Check if the file could be opened.
		if (file)
		{
			// Find out the file size.
			fseek(file, 0, SEEK_END);
			fileSize = ftell(file);
			fseek(file, 0, SEEK_SET);

			// Allocate memory for the file and read it into the memory.
			fileText = new char[fileSize];
			fread(fileText, 1, fileSize, file);

			fclose(file);

			// Copy the contents into a C++ string and delete it from memory.
			std::string text(fileText, fileText + fileSize);
			delete[] fileText;

			// Create a tiny xml document and use it to parse the text.
			XMLDocument doc;
			doc.Parse(text.c_str());

			XMLNode *mapNode = doc.FirstChildElement("collision");
			XMLElement* mapElem = mapNode->ToElement();

			// Read the map attributes.
			version = mapElem->DoubleAttribute("version");
			entries = mapElem->IntAttribute("entries");
			AA_ASSERT(256 == entries);

			// Iterate through all of the tileset elements.
			XMLNode *tileNode = mapNode->FirstChildElement("tile");
			while (tileNode)
			{
				XMLElement* tileElem = tileNode->ToElement();
				int gid = tileElem->IntAttribute("gid");
				int value = tileElem->IntAttribute("value");

				walkableMapping[gid] = value;

				tileNode = tileNode->NextSiblingElement("tile");
			}
		}
	}
	// now build the walkability layer data
	for (int i = 0; i < numLayers; i++)
	{
		//	Drawable layer
		TSurfaceDesc** thisLayer = CreateWalkableLayer(mapWidth, mapHeight);
		_walkableLayer.push_back(thisLayer);

		//	Drawable layer
		TMapTile** mapLayer = _mapLayer[i];

		// blah blah
		for (int x = 0; x < mapWidth; x++)
		{
			for (int y = 0; y < mapHeight; y++)
			{
				// Get a tile global id
				thisLayer[x][y].walkableType = walkableMapping[mapLayer[x][y].imageID];
			}
		}
	} // end of for loop

	delete map;
}


int TiledMap::FindTileset(int gid)
{
	for (int i = numTilesets - 1; i > -1; i--)
	{
		// If the gid beyond the tileset gid return it.
		if (gid >= _gids[i])
		{
			return i;
		}
	}
	return -1;
}


TiledMap::~TiledMap()
{
	_imageList.clear();
	_gids.clear();

	for (size_t i = 0; i < _mapLayer.size(); i++) {
		DeleteMapLayer(_mapLayer[i]);
	}
	_mapLayer.clear();
	for (size_t i = 0; i < _walkableLayer.size(); i++) {
		DeleteWalkableLayer(_walkableLayer[i]);
	}
	_walkableLayer.clear();
}


TiledMap::TSurfaceDesc** TiledMap::CreateWalkableLayer(const int w, const int h)
{
	TSurfaceDesc** map = new TSurfaceDesc*[w];
	if (map != NULL)
	{
		for (int i = 0; i < w; i++)
		{
			map[i] = new TSurfaceDesc[h];
			memset(map[i], 0, h * sizeof(TSurfaceDesc));
		}
	}

	return map;
}


void TiledMap::DeleteWalkableLayer(TSurfaceDesc** map)
{
	if (map != NULL)
	{
		for (int i = 0; i < mapWidth; i++)
		{
			if (map[i] != NULL)
			{
				delete[] map[i];
			}
		}

		delete[] map;
		map = NULL;
	}
}

TMapTile** TiledMap::CreateMapLayer(const int w, const int h)
{
	TMapTile** map = new TMapTile*[w];
	if (map != NULL)
	{
		for (int i = 0; i < w; i++)
		{
			map[i] = new TMapTile[h];
			memset(map[i], 0, h * sizeof(TMapTile));
		}
	}

	return map;
}


void TiledMap::DeleteMapLayer(TMapTile** map)
{
	if (map != NULL)
	{
		for (int i = 0; i < mapWidth; i++)
		{
			if (map[i] != NULL)
			{
				delete[] map[i];
			}
		}

		delete[] map;
		map = NULL;
	}
}


void TiledMap::Update(const bool mouseDown, const int xx, const int yy)
{
	//	Check for screen scrolls etc
	static bool pointerDown = false;
	static int pointerX;
	static int pointerY;

	// HACK - MUST fix
	bool isDown = mouseDown;

	if (isDown)
	{
		if (pointerDown)	//	previous x,y valid
		{
			mapDrawX -= (xx - pointerX);
			mapDrawY -= (yy - pointerY);
		}
		pointerX = xx;
		pointerY = yy;
	}
	else {
		if (pointerDown)	// previously pressed
		{
			int layerId = 0;
			glm::vec2 layeredTileCoords = pixelToTileCoords(xx, yy, layerId);
			setHighlight(layeredTileCoords.x, layeredTileCoords.y, layerId);
		}
	}

	pointerDown = isDown;
}

void TiledMap::Render(SDL_Window* renderWindow, SDL_Renderer* renderer, const int iSingleLayer/*=-1*/)
{
	RenderMap(renderWindow, renderer, mapDrawX, mapDrawY, iSingleLayer);
}

void TiledMap::RenderMap(SDL_Window* renderWindow, SDL_Renderer* renderer, const int drawX, const int drawY, const int iSingleLayer)
{
	// find the top left, i.e. startX & startY

	//	Render the map
	//	Lets just start at the top of the screen for now!
	const int clipMaxW = screenWidth;
	const int clipMaxH = screenHeight;

	// for each layer of the map (vertical)
	for (size_t lyr = 0; lyr < _mapLayer.size(); lyr++)
	{
		if (iSingleLayer >= 0 && iSingleLayer < _mapLayer.size()) {
			if (iSingleLayer < lyr) {
				continue;
			}
		}
		TMapTile** thisLayer = _mapLayer[lyr];

		if (thisLayer != NULL)
		{
			const int ylayeroffset = lyr * halftileH;
			int xrow = -drawX - halftileW; // -drawX + startOffsetX - halftileW;
			int yrow = -drawY - ylayeroffset;

			// from 0 to mapHeight???
			for (int y = 0; y < mapHeight; y++)
			{
				int xtile = xrow;
				int ytile = yrow;

				for (int x = 0; x < mapWidth; x++)
				{
					TMapTile* tile = &thisLayer[x][y];
					if (tile->drawn && (ytile >= -tileHeight) && (xtile >= -tileWidth))
					{
						TileImage* tileImg = &_imageList[0];

						CSprite& sprite = tileImg->pSpriteSheet->getSprite(tile->imageID);
						
						//Set rendering space and render to screen
						auto tl = sprite.GetTopLeft();
						SDL_Rect srcQuad = { tl.x, tl.y, tileWidth, tileHeight };
						SDL_Rect renderQuad = { xtile, ytile, tileWidth, tileHeight };
						SDL_RenderCopy(renderer, tileImg->pSpriteSheet->getSpriteTexture(), &srcQuad, &renderQuad);
					}

					ytile += halftileH;
					xtile += halftileW;

					if (ytile > clipMaxH) {
						break;
					}
					if (xtile > clipMaxW) {
						break;
					}
				}

				yrow += halftileH;
				xrow -= halftileW;
			}
		}
	}
} // ::RenderMap

glm::vec2 TiledMap::pixelToTileCoords(const float xIn, const float yIn, int &layerOut) const
{
	const float ratio = (float)tileWidth / (float)tileHeight;

	for (int lyr = getNumLayers() - 1; lyr >= 0; lyr--)
	{
		float x = xIn + mapDrawX;
		float y = yIn + mapDrawY + (lyr * halftileH);

		x -= mapHeight * halftileW - startOffsetX;
		const float mx = y + (x / ratio);
		const float my = y - (x / ratio);

		glm::vec2 tileCoords(mx / tileHeight, my / tileHeight);

		const int tilex = (int)tileCoords.x;
		const int tiley = (int)tileCoords.y;
		TMapTile** thisLayer = _mapLayer[lyr];
		const bool withinMap = (tilex >= 0 && tilex < mapWidth) && (tiley > 0 && tiley < mapHeight);
		if (withinMap && (NULL != thisLayer))
		{
			TMapTile* tile = &thisLayer[tilex][tiley];
			if (tile && tile->drawn) {
				layerOut = lyr;
				return tileCoords;
			}
		}
	}

	return glm::vec2(0.0f, 0.0f);
}

glm::vec2 TiledMap::pixelToTileCoords(float x, float y) const
{
	const float ratio = (float)tileWidth / (float)tileHeight;

	x += mapDrawX;
	y += mapDrawY;

	x -= mapHeight * halftileW - startOffsetX;
	const float mx = y + (x / ratio);
	const float my = y - (x / ratio);

	return glm::vec2(mx / tileHeight, my / tileHeight);
}

glm::vec2 TiledMap::tileToPixelCoords(float x, float y) const
{
	const int originX = mapHeight * halftileW - startOffsetX;
	return glm::vec2((x - y) * halftileW + originX - mapDrawX, (x + y) * halftileH - mapDrawY);
}

#if 0
void TiledMap::drawTileLayers(SDL_Window* renderWindow, const sf::FloatRect &exposed) const
{
	if (tileWidth <= 0 || tileHeight <= 1)
		return;

	sf::FloatRect rect = exposed.toAlignedRect();
	if (rect.isNull())
		rect = boundingRect(layer->bounds());

	QMargins drawMargins = layer->drawMargins();
	drawMargins.setTop(drawMargins.top() - tileHeight);
	drawMargins.setRight(drawMargins.right() - tileWidth);

	rect.adjust(-drawMargins.right(),
		-drawMargins.bottom(),
		drawMargins.left(),
		drawMargins.top());

	// Determine the tile and pixel coordinates to start at
	glm::vec2 tilePos = pixelToTileCoords(rect.x(), rect.y());
	sf::Vector2i rowItr = sf::Vector2i((int)std::floor(tilePos.x), (int)std::floor(tilePos.y));
	glm::vec2 startPos = tileToPixelCoords(rowItr);
	startPos.x -= tileWidth / 2;
	startPos.y += tileHeight;

	// Compensate for the layer position
	rowItr -= sf::Vector2i(layer->x(), layer->y());

	/* Determine in which half of the tile the top-left corner of the area we
	 * need to draw is. If we're in the upper half, we need to start one row
	 * up due to those tiles being visible as well. How we go up one row
	 * depends on whether we're in the left or right half of the tile.
	 */
	const bool inUpperHalf = startPos.y - rect.y() > tileHeight / 2;
	const bool inLeftHalf = rect.x() - startPos.x < tileWidth / 2;

	if (inUpperHalf) {
		if (inLeftHalf) {
			--rowItr.x;
			startPos.x -= tileWidth / 2;
		}
		else {
			--rowItr.y;
			startPos.x += tileWidth / 2;
		}
		startPos.y -= tileHeight / 2;
	}

	// Determine whether the current row is shifted half a tile to the right
	bool shifted = inUpperHalf ^ inLeftHalf;

	QTransform baseTransform = painter->transform();

	for (int y = startPos.y(); y - tileHeight < rect.bottom();
		y += tileHeight / 2)
	{
		sf::Vector2i columnItr = rowItr;

		for (int x = startPos.x(); x < rect.right(); x += tileWidth) {
			if (layer->contains(columnItr)) {
				const Cell &cell = layer->cellAt(columnItr);
				if (!cell.isEmpty()) {
					const QPixmap &img = cell.tile->image();
					const sf::Vector2i offset = cell.tile->tileset()->tileOffset();

					float m11 = 1;      // Horizontal scaling factor
					float m12 = 0;      // Vertical shearing factor
					float m21 = 0;      // Horizontal shearing factor
					float m22 = 1;      // Vertical scaling factor
					float dx = offset.x + x;
					float dy = offset.y + y - img.height();

					if (cell.flippedAntiDiagonally) {
						// Use shearing to swap the X/Y axis
						m11 = 0;
						m12 = 1;
						m21 = 1;
						m22 = 0;

						// Compensate for the swap of image dimensions
						dy += img.height() - img.width();
					}
					if (cell.flippedHorizontally) {
						m11 = -m11;
						m21 = -m21;
						dx += cell.flippedAntiDiagonally ? img.height()
							: img.width();
					}
					if (cell.flippedVertically) {
						m12 = -m12;
						m22 = -m22;
						dy += cell.flippedAntiDiagonally ? img.width()
							: img.height();
					}

					const QTransform transform(m11, m12, m21, m22, dx, dy);
					painter->setTransform(transform * baseTransform);

					painter->drawPixmap(0, 0, img);
				}
			}

			// Advance to the next column
			++columnItr.x;
			--columnItr.y;
		}

		// Advance to the next row
		if (!shifted) {
			++rowItr.x;
			startPos.x += tileWidth / 2;
			shifted = true;
		}
		else {
			++rowItr.y;
			startPos.x -= tileWidth / 2;
			shifted = false;
		}
	}

	painter->setTransform(baseTransform);
}
#endif