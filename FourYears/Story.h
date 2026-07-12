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



    //背景图片
    //例如：
    //school_gate.png
    string background;



    //人物立绘
    //例如：
    //wuhongtao.png
    string character;



    //是否为选择
    bool isChoice=false;



    //选择内容
    //例如：
    //  去图书馆
    //  回宿舍
    vector<string> choices;



    //每个选择对应的结果
    //
    // choices[0]
    // 对应
    // choiceResult[0]
    //
    // 例如：
    // li
    // zhang
    // none
    string choiceResult;


};






//================================
// 剧情容器
//================================

class Story
{

public:


    vector<StoryEvent> events;



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