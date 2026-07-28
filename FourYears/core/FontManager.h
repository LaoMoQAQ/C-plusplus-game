#ifndef FONT_MANAGER_H
#define FONT_MANAGER_H


#include <SDL_ttf.h>

#include <string>


class FontManager
{


public:


    FontManager();


    ~FontManager();



    bool Load(
        const std::string& path
    );



    TTF_Font* GetFont();



private:


    TTF_Font* font;


};



#endif