#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H


#include <string>

#include <SDL.h>
#include <SDL_mixer.h>



class AudioManager
{


public:


    AudioManager();



    ~AudioManager();




    bool Init();




    void PlayBGM(
        const std::string& path
    );



    void StopBGM();




    void PlaySE(
        const std::string& path
    );




    void SetBGMVolume(
        int volume
    );



    void SetSEVolume(
        int volume
    );



    void Clean();



private:


    Mix_Music* currentMusic;



    int bgmVolume;


    int seVolume;



};



#endif