//================================================================================
//
//		Filename:	gameapp.cpp
//		Created:	07/08/12
//		Author:	    Andrew Copland
//
//================================================================================

//#include <SFML/Graphics.hpp>

#include "gameapp.h"

#include "TiledMap.h"

#include "MacrosAndWrappers.h"

#include <algorithm>
#include <vector>
#include <string>
#include "SDL.h"

////////////////////////////////////////////////////////////
/*
 * A function to list all contents of a given directory
 * author: Danny Battison
 * contact: gabehabe@hotmail.com
 */
#include "dirent.h" // directory header
#include "alphanum.hpp"

#undef max
#undef min

void getTmxFilenames(const char *path, VecOfStr& filenames)
{
	// remove all current entries
	filenames.clear();

	// first off, we need to create a pointer to a directory
	DIR *pdir = nullptr; // remember, it's good practice to initialise a pointer to nullptr!
	pdir = opendir (path); // "." will refer to the current directory
	struct dirent *pent = nullptr;
	if (pdir == nullptr) // if pdir wasn't initialised correctly
	{ // print an error message and exit the program
		printf ("\nERROR! pdir could not be initialised correctly");
		return; // exit the function
	} // end if

	while (pent = readdir (pdir)) // while there is still something in the directory to list
	{
		if (pent == nullptr) // if pent has not been initialised correctly
		{ // print an error message, and exit the program
			printf ("\nERROR! pent could not be initialised correctly");
			return; // exit the function
		}
		// otherwise, it was initialised correctly. let's print it on the console:
		if (!S_ISDIR(pent->d_type)) {
			printf ("%s\n", pent->d_name);
			if (nullptr != strstr(pent->d_name, ".tmx")) {
				filenames.push_back(pent->d_name);
			}
		}
	}

	// sort them into some order more familiar to regular human beings!
	std::sort(filenames.begin(), filenames.end(), doj::alphanum_less<std::string>());

	// finally, let's close the directory
	closedir (pdir);
}

////////////////////////////////////////////////////////////
// static constants
static const char sMapsDir[] = { "./data/maps/" };

////////////////////////////////////////////////////////////
//
CGameApp::CGameApp(SDL_Window *window) :
	mTiledMap(nullptr),
	mRenderWindow(window),
	miCurrentMap(0),
	miSingleLayerN(-1),
	mouseX(0),
	mouseY(0),
	bMouseDown(false)
{
	// get all of the file systems
	getTmxFilenames(sMapsDir, mMapFilenames);
	size_t iCurrentMap = 0;
	iCurrentMap = std::max(std::min(iCurrentMap, mMapFilenames.size() - 1), 0U);
	if (iCurrentMap < mMapFilenames.size())
	{
		int width, height;
		SDL_GetWindowSize(mRenderWindow, &width, &height);
		std::string mapName = mMapFilenames[iCurrentMap];
		TiledMap *pMap = new TiledMap(sMapsDir, mapName.c_str(), width, height);
		if (!pMap->doesMapHaveErrors() && pMap->getNumLayers() > 0) {
			mTiledMap = pMap;
		}
		else {
			delete pMap;
		}
	}
}

CGameApp::~CGameApp()
{
	SAFE_DELETE(mTiledMap);
	mMapFilenames.clear();
}

void CGameApp::handleEvent(SDL_Event& EventIn)
{
	if (EventIn.type == SDL_KEYDOWN)
	{
		switch (EventIn.key.keysym.sym)
		{
		case SDLK_KP_PLUS:
			miSingleLayerN++;
			break;
		case SDLK_KP_MINUS:
			miSingleLayerN--;
			break;
		}

		if (mTiledMap) {
			const int minval = -1;
			const int maxval = mTiledMap->getNumLayers();
			miSingleLayerN = std::max(std::min(miSingleLayerN, maxval), minval);
		}
	}

	// Resize event : adjust viewport
	if (EventIn.type == SDL_WINDOWEVENT)
	{
		// Setup a perspective projection
		/*glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		float aspect = (float)EventIn.Size.Width / (float)EventIn.Size.Height;
		gluPerspective(90.f, aspect, 1.f, 500.f);
		glViewport(0, 0, EventIn.Size.Width, EventIn.Size.Height);*/
	}

	if (EventIn.type == SDL_KEYUP)
	{
		if (EventIn.key.keysym.sym == SDLK_F1)
		{
			//sf::Image Screen = mRenderWindow.capture();
			//Screen.saveToFile("screenshot.jpg");
		}
	}

	//if (EventIn.type == sf::Event::MouseButtonPressed)
	//{
	//	bMouseBtnDown[EventIn.mouseButton.button] = true;
	//	bMouseDown = true;
	//	mouseX = EventIn.mouseButton.x;
	//	mouseY = EventIn.mouseButton.y;
	//}
	//else if (EventIn.type == sf::Event::MouseButtonReleased)
	//{
	//	bMouseBtnDown[EventIn.mouseButton.button] = false;
	//	bMouseDown = false;
	//}
	//else if (EventIn.type == sf::Event::MouseMoved)
	//{
	//	mouseX = EventIn.mouseMove.x;
	//	mouseY = EventIn.mouseMove.y;
	//}
}

void CGameApp::update()
{
	//
	if (mTiledMap) {
		mTiledMap->Update(bMouseDown, mouseX, mouseY);
	}
}

void CGameApp::render()
{
	//
	if (mTiledMap) {
		mTiledMap->Render(mRenderWindow, miSingleLayerN);
	}
}

void CGameApp::changeToNewMap()
{
	// limit to safe values
	miCurrentMap = std::max(std::min(miCurrentMap, mMapFilenames.size() - 1), 0U);
	// sanity check
	if (miCurrentMap < mMapFilenames.size())
	{
		// load the new map
		int width, height;
		SDL_GetWindowSize(mRenderWindow, &width, &height);
		std::string mapName = mMapFilenames[miCurrentMap];
		TiledMap *pMap = new TiledMap(sMapsDir, mapName.c_str(), width, height);
		if (!pMap->doesMapHaveErrors() && pMap->getNumLayers() > 0) {
			delete mTiledMap;
			mTiledMap = nullptr;
			mTiledMap = pMap;
		}
		else {
			delete pMap;
		}
	}
}

void CGameApp::nextMap()
{
	miCurrentMap++;
	changeToNewMap();
}

void CGameApp::prevMap()
{
	miCurrentMap--;
	changeToNewMap();
}

const std::string& CGameApp::getMapName() const
{
	return mTiledMap->getMapFilename();
}