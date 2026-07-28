#include "DialogueUI.h"



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

    const std::string& text

)
{

    textSystem.SetText(
        text
    );


}








void DialogueUI::Update()
{

    textSystem.Update();


}








void DialogueUI::Render(

    Renderer& renderer

)
{

    renderer.DrawText(
        speaker,
        80,
        520
    );


    renderer.DrawText(
        text,
        80,
        580
    );


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

    Render(renderer);

}