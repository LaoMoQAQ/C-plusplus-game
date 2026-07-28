#ifndef CONFIG_H
#define CONFIG_H


#include <string>



class Config
{


public:


    Config();




    bool Load(
        const std::string& path
    );



    bool Save(
        const std::string& path
    );




    int GetWidth();


    int GetHeight();



    int GetBGMVolume();



    int GetSEVolume();



    int GetTextSpeed();



    bool IsFullscreen();



    bool IsAutoPlay();




    void SetWidth(
        int value
    );


    void SetHeight(
        int value
    );


    void SetBGMVolume(
        int value
    );


    void SetSEVolume(
        int value
    );


    void SetTextSpeed(
        int value
    );


    void SetFullscreen(
        bool value
    );


    void SetAutoPlay(
        bool value
    );




private:


    int width;


    int height;


    int bgmVolume;


    int seVolume;


    int textSpeed;



    bool fullscreen;


    bool autoPlay;



};



#endif