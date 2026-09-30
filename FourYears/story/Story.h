#ifndef STORY_H
#define STORY_H


#include <string>
#include <vector>
#include <map>


#include "Character.h"
#include "History.h"
#include "RouteManager.h"



struct AffectionChange
{
    std::string character;
    int delta;
};



struct StoryEvent
{

    std::string name;

    std::string text;

    std::string background;

    std::string character;


    bool isChoice=false;

    bool isEnding=false;


    // ==========================================================
    // [新增] 同文件跳转事件
    // ==========================================================
    //
    // 由 [跳转] #名字 生成。
    // Story::Next() 与 JumpToLabel() 会自动跳过它，
    // 不会显示到对话框里。

    bool isGoto = false;
    std::string gotoLabel;

    // ==========================================================


    std::string choiceResult;

    
    std::vector<std::string> choices;

    std::vector<std::string> choiceTargets;


    std::vector<std::vector<AffectionChange>> choiceAffection;



    bool isEndingBranch = false;

    std::string branchLiJunhao;
    std::string branchZhangHanyu;
    std::string branchNormal;



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



    const std::string& GetCurrentFile() const;



    // ==========================================================
    // [新增] 标签系统
    // ==========================================================
    //
    // StoryParser 解析到 [标签] 名字 时，
    // 把当前事件列表的长度记下来。
    //
    // 之后可以通过 JumpToLabel("名字")
    // 跳到这个位置。

    void AddLabel(
        const std::string& name,
        int eventIndex
    );

    bool HasLabel(
        const std::string& name
    ) const;

    int GetLabel(
        const std::string& name
    ) const;

    // 跳转到标签位置，返回是否成功。
    // 会自动跳过途中遇到的 [跳转] 链。
    bool JumpToLabel(
        const std::string& name
    );

    // ==========================================================



public:


    std::vector<StoryEvent> events;



private:


    int currentIndex;



    std::vector<Character> characters;


    History history;


    RouteManager routeManager;



    std::string nextFile;


    std::string currentFile;


    // [新增] 标签名 -> 事件索引
    std::map<std::string, int> labels;



};



#endif