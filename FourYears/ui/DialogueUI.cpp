#include "DialogueUI.h"


#include <vector>




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









// [重写] 自动换行 + 强制换行
//
// 规则：
//   1. 遇到 '\n'  -> 立即结束当前行，
//                    开始新行（强制换行）。
//   2. 已累计到 maxChar 个字 -> 软换行。
//
// 这个函数是 DialogueUI 的成员函数，
// 渲染时由 DialogueUI::Render 调用。
//
// 注意：'\\n' 永远不会传到 Renderer::DrawText，
// 所以不会出现“方框”问题。
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

        // [新增] 强制换行：
        // 直接看原始字节，'\n' 是单字节，
        // 不需要走 UTF-8 解码逻辑。
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


    //
    // 人名
    //

    renderer.DrawText(
        speaker,
        80,
        520
    );






    //
    // 正文
    //

    std::string text =
    textSystem.GetCurrentText();




    auto lines =
    SplitTextLine(
        text,
        34
    );




    int y=580;



    for(
        auto& line : lines
    )
    {


        renderer.DrawText(
            line,
            80,
            y
        );

        y+=45;

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