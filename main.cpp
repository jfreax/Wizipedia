#include <iostream>

#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_framerate.h>

#include "main.h"
#include "index.h"
#include "input.h"
#include "util.h"
#include "render.h"


CWizipedia* wizipedia = NULL;


int main ( int argc, char *argv[] )
{
	wizipedia = new CWizipedia ( argc, argv );
	
	if ( wizipedia->Initialize() )
		wizipedia->RunMenu();
	else
		return false;

	delete wizipedia;
	return 0;
}


CWizipedia* GetWizipedia()
{
	return wizipedia;
}


CWizipedia::CWizipedia ( int argc_, char **argv_ ) :
	run ( true ), screenX ( 320 ), screenY ( 240 )
{
	/* Read argumentlines */
	for ( int i = 1; i != argc_; ++i ) {
		const std::string val ( argv_[i] );
		
		
		if ( val.empty() ) {
			continue;
		} else if ( val == "-l" || val == "--lang" ) {
			if ( i + 1 != argc_ ) {
				++i;
				lang = argv_[i];
			}
		}
		
	}

	if ( lang.empty() ) {
		lang = "en";
	}
}


bool CWizipedia::Initialize()
{
	index = new CIndex ( "db/" ); 
	srand ( (unsigned)time ( NULL ) ); 
	
	if ( SDL_Init ( SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER ) == -1 ) {
		std::cerr << "ERROR: SDL_INIT" << std::endl;
		return false;
	}
	
	screen = SDL_SetVideoMode( 320, 240, 16, SDL_SWSURFACE );
	if ( screen == NULL ) {
		std::cerr << "ERROR: SDL_INIT" << std::endl;
		return false;
	}
	
	SDL_WM_SetCaption ( "Wizipedia", NULL );
	
	#ifdef linux
		SDL_ShowCursor ( true );
	#else
		SDL_ShowCursor ( false );
	#endif
	
	/* Init fontsystem */
	TTF_Init();
	defaultFont = keyFont = barFont = defaultFontBold = defaultFontOblique = headerFont = NULL;
	this->SetDefaultFontSize ( 9 );
	
	barFont = TTF_OpenFont ( "data/fonts/DejaVuSans.ttf", 12 );
	keyFont = TTF_OpenFont ( "data/fonts/DejaVuSans-Bold.ttf", 18 );
	
	if ( !defaultFont || !keyFont || !barFont || !defaultFontBold || !defaultFontOblique || !headerFont ) {
		std::cerr << "ERROR: Can't load font from \"./data/fonts/\"" << std::endl;
		return false;
	}

	/* Init frameratemanager */
	SDL_initFramerate ( &fps );
	SDL_setFramerate ( &fps, 120 );
	
	/* Initialize the gui subsystem */
	gui = new CGui();
	
	return true;
}


bool CWizipedia::RunMenu()
{
	this->Run();
}


bool CWizipedia::Run()
{
	while ( this->IsRunning() ) {
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
	gui->Calc();
}


void CWizipedia::Draw()
{
	render.Draw();
	gui->Draw();
	
	SDL_Flip ( this->GetScreen() );
// 	SDL_FillRect ( this->GetScreen(), 0, SDL_MapRGB ( this->GetScreen()->format, 33, 36, 33 ) );
	SDL_FillRect ( this->GetScreen(), 0, SDL_MapRGB ( this->GetScreen()->format, 0, 0, 0 ) );
}


void CWizipedia::SetDefaultFontSize ( int size_ )
{
	if ( defaultFont )
		TTF_CloseFont ( defaultFont );
	if ( defaultFontBold )
		TTF_CloseFont ( defaultFontBold );
	if ( defaultFontOblique )
		TTF_CloseFont ( defaultFontOblique );
	if ( headerFont )
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


