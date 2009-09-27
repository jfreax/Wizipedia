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

#include <cstdlib>
#include <SDL/SDL.h>

#include <iostream>
#include "util.h"


namespace util
{

int random ( int a )
{
	return -a + ( rand() % ( 2*a + 1 ) );
}


int random ( int a, int b )
{
	if ( ! ( b - a + 1 ) )
		return 0;
	
	return a + ( rand() % ( b - a + 1 ) );
}

int collideBox ( SDL_Rect a , SDL_Rect b )
{
	if(b.x + b.w < a.x)
		return 0;
	if(b.x > a.x + a.w)
		return 0;
			
	if(b.y + b.h < a.y)
		return 0;
	if(b.y > a.y + a.h)
		return 0;	
	return 1;
}


int isTextWildcard ( std::string* text, std::string searchBegin, std::string searchEnd, bool replace )
{
	int results = 0;
	
	int i = text->find ( searchBegin );
	while ( i != std::string::npos ) {
		int k = text->find ( searchEnd, i + searchBegin.length() );
		if ( k != std::string::npos ) {
			++results;
			
			if ( replace )
			{
				util::replace ( text, searchBegin );
				util::replace ( text, searchEnd );
				i = 0;
			} else {
				i = text->find ( searchBegin, i + k + searchEnd.length() );		
			}
		} else {
			i = std::string::npos;
		}
		

	}
	
	return results;
}


bool findTextBetweenSearch ( std::string* text, std::string searchBegin, std::string searchEnd )
{
	bool isFound = false;
	
	int i = text->find ( searchBegin );
	if ( i != std::string::npos ) {
		int k = text->find ( searchEnd, i + searchBegin.length() );
		if ( k != std::string::npos ) {
			isFound = true;
			
			*text = text->substr ( i + searchBegin.length(), k - i - searchBegin.length() );
			std::cout << i << " und " << k << std::endl;
		}
		
	}
	
	return isFound;
}



int replace ( std::string* text, std::string searchStr, std::string replaceStr )
{
	int results = 0;
	
	int i = text->find ( searchStr );
	while ( i != std::string::npos ) {
		++results;
		
		text->replace ( i, searchStr.length(), replaceStr );
		i = text->find ( searchStr, i+replaceStr.length() );
	}

	return results;
}


std::vector < std::string > replaceWildcard ( std::string* text, std::string searchBegin, std::string searchEnd, std::string replaceStr )
{
	int results = 0;
	std::vector < std::string > resultsStr;
	
	int i = text->find ( searchBegin ), j;
	while ( i != std::string::npos ) {
		++results;
		j = text->find ( searchEnd, i+1 ) - i;
		
		resultsStr.push_back ( text->substr ( i+searchBegin.length(), j - searchEnd.length() ) );
		text->replace ( i, j + searchEnd.length(), replaceStr );
	
		i = text->find ( searchBegin, i + replaceStr.length() );
	}
	
	return resultsStr;
}

} // namespace util
