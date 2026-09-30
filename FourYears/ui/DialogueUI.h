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



    void ShowChoice(
        const std::vector<std::string>& options
    );

    void ClearChoice();

    bool HasChoice() const;

    void MoveChoice(
        int delta
    );

    int GetChoiceIndex() const;


    int HitTestChoice(
        int x,
        int y
    ) const;


    void SetChoiceIndex(
        int idx
    );



    // ==========================================================
    // [新增] 转发文字速度到 TextSystem
    // ==========================================================

    void SetTextSpeed(int cps);

    // ==========================================================



private:


    std::vector<std::string>
    SplitTextLine(
        const std::string& text,
        int maxChar
    );






private:


    std::string speaker;



    std::string text;



    TextSystem textSystem;



    std::vector<std::string> choiceOptions;

    int choiceIndex = 0;

};




#endif