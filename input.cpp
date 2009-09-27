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

#include <SDL/SDL.h>
#include "main.h"
#include "input.h"
#include "util.h"
#include <SDL/SDL_gfxPrimitives.h>

void CInput::Check ( CWizipedia* wizipedia )
{
	SDL_Joystick* joystick = SDL_JoystickOpen(0);
	SDL_JoystickUpdate();
	
	static int x, y, oldX, oldY, downOldY;
	static int first = 0;
	static Uint8* keys;
	SDL_GetMouseState ( &x, &y );
	
	SDL_Event event;
	while( SDL_PollEvent( &event ) ) {
		switch ( event.type ) {
			case SDL_QUIT:
				wizipedia->Quit();
				break;
			/* Gp2x Joystick */
			case SDL_JOYBUTTONDOWN:
				switch( event.jbutton.button ) {
					case GP2X_BUTTON_START:
						GetWizipedia()->GetGui()->ToggleShowMenu();
						break;
					case GP2X_BUTTON_SELECT:
						GetWizipedia()->Quit();
						break;
					case GP2X_BUTTON_LEFT:
						GetWizipedia()->GetGui()->MoveCursorLeft();
						break;
					case GP2X_BUTTON_RIGHT:
						GetWizipedia()->GetGui()->MoveCursorRight();
						break;
					case GP2X_BUTTON_UP:
						GetWizipedia()->GetGui()->GoUp();
						break;
					case GP2X_BUTTON_DOWN:
						GetWizipedia()->GetGui()->GoDown();
						break;
					case GP2X_BUTTON_B:
						wizipedia->GetGui()->SetShowMenu ( false );
						wizipedia->GetRender()->Lockup ( GetWizipedia()->GetGui()->GetSelectedFile(), GetWizipedia()->GetGui()->GetSelected() );
						wizipedia->GetRender()->Render();
						break;
					case GP2X_BUTTON_L:
						GetWizipedia()->GetGui()->AddChar ( 'D' );
						break;
				}
			/* Computerkeyboard */
			case SDL_KEYDOWN:
				switch ( event.key.keysym.sym ) {
					case SDLK_LEFT:
						GetWizipedia()->GetGui()->MoveCursorLeft();
						break;
					case SDLK_RIGHT:
						GetWizipedia()->GetGui()->MoveCursorRight();
						break;
					case SDLK_UP:
// 						GetWizipedia()->GetGui()->ToggleShowMenu();
						GetWizipedia()->GetGui()->GoUp();
						break;
					case SDLK_DOWN:
// 						GetWizipedia()->GetGui()->AddChar ( 'D' );
						GetWizipedia()->GetGui()->GoDown();
						break;
				}
				keys = SDL_GetKeyState (NULL);
				
				if ( keys[SDLK_ESCAPE] ) {
					wizipedia->Quit();
				}
				
				if ( keys[SDLK_RETURN] ) {
					wizipedia->GetGui()->SetShowMenu ( false );
					wizipedia->GetRender()->Lockup ( GetWizipedia()->GetGui()->GetSelectedFile(), GetWizipedia()->GetGui()->GetSelected() );
					wizipedia->GetRender()->Render();
				}
				
				if ( keys[SDLK_m] ) {
					wizipedia->GetGui()->ToggleShowMenu();
				}
				
				if ( keys[SDLK_KP_PLUS] ) {
					wizipedia->SetDefaultFontSize ( wizipedia->GetDefaultFontSize() + 1 );
				}
				if ( keys[SDLK_KP_MINUS] ) {
					wizipedia->SetDefaultFontSize ( wizipedia->GetDefaultFontSize() - 1 );
				}
				
				break;
				
			case SDL_MOUSEBUTTONDOWN:
				GetWizipedia()->GetGui()->MouseClick();
				
				downOldY = y;
				break;
			case SDL_MOUSEBUTTONUP:
				downOldY = 0;
				first = 0;
				
				this->MouseClick ( x, y );
				break;
		}
	}
	
	/* Mouse moved */
	if ( x != oldX || y != oldY ) {
		this->MouseMove ( x, y );
	}
	
	/* To move screen with touchscreen */
	if ( downOldY ) {
		if ( first < 10 ) {
			downOldY = y;
			++first;
		} else {
			wizipedia->GetRender()->ChangePosition ( -(y - downOldY) * wizipedia->GetFrameTime()*100 );
			downOldY = y;
		}
		
	}
	
	oldX = x; oldY = y;
}


void CInput::MouseClick ( int x, int y )
{
	SDL_Rect mouseLoc = { x-1, y-1, 2, 2 };
	
	/* Test if clicked on GUI */
	if ( !GetWizipedia()->GetGui()->MouseClick ( true ) )
	{ /* Not clicked gui */
		GetWizipedia()->GetGui()->SetShowMenu ( false );
	}
	
// 	/* Click on Wikipedia-Logo */
// 	if ( util::collideBox ( GetWizipedia()->GetGui()->GetWikiLogoLocation(), mouseLoc ) ) {
// 		GetWizipedia()->GetGui()->ToggleShowMenu();
// 		
// 	/*  ... on the OSD-Keyboard */
// // 	} else if ( util::collideBox ( GetWizipedia()->GetGui()->GetKeyboardLocation(), mouseLoc ) ) {
// 		
// 	/*  ... on Inputbar */
// 	} else if ( util::collideBox ( GetWizipedia()->GetGui()->GetBarLocation(), mouseLoc ) ) {
// 		
// 	/* ... on - ohhh... other */
// 	} else {
// 		GetWizipedia()->GetGui()->SetShowMenu ( false );
// 	}
}


void CInput::MouseMove ( int x, int y )
{
	SDL_Rect mouseLoc = { x-1, y-1, 2, 2 };
	
	if ( util::collideBox ( GetWizipedia()->GetGui()->GetWikiLogoLocation(), mouseLoc ) )
		GetWizipedia()->GetGui()->MarkWikiLogo ( true );
	else
		GetWizipedia()->GetGui()->MarkWikiLogo ( false );
}