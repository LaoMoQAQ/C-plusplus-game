#ifndef TEXT_SYSTEM_H
#define TEXT_SYSTEM_H

#include <string>
#include <vector>

class TextSystem
{

public:

    TextSystem();

    void Init();

    void SetText(
        const std::string& text,
        float wait=0.0f
    );

    void Update();

    void Skip();

    bool Finished();

    std::string GetCurrentText();

    bool IsFinished();

    std::string GetText();

private:

    // UTF-8 字符切分
    void SplitUTF8();

private:

    std::string fullText;

    std::string displayText;

    std::vector<std::string> characters;

    int currentIndex;

    float timer;

    float speed;

    float waitTimer;

    float waitTarget;
    
    bool finished;

};

#endif