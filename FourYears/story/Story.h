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



// ==========================================================
// StoryEvent
// ==========================================================
//
// 立绘有三个位置：左 / 中 / 右。
// 每条事件记录这三个位置"目标纹理"，空字符串表示不变。
// 特殊值 "clear" 表示清除该位置的立绘。

struct StoryEvent
{
    // ---- 通用 ----

    std::string name;
    std::string text;
    std::string background;

    // [修改] 立绘拆成三个位置
    std::string characterLeft;
    std::string characterCenter;
    std::string characterRight;

    // 音频
    std::string bgm;
    std::string se;

    float waitTime = 0.0f;


    // ---- 选择 ----

    bool isChoice = false;

    std::vector<std::string> choices;
    std::vector<std::string> choiceTargets;
    std::vector<std::vector<AffectionChange>> choiceAffection;


    // ---- 结局分支 ----

    bool isEndingBranch = false;

    std::string branchLiJunhao;
    std::string branchZhangHanyu;
    std::string branchNormal;


    // ---- 跳转 ----

    bool isGoto = false;
    std::string gotoLabel;
};



class Story
{

public:

    Story();

    void Add(const StoryEvent& event);
    bool Load(const std::string& file);
    bool Next();
    StoryEvent GetCurrentEvent();
    bool IsEnd() const;

    Character& GetCharacter(const std::string& name);
    History& GetHistory();
    RouteManager& GetRouteManager();

    int  GetIndex() const;
    void SetIndex(int value);

    void SetNextFile(const std::string& file);
    const std::string& GetNextFile()    const;
    const std::string& GetCurrentFile() const;

    void AddLabel(const std::string& name, int eventIndex);
    bool HasLabel(const std::string& name) const;
    int  GetLabel(const std::string& name) const;
    bool JumpToLabel(const std::string& name);



public:

    std::vector<StoryEvent> events;



private:

    void SkipGotoChain();



private:

    int currentIndex;

    std::vector<Character> characters;
    History       history;
    RouteManager  routeManager;

    std::string nextFile;
    std::string currentFile;

    std::map<std::string, int> labels;

};


#endif