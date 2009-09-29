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

#include <fstream>

#include <SDL/SDL_image.h>
#include <SDL/SDL_rotozoom.h>
#include <SDL/SDL_gfxPrimitives.h>

#include "main.h"
#include "gui.h"
#include "util.h"


CGui::CGui()
{
	showMenu = true;
	clickAnimation = 0;
	
	/* Load graphics */
	wikiLogo = wikiLogoOrig = IMG_Load ( "data/gfx/wikipedia-globe-half.png" );
	bar = IMG_Load ( "data/gfx/bar.png" );
	key = IMG_Load ( "data/gfx/key.png" );
	keyDel = IMG_Load ( "data/gfx/keyDel.png" );
	keyExtra = IMG_Load ( "data/gfx/keyExtra.png" );
	keySpace = IMG_Load ( "data/gfx/keySpace.png" );
	keyCaps = IMG_Load ( "data/gfx/keyCaps.png" );
	result = IMG_Load ( "data/gfx/result.png" );
	
	loading = new SDL_Surface[12];
	for ( int i = 1; i <= 12; ++i ) {
		std::string str = "data/gfx/loading" + util::lCast<std::string>( i ) + ".png";
		loading[i-1] = *IMG_Load ( str.c_str() );
		
		if ( !&loading[i-1] ) {
			std::cerr << "ERROR: Can't load graphics from folder \"./data/gfx/\"" << std::endl;
			GetWizipedia()->Quit();
		}
	}
	
	/* Test if all loaded correct */
	if ( !wikiLogo || !bar || !key || !keyDel || !keyExtra || !keySpace || !keyCaps || !result || !loading ) {
		std::cerr << "ERROR: Can't load graphics from folder \"./data/gfx/\"" << std::endl;
		GetWizipedia()->Quit();
	}

	
	/* Load keyboardlayout */
	this->LoadKeyboard();
	
	/* Set graphic-locations */
	wikiLogoLocation.x = 293; wikiLogoLocation.y = 0;
	wikiLogoLocation.w = 27;  wikiLogoLocation.h = 28;
	wikiLogoZoom = 1.f;
	
	/* Inputbar */
	barLocation.x = -280; barLocation.y = 3;
	barLocation.w = 280; barLocation.h = 30;
	cursorPosition = 0;
	barRenderText = NULL;
	
	/* Resultbar */
	resultLocation.y = 31; resultLocation.x = 30;
	resultLocation.h = 232; resultLocation.w = 96;
	newSearch = false;
	selectedResult = 0;
}


CGui::~CGui()
{
	SDL_FreeSurface ( wikiLogo );
}


void CGui::LoadKeyboard()
{
	drawExtraKey = 0;
	
	keyboardLocation.x = 0; keyboardLocation.y = 240;
	keyPositions = new std::vector < std::string >[10];
	
	std::string filename = "data/keyboard/" + GetWizipedia()->GetLang();
	std::ifstream file ( filename.c_str(), std::ios::in );
	
	if ( !file ) {
		GetWizipedia()->Quit();
		return;
	}
	
	std::string fileLine, word;
	std::stringstream streamLine, emptyStream();
	for ( int i = 0; std::getline ( file, fileLine ); ++i ) {
		streamLine.clear(); streamLine.str("");
		streamLine << fileLine;

		for ( int j = 0; j < 10 && !streamLine.str().empty(); ++j ) {
			word.clear(); streamLine >> word;

			if ( !word.empty() )
				keyPositions[i].push_back ( word );
		}
	}
}


bool CGui::Calc()
{
	if ( showMenu )	{
		/* Fade in keyboard */
		if ( keyboardLocation.y > 94 ) {
			keyboardLocation.y -= 500 * GetWizipedia()->GetFrameTime();
			if ( keyboardLocation.y < 94 ) /* Keyboard was set to high... */
				keyboardLocation.y = 94;
		}
		
		/* Fade in inputbar */
		if ( barLocation.x <= 30 ) {
			barLocation.x += 1050 * GetWizipedia()->GetFrameTime();
			if ( barLocation.x > 30 ) /* Moving to fast */
				barLocation.x = 30;
		}
		
		/* Fade int result */
		if ( resultLocation.y < 30 ) {
			resultLocation.y +=  400 * GetWizipedia()->GetFrameTime();
			if ( resultLocation.y > 30 ) /* Moving to fast */
				resultLocation.y = 30;
		}
		
	/* Render animation to fade-out menu */
	} else {
		/* Fade out keyboard */
		if ( keyboardLocation.y < 250 ) {
			keyboardLocation.y += 500 * GetWizipedia()->GetFrameTime();
		} else {
			cursorPosition = 0;
			barText = "";
			AddChar( 'D' );
		}
		
		/* Fade out inputbar */
		if ( barLocation.x > -280 ) {
			barLocation.x -= 1050 * GetWizipedia()->GetFrameTime();
		}
		
		/* Fade out result */
		if ( resultLocation.y > -100 )
			resultLocation.y -= 400 * GetWizipedia()->GetFrameTime();
	}
	
	return true;
}


bool CGui::Draw()
{
	/* Temporaly data */
	static int x, y;
	SDL_GetMouseState ( &x, &y );
	
	/* Render graphics */
	SDL_BlitSurface ( wikiLogo, NULL, GetWizipedia()->GetScreen(), &wikiLogoLocation );
	
	/* Draw the keyboard */
	this->DrawKeyboard();
	
	/* Draw searchresults */
	this->DrawResults();
	
	/* Draw inputbar */
	this->DrawInputbar();
	

	/* Mouseclick animation */
	if ( clickAnimation ) {
		if ( clicked )
			clickAnimation += 60 * GetWizipedia()->GetFrameTime();
		
		for ( int i = 0; i<3; ++i ) {
			aacircleRGBA ( GetWizipedia()->GetScreen(), x, y, clickAnimation+i, 100, 200, 100, 255 - (clickAnimation*5) );
		}
		
		if ( clickAnimation > 8 ) {
			clickAnimation = 0;
		}
	}
	
	return true;
}


void CGui::DrawKeyboard()
{
	if ( keyboardLocation.x > 320 || keyboardLocation.y > 240 )
		return;
	

	static SDL_Color color = { 220, 220, 220 };
	static SDL_Rect rect, rectStr;
	
	for ( int i = drawExtraKey; i < (4+drawExtraKey); ++i ) {
		rect.x = keyPositions[i].size() % 2 ? 16 : 0;

		for ( int j = 0; j < keyPositions[i].size(); ++j ) {
			rect.y = 36 * (i-drawExtraKey) + keyboardLocation.y;
			
			static SDL_Rect randRect;
			randRect.x = randRect.y = 0;
			if ( keyPressed == keyPositions[i][j].c_str()[0] ) {
				static int nr = 0;
				
				randRect.x = util::random ( -2, 2 ); randRect.y = util::random ( -2, 2 );
				rect.x += randRect.x;
				rect.y += randRect.y;
				
				if ( ++nr > 400 * GetWizipedia()->GetFrameTime() ) {
					keyPressed = -1;
					nr = 0;
				}
			}

			/* Draw key-background */
			switch ( keyPositions[i][j].c_str()[0] ) {
				case '$':
					break;
				case 'D':
					SDL_BlitSurface ( keyDel, NULL, GetWizipedia()->GetScreen(), &rect );
					break;
				case 'E':
					SDL_BlitSurface ( keyExtra, NULL, GetWizipedia()->GetScreen(), &rect );
					break;	
				case 'S':
					SDL_BlitSurface ( keySpace, NULL, GetWizipedia()->GetScreen(), &rect );
					break;
				case 'C':
					SDL_BlitSurface ( keyCaps, NULL, GetWizipedia()->GetScreen(), &rect );
					break;
				default:
					SDL_BlitSurface ( key, NULL, GetWizipedia()->GetScreen(), &rect );
					
					/* Draw text */
					if ( !keyPositions[i][j].empty() ) {
						SDL_Surface* keyText = TTF_RenderText_Blended( GetWizipedia()->GetKeyFont(), keyPositions[i][j].c_str(), color );

					rectStr = rect; rectStr.x += ( 15 - ( keyText->clip_rect.w ) / 2 ); rect.y += 4;
					SDL_BlitSurface ( keyText, NULL, GetWizipedia()->GetScreen(), &rectStr );
					SDL_FreeSurface ( keyText );

					}
					break;
			}

			rect.x += 32;
			rect.x -= randRect.x;
			rect.y -= randRect.y;
		}
	}
}


SDL_Rect CGui::CalcCursorPosition()
{
	/* Static variables */
	static SDL_Color color = { 0, 0, 0 };
	static SDL_Rect rect;
	
	/* Cursor position in px */
	int x = barLocation.x + 8;
	if ( barRenderText ) {
		if ( cursorPosition == barText.length() ) {
			x += barRenderText->w;
		} else {
			/* Teilausschnitt bis zum Cursor berechnen */
			std::string substring = barText.substr ( 0, cursorPosition );
			
			SDL_Surface* barRenderTextTmp = TTF_RenderText_Blended( GetWizipedia()->GetBarFont(), substring.c_str(), color );
			if ( barRenderTextTmp )
				x += barRenderTextTmp->w;
			SDL_FreeSurface ( barRenderTextTmp );
		}
	}
	
	rect.x = x;
	rect.y = barLocation.y + 8;
	rect.h = barLocation.y + 22;
	
	return rect;	
}


void CGui::DrawInputbar()
{
	/* Only draw when inside window */
	if ( barLocation.x < -270 )
		return;
	
	/* Static variables */
	static SDL_Rect rect;
	static SDL_Rect cursorRect;
	static int nr = 0;
	
	/* Draw background */
	static SDL_Rect barLocationTmp; barLocationTmp = barLocation;
	SDL_BlitSurface ( bar, NULL, GetWizipedia()->GetScreen(), &barLocationTmp );
	
	/* Draw loading animation */
	if ( newSearch ) {
		static float angle = 0;		
		angle += 10 * GetWizipedia()->GetFrameTime();
		if ( (int)angle > 11 )
			angle = 0;
		
		rect = barLocationTmp;
		rect.x += 210; rect.y += 8;
		SDL_BlitSurface ( &loading[(int)angle], NULL, GetWizipedia()->GetScreen(), &rect );
	}

	/* Draw Text */
	if ( barRenderText ) {
		rect.x = barLocation.x + 8; rect.y = barLocation.y + 7;
		SDL_BlitSurface ( barRenderText, NULL, GetWizipedia()->GetScreen(), &rect );
	}
	

	/* Draw cursor */
	if ( ++nr > 2000 * GetWizipedia()->GetFrameTime() ) {
		cursorRect = CalcCursorPosition();
		vlineRGBA ( GetWizipedia()->GetScreen(), cursorRect.x, cursorRect.y , cursorRect.h, 0, 0, 0, 255 );
		if ( nr > 6000 * GetWizipedia()->GetFrameTime() ) 
			nr = 0;
	}
}


void CGui::DrawResults()
{
	/* Temporaly data */
	static int x, y;
	int pos; std::string substring;
	static SDL_Color color = { 0, 0, 0 };
	static SDL_Rect rect; rect.x = resultLocation.x + 8;
	
	/* Draw search results */
	if ( !searchResults.empty() ) {
		/* background */
		static SDL_Rect resultLocationTmp; resultLocationTmp = resultLocation;
		SDL_BlitSurface ( result, NULL, GetWizipedia()->GetScreen(), &resultLocationTmp );
		
		if ( searchResultsSize ) {
			
			/* results */
			resultLocationTmp.y += resultLocation.y + 2;
			std::vector < std::string >::iterator resultCurrent = searchResults.begin();
			for ( int i = searchResultsSize; resultCurrent != searchResults.end(); ++resultCurrent, --i )
			{
				/* marked */
				if ( i == selectedResult ) {
					boxRGBA ( GetWizipedia()->GetScreen(), resultLocationTmp.x+3, rect.y+2, resultLocationTmp.x + resultLocationTmp.w - 5, rect.y + rect.h - 3, 0, 0, 150, 80);
				}
				
				pos = (*resultCurrent).find(":");
				substring = (*resultCurrent).substr ( pos+1 );
				
				SDL_Surface* text = TTF_RenderText_Blended( GetWizipedia()->GetBarFont(), substring.c_str(), color );
				SDL_BlitSurface ( text, NULL, GetWizipedia()->GetScreen(), &rect );
				SDL_FreeSurface ( text );
				
				rect.y = resultLocation.y + 2 + (i-1)*12;
			}
		}
		
	}
}


void CGui::AddChar ( char char_ )
{
	static std::string substring, substring2;
	
	/* Set new string */
	switch ( char_ ) {
		case 'D': /* Delete */
			if ( cursorPosition ) {
				substring = barText.substr ( 0, cursorPosition-1 );
				substring2 = barText.substr ( cursorPosition );
				barText = substring + substring2;
			
				this->MoveCursorLeft();
			} else {
				searchResults.clear();
			}
			--cursorPosition;
			break;
		case 'E':
			drawExtraKey = 4;
			return;
		case 'Q':
			drawExtraKey = 0;
			return;
		case 'S':
			char_ = ' ';
		default:
			substring = barText.substr ( 0, cursorPosition );
			substring2 = barText.substr ( cursorPosition );
			barText = substring + char_ + substring2;
			break;
	
	}
	
	/* Move Cursor */
	++cursorPosition;

	/* Perform a search in the database */
	static SDL_Thread* searchThread = NULL; static int status;
	if ( !barText.empty() ) {
		
		SDL_WaitThread ( searchThread, &status );
		// SDL_KillThread ( searchThread );
		
		newSearch = true;
		searchThread = SDL_CreateThread ( performSearch, &barText ); 
	}
	
	/* Static variables */
	static SDL_Color color = { 0, 0, 0 };
	
	/* Calc Render Text */
	SDL_FreeSurface ( barRenderText );
	barRenderText = TTF_RenderText_Blended( GetWizipedia()->GetBarFont(), barText.c_str(), color );
	
	/* Text too long? */
	if ( barRenderText->w > 220 )
	{
		
	}

}


void CGui::MoveCursorRight()
{
	if ( ++cursorPosition > barText.length() )
		cursorPosition = barText.length();
}


void CGui::MoveCursorLeft()
{
	if ( --cursorPosition < 0 )
		cursorPosition = 0;
}


bool CGui::MouseClick ( bool clicked_ )
{
	clickAnimation = 1;
	clicked = clicked_;
	
	if ( clicked ) {
		static int x, y;
		SDL_GetMouseState ( &x, &y );
		SDL_Rect mouseLoc = { x-1, y-1, 2, 2 };
		
		/* Click on Wikipedia-Logo */
		if ( util::collideBox ( wikiLogoLocation, mouseLoc ) ) {
			GetWizipedia()->GetGui()->ToggleShowMenu();
			return true;
		}
		/* ... on Inputbar */
		if ( util::collideBox ( barLocation, mouseLoc ) ) {
			return true;
		}
		
		
		/* ... on Searchresults */
		if ( !searchResults.empty() && util::collideBox ( resultLocation, mouseLoc ) ) {
			selectedResult = ( y-resultLocation.y-3 ) / 12;
			if ( !selectedResult )
				selectedResult = searchResultsSize;
			
			this->SetShowMenu ( false );
			GetWizipedia()->GetRender()->Lockup ( GetWizipedia()->GetGui()->GetSelectedFile(), GetWizipedia()->GetGui()->GetSelected() );
			GetWizipedia()->GetRender()->Render();
			
			return true;
		}
		
		/*  ... on the OSD-Keyboard */
		static SDL_Rect rect; rect.x = 0; rect.h = 36;
		for ( int i = drawExtraKey; i < (4+drawExtraKey); ++i ) {
			
			rect.x = keyPositions[i].size() % 2 ? 16 : 0;
			for ( int j = 0; j < keyPositions[i].size(); ++j ) {
				rect.y = 36 * (i-drawExtraKey) + keyboardLocation.y;
				
				switch ( keyPositions[i][j].at(0) ) {
					case 'D':
						rect.w = 64;
						break;
					case 'S':
						rect.w = 64;
						break;
					case 'l':
						rect.w = 48;
						break;
					default:
						rect.w = 32;
				}
				
				if ( util::collideBox ( rect, mouseLoc ) && i != 10 ) {
					keyPressed = keyPositions[i][j].c_str()[0];
					
					/* Add character to inputbar */
					this->AddChar ( keyPositions[i][j].c_str()[0] );
					
					return true;
				}
				
				rect.x += 32;

			}
			

		}
		
	}
	
	return false;
}


void CGui::GoDown()
{
	if ( ++selectedResult > searchResultsSize )
		selectedResult = 1;
}


void CGui::GoUp()
{
	if ( --selectedResult <= 0 )
		selectedResult = searchResultsSize;
}


void CGui::SetSearchResults ( std::vector < std::string > searchResults_, int *size_ )
{
	newSearch = false;
	searchResults = searchResults_;
	searchResultsSize = selectedResult = *size_;
}


std::string CGui::GetSelected()
{
	std::vector < std::string >::iterator resultCurrent = searchResults.begin();
	for ( int i = searchResultsSize; resultCurrent != searchResults.end(); ++resultCurrent, --i )
	{
		if ( i == selectedResult ) {
			return (*resultCurrent).substr ( (*resultCurrent).find(":") + 1 );
		}
	}
	return "";
}


std::string CGui::GetSelectedFile()
{
	std::vector < std::string >::iterator resultCurrent = searchResults.begin();
	for ( int i = searchResultsSize; resultCurrent != searchResults.end(); ++resultCurrent, --i )
	{
		if ( i == selectedResult ) {
			return (*resultCurrent).substr ( 0, (*resultCurrent).find(":") );
		}
	}
	return "";
}


bool CGui::IsMenuShow()
{
	return showMenu;
}
