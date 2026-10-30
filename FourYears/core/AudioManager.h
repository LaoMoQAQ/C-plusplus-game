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

    // 播放 BGM。
    //
    // 换曲走"淡出 -> 换曲 -> 淡入"：
    //   SDL_mixer 只有一个音乐通道，没法真正交叉淡化，
    //   所以先让旧曲淡出，淡出结束后再让新曲淡入。
    //
    // 同一首已经在放则不重新开始。
    void PlayBGM(const std::string& path);

    // 淡出后停止。
    void StopBGM();

    // 播放一次性音效。
    // 内部按路径缓存 Mix_Chunk，同一文件重复播放不会重复加载。
    void PlaySE(const std::string& path);

    void SetBGMVolume(int volume);
    void SetSEVolume(int volume);

    // 每帧调用，推进淡出到淡入的切换。delta 单位秒。
    void Update(float delta);

    // 释放所有音乐/音效并关闭音频设备。
    // 必须在 SDL_Quit() 之前调用。可重复调用。
    void Clean();



private:

    // 淡入淡出时长（毫秒）
    static constexpr int FADE_MS = 500;

    // 立刻切到指定曲目（带淡入）。path 为空表示只停止。
    void SwitchTo(const std::string& path);

    // 当前曲目的路径，用来判断"同一首不要重播"
    std::string currentPath;

    // 淡出期间暂存的目标路径。淡出结束才真正切。
    std::string pendingPath;

    // 是否正在淡出等待切换
    bool switching = false;

    // 淡出已经等了多久（毫秒）
    float switchElapsed = 0.0f;

    Mix_Music* currentMusic;

    int bgmVolume;
    int seVolume;

    // Mix_OpenAudio 是否成功过。Clean 用它防止重复关闭。
    bool audioOpen = false;


    // [新增] SE 缓存：路径 -> Mix_Chunk*
    // 避免每次播放都从磁盘重新加载。
    std::map<std::string, Mix_Chunk*> seCache;

};


#endif