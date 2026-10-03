#include "AudioManager.h"

#include <iostream>



AudioManager::AudioManager()
{
    currentMusic = nullptr;

    bgmVolume = 64;
    seVolume  = 64;
}





AudioManager::~AudioManager()
{
    Clean();
}





bool AudioManager::Init()
{
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        std::cout << "Audio init failed\n";
        return false;
    }

    Mix_VolumeMusic(bgmVolume);

    // 允许最多 16 个 SE 通道同时播放。
    // 打字音效一帧可能触发多个，默认 8 个不够用。
    Mix_AllocateChannels(16);

    return true;
}





void AudioManager::PlayBGM(const std::string& path)
{
    if(currentMusic)
    {
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }

    currentMusic = Mix_LoadMUS(path.c_str());

    if(!currentMusic)
    {
        std::cout
            << "BGM load failed:"
            << path
            << std::endl;
        return;
    }

    Mix_PlayMusic(currentMusic, -1);
}





void AudioManager::StopBGM()
{
    Mix_HaltMusic();
}





// ==========================================================
// PlaySE
// ==========================================================
//
// 走缓存：路径第一次用时加载，之后直接复用。
// 每次调用随机分配通道，播完通道自动释放。

void AudioManager::PlaySE(const std::string& path)
{
    Mix_Chunk* sound = nullptr;

    auto it = seCache.find(path);

    if(it != seCache.end())
    {
        sound = it->second;
    }
    else
    {
        sound = Mix_LoadWAV(path.c_str());

        if(!sound)
        {
            // 加载失败也记下来，避免下次反复尝试
            seCache[path] = nullptr;
            return;
        }

        seCache[path] = sound;
    }


    if(!sound)
        return;


    Mix_VolumeChunk(sound, seVolume);

    Mix_PlayChannel(-1, sound, 0);
}





void AudioManager::SetBGMVolume(int volume)
{
    bgmVolume = volume;
    Mix_VolumeMusic(bgmVolume);
}





void AudioManager::SetSEVolume(int volume)
{
    seVolume = volume;
}





void AudioManager::Clean()
{
    if(currentMusic)
    {
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }


    // 释放 SE 缓存
    for(auto& pair : seCache)
    {
        if(pair.second)
            Mix_FreeChunk(pair.second);
    }

    seCache.clear();


    Mix_CloseAudio();
}