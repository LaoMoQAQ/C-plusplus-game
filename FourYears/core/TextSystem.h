#ifndef TEXT_SYSTEM_H
#define TEXT_SYSTEM_H


#include <string>


class TextSystem
{

public:


    TextSystem();



    void Init();



    void SetText(
        const std::string& text
    );



    void Update();



    void Skip();



    bool Finished();


    std::string GetCurrentText();


    bool IsFinished();


    std::string GetText();



private:


    std::string fullText;


    std::string displayText;



    std::string currentText;


    
    int currentIndex;



    float timer;



    float speed;



    bool finished;



};



#endif