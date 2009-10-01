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
	
	position = 50;
	postionOffset = 0.0f;
	
	format = NONE;
}


bool CRender::Lockup ( std::string filename_, std::string title_ )
{
	if ( filename_.empty() || title_.empty() )
		return false;
	
	title = title_; 
	
	/* Delete old content */
	std::map < SDL_Surface*, SDL_Rect >::iterator renderTextCurrent = renderText.begin();
	for ( ; renderTextCurrent != renderText.end(); ++renderTextCurrent ) {
		SDL_FreeSurface ( ((*renderTextCurrent).first) );
	}
	std::vector < SDL_Surface* >::iterator renderLinksCurrent = renderLinks.begin();
	for ( ; renderLinksCurrent != renderLinks.end(); ++renderLinksCurrent ) {
		SDL_FreeSurface ( (*renderLinksCurrent) );
	}
	
	
	position = 0;
	postionOffset = 0;
	data.clear();
	text.clear();
	renderText.clear();
	renderLinks.clear();
	renderLinksPosition.clear();
	linkName.clear();
	linkURL.clear();
	this->Clear();
	
	BZFILE * bFile = NULL;
	std::string filename = filename_;
	
	/* Open file */
	bFile = BZ2_bzopen ( filename.c_str(), "rb" );
	if ( !bFile )
	{
		std::cerr << "Error while opening file \"" + filename + "\"" << std::endl;
		data = "Error while opening file: \"" + filename + "\"";
		text = data;
		return false;
	}
	
	/* read uncrompressed data */
	length = BZ2_bzread ( bFile, rawData, 0x100000 );
	data = rawData;
	
	/* close it */
	BZ2_bzclose ( bFile );
	
	SDL_Color color = { 100, 100, 100 };
	SDL_Rect rect; rect.x = 0; rect.y = 0;
	
	/* find article */
	int pageBegin = data.find ( "<title>" + title_ + "</title>" );
	if ( pageBegin != std::string::npos )
		data = data.substr( pageBegin );
	
	int pageEnd = data.find ( "</page>" );
	if ( pageEnd != std::string::npos ) /* TODO open next file */
		data = data.substr( 0, pageEnd );
	
	/* find the text */
	int textBegin = data.find ( "<text" );
	if ( textBegin != std::string::npos )
		text = data.substr( textBegin );
	
	int textEnd = text.find ( "</text>" ); /* TODO open next file, if end is not here! */
	if ( textEnd != std::string::npos )
		text = text.substr( 0, textEnd );
	
	/* delete <text> */
	util::replace ( &text, "<text xml:space=\"preserve\">" );
	
	/* Delete unneeded 
	   --------------- */
	util::replaceWildcard ( &text, "[[Datei:", "]]" );
	
	/* */
	std::string str;
	str = (char)0xc3; str += (char)0x84;
	util::replace ( &text, str, "Ä" );
	str = (char)0xc3; str += (char)0xA4;
	util::replace ( &text, str, "ä" );
	str = (char)0xc3; str += (char)0x96;
	util::replace ( &text, str, "Ö" );
	str = (char)0xc3; str += (char)0xb6;
	util::replace ( &text, str, "ö" );
	str = (char)0xc3; str += (char)0x9c;
	util::replace ( &text, str, "Ü" );
	str = (char)0xc3; str += (char)0xbc;
	util::replace ( &text, str, "ü" );
	str = (char)0xc3; str += (char)0x9f;
	util::replace ( &text, str, "ß" );

	/* Add needed stuff
	/* ---------------- */
	util::replace ( &text, "\n", " \n " );
	util::replace ( &text, "\t", " \n " );
// 	util::replace ( &text, "=== ", "===" ); util::replace ( &text, " ===", "===" );
// 	util::replace ( &text, "== ", "==" ); util::replace ( &text, " ==", "==" );
// 	util::replace ( &text, "\'\'\'", " \'\'\' " ); util::replace ( &text, "\'\'", " \'\' " );

	/* find links */
	std::vector < std::string > linksTmp = util::replaceWildcard ( &text, "[[", "]]", " .-hyper|link-. " );
	std::vector < std::string >::iterator currentLinks = linksTmp.begin();
	std::string name, url;
	for ( ; currentLinks != linksTmp.end(); ++currentLinks ) {
		
		int findPipe = (*currentLinks).find ( "|" );
		if ( findPipe != std::string::npos ) {
			name = (*currentLinks).substr ( findPipe + 1 ); 
			url = (*currentLinks).substr ( 0, findPipe ); 
		} else {
			name = url = (*currentLinks);
		}
		
		linkName.push_back( name ); linkURL.push_back( url );
	}
	
	return true;
}


bool CRender::Render()
{
	this->Clear();
	
	if ( data.empty() )
		return false;
	
	renderFont = GetWizipedia()->GetDefaultFont();
	SDL_Color colorDefault = { 255, 255, 255 };
	SDL_Color colorLink = { 70, 70, 220 };
	SDL_Color* color = &colorDefault;
	SDL_Rect rect; rect.x = 1; rect.y = 5;

	/* Title */
	SDL_Surface* currentWord = TTF_RenderText_Blended ( GetWizipedia()->GetHeaderFont(), title.c_str(), *color );
	SDL_Surface* lastWord = NULL;
	renderText.insert ( std::make_pair < SDL_Surface*, SDL_Rect > ( currentWord, rect ) );
	
	/* Add line under title */
	rect.x = 0; rect.y += currentWord->h; rect.w = 320;
	lines.push_back ( rect );
	rect.y += 3; rect.x = 2;
	
	/* Width of a Space-Character */
	int width; TTF_SizeText ( GetWizipedia()->GetDefaultFont(), " ", &width, NULL );
	
	/* Hyperlinklist */
	std::vector < std::string >::iterator currentLinkName = linkName.begin();

	/* Find the next word -> render it -> save the rendered image */
	int addLine = false;
	bool isLink = false;
	int addToIndex = 0, indexOne = 0, indexTwo = 0;
	int lineBreak = 0, hightest = 0;

	int i = 0, oldi = 0, lb = 0;
	while ( i != -1 ) {
		/* Restore defaults */
		color = &colorDefault;
		
		/* Save old data */
		oldi = i;
		lastWordToRender = wordToRender;
		if ( currentWord )
			lastWord = currentWord;
		
		/* Search for a space */
		i = text.find ( " ", i+1 );
		
		/* Grep the current Word */
		wordToRender = text.substr( oldi, i-oldi );
		
		/* Format the text
		   ----------------- */
		/* at first, find all LFs */
		lineBreak += util::replace ( &wordToRender, "\n" );
		
		
		/* Find hyperlinks
		   --------------- */
		if ( util::replace ( &wordToRender, ".-hyper|link-." ) ) {
			wordToRender = (*currentLinkName);
			++currentLinkName;
			
			color = &colorLink;
			isLink = true;
		}
		
		/* Pre-Format */
		this->Format ( &wordToRender, "\'\'\'", BOLD );
		this->Format ( &wordToRender, "\'\'", ITALIC );
		
		if ( this->Format ( &wordToRender, "===", HEADER3 ) ) {
			indexTwo++;
			wordToIndex = util::lCast<std::string>( indexOne ) + "." + util::lCast<std::string>( indexTwo ) + ".";
			util::replace ( &wordToRender, " " );
		} else if ( this->Format ( &wordToRender, "==", HEADER2 ) ) {
			addLine = 1; indexOne++; indexTwo = 0;
			wordToIndex = util::lCast<std::string>( indexOne ) + ".";
			util::replace ( &wordToRender, " " );
		}
		
		/* Render one word
		--------------- */
		currentWord = TTF_RenderText_Blended ( renderFont, wordToRender.c_str(), *color );

		
		/* Rendering ok? */
		if ( currentWord ) {
			
			/* Hightes word in this line */
			if ( currentWord->h > hightest ) {
				hightest = currentWord->h;
			}
			
			
			
			/* End of a line? */
			if ( rect.x + currentWord->w > 320 ) {
				rect.y += lastWord->h + 1;
				rect.x = 1;
			}
			
			/* Save the rendered image 
			   ----------------------- */
			if ( isLink ) {
				renderLinks.push_back ( currentWord );
				renderLinksPosition.push_back ( rect );
// 				renderLinks.insert ( std::make_pair < SDL_Surface*, SDL_Rect > ( currentWord, rect ) );
			} else
				renderText.insert ( std::make_pair < SDL_Surface*, SDL_Rect > ( currentWord, rect ) );
			
			
			/* Move "cursor" to next position */
			rect.x += currentWord->w;
			
			/* A line break is needed */
			while ( lineBreak ) {
				rect.y += lastWord->h + 1;
				rect.x = 1;
				
				--lineBreak;
			}
			
			
			/* Add a horizontal line */
			if ( addLine ) {
				static SDL_Rect lineRect;
				addLine = false;
				
				lineRect = rect;
				lineRect.y = rect.y + currentWord->h-1; lineRect.x = 0; lineRect.w = 320;
				lines.push_back ( lineRect );
			}			
			
			/* Add a new entry to index */
			if ( format == HEADER2 ) {
				addToIndex = 0;
				wordToIndex += wordToRender;
			} else if ( format == HEADER3 ) {
				addToIndex = 1;
				wordToIndex += wordToRender;
			} else if ( !wordToIndex.empty() ) {
				SDL_Surface* currentIndexWord = TTF_RenderText_Blended ( GetWizipedia()->GetDefaultFont(), wordToIndex.c_str(), colorLink );
				currentIndexWord->unused1 = addToIndex;

				index.push_back ( currentIndexWord );	
				
				wordToIndex.clear();
				rect.y += currentWord->h + 5;
				
				if ( indexWidth < currentIndexWord->w ) {
					indexWidth = currentIndexWord->w;
				}
			}
			
			/* Remove the space character after a link in the next word */
			if ( color == &colorLink ) {
				rect.x -= width;
			}
		}
		
		/* Post-Format */
		this->Format ( &formatWord, "\'\'\'", BOLD );
		this->Format ( &formatWord, "\'\'", ITALIC );
		this->Format ( &formatWord, "===", HEADER3 );
		this->Format ( &formatWord, "==", HEADER2 );
		
		maxPosition = rect.y;
		lineBreak = 0;
		isLink = false;
	}
	
	return true;
}


bool CRender::Format ( std::string* word_, std::string find_, FORMAT format_ )
{
	if ( util::replaceFirst ( word_, find_ ) ) {
		formatWord = *word_;
		util::replace ( word_, find_ );
		
		switch ( format_ ) {
			case BOLD:
				if ( format == BOLD ) {
					format = NONE;
				} else {
					format = BOLD;
					renderFont = GetWizipedia()->GetDefaultFontBold();
					return true;
				}
				break;
			case ITALIC:
				if ( format == ITALIC ) {
					format = NONE;
				} else {
					format = ITALIC;
					renderFont = GetWizipedia()->GetDefaultFontOblique();
					return true;
				}
				break;
			case HEADER2:
				if ( format == HEADER2 ) {
					format = NONE;
				} else {
					format = HEADER2;
					renderFont = GetWizipedia()->GetHeaderFont();
					return true;
				}
				break;
			case HEADER3:
				if ( format == HEADER3 ) {
					format = NONE;
				} else {
					format = HEADER3;
					renderFont = GetWizipedia()->GetHeader2Font();
					return true;
				}
				break;
			default:
				renderFont = GetWizipedia()->GetDefaultFont();
				break;	
			
		}

		renderFont = GetWizipedia()->GetDefaultFont();
	}

	return false;
}


bool CRender::MouseClick ( int x_, int y_ )
{
	mouseLoc.x = x_-1;
	mouseLoc.y = y_-1;
	mouseLoc.w = 2;
	mouseLoc.h = 2;
	
	return false;
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
	index.clear();
	
	format = NONE;
	indexWidth = 0;
	
}


bool CRender::Calc()
{
	if ( postionOffset ) {
		if ( postionOffset > 0.1f )
			this->ChangePosition ( postionOffset - ( GetWizipedia()->GetFrameTime() * 8 ) );
		if ( postionOffset < -0.1f )
			this->ChangePosition ( postionOffset + ( GetWizipedia()->GetFrameTime() * 8 ) );
		
	}
	
	return true;
}


bool CRender::Draw()
{
	/* Temporaly data */
	static SDL_Rect rect, rect2;
	static int positionWIndex;
	positionWIndex = position - (index.size()+1) * ( TTF_FontHeight ( GetWizipedia()->GetDefaultFont() ) );
	
	/* Draw index */
	if ( !index.empty() ) {
		boxRGBA ( GetWizipedia()->GetScreen(), 20, 10 - position, indexWidth + 40, -positionWIndex, 30, 30, 30, 255 );
		rect.y = -position;
		for ( int i = 0; i < index.size(); ++i ) {
			rect.y += TTF_FontHeight ( GetWizipedia()->GetDefaultFont() );
			rect.x = 25 + 10 * index[i]->unused1;
			
			rect2 = rect;
			SDL_BlitSurface ( index[i], NULL, GetWizipedia()->GetScreen(), &rect2 );
			
			if ( util::collideBox ( rect2, mouseLoc ) ) {
				mouseLoc.x = mouseLoc.y = mouseLoc.w = mouseLoc.h = 0;
				std::cout << "jep" << std::endl;
			}
		}
		
		positionWIndex -= 8;
	}
	
	/* Draw text */
	std::map < SDL_Surface*, SDL_Rect >::iterator textCurrent = renderText.begin();
	for ( ; textCurrent != renderText.end(); ++textCurrent ) {
		rect = (*textCurrent).second; rect.y -= positionWIndex;
		SDL_BlitSurface ( (*textCurrent).first, NULL, GetWizipedia()->GetScreen(), &rect );
	}
	
	/* Draw hyperlink */
	static std::string link; static int findPipe;
	std::vector < std::string >::iterator currentLinkURL = linkURL.begin();
	std::vector < SDL_Surface* >::iterator linkCurrent = renderLinks.begin();
	std::vector < SDL_Rect >::iterator linkCurrentPosition = renderLinksPosition.begin();
	for ( ; linkCurrent != renderLinks.end(); ++linkCurrent ) {
		rect = (*linkCurrentPosition); rect.y -= positionWIndex;
		SDL_BlitSurface ( (*linkCurrent), NULL, GetWizipedia()->GetScreen(), &rect );
		
		if ( util::collideBox ( rect, mouseLoc ) ) {
			mouseLoc.x = mouseLoc.y = mouseLoc.w = mouseLoc.h = 0;
			int size = 1;
			
			std::vector < std::string > results = GetWizipedia()->GetIndex()->Search ( (*currentLinkURL), &size );
			
			if ( size ) {
				std::string data = ( *results.begin() );
				
				int pos = data.find ( ":" );
				if ( pos != std::string::npos )
					data = data.substr ( 0, pos );
				
				std::cout <<(*currentLinkURL) << ": " << data << " -> " << pos << std::endl;
				GetWizipedia()->GetRender()->Lockup ( data, (*currentLinkURL) );
				std::cout << "Lockup fertig" << std::endl;
				GetWizipedia()->GetRender()->Render();
				
				std::cout << "Render fertig" << std::endl;
				return false;
			} else {
				std::cerr << "Article not found: " << (*currentLinkURL) << std::endl;;
			}
			
// 			std::cout << (*currentLinkURL) << std::endl;
		}
		++currentLinkURL;
		++linkCurrentPosition;
	}
	
	/* Draw horizontal lines */
	std::vector < SDL_Rect >::iterator linesCurrent = lines.begin();
	for ( ; linesCurrent != lines.end(); ++linesCurrent ) {
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
	else if ( position > maxPosition )
		position = maxPosition;
		
	
	if ( postionOffset > -0.1f & postionOffset < 0.1f )
		postionOffset = 0;

}
