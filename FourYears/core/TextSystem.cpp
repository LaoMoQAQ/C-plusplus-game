#include "TextSystem.h"

TextSystem::TextSystem()
{

    currentIndex = 0;

    timer = 0.0f;

    speed = 0.035f;      // 比以前顺滑

    waitTimer=0;

    waitTarget=0;

    finished = true;

}

void TextSystem::Init()
{

    fullText.clear();

    displayText.clear();

    characters.clear();

    currentIndex = 0;

    timer = 0.0f;

    finished = true;

}

void TextSystem::SplitUTF8()
{

    characters.clear();

    for(size_t i = 0; i < fullText.size(); )
    {

        unsigned char c =
            (unsigned char)fullText[i];

        size_t len = 1;

        if((c & 0x80) == 0)
        {
            len = 1;
        }
        else if((c & 0xE0) == 0xC0)
        {
            len = 2;
        }
        else if((c & 0xF0) == 0xE0)
        {
            len = 3;
        }
        else if((c & 0xF8) == 0xF0)
        {
            len = 4;
        }

        characters.push_back(
            fullText.substr(i, len)
        );

        i += len;

    }

}

void TextSystem::SetText(
    const std::string& text,
    float wait
)
{
    fullText=text;

    displayText="";

    currentIndex=0;

    timer=0;

    waitTimer=0;

    waitTarget=wait;

    finished=false;
}

void TextSystem::Update()
{

    if(finished)
        return;

    waitTimer+=0.016f;

    if(waitTimer<waitTarget)
    {
        return;
    }

    timer += 0.016f;

    while(timer >= speed)
    {

        timer -= speed;

        if(currentIndex < (int)characters.size())
        {

            displayText +=
                characters[currentIndex];

            currentIndex++;

        }
        else
        {

            finished = true;

            break;

        }

    }

}

void TextSystem::Skip()
{

    displayText = fullText;

    currentIndex =
        (int)characters.size();

    finished = true;

}

bool TextSystem::Finished()
{

    return finished;

}

bool TextSystem::IsFinished()
{

    return finished;

}

std::string TextSystem::GetText()
{

    return displayText;

}

std::string TextSystem::GetCurrentText()
{

    return displayText;

}