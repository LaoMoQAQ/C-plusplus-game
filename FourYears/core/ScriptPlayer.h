#ifndef SCRIPT_PLAYER_H
#define SCRIPT_PLAYER_H


#include <string>
#include <vector>


#include "TextSystem.h"



class Renderer;



struct ScriptCommand
{

    std::string type;


    std::string value;


};




class ScriptPlayer
{

public:


    ScriptPlayer();



    bool LoadScript(
        const std::string& path
    );



    void Update();



    void Draw(
        Renderer& renderer
    );



    bool Finished();



    std::string GetBackground();



    std::string GetCharacter();



    std::string GetSpeaker();



    TextSystem& GetTextSystem();



private:


    void ParseLine(
        const std::string& line
    );



    void NextCommand();



private:


    std::vector<ScriptCommand> commands;



    int currentIndex;



    bool finished;



    std::string background;



    std::string character;



    std::string speaker;



    TextSystem textSystem;



};



#endif