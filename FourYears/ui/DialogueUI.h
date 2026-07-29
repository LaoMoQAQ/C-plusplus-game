#ifndef DIALOGUE_UI_H
#define DIALOGUE_UI_H


#include <string>


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


    std::string speaker;


    std::string text;
    

    TextSystem textSystem;


};



#endif