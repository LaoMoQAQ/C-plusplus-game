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










// UTF8简单分行

std::vector<std::string>
SplitTextLine(
    const std::string& text,
    int maxChar
)
{


    std::vector<std::string> lines;


    std::string line;



    int count=0;



    for(size_t i=0;i<text.size();)
    {


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



        if(
            count>=maxChar
        )
        {

            lines.push_back(
                line
            );


            line.clear();


            count=0;

        }



        i+=len;


    }






    if(
        !line.empty()
    )
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