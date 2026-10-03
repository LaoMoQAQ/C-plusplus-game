#ifndef AUDIO_MANAGER_H
#define AUDIO_MANAGER_H


#include <string>
#include <map>

#include <SDL.h>
#include <SDL_mixer.h>



class AudioManager
{

public:

    AudioManager();
    ~AudioManager();

    bool Init();

    void PlayBGM(const std::string& path);
    void StopBGM();

    // 播放一次性音效。
    // 内部按路径缓存 Mix_Chunk，同一文件重复播放不会重复加载。
    void PlaySE(const std::string& path);

    void SetBGMVolume(int volume);
    void SetSEVolume(int volume);

    void Clean();



private:

    Mix_Music* currentMusic;

    int bgmVolume;
    int seVolume;


    // [新增] SE 缓存：路径 -> Mix_Chunk*
    // 避免每次播放都从磁盘重新加载。
    std::map<std::string, Mix_Chunk*> seCache;

};


#endif