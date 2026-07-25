#pragma once

#include <string>
#include <vector>


using namespace std;



//================================
// 单个剧情事件
//================================

struct StoryEvent
{


    //角色名
    string name;



    //正文
    string text;



    //背景
    string background;



    //立绘
    string character;



    //选择
    bool isChoice=false;



    //选项
    vector<string> choices;



    //路线标记
    string choiceResult;



    //是否结局
    bool isEnding=false;


};






//================================
// 剧情容器
//================================

class Story
{

public:


    vector<StoryEvent> events;



    StoryEvent& Get(
        int index
    )
    {

        return events[index];

    }
    


    void Add(
        StoryEvent event
    )
    {

        events.push_back(event);

    }




    void Clear()
    {

        events.clear();

    }




    size_t Size()
    {

        return events.size();

    }


};