#include "TextSystem.h"

#include <iostream>


TextSystem::TextSystem()
{

    currentIndex=0;

    timer=0.0f;

    speed=0.035f;


    waitTimer=0.0f;

    waitTarget=0.0f;


    punctuationTimer=0.0f;

    punctuationTarget=0.0f;


    finished=true;

}








void TextSystem::Init()
{

    fullText.clear();

    displayText.clear();

    characters.clear();


    currentIndex=0;


    timer=0;


    waitTimer=0;

    waitTarget=0;


    punctuationTimer=0;

    punctuationTarget=0;


    finished=true;


}










// UTF-8字符切割

void TextSystem::SplitUTF8()
{


    characters.clear();



    for(size_t i=0;i<fullText.size();)
    {


        unsigned char c =
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

        else
        {

            // 非法UTF8跳过

            i++;

            continue;

        }





        if(
            i+len <= fullText.size()
        )
        {

            std::string ch =
            fullText.substr(
                i,
                len
            );



            characters.push_back(
                ch
            );

        }



        i+=len;


    }



}










// 标点停顿

float TextSystem::GetPunctuationDelay(
    const std::string& ch
)
{


    if(
        ch=="，" ||
        ch==","
    )
    {

        return 0.08f;

    }



    if(
        ch=="。"
        ||
        ch=="."
    )
    {

        return 0.18f;

    }




    if(
        ch=="！"
        ||
        ch=="!"
        ||
        ch=="？"
        ||
        ch=="?"
    )
    {

        return 0.25f;

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



    /*
        修复中文省略号

        防止字体缺失导致方框

    */


    std::string replaceText;



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



        std::string ch =
        fullText.substr(
            i,
            len
        );



        // 中文省略号替换

        if(
            ch=="…"
        )
        {

            replaceText+=".";

        }

        else
        {

            replaceText+=ch;

        }


        i+=len;


    }



    fullText=
    replaceText;






    displayText.clear();


    currentIndex=0;


    timer=0;


    waitTimer=0;


    waitTarget=wait;



    punctuationTimer=0;

    punctuationTarget=0;



    finished=false;



    SplitUTF8();



}









void TextSystem::Update()
{

    if(finished)
    {

        return;

    }



    float delta=0.016f;





    if(
        waitTimer < waitTarget
    )
    {

        waitTimer+=delta;

        return;

    }






    if(
        punctuationTimer
        <
        punctuationTarget
    )
    {

        punctuationTimer+=delta;

        return;

    }





    timer+=delta;






    while(
        timer>=speed
    )
    {


        timer-=speed;




        if(
            currentIndex
            >=
            characters.size()
        )
        {

            finished=true;

            break;

        }






        std::string ch =
        characters[currentIndex];




        displayText+=ch;



        punctuationTarget =
        GetPunctuationDelay(
            ch
        );



        punctuationTimer=0;



        currentIndex++;





        if(
            punctuationTarget>0
        )
        {

            break;

        }



    }



}









void TextSystem::Skip()
{


    displayText=fullText;


    currentIndex=
    characters.size();



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