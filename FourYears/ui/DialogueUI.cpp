#include "DialogueUI.h"


#include <vector>




// ==========================================================
// 对话框与选项的布局、配色
// ==========================================================
//
// 选项鼠标命中判定和绘制共用同一套常量，
// 改位置时两边一起生效。

namespace
{

    // 对话框
    constexpr int BOX_X = 40;
    constexpr int BOX_Y = 520;
    constexpr int BOX_W = 1520;
    constexpr int BOX_H = 340;

    constexpr Uint8 BOX_R = 20;
    constexpr Uint8 BOX_G = 20;
    constexpr Uint8 BOX_B = 20;
    constexpr Uint8 BOX_A = 190;

    constexpr int SPEAKER_X = 80;
    constexpr int SPEAKER_Y = 545;

    constexpr int TEXT_X = 80;
    constexpr int TEXT_Y = 615;

    constexpr int LINE_GAP = 45;


    // 选项
    constexpr int CHOICE_X = 120;
    constexpr int CHOICE_Y = 570;
    constexpr int CHOICE_GAP = 70;

    // 鼠标命中判定用的矩形
    // 宽 600 足够覆盖长选项文本，高 = 行高
    constexpr int CHOICE_HIT_W = 600;
    constexpr int CHOICE_HIT_H = 55;

}

// ==========================================================




DialogueUI::DialogueUI()
{

}









void DialogueUI::SetSpeaker(
    const std::string& name
)
{

    speaker=name;

}









void DialogueUI::SetText(
    const std::string& text,
    float wait
)
{

    textSystem.SetText(
        text,
        wait
    );

}









void DialogueUI::Update()
{

    textSystem.Update();

}









std::vector<std::string> DialogueUI::SplitTextLine(
    const std::string& text,
    int maxLength
)
{

    std::vector<std::string> lines;


    std::string line;


    int count=0;


    for(size_t i=0;i<text.size();)
    {

        if(
            text[i]=='\n'
        )
        {

            lines.push_back(
                line
            );


            line.clear();


            count=0;


            i++;


            continue;

        }



        unsigned char c=
        (unsigned char)text[i];


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



        std::string ch=
        text.substr(
            i,
            len
        );


        line+=ch;

        count++;


        if(count>=maxLength)
        {

            lines.push_back(
                line
            );


            line.clear();


            count=0;

        }



        i+=len;

    }



    if(!line.empty())
    {

        lines.push_back(
            line
        );

    }



    return lines;

}









void DialogueUI::Render(
    Renderer& renderer
)
{

    renderer.DrawFilledRect(
        BOX_X,
        BOX_Y,
        BOX_W,
        BOX_H,
        BOX_R,
        BOX_G,
        BOX_B,
        BOX_A
    );



    if(!HasChoice())
    {

        renderer.DrawText(
            speaker,
            SPEAKER_X,
            SPEAKER_Y
        );

    }




    std::string text =
    textSystem.GetCurrentText();




    auto lines =
    SplitTextLine(
        text,
        34
    );




    int y=TEXT_Y;



    for(
        auto& line : lines
    )
    {


        renderer.DrawText(
            line,
            TEXT_X,
            y
        );

        y+=LINE_GAP;

    }




    if(!choiceOptions.empty())
    {

        int oy = CHOICE_Y;

        for(
            int i=0;
            i<(int)choiceOptions.size();
            i++
        )
        {

            std::string line;


            if(i == choiceIndex)
            {
                line = "> ";
            }
            else
            {
                line = "  ";
            }


            line += choiceOptions[i];


            renderer.DrawText(
                line,
                CHOICE_X,
                oy
            );


            oy += CHOICE_GAP;

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









void DialogueUI::Draw(
    Renderer& renderer
)
{

    Render(
        renderer
    );

}





// ==========================================================
// 选项显示
// ==========================================================

void DialogueUI::ShowChoice(
    const std::vector<std::string>& options
)
{

    choiceOptions = options;

    choiceIndex = 0;

}





void DialogueUI::ClearChoice()
{

    choiceOptions.clear();

    choiceIndex = 0;

}





bool DialogueUI::HasChoice() const
{

    return !choiceOptions.empty();

}





void DialogueUI::MoveChoice(
    int delta
)
{

    if(choiceOptions.empty())
    {
        return;
    }


    choiceIndex += delta;


    if(choiceIndex < 0)
    {
        choiceIndex =
            (int)choiceOptions.size() - 1;
    }


    if(choiceIndex >= (int)choiceOptions.size())
    {
        choiceIndex = 0;
    }

}





int DialogueUI::GetChoiceIndex() const
{

    return choiceIndex;

}





// ==========================================================
// [新增] 鼠标点击命中判定
// ==========================================================

int DialogueUI::HitTestChoice(
    int x,
    int y
) const
{

    if(choiceOptions.empty())
    {
        return -1;
    }


    for(
        int i=0;
        i<(int)choiceOptions.size();
        i++
    )
    {

        int iy = CHOICE_Y + i * CHOICE_GAP;


        if(
            x >= CHOICE_X &&
            x <  CHOICE_X + CHOICE_HIT_W &&
            y >= iy &&
            y <  iy + CHOICE_HIT_H
        )
        {
            return i;
        }

    }


    return -1;

}





void DialogueUI::SetChoiceIndex(
    int idx
)
{

    if(
        idx >= 0 &&
        idx < (int)choiceOptions.size()
    )
    {
        choiceIndex = idx;
    }

}




// ==========================================================
// [新增] 设置文字速度
// ==========================================================

void DialogueUI::SetTextSpeed(int cps)
{

    textSystem.SetCharsPerSecond(cps);

}