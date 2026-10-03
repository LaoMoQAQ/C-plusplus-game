#include "TextSystem.h"

#include <iostream>


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
    timer = 0;

    waitTimer = 0;
    waitTarget = 0;

    punctuationTimer = 0;
    punctuationTarget = 0;

    finished = true;
}





// UTF-8 字符切割
void TextSystem::SplitUTF8()
{
    characters.clear();

    for(size_t i = 0; i < fullText.size();)
    {
        unsigned char c = (unsigned char)fullText[i];

        size_t len = 1;

        if((c & 0x80) == 0)         len = 1;
        else if((c & 0xE0) == 0xC0) len = 2;
        else if((c & 0xF0) == 0xE0) len = 3;
        else if((c & 0xF8) == 0xF0) len = 4;
        else
        {
            i++;
            continue;
        }

        if(i + len <= fullText.size())
        {
            std::string ch = fullText.substr(i, len);
            characters.push_back(ch);
        }

        i += len;
    }
}





// 标点停顿
float TextSystem::GetPunctuationDelay(const std::string& ch)
{
    if(ch == "，" || ch == ",")
        return 0.08f;

    if(ch == "。" || ch == ".")
        return 0.18f;

    if(ch == "！" || ch == "!" || ch == "？" || ch == "?")
        return 0.25f;

    if(ch == "…")
        return 0.15f;

    return 0.0f;
}





void TextSystem::SetText(
    const std::string& text,
    float wait
)
{
    fullText = text;


    // 中文省略号替换：字体缺字形会渲染成方框
    std::string replaceText;

    for(size_t i = 0; i < fullText.size();)
    {
        unsigned char c = (unsigned char)fullText[i];

        size_t len = 1;

        if((c & 0x80) == 0)         len = 1;
        else if((c & 0xE0) == 0xC0) len = 2;
        else if((c & 0xF0) == 0xE0) len = 3;
        else if((c & 0xF8) == 0xF0) len = 4;

        std::string ch = fullText.substr(i, len);

        // 中文标点替换成 ASCII，避免字体缺字形变方框
        if(ch == "…")       replaceText += "...";
        else if(ch == "—")  replaceText += "-";
        else if(ch == "「") replaceText += "\"";
        else if(ch == "」") replaceText += "\"";
        else if(ch == "『") replaceText += "'";
        else if(ch == "』") replaceText += "'";
        else if(ch == "·")  replaceText += ".";
        else                replaceText += ch;

        i += len;
    }

    fullText = replaceText;


    displayText.clear();
    currentIndex = 0;
    timer = 0;
    waitTimer = 0;
    waitTarget = wait;

    punctuationTimer = 0;
    punctuationTarget = 0;

    finished = false;

    SplitUTF8();
}





// ==========================================================
// Update
// ==========================================================
//
// 更新打字机。
//
// 每帧最多显示一个字符——这样打字音效才能和字符一一对应。
// 之前用 while 会一帧加多个字，导致音效"漏拍"。

void TextSystem::Update()
{
    if(finished)
        return;

    float delta = 0.016f;


    // 等待阶段
    if(waitTimer < waitTarget)
    {
        waitTimer += delta;
        return;
    }


    // 标点停顿阶段
    if(punctuationTimer < punctuationTarget)
    {
        punctuationTimer += delta;
        return;
    }


    timer += delta;


    // 每帧最多一个字符
    if(timer >= speed)
    {
        timer -= speed;


        if(currentIndex >= (int)characters.size())
        {
            finished = true;
            return;
        }


        std::string ch = characters[currentIndex];

        displayText += ch;

        punctuationTarget = GetPunctuationDelay(ch);
        punctuationTimer = 0;

        currentIndex++;
    }
}





void TextSystem::Skip()
{
    displayText = fullText;
    currentIndex = (int)characters.size();

    finished = true;
}





// ==========================================================
// 设置每秒多少字
// ==========================================================

void TextSystem::SetCharsPerSecond(int cps)
{
    if(cps < 1)  cps = 1;
    if(cps > 60) cps = 60;   // 一帧最多 1 字，60 是上限

    speed = 1.0f / (float)cps;
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