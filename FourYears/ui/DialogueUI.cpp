#include "DialogueUI.h"

#include "MenuCommon.h"

#include <vector>



namespace
{

    constexpr int BOX_X      = 40;
    constexpr int BOX_W      = 1520;
    constexpr int BOX_BOTTOM = 860;

    constexpr Uint8 BOX_R = 20;
    constexpr Uint8 BOX_G = 20;
    constexpr Uint8 BOX_B = 20;
    constexpr Uint8 BOX_A = 200;

    constexpr int BOX_RADIUS = 24;

    constexpr int HEADER_H       = 80;
    constexpr int PAD_BOTTOM     = 50;
    constexpr int MIN_H          = 220;
    constexpr int CHOICE_PAD_TOP = 70;

    constexpr int SPEAKER_X      = 80;
    constexpr int SPEAKER_OFFSET = 25;

    constexpr int TEXT_X      = 80;
    constexpr int TEXT_OFFSET = 95;
    constexpr int LINE_GAP    = 45;

    constexpr int CHOICE_X      = 120;
    constexpr int CHOICE_OFFSET = 70;
    constexpr int CHOICE_GAP    = 70;

    constexpr int CHOICE_HIT_W = 600;
    constexpr int CHOICE_HIT_H = 55;

    constexpr int CHOICE_HL_W  = 700;

}


DialogueUI::DialogueUI()
{
}


void DialogueUI::SetSpeaker(const std::string& name)
{
    speaker = name;
}


void DialogueUI::SetText(const std::string& text, float wait)
{
    textSystem.SetText(text, wait);
}


void DialogueUI::Update()
{
    // ---- 1. 打字机 ----

    int beforeLen = (int)textSystem.GetCurrentText().size();
    textSystem.Update();
    int afterLen = (int)textSystem.GetCurrentText().size();


    // ---- 2. 打字音效 ----

    if(afterLen > beforeLen)
    {
        if(typingAudio && !typingSoundPath.empty())
        {
            typingAudio->PlaySE(typingSoundPath);
        }
    }


    // ---- 3. 对话框高度平滑 ----

    std::string current = textSystem.GetCurrentText();
    auto lines = SplitTextLine(current, 34);

    int lineCount = (int)lines.size();
    if(lineCount < 1) lineCount = 1;

    bool hasChoice = !choiceOptions.empty();

    int targetBoxH;

    if(hasChoice)
    {
        targetBoxH = CHOICE_PAD_TOP
                   + (int)choiceOptions.size() * CHOICE_GAP
                   + PAD_BOTTOM;
    }
    else
    {
        targetBoxH = HEADER_H
                   + lineCount * LINE_GAP
                   + PAD_BOTTOM;
    }

    if(targetBoxH < MIN_H) targetBoxH = MIN_H;

    float diff = (float)targetBoxH - displayBoxH;

    if(diff > -0.5f && diff < 0.5f)
        displayBoxH = (float)targetBoxH;
    else
        displayBoxH += diff * BOX_ANIM_SPEED;


    // ---- 4. 选项高亮块滑动 ----

    if(hasChoice)
    {
        float cdiff = (float)choiceIndex - displayChoiceIndex;

        if(cdiff > -0.02f && cdiff < 0.02f)
            displayChoiceIndex = (float)choiceIndex;
        else
            displayChoiceIndex += cdiff * choiceAnimSpeed;
    }
}


std::vector<std::string> DialogueUI::SplitTextLine(
    const std::string& text,
    int maxLength
)
{
    std::vector<std::string> lines;
    std::string line;
    int count = 0;

    for(size_t i = 0; i < text.size();)
    {
        if(text[i] == '\n')
        {
            lines.push_back(line);
            line.clear();
            count = 0;
            i++;
            continue;
        }

        unsigned char c = (unsigned char)text[i];
        size_t len = 1;

        if((c & 0x80) == 0)         len = 1;
        else if((c & 0xE0) == 0xC0) len = 2;
        else if((c & 0xF0) == 0xE0) len = 3;
        else if((c & 0xF8) == 0xF0) len = 4;

        std::string ch = text.substr(i, len);
        line += ch;
        count++;

        if(count >= maxLength)
        {
            lines.push_back(line);
            line.clear();
            count = 0;
        }

        i += len;
    }

    if(!line.empty())
        lines.push_back(line);

    return lines;
}


void DialogueUI::Render(Renderer& renderer)
{
    int boxH = (int)displayBoxH;
    int boxY = BOX_BOTTOM - boxH;
    if(boxY < 10) boxY = 10;

    bool hasChoice = !choiceOptions.empty();



    // ---- 1. 背景模糊 ----

    if(backgroundTexture)
    {
        renderer.DrawBlurredRegion(
            backgroundTexture,
            BOX_X, boxY,
            BOX_W, boxH
        );
    }



    // ---- 2. 圆角半透明灰底 ----

    renderer.DrawFilledRoundRect(
        BOX_X, boxY, BOX_W, boxH,
        BOX_RADIUS,
        BOX_R, BOX_G, BOX_B, BOX_A
    );



    // ---- 3. 正文 ----

    std::string text = textSystem.GetCurrentText();
    auto lines = SplitTextLine(text, 34);

    if(!hasChoice)
    {
        renderer.DrawText(
            speaker,
            SPEAKER_X,
            boxY + SPEAKER_OFFSET
        );
    }

    int ty = boxY + TEXT_OFFSET;

    for(auto& line : lines)
    {
        renderer.DrawText(line, TEXT_X, ty);
        ty += LINE_GAP;
    }



    // ---- 4. 选项 ----

    if(hasChoice)
    {
        // 高亮块：用平滑后的浮点索引
        int hlY = boxY + CHOICE_OFFSET
                + (int)(displayChoiceIndex * CHOICE_GAP);

        renderer.DrawFilledRoundRect(
            CHOICE_X - 20,
            hlY - 6,
            CHOICE_HL_W,
            CHOICE_GAP - 10,
            UILayout::HL_RADIUS,
            UILayout::HL_R,
            UILayout::HL_G,
            UILayout::HL_B,
            UILayout::HL_A
        );


        // 选项文字：位置固定，只有选中项带 >
        for(int i = 0; i < (int)choiceOptions.size(); i++)
        {
            int oy = boxY + CHOICE_OFFSET + i * CHOICE_GAP;

            std::string line;

            if(i == choiceIndex) line = "> ";
            else                 line = "  ";

            line += choiceOptions[i];

            renderer.DrawText(line, CHOICE_X, oy);
        }
    }
}


bool DialogueUI::Finished()
{
    return textSystem.IsFinished();
}


void DialogueUI::Skip()
{
    textSystem.Skip();
}


void DialogueUI::Draw(Renderer& renderer)
{
    Render(renderer);
}


void DialogueUI::ShowChoice(const std::vector<std::string>& options)
{
    choiceOptions = options;
    choiceIndex = 0;

    // 显示新选项时立即对齐，避免从上一个选择事件的位置滑过来
    displayChoiceIndex = 0.0f;
}


void DialogueUI::ClearChoice()
{
    choiceOptions.clear();
    choiceIndex = 0;
    displayChoiceIndex = 0.0f;
}


bool DialogueUI::HasChoice() const
{
    return !choiceOptions.empty();
}


// ==========================================================
// MoveChoice
// ==========================================================
//
// 键盘上下切。用慢速动画。

void DialogueUI::MoveChoice(int delta)
{
    if(choiceOptions.empty()) return;

    choiceAnimSpeed = CHOICE_ANIM_SPEED;

    choiceIndex += delta;

    if(choiceIndex < 0)
        choiceIndex = (int)choiceOptions.size() - 1;

    if(choiceIndex >= (int)choiceOptions.size())
        choiceIndex = 0;
}


int DialogueUI::GetChoiceIndex() const
{
    return choiceIndex;
}


int DialogueUI::HitTestChoice(int x, int y) const
{
    if(choiceOptions.empty()) return -1;

    int boxH = (int)displayBoxH;
    int boxY = BOX_BOTTOM - boxH;

    for(int i = 0; i < (int)choiceOptions.size(); i++)
    {
        int iy = boxY + CHOICE_OFFSET + i * CHOICE_GAP;

        if(x >= CHOICE_X &&
           x <  CHOICE_X + CHOICE_HIT_W &&
           y >= iy &&
           y <  iy + CHOICE_HIT_H)
        {
            return i;
        }
    }

    return -1;
}


// ==========================================================
// SetChoiceIndex
// ==========================================================
//
// 鼠标 hover 用。用快速动画，接近跟手但仍有滑动。

void DialogueUI::SetChoiceIndex(int idx)
{
    if(idx >= 0 && idx < (int)choiceOptions.size())
    {
        choiceAnimSpeed = CHOICE_ANIM_SPEED_FAST;
        choiceIndex = idx;
    }
}


void DialogueUI::SetTextSpeed(int cps)
{
    textSystem.SetCharsPerSecond(cps);
}


void DialogueUI::SetTypingSound(
    AudioManager* audio,
    const std::string& path
)
{
    typingAudio     = audio;
    typingSoundPath = path;
}


void DialogueUI::SetBackgroundTexture(SDL_Texture* tex)
{
    backgroundTexture = tex;
}