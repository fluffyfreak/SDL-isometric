//================================================================================
//
//		Filename:	gameapp.h
//		Created:	07/08/12
//		Author:	    Andrew Copland
//
//================================================================================

#ifndef GAMEAPP_H
#define GAMEAPP_H

// includes
#include <vector>
#include <string>
#include <SDL_events.h>

// forward declarations
class CGameApp;
class TiledMap;
struct SDL_Window;
typedef std::vector<std::string> VecOfStr;

////////////////////////////////////////////////////////////
//
class CGameApp
{
private:
	TiledMap*				mTiledMap;
	SDL_Window*				mRenderWindow;
	VecOfStr				mMapFilenames;

	size_t miCurrentMap;
	int miSingleLayerN;

	int mouseX;
	int mouseY;
	bool bMouseBtnDown[3];
	bool bMouseDown;

	void changeToNewMap();

public:
	CGameApp(SDL_Window *);
	~CGameApp();

	void handleEvent(SDL_Event &);
	void update();
	void render();

	void nextMap();
	void prevMap();

	const std::string& getMapName() const;
};

#endif	// GAMEAPP_H
