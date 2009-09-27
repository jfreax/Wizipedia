#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_framerate.h>

#include "main.h"
#include "index.h"
#include "input.h"
#include "util.h"
#include "render.h"


CWizipedia wizipedia;


int main ( int argc, char *argv[] )
{
	wizipedia.RunMenu();



	return 0;
}


CWizipedia* GetWizipedia()
{
	return &wizipedia;
}


CWizipedia::CWizipedia() :
	run ( true ), screenX ( 320 ), screenY ( 240 )
{
	index = new CIndex ( "db/" ); 
	srand ( (unsigned)time ( NULL ) ); 
		
	SDL_Init( SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER );
	screen = SDL_SetVideoMode( 320, 240, 16, SDL_SWSURFACE );
	
	SDL_WM_SetCaption ( "Wizipedia", NULL );
// 	SDL_ShowCursor ( false );
		
	/* Init fontsystem */
	TTF_Init();
	
	this->SetDefaultFontSize ( 9 );
	
	barFont = TTF_OpenFont ( "data/fonts/DejaVuSans.ttf", 12 );
	keyFont = TTF_OpenFont ( "data/fonts/DejaVuSans-Bold.ttf", 18 );
	
	if ( !defaultFont || !keyFont || !barFont || !defaultFontBold || !defaultFontOblique || !headerFont ) {
		std::cerr << "ERROR: Can't load font from \"./data/fonts/\"" << std::endl;
		GetWizipedia()->Quit();
	}
	
	/* Init frameratemanager */
	SDL_initFramerate ( &fps );
	SDL_setFramerate ( &fps, 120 );
}


bool CWizipedia::RunMenu()
{
	this->Run();
}


bool CWizipedia::Run()
{
	while ( this->IsRunning() )
	{
		SDL_framerateDelay( &fps );
		
		input.Check( this );
		
		this->Calc();
		this->Draw();
	}
	
	return true;
}


void CWizipedia::Calc()
{
	render.Calc();
	gui.Calc();
}


void CWizipedia::Draw()
{
	render.Draw();
	gui.Draw();
	
	SDL_Flip ( this->GetScreen() );
// 	SDL_FillRect ( this->GetScreen(), 0, SDL_MapRGB ( this->GetScreen()->format, 33, 35, 33 ) );
	SDL_FillRect ( this->GetScreen(), 0, SDL_MapRGB ( this->GetScreen()->format, 0, 0, 0 ) );
	
}


void CWizipedia::SetDefaultFontSize ( int size_ )
{
	TTF_CloseFont ( defaultFont );
	TTF_CloseFont ( defaultFontBold );
	TTF_CloseFont ( defaultFontOblique );
	TTF_CloseFont ( headerFont );
	
	defaultFontSize = size_;
	
	defaultFont = TTF_OpenFont ( "data/fonts/DejaVuSans.ttf", defaultFontSize );
	defaultFontBold = TTF_OpenFont ( "data/fonts/DejaVuSans-Bold.ttf", defaultFontSize );
	defaultFontOblique = TTF_OpenFont ( "data/fonts/DejaVuSans-Oblique.ttf", defaultFontSize );
	
	headerFont = TTF_OpenFont ( "data/fonts/DejaVuSans.ttf", defaultFontSize+14 );
	header2Font = TTF_OpenFont ( "data/fonts/DejaVuSans.ttf", defaultFontSize+10 );
	
	/* Refresh site */
	render.Render();
}


