#include "ScriptPlayer.h"


#include "Renderer.h"


#include <fstream>
#include <iostream>



ScriptPlayer::ScriptPlayer()
{

    currentIndex = 0;

    finished = true;

}






bool ScriptPlayer::LoadScript(
    const std::string& path
)
{


    std::ifstream file(path);



    if(!file.is_open())
    {

        std::cout
        << "Cannot open script: "
        << path
        << std::endl;


        return false;

    }



    commands.clear();



    std::string line;



    while(std::getline(file,line))
    {


        if(line.empty())
            continue;



        ParseLine(line);


    }



    file.close();



    currentIndex = 0;


    finished = false;



    NextCommand();



    return true;


}







void ScriptPlayer::ParseLine(
    const std::string& line
)
{


    ScriptCommand cmd;



    if(line[0]=='[')
    {


        size_t end =
            line.find(']');



        if(end != std::string::npos)
        {

            cmd.type =
                line.substr(
                    1,
                    end-1
                );


            cmd.value = "";

        }


    }

    else
    {

        cmd.type="TEXT";

        cmd.value=line;

    }



    commands.push_back(cmd);


}







void ScriptPlayer::NextCommand()
{


    if(currentIndex >= commands.size())
    {

        finished=true;

        return;

    }




    ScriptCommand& cmd =
        commands[currentIndex];



    currentIndex++;





    if(cmd.type=="背景")
    {

        if(currentIndex < commands.size())
        {

            background =
                commands[currentIndex].value;


            currentIndex++;

        }


        NextCommand();

    }





    else if(cmd.type=="立绘")
    {


        if(currentIndex < commands.size())
        {

            character =
                commands[currentIndex].value;


            currentIndex++;

        }


        NextCommand();

    }





    else if(cmd.type!="TEXT")
    {


        speaker =
            cmd.type;


        if(currentIndex < commands.size())
        {

            textSystem.SetText(
                commands[currentIndex].value
            );


            currentIndex++;

        }


    }



}







void ScriptPlayer::Update()
{


    textSystem.Update();



    if(textSystem.Finished())
    {

        // 后面接鼠标点击继续
        // 现在自动播放

    }


}







void ScriptPlayer::Draw(
    Renderer& renderer
)
{


    // 当前版本先留空


    // 后续接：
    //
    // renderer.DrawTexture(background)
    //
    // renderer.DrawTexture(character)
    //
    // UI显示textSystem.GetText()


}







bool ScriptPlayer::Finished()
{

    return finished;

}







std::string ScriptPlayer::GetBackground()
{

    return background;

}







std::string ScriptPlayer::GetCharacter()
{

    return character;

}







std::string ScriptPlayer::GetSpeaker()
{

    return speaker;

}







TextSystem& ScriptPlayer::GetTextSystem()
{

    return textSystem;

}