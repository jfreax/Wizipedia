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

#ifndef UTIL_H
#define UTIL_H

#include <sstream>
#include <vector>

namespace util
{

template<typename To, typename From>
To lCast ( From a, To def = To() )
{
	To res;
	std::stringstream str;
	
	if ( ! ( str << a && str >> res ) )
	{
		return def;
	}
	else
	{
		return res;
	}
}

int random ( int a );
int random ( int a, int b );
int collideBox ( SDL_Rect a , SDL_Rect b );

/* Is there a text between two search strings? */
int isTextWildcard ( std::string* text, std::string searchBegin, std::string searchEnd, bool replace = false );

/* Search for text between two strings */
bool findTextBetweenSearch ( std::string* text, std::string searchBegin, std::string searchEnd );

/* Replace a string with a new one */
int replace ( std::string* text, std::string searchStr, std::string replaceStr = "" );

/* Replace a strint from searchBegin to searchEnd with a new text and return the text between this */
std::vector < std::string > replaceWildcard ( std::string* text, std::string searchBegin, std::string searchEnd, std::string replaceStr = "" );

} // namespace util

#endif // UTIL_H
