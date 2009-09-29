#ifndef MAIN_HPP
  #define MAIN_HPP
  
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_framerate.h>
  
#include "input.h"
#include "index.h"
#include "gui.h"
#include "render.h"


class CWizipedia* GetWizipedia();
class CWizipedia
{
	private:
		bool run;
		
		std::string lang;
		
		CIndex* index;
		
		SDL_Surface* screen;
		int screenX, screenY;
		
		int defaultFontSize;
		TTF_Font* defaultFont;
		TTF_Font* defaultFontBold;
		TTF_Font* defaultFontOblique;
		TTF_Font* headerFont;
		TTF_Font* header2Font;
		
		TTF_Font* barFont;
		TTF_Font* keyFont;
		
		FPSmanager fps;
		
		CGui* gui;
		CInput input;
		CRender render;
		
	public:
		CWizipedia ( int argc_, char **argv_ );
		bool Initialize();
		
		bool RunMenu();
		bool Run();
		void Quit() { run = false; }
		bool IsRunning() { return run; }
		
		void Calc();
		void Draw();
		
		std::string GetLang() { return lang; }
		
		SDL_Surface* GetScreen() { return screen; }
		CIndex* GetIndex() { return index; }
		CGui* GetGui() { return gui; }	
		CRender* GetRender() { return &render; }
		
		int GetDefaultFontSize() { return defaultFontSize; }
		void SetDefaultFontSize ( int size_ );
		TTF_Font* GetDefaultFont() { return defaultFont; }
		TTF_Font* GetDefaultFontBold() { return defaultFontBold; }
		TTF_Font* GetDefaultFontOblique() { return defaultFontOblique; }
		TTF_Font* GetHeaderFont() { return headerFont; }
		TTF_Font* GetHeader2Font() { return header2Font; }
		
		TTF_Font* GetBarFont() { return barFont; }
		TTF_Font* GetKeyFont() { return keyFont; }
		
		
		int GetFps() { return SDL_getFramerate( &fps ); }
		double GetFrameTime() { return 1.f/SDL_getFramerate( &fps ); }
	
};
  
#endif // MAIN_HPP
