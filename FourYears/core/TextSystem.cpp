#include "TextSystem.h"



TextSystem::TextSystem()
{

    currentIndex = 0;

    timer = 0;

    speed = 0.05f;

    finished = true;

}






void TextSystem::Init()
{

    fullText = "";

    displayText = "";

    currentIndex = 0;

    finished = true;

}






void TextSystem::SetText(
    const std::string& text
)
{

    fullText = text;


    displayText = "";


    currentIndex = 0;


    timer = 0;


    finished = false;


}






void TextSystem::Update()
{

    if(finished)
        return;



    timer += 0.016f;



    if(timer >= speed)
    {


        timer = 0;



        if(currentIndex < fullText.size())
        {

            displayText +=
                fullText[currentIndex];


            currentIndex++;

        }


        else
        {

            finished = true;

        }


    }


}






void TextSystem::Skip()
{


    displayText = fullText;


    currentIndex =
        fullText.size();



    finished = true;


}






bool TextSystem::Finished()
{

    return finished;

}






std::string TextSystem::GetText()
{

    return displayText;

}






std::string TextSystem::GetCurrentText()
{

    return currentText;

}





bool TextSystem::IsFinished()
{

    return Finished();

}