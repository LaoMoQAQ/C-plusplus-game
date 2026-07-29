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



};




#endif