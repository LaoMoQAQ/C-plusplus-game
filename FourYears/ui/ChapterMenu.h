#ifndef CHAPTER_MENU_H
#define CHAPTER_MENU_H


#include <string>
#include <vector>

#include "../core/Renderer.h"
#include "../story/StoryGraph.h"

#include "MenuCommon.h"


// ==========================================================
// ChapterMenu
// ==========================================================
//
// 章节选择。按 StoryGraph 扫描出的剧本结构列出所有章节：
//   - 已走过的节点打勾
//   - 未解锁的节点显示成灰色
//   - Enter 跳到该章节开头
//
// 布局沿用暂停菜单那套：屏幕中央一块玻璃面板，
// 列表在面板内左对齐，超出可视行数时按选择滚动。

class ChapterMenu
{

public:

    void SetGraph(StoryGraph* graph);

    void SetFontManager(FontManager* fm) { fontManager = fm; }

    void Reset();

    // 打开时滚到第一个没走过的节点
    void FocusFirstUnvisited();

    void Update(float delta);

    void Render(Renderer& renderer);

    void HandleInput(int key);

    void HandleMouseMove(int x, int y);
    MenuMouseResult HandleMouseClick(int x, int y);

    int GetChoice() const;

    // 当前选中节点对应的剧本路径（没有返回空串）
    std::string GetSelectedFile() const;

    // 当前选中的章节解锁了没有
    bool CanEnterSelected() const;



private:

    int NodeCount() const;

    const StoryNode* NodeAt(int index) const;


    StoryGraph* graph = nullptr;

    FontManager* fontManager = nullptr;

    int choice = 0;

    int scroll = 0;

    float displayHighlightY = 0.0f;

    float appear = 0.0f;


    // ---- 布局 ----
    //
    // 面板固定宽高，内部列表逐行排。

    static constexpr int PANEL_W = 720;
    static constexpr int PANEL_H = 706;
    static constexpr int PANEL_X = (UILayout::SCREEN_W - PANEL_W) / 2;
    static constexpr int PANEL_Y = 96;
    static constexpr int PANEL_RADIUS = 22;

    static constexpr int PAD_X = 46;      // 列表左右内边距
    static constexpr int LIST_Y = PANEL_Y + 108;
    static constexpr int LIST_GAP = 52;
    static constexpr int ROW_H = 44;

    static constexpr int VISIBLE_ROWS = 10;

    static constexpr float HL_ANIM_SPEED = 0.25f;

};


#endif
