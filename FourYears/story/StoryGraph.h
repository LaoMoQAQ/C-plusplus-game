#ifndef STORY_GRAPH_H
#define STORY_GRAPH_H


#include <set>
#include <string>
#include <vector>


// ==========================================================
// 剧情结构图
// ==========================================================
//
// 扫描 script/*.txt，把每个剧本文件当一个节点，把文件里出现的
// "script/xxx.txt" 引用（[下一章] / [跳转] / [选择] / [结局分支]）
// 当作有向边，最后从第一章做一次 BFS 排出树状层级。
//
// 用途：
//   * 章节选择界面（ChapterMenu）按树的形状列出来
//   * 记录玩家走到过哪些节点（存档目录里的 progress.dat）

struct StoryNode
{
    std::string file;      // "script/chapter01.txt"
    std::string title;     // 显示名
    int depth = 0;         // 树里的层级，用来做缩进
    int order = 0;         // 章节顺序（线性解锁用）
    bool visited = false;  // 玩家是否到达过

    std::vector<std::string> children;  // 指向的下游文件
};


class StoryGraph
{

public:

    // 扫描目录并建立结构。rootFile 是树根（一般是第一章）。
    void Build(
        const std::string& scriptDir,
        const std::string& rootFile
    );


    const std::vector<StoryNode>& GetNodes() const;


    int IndexOf(
        const std::string& file
    ) const;


    void MarkVisited(
        const std::string& file
    );


    bool IsVisited(
        const std::string& file
    ) const;


    // ==========================================================
    // 线性解锁
    // ==========================================================
    //
    // 章节按固定顺序排：看过第 N 章之后，第 N+1 章才解锁。
    // 更后面的（还有没看过的）都锁着，显示成灰色。

    int UnlockedUpTo() const;

    bool CanEnter(
        int nodeIndex
    ) const;

    // 说明文字用的：越过的最高章节序号
    int FurthestVisitedOrder() const;

    // 进度存档：一行一个剧本路径
    void LoadProgress(
        const std::string& path
    );

    void SaveProgress(
        const std::string& path
    ) const;


    // 文件名 → 章节显示名
    static std::string TitleFor(
        const std::string& file
    );

    // 文件名 → 章节副标题（进场标题用）
    static std::string SubtitleFor(
        const std::string& file
    );


private:

    std::vector<StoryNode> nodes;

    std::set<std::string> visited;

    std::string directory;

};


#endif
