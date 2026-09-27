#ifndef STORY_H
#define STORY_H


#include <string>
#include <vector>


#include "Character.h"
#include "History.h"
#include "RouteManager.h"



struct StoryEvent
{

    std::string name;

    std::string text;

    std::string background;

    std::string character;


    bool isChoice=false;

    bool isEnding=false;


    std::string choiceResult;

    
    std::vector<std::string> choices;


    std::vector<std::string> choiceTargets;


    float waitTime=0.0f;

};





class Story
{


public:


    Story();



    void Add(
        const StoryEvent& event
    );



    bool Load(
        const std::string& file
    );



    bool Next();



    StoryEvent GetCurrentEvent();



    bool IsEnd() const;



    Character& GetCharacter(
        const std::string& name
    );



    History& GetHistory();



    RouteManager& GetRouteManager();



    int GetIndex() const;



    void SetIndex(
        int value
    );



    void SetNextFile(
        const std::string& file
    );

    const std::string& GetNextFile() const;



    // ==========================================================
    // [新增] 当前脚本文件路径
    // ==========================================================
    //
    // 存档时需要记录"玩家在哪个脚本里"，
    // 读档时需要按这个路径重新加载，
    // 否则 index 对不上另一个章节的事件列表。

    const std::string& GetCurrentFile() const;

    // ==========================================================



public:


    std::vector<StoryEvent> events;



private:


    int currentIndex;



    std::vector<Character> characters;


    History history;


    RouteManager routeManager;



    std::string nextFile;


    // [新增] 当前脚本路径
    std::string currentFile;



};



#endif