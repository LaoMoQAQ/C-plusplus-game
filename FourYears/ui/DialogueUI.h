#ifndef DIALOGUE_UI_H
#define DIALOGUE_UI_H


#include <string>

#include <vector>


#include "../core/Renderer.h"

#include "../core/TextSystem.h"





class DialogueUI
{


public:


    DialogueUI();




    void Draw(
        Renderer& renderer
    );




    void SetSpeaker(
        const std::string& name
    );




    void SetText(
        const std::string& text,
        float wait=0.0f
    );




    void Update();




    void Render(
        Renderer& renderer
    );




    bool Finished();




    void Skip();



    // ==========================================================
    // [新增] 选项显示
    // ==========================================================
    //
    // 进入选择事件时，Game 调用 ShowChoice() 把选项传进来。
    // DialogueUI 负责显示和上下切换。
    //
    // 确认键和实际跳转由 Game 处理。

    void ShowChoice(
        const std::vector<std::string>& options
    );

    void ClearChoice();

    bool HasChoice() const;

    // 上下移动选中项。
    // delta = -1 上，+1 下。
    void MoveChoice(
        int delta
    );

    int GetChoiceIndex() const;

    // ==========================================================



private:


    // 自动换行
    std::vector<std::string>
    SplitTextLine(
        const std::string& text,
        int maxChar
    );






private:


    // 当前说话人

    std::string speaker;



    // 原始文本

    std::string text;



    // 打字机

    TextSystem textSystem;



    // ==========================================================
    // [新增] 选项状态
    // ==========================================================

    std::vector<std::string> choiceOptions;

    int choiceIndex = 0;

    // ==========================================================

};




#endif