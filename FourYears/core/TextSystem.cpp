#include "TextSystem.h"

TextSystem::TextSystem()
{
    currentIndex = 0;

    timer = 0.0f;

    speed = 0.035f;

    waitTimer = 0.0f;
    waitTarget = 0.0f;

    punctuationTimer = 0.0f;
    punctuationTarget = 0.0f;

    finished = true;
}

void TextSystem::Init()
{
    fullText.clear();

    displayText.clear();

    characters.clear();

    currentIndex = 0;

    timer = 0.0f;

    waitTimer = 0.0f;
    waitTarget = 0.0f;

    punctuationTimer = 0.0f;
    punctuationTarget = 0.0f;

    finished = true;
}

void TextSystem::SplitUTF8()
{
    characters.clear();

    for(size_t i=0;i<fullText.size();)
    {
        unsigned char c=
            (unsigned char)fullText[i];

        size_t len=1;

        if((c&0x80)==0)
        {
            len=1;
        }
        else if((c&0xE0)==0xC0)
        {
            len=2;
        }
        else if((c&0xF0)==0xE0)
        {
            len=3;
        }
        else if((c&0xF8)==0xF0)
        {
            len=4;
        }

        characters.push_back(
            fullText.substr(i,len)
        );

        i+=len;
    }
}

float TextSystem::GetPunctuationDelay(
    const std::string& ch
)
{
    if(
        ch=="，"||
        ch==","||
        ch=="、"
    )
    {
        return 0.08f;
    }

    if(
        ch=="。"
        ||ch=="."
    )
    {
        return 0.18f;
    }

    if(
        ch=="！"
        ||ch=="!"
    )
    {
        return 0.22f;
    }

    if(
        ch=="？"
        ||ch=="?"
    )
    {
        return 0.22f;
    }

    if(
        ch=="："
        ||ch==":"
        ||ch=="；"
        ||ch==";"
    )
    {
        return 0.12f;
    }

    if(
        ch=="…"
    )
    {
        return 0.15f;
    }

    return 0.0f;
}

void TextSystem::SetText(
    const std::string& text,
    float wait
)
{
    fullText=text;

    displayText.clear();

    currentIndex=0;

    timer=0.0f;

    waitTimer=0.0f;
    waitTarget=wait;

    punctuationTimer=0.0f;
    punctuationTarget=0.0f;

    finished=false;

    SplitUTF8();
}

void TextSystem::Update()
{
    if(finished)
    {
        return;
    }

    constexpr float delta=0.016f;

    if(waitTimer<waitTarget)
    {
        waitTimer+=delta;
        return;
    }

    if(punctuationTimer<punctuationTarget)
    {
        punctuationTimer+=delta;
        return;
    }

    timer+=delta;

    while(timer>=speed)
    {
        timer-=speed;

        if(currentIndex>=
            (int)characters.size())
        {
            finished=true;
            break;
        }

        displayText+=
            characters[currentIndex];

        punctuationTarget=
            GetPunctuationDelay(
                characters[currentIndex]
            );

        punctuationTimer=0.0f;

        currentIndex++;

        if(punctuationTarget>0.0f)
        {
            break;
        }
    }
}

void TextSystem::Skip()
{
    displayText=fullText;

    currentIndex=
        (int)characters.size();

    finished=true;
}

bool TextSystem::Finished()
{
    return finished;
}

bool TextSystem::IsFinished()
{
    return finished;
}

std::string TextSystem::GetCurrentText()
{
    return displayText;
}

std::string TextSystem::GetText()
{
    return displayText;
}