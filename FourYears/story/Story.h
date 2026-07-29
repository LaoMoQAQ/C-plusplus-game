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



public:


    std::vector<StoryEvent> events;



private:


    int currentIndex;



    std::vector<Character> characters;


    History history;


    RouteManager routeManager;



};



#endif