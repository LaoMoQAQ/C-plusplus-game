#include "DialogueUI.h"


#include <vector>




// ==========================================================
// 对话框与选项的布局、配色
// ==========================================================
//
// 全部按 1600x900 屏幕估算。
//
// 对话框矩形 BOX_X/Y/W/H
// 选项都放在对话框内部，
// 位置从 BOX 派生，不单独写死，
// 这样调整对话框时选项会跟着走。

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
    // 位置基于对话框内部，
    // 不再浮在屏幕中央。
    constexpr int CHOICE_X = 120;
    constexpr int CHOICE_Y = 570;
    constexpr int CHOICE_GAP = 70;

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

    // ------------------------------------------------
    // 对话框背景
    // ------------------------------------------------

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



    // ------------------------------------------------
    // 人名
    // ------------------------------------------------
    //
    // [修改] 显示选项时，不画人名。
    //
    // 选择事件的 speaker 是解析器写死的 "选择"，
    // 那不是一个真正的角色名。
    // 判断 HasChoice() 就能区分：
    //   - 有选项   -> 是选择事件，跳过人名
    //   - 没有选项 -> 正常对话，照常画人名

    if(!HasChoice())
    {

        renderer.DrawText(
            speaker,
            SPEAKER_X,
            SPEAKER_Y
        );

    }




    // ------------------------------------------------
    // 正文
    // ------------------------------------------------
    //
    // 选择事件没有正文，
    // textSystem.GetCurrentText() 返回空字符串，
    // SplitTextLine 返回空 vector，
    // 下面的循环一次都不执行。

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




    // ------------------------------------------------
    // 选项
    // ------------------------------------------------
    //
    // [修改] 选项现在画在对话框内部，
    // 位置由 CHOICE_X / CHOICE_Y 决定。
    // 选中项带 "> " 前缀，
    // 未选中项用两个空格缩进，保持左对齐。

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