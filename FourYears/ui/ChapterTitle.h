#ifndef CHAPTER_TITLE_H
#define CHAPTER_TITLE_H


#include <string>

#include "../core/Renderer.h"
#include "../core/FontManager.h"


// ==========================================================
// ChapterTitle
// ==========================================================
//
// 进入章节时播放的标题卡：
//   淡入 0.45s → 停留 1.25s → 淡出 0.55s
//
// 中间一块玻璃面板写着「第 N 章」和副标题。
// 播放期间对话不推进；按键可以跳过。

class ChapterTitle
{

public:

    void SetFontManager(FontManager* fm);

    void Show(
        const std::string& title,
        const std::string& subtitle
    );

    void Skip();

    void Update(float delta);

    void Render(Renderer& renderer);

    bool IsActive() const;



private:

    bool active = false;

    float time = 0.0f;

    std::string title;

    std::string subtitle;

    FontManager* fontManager = nullptr;

};


#endif
