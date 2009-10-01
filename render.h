/*
Copyright (C) Copyright (C) 2009 Jens Dieskau

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

#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <vector>
#include <map>
#include <SDL/SDL_main.h>

class SDL_Surface;
class SDL_Rect;


enum FORMAT
{
	NONE,
	
	BOLD,
	ITALIC,
	
	HEADER1,
	HEADER2,
	HEADER3
};


class CRender
{
	private:
		std::string title;
		
		char* rawData;
		std::string data;
		int length;
		
		std::string text;
		
		int position;
		int maxPosition;
		double postionOffset;
		std::map < SDL_Surface*, SDL_Rect > renderText;
		std::map < SDL_Surface*, SDL_Rect > renderLinks;
		std::vector < SDL_Rect > lines;
		std::vector < std::string > links;
		
		std::vector < SDL_Surface* > index; /* int = the level in the index */
		
		std::string wordToRender, lastWordToRender, wordToIndex;
		
		TTF_Font* renderFont;
		FORMAT format;
		std::string formatWord;
		
		int indexWidth;
		
	public:
		CRender();
		bool Lockup ( std::string filename_, std::string title_ );
		
		bool Render();
		bool Format ( std::string* word_, std::string find_, FORMAT format_ );
		
		void Clear();
		
		bool Calc();
		bool Draw();
		
		bool ChangePosition ( double offset_ );
};

#endif // RENDER_H
