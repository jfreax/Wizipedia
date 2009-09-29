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

#ifndef RENDER_H
#define RENDER_H

#include <string>
#include <vector>
#include <map>
#include <SDL/SDL_main.h>

class SDL_Surface;
class SDL_Rect;

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
		
	public:
		CRender();
		
		bool Lockup ( std::string filename_, std::string title_ );
		
		bool Render();
		void Clear();
		
		bool Calc();
		bool Draw();
		
		bool ChangePosition ( double offset_ );
};

#endif // RENDER_H
