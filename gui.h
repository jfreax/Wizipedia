/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) <year>  <name of author>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef GUI_H
#define GUI_H

// #include <SDL/SDL_video.h>
#include <vector>
#include <map>

class SDL_Surface;
class SDL_Rect;

class CGui
{
	private:
		bool showMenu;
		
		float clickAnimation;
		bool clicked;
		
		SDL_Surface* wikiLogo;
		SDL_Surface* wikiLogoOrig;
		SDL_Rect wikiLogoLocation;
		bool wikiLogoMark;
		double wikiLogoZoom;

		int drawExtraKey;
		SDL_Surface* key;
		SDL_Surface* keyDel;
		SDL_Surface* keyExtra;
		SDL_Surface* keySpace;
		SDL_Surface* keyCaps;
		SDL_Surface* keyText;
		SDL_Rect keyboardLocation;
		std::vector < std::string >* keyPositions;
		char keyPressed;
		
		SDL_Surface* bar;
		SDL_Rect barLocation;
		
		std::string barText;
		SDL_Surface* barRenderText;
		int cursorPosition;
		
		SDL_Surface* loading;
		bool newSearch;
		SDL_Surface* result;
		SDL_Rect resultLocation;
		std::vector < std::string > searchResults;
		int searchResultsSize;
		
		int selectedResult;
		
	private:
		void DrawKeyboard();
		void DrawInputbar();
		void DrawResults();
		
		SDL_Rect CalcCursorPosition();
		
	public:
		CGui();
		~CGui();
		
		void LoadKeyboard();
		
		bool Calc();
		bool Draw();
		
		void AddChar ( char char_ );
		void MoveCursorRight();
		void MoveCursorLeft();
		
		bool MouseClick ( bool clicked_ = false );
		void GoDown();
		void GoUp();
		
		void SetSearchResults ( std::vector < std::string > searchResults_, int *size_ );
		std::string GetSelected();
		std::string GetSelectedFile();
	
		class SDL_Rect GetWikiLogoLocation() { return wikiLogoLocation; }
		class SDL_Rect GetBarLocation() { return barLocation; }
		
		bool IsMenuShow();
		void MarkWikiLogo ( bool mark_ ) { wikiLogoMark = mark_; }
		void SetShowMenu ( bool show_ ) { showMenu = show_; }
		void ToggleShowMenu() { showMenu = !showMenu; }
		
};

#endif // GUI_H
