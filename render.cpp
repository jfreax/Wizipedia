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

#include <iostream>
#include <bzlib.h>

#include <SDL/SDL_ttf.h>

#include "main.h"
#include "render.h"
#include <SDL/SDL_gfxPrimitives.h>
#include "util.h"



CRender::CRender()
{
	rawData = new char[0x100000];
	rawData2 = new Uint16[0x100000];
	
	position = 50;
	postionOffset = 0.0f;
}


bool CRender::Lockup ( std::string filename_, std::string title_ )
{
	title = title_; 
	
	/* Delete old content */
	position = 0;
	postionOffset = 0;
	data.clear();
	text.clear();
	this->Clear();
	
	BZFILE * bFile = NULL;
	std::string filename = "wikipedia/" + filename_;
	
	/* Open file */
	bFile = BZ2_bzopen ( filename.c_str(), "rb" );
	if ( !bFile )
	{
		std::cout << "Error while opening file \"" + filename_ + "\"" << std::endl;
		return false;
	}
	
	/* read uncrompressed data */
// 	length = BZ2_bzread ( bFile, rawData2, 0x100000 );
	length = BZ2_bzread ( bFile, rawData, 0x100000 );
	data = rawData;
	
	/* close it */
	BZ2_bzclose ( bFile );
	
	SDL_Color color = { 100, 100, 100 };
	SDL_Rect rect; rect.x = 0; rect.y = 0;
// TTF_RenderUNICODE_Blended ( GetWizipedia()->GetHeaderFont(),rawData2, color );
// 	SDL_BlitSurface ( TTF_RenderUNICODE_Blended ( GetWizipedia()->GetHeaderFont(),rawData2, color ), NULL, GetWizipedia()->GetScreen(), &rect );
	
// 	std::cout << "HIER!!!" << std::endl;
	
	/* find article */
	int pageBegin = data.find ( "<title>" + title_ + "</title>" );
	data = data.substr( pageBegin );
	int pageEnd = data.find ( "</page>" );
	data = data.substr( 0, pageEnd );
	
	/* find the text */
	int textBegin = data.find ( "<text" );
	text = data.substr( textBegin );
	int textEnd = text.find ( "</text>" ); /* TODO open next file, if end is not here! */
	text = text.substr( 0, textEnd );
	
	/* delete <text> */
	text = text.substr ( text.find ( "<text xml:space=\"preserve\">" )+27 );
	
	/* Delete unneeded 
	   --------------- */
	util::replaceWildcard ( &text, "[[Datei:", "]]" );

	/* Add needed stuff
	/* ---------------- */
	util::replace ( &text, "\n", " \n " );
	util::replace ( &text, "\t", " \n " );
	util::replace ( &text, "== ", "==" ); util::replace ( &text, " ==", "==" );
	
	/* */
	std::string str;
	str = (char)0xc3; str += (char)0x84;
	util::replace ( &text, str, "�" );
	str = (char)0xc3; str += (char)0xb6;
	util::replace ( &text, str, "�" );
	str = (char)0xc3; str += (char)0xbc;
	util::replace ( &text, str, "�" );
	str = (char)0xc3; str += (char)0x9f;
	util::replace ( &text, str, "�" );

	
	return true;
}


bool CRender::Render()
{
	this->Clear();
	
	if ( data.empty() )
		return false;
	
	TTF_Font* font = GetWizipedia()->GetDefaultFont();
	SDL_Color color = { 255, 255, 255 };
	SDL_Rect rect; rect.x = 1; rect.y = 3;

// 	std::wstring test = L"LALA";
	/* Title */
// 	TTF_RenderUNICODE_Blended ( 
	SDL_Surface* currentWord = TTF_RenderText_Blended ( GetWizipedia()->GetHeaderFont(), title.c_str(), color );
// 	SDL_Surface* currentWord = /*TTF_RenderUNICODE_Blended ( GetWizipedia()->GetHeaderFont(), test.c_str(), color );*/
	SDL_Surface* lastWord = NULL;
	renderText.insert ( std::make_pair < SDL_Surface*, SDL_Rect > ( currentWord, rect ) );
	
	/* Add line under title */
	rect.x = 0; rect.y += currentWord->h; rect.w = 320;
	lines.push_back ( rect );
	rect.y += 3;
	
	/* Find the next word -> render it -> save the rendered image */
	bool addLine = false; int addToIndex = 0;
	int lineBreak = 0, hightest = 0;
	std::string wordToRender;
	int i = 0, oldi = 0, lb = 0;
	while ( i != -1 ) {
		oldi = i;
		
		/* Search for a space */
		i = text.find ( " ", i+1 );
		
		/* Grep the current Word */
		wordToRender = text.substr( oldi, i-oldi );
		
		/* Format the text
		   ----------------- */
		/* at first, find all LFs */
		lineBreak += util::replace ( &wordToRender, "\n" );
		
		/* ''' Bold ''' */
		if ( util::isTextWildcard ( &wordToRender, "\'\'\'", "\'\'\'", true ) ) {
			font = GetWizipedia()->GetDefaultFontBold();
		/* '' Oblique '' */
		} else if ( util::isTextWildcard ( &wordToRender, "\'\'", "\'\'", true ) ) {
			font = GetWizipedia()->GetDefaultFontOblique();
		/* '' Header 2 '' */	
		} else if ( util::isTextWildcard ( &wordToRender, "==", "==", true ) ) {
			font = GetWizipedia()->GetHeader2Font();
			addLine = true;
			addToIndex = 1;
		} else {
			font = GetWizipedia()->GetDefaultFont();
		}
		
		/* Render one word
		   --------------- */
		if ( currentWord )
			lastWord = currentWord;
		currentWord = TTF_RenderText_Blended ( font, wordToRender.c_str(), color );
		
		/* Rendering ok? */
		if ( currentWord ) {
			
			/* Hightes word in this line */
			if ( currentWord->h > hightest ) {
				hightest = currentWord->h;
			}
			
			
			/* Add a horizontal line */
			if ( addLine ) {
				static SDL_Rect lineRect;
				addLine = false;
				
				lineRect = rect;
				lineRect.y = rect.y + currentWord->h; lineRect.x = 0; lineRect.w = 320;
				lines.push_back ( lineRect );
			}
			
			/* Add a new entry to index */
			if ( addToIndex ) {
				rect.h = addToIndex;
				addToIndex = 0;
				index.insert ( std::make_pair < SDL_Surface*, SDL_Rect > ( currentWord, rect ) );
			}
			
			/* End of a line? */
			if ( rect.x + currentWord->w > 320 ) {
				rect.y += lastWord->h + 1;
				rect.x = 1;
			}
			
			/* Save the rendered image 
			   ----------------------- */
			renderText.insert ( std::make_pair < SDL_Surface*, SDL_Rect > ( currentWord, rect ) );
			
			/* Move "cursor" to next position */
			rect.x += currentWord->w;
			
			/* A line break is needed */
			while ( lineBreak > 0 ) {
				rect.y += lastWord->h + 1;
				rect.x = 1;
				
				--lineBreak;
			}
		}
		
		
		lineBreak = 0;
	}
	
	return true;
}


void CRender::Clear()
{
	std::map < SDL_Surface*, SDL_Rect >::iterator textCurrent = renderText.begin();
	for ( ; textCurrent != renderText.end(); ++textCurrent )
	{
		SDL_FreeSurface ( (*textCurrent).first );		
	}
	
	renderText.clear();
	lines.clear();
}


bool CRender::Calc()
{

	
	if ( postionOffset ) {
		if ( postionOffset > 0.1f )
			this->ChangePosition ( postionOffset - ( GetWizipedia()->GetFrameTime() * 8 ) );
		if ( postionOffset < -0.1f )
			this->ChangePosition ( postionOffset + ( GetWizipedia()->GetFrameTime() * 8 ) );
		
	}
	
}


bool CRender::Draw()
{
	/* Temporaly data */
	static SDL_Rect rect;
	static int positionWIndex;
	positionWIndex = position - index.size() * ( 2 + TTF_FontHeight ( GetWizipedia()->GetDefaultFont() ) );
	
	/* Draw index */
	boxRGBA ( GetWizipedia()->GetScreen(), 10, 10 - position, 200, -positionWIndex, 50, 50, 50, 255 );
	std::map < SDL_Surface*, SDL_Rect >::iterator indexCurrent = index.begin();
	for ( ; indexCurrent != index.end(); ++indexCurrent )
	{
		rect = (*indexCurrent).second; rect.y -= position;
		
		
// 		SDL_BlitSurface ( (*textCurrent).first, NULL, GetWizipedia()->GetScreen(), &rect );
	}
	

	
	
	/* Draw text */
	std::map < SDL_Surface*, SDL_Rect >::iterator textCurrent = renderText.begin();
	for ( ; textCurrent != renderText.end(); ++textCurrent )
	{
		rect = (*textCurrent).second; rect.y -= positionWIndex;
		SDL_BlitSurface ( (*textCurrent).first, NULL, GetWizipedia()->GetScreen(), &rect );
	}
	
	/* Draw horizontal lines */
	std::vector < SDL_Rect >::iterator linesCurrent = lines.begin();
	for ( ; linesCurrent != lines.end(); ++linesCurrent )
	{
		rect = (*linesCurrent); rect.y -= positionWIndex;
		hlineRGBA ( GetWizipedia()->GetScreen(), rect.x, rect.w, rect.y, 100, 100, 100, 255 );
	}
	
}


bool CRender::ChangePosition ( double offset_ )
{
	position += offset_;
	postionOffset = offset_;
	
	if ( position < 0 )
		position = 0;
	
	if ( postionOffset > -0.1f & postionOffset < 0.1f )
		postionOffset = 0;

}