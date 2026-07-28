#include "AudioManager.h"


#include <iostream>




AudioManager::AudioManager()
{

    currentMusic = nullptr;


    bgmVolume = 64;


    seVolume = 64;


}





AudioManager::~AudioManager()
{

    Clean();

}





bool AudioManager::Init()
{


    if(
        Mix_OpenAudio(
            44100,
            MIX_DEFAULT_FORMAT,
            2,
            2048
        )
        <0
    )
    {


        std::cout
        <<"Audio init failed\n";


        return false;

    }



    Mix_VolumeMusic(
        bgmVolume
    );



    return true;


}








void AudioManager::PlayBGM(

    const std::string& path

)
{


    if(currentMusic)
    {

        Mix_FreeMusic(
            currentMusic
        );


        currentMusic=nullptr;

    }



    currentMusic =
    Mix_LoadMUS(
        path.c_str()
    );



    if(!currentMusic)
    {

        std::cout
        <<"BGM load failed:"
        <<path
        <<std::endl;


        return;

    }




    Mix_PlayMusic(
        currentMusic,
        -1
    );


}








void AudioManager::StopBGM()
{


    Mix_HaltMusic();


}








void AudioManager::PlaySE(

    const std::string& path

)
{


    Mix_Chunk* sound =
    Mix_LoadWAV(
        path.c_str()
    );



    if(!sound)
    {

        return;

    }



    Mix_VolumeChunk(
        sound,
        seVolume
    );



    Mix_PlayChannel(
        -1,
        sound,
        0
    );



}








void AudioManager::SetBGMVolume(

    int volume

)
{


    bgmVolume = volume;


    Mix_VolumeMusic(
        bgmVolume
    );


}







void AudioManager::SetSEVolume(

    int volume

)
{


    seVolume=volume;


}








void AudioManager::Clean()
{


    if(currentMusic)
    {


        Mix_FreeMusic(
            currentMusic
        );


        currentMusic=nullptr;

    }



    Mix_CloseAudio();


}