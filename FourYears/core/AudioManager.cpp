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

    audioOpen = true;

    Mix_VolumeMusic(bgmVolume);

    // 允许最多 16 个 SE 通道同时播放。
    // 打字音效一帧可能触发多个，默认 8 个不够用。
    Mix_AllocateChannels(16);

    return true;
}





// ==========================================================
// PlayBGM / StopBGM / SwitchTo
// ==========================================================
//
// SDL_mixer 只有一条音乐通道，做不了真正的交叉淡化。
// 这里用"先淡出旧曲、再换曲淡入"的两段式：
//
//   PlayBGM(新)  ->  switching = true，旧曲开始淡出
//   Update()     ->  淡出满 FADE_MS 后调 SwitchTo(新)
//   SwitchTo     ->  释放旧曲、加载新曲、Mix_FadeInMusic
//
// 淡出期间如果又来了 PlayBGM，直接覆盖 pendingPath，
// 不会堆一串切换。

void AudioManager::PlayBGM(const std::string& path)
{
    // 同一首已经在放（或已经在切到它），不重来
    if(!switching && path == currentPath && currentMusic)
        return;

    if(switching && path == pendingPath)
        return;


    if(!currentMusic)
    {
        // 当前没在放，直接上
        SwitchTo(path);
        return;
    }


    // 有曲子在放：先淡出
    pendingPath = path;
    switching = true;
    switchElapsed = 0.0f;

    Mix_FadeOutMusic(FADE_MS);
}




void AudioManager::StopBGM()
{
    pendingPath.clear();
    currentPath.clear();
    switching = false;

    Mix_HaltMusic();

    if(currentMusic)
    {
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }
}




void AudioManager::SwitchTo(const std::string& path)
{
    if(currentMusic)
    {
        Mix_HaltMusic();
        Mix_FreeMusic(currentMusic);
        currentMusic = nullptr;
    }

    currentPath.clear();

    if(path.empty())
        return;


    currentMusic = Mix_LoadMUS(path.c_str());

    if(!currentMusic)
    {
        std::cout
            << "BGM load failed:"
            << path
            << std::endl;
        return;
    }

    currentPath = path;

    // 从 0 音量淡入到目标音量
    Mix_VolumeMusic(0);

    if(Mix_FadeInMusic(currentMusic, -1, FADE_MS) < 0)
    {
        // 淡入不支持就退化成直接播放
        Mix_VolumeMusic(bgmVolume);
        Mix_PlayMusic(currentMusic, -1);
    }
}




// ==========================================================
// Update
// ==========================================================
//
// 推进"淡出 -> 换曲"的计时。
// 淡入由 SDL_mixer 自己处理，这里只负责在淡出结束后切换。

void AudioManager::Update(float delta)
{
    if(!switching)
        return;

    switchElapsed += delta * 1000.0f;

    if(switchElapsed < (float)FADE_MS)
        return;


    // 淡出完成，切到新曲
    switching = false;
    switchElapsed = 0.0f;

    std::string target = pendingPath;
    pendingPath.clear();

    SwitchTo(target);
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

    // 淡出/淡入过程中不要抢着设音量，
    // 否则会把淡入效果直接顶到满音量。
    if(switching)
        return;

    Mix_VolumeMusic(bgmVolume);
}





void AudioManager::SetSEVolume(int volume)
{
    seVolume = volume;
}





void AudioManager::Clean()
{
    if(!audioOpen)
        return;

    switching = false;
    pendingPath.clear();
    currentPath.clear();

    if(currentMusic)
    {
        Mix_HaltMusic();
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
    audioOpen = false;
}