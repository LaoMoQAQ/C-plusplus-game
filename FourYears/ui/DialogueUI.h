#ifndef DIALOGUE_UI_H
#define DIALOGUE_UI_H


#include <string>
#include <vector>

#include "../core/Renderer.h"
#include "../core/TextSystem.h"
#include "../core/AudioManager.h"



class DialogueUI
{

public:

    DialogueUI();

    void Draw(Renderer& renderer);
    void Render(Renderer& renderer);

    void SetSpeaker(const std::string& name);
    void SetText(const std::string& text, float wait = 0.0f);

    void Update();

    bool Finished();
    void Skip();



    // ---- 选项 ----

    void ShowChoice(const std::vector<std::string>& options);
    void ClearChoice();
    bool HasChoice() const;

    void MoveChoice(int delta);
    int  GetChoiceIndex() const;

    int  HitTestChoice(int x, int y) const;
    void SetChoiceIndex(int idx);



    // ---- 设置 ----

    void SetTextSpeed(int cps);

    void SetTypingSound(
        AudioManager* audio,
        const std::string& path
    );

    void SetBackgroundTexture(SDL_Texture* tex);



private:

    std::vector<std::string> SplitTextLine(
        const std::string& text,
        int maxChar
    );



private:

    std::string speaker;
    std::string text;

    TextSystem textSystem;

    std::vector<std::string> choiceOptions;
    int choiceIndex = 0;


    // ---- 打字音效 ----

    AudioManager* typingAudio = nullptr;
    std::string   typingSoundPath;


    // ---- 对话框高度动画 ----

    float displayBoxH = 220.0f;

    static constexpr float BOX_ANIM_SPEED = 0.35f;


    // ==========================================================
    // [新增] 选项高亮块滑动动画
    // ==========================================================
    //
    // displayChoiceIndex 是"浮点索引"，
    // 每帧向 choiceIndex 靠近，
    // 高亮块的 Y 位置 = boxY + CHOICE_OFFSET
    //                   + displayChoiceIndex * CHOICE_GAP
    //
    // 键盘切换用慢速，鼠标 hover 用快速。

    float displayChoiceIndex = 0.0f;

    static constexpr float CHOICE_ANIM_SPEED      = 0.25f;
    static constexpr float CHOICE_ANIM_SPEED_FAST = 0.55f;

    float choiceAnimSpeed = CHOICE_ANIM_SPEED;



    // ---- 背景纹理 ----

    SDL_Texture* backgroundTexture = nullptr;

};


#endif