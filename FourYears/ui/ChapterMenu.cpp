#include "ChapterMenu.h"

#include "../core/Anim.h"



void ChapterMenu::SetGraph(StoryGraph* g)
{
    graph = g;
}




int ChapterMenu::NodeCount() const
{
    if(graph == nullptr)
        return 0;

    return (int)graph->GetNodes().size();
}




const StoryNode* ChapterMenu::NodeAt(int index) const
{
    if(graph == nullptr)
        return nullptr;

    const auto& nodes = graph->GetNodes();

    if(index < 0 || index >= (int)nodes.size())
        return nullptr;

    return &nodes[index];
}




void ChapterMenu::Reset()
{
    choice = 0;
    scroll = 0;
    appear = 0.0f;
    displayHighlightY = (float)LIST_Y;
}




void ChapterMenu::FocusFirstUnvisited()
{
    const int n = NodeCount();

    if(n <= 0)
        return;

    for(int i = 0; i < n; i++)
    {
        const StoryNode* node = NodeAt(i);

        if(node && !node->visited)
        {
            choice = i;
            break;
        }
    }

    if(choice < 0) choice = 0;
    if(choice >= n) choice = n - 1;

    scroll = 0;
}




void ChapterMenu::Update(float delta)
{
    const int n = NodeCount();

    // 保证选中项在可见范围内
    if(choice < scroll)
        scroll = choice;

    if(choice >= scroll + VISIBLE_ROWS)
        scroll = choice - VISIBLE_ROWS + 1;


    // 高亮块平滑跟随
    float target =
        (float)(LIST_Y + (choice - scroll) * LIST_GAP);

    float diff = target - displayHighlightY;

    if(diff > -0.5f && diff < 0.5f)
        displayHighlightY = target;
    else
        displayHighlightY += diff * SmoothFactor(HL_ANIM_SPEED, delta);


    // 入场动画
    if(appear < 1.0f)
    {
        appear += delta * 6.0f;

        if(appear > 1.0f)
            appear = 1.0f;
    }

    (void)n;
}




void ChapterMenu::Render(Renderer& renderer)
{
    float eased = Anim::EaseOutCubic(appear);


    // ---- 背景：压暗 + 玻璃面板 ----

    DrawMenuBackdrop(
        renderer,
        (float)PANEL_X, (float)PANEL_Y,
        (float)PANEL_W, (float)PANEL_H,
        (float)PANEL_RADIUS
    );


    renderer.SetContentOffset(0.0f, (1.0f - eased) * 14.0f);
    renderer.SetContentAlpha(eased);


    // ---- 标题 ----

    if(fontManager)
        DrawMenuTitle(renderer, *fontManager, "章节选择", PANEL_Y + 34);
    else
        renderer.DrawText("章节选择", PANEL_X + 260, PANEL_Y + 34);


    // ---- 空列表 ----

    if(NodeCount() <= 0)
    {
        renderer.DrawText("（没有可用的章节）", PANEL_X + PAD_X, (float)LIST_Y);
        renderer.SetContentOffset(0.0f, 0.0f);
        renderer.SetContentAlpha(1.0f);
        return;
    }


    // ---- 高亮块 ----

    int row = choice - scroll;

    int hlY = LIST_Y + row * LIST_GAP;

    renderer.DrawFilledRoundRect(
        PANEL_X + PAD_X - 14,
        hlY - 8,
        PANEL_W - PAD_X * 2 + 28,
        ROW_H,
        UILayout::HL_RADIUS,
        UILayout::HL_R,
        UILayout::HL_G,
        UILayout::HL_B,
        (Uint8)(UILayout::HL_A * eased)
    );


    // ---- 列表 ----

    TTF_Font* smallFont = fontManager ? fontManager->GetFont(22) : nullptr;

    for(int i = 0; i < VISIBLE_ROWS; i++)
    {
        int idx = scroll + i;

        const StoryNode* node = NodeAt(idx);

        if(!node)
            break;

        int iy = LIST_Y + i * LIST_GAP;

        // 已走过打勾，未解锁显示成灰
        std::string prefix = node->visited ? "✓ " : "  ";

        if(idx == choice)
            prefix += "> ";

        std::string text = prefix + node->title;

        bool locked = graph && !graph->CanEnter(idx);

        if(locked && fontManager)
        {
            renderer.DrawText(
                text,
                PANEL_X + PAD_X,
                iy,
                smallFont ? smallFont : fontManager->GetFont(),
                SDL_Color{ 120, 124, 134, 255 }
            );
        }
        else
        {
            renderer.DrawText(text, PANEL_X + PAD_X, iy);
        }


        // 副标题贴右侧（灰字）
        if(smallFont)
        {
            std::string sub = StoryGraph::SubtitleFor(node->file);

            if(!sub.empty())
            {
                int sw = 0, sh = 0;

                if(renderer.MeasureText(sub, smallFont, sw, sh))
                {
                    renderer.DrawText(
                        sub,
                        PANEL_X + PANEL_W - PAD_X - sw,
                        iy + 6,
                        smallFont,
                        SDL_Color{ 150, 154, 166, 200 }
                    );
                }
            }
        }
    }


    renderer.SetContentOffset(0.0f, 0.0f);
    renderer.SetContentAlpha(1.0f);


    // ---- 底部按键提示（面板外）----

    if(fontManager)
    {
        DrawKeyHints(
            renderer, *fontManager, 826,
            {
                { "↑↓",  "选择" },
                { "Enter", "进入" },
                { "ESC",  "返回" }
            }
        );
    }
}




void ChapterMenu::HandleInput(int key)
{
    const int n = NodeCount();

    if(n <= 0)
        return;

    // 参数约定：1 = 上，2 = 下
    //
    // 这是 UIManager::HandleInput(int) 的统一约定，
    // 由 Game::HandleKeyDown 里的 ui.HandleInput(1) / (2) 传入。
    // 之前这里写的是比较 SDLK_UP / SDLK_DOWN，永远不成立，
    // 所以键盘完全没反应。
    if(key == 1)
    {
        choice--;

        if(choice < 0)
            choice = n - 1;
    }
    else if(key == 2)
    {
        choice++;

        if(choice >= n)
            choice = 0;
    }
}




void ChapterMenu::HandleMouseMove(int x, int y)
{
    const int n = NodeCount();

    for(int i = 0; i < VISIBLE_ROWS && scroll + i < n; i++)
    {
        int iy = LIST_Y + i * LIST_GAP;

        if(x >= PANEL_X + PAD_X - 14 &&
           x < PANEL_X + PANEL_W - PAD_X + 14 &&
           y >= iy - 8 && y < iy - 8 + ROW_H)
        {
            choice = scroll + i;
            return;
        }
    }
}




MenuMouseResult ChapterMenu::HandleMouseClick(int x, int y)
{
    const int n = NodeCount();

    for(int i = 0; i < VISIBLE_ROWS && scroll + i < n; i++)
    {
        int iy = LIST_Y + i * LIST_GAP;

        if(x >= PANEL_X + PAD_X - 14 &&
           x < PANEL_X + PANEL_W - PAD_X + 14 &&
           y >= iy - 8 && y < iy - 8 + ROW_H)
        {
            choice = scroll + i;
            return MenuMouseResult::ACTIVATE;
        }
    }

    return MenuMouseResult::NONE;
}




int ChapterMenu::GetChoice() const
{
    return choice;
}




std::string ChapterMenu::GetSelectedFile() const
{
    const StoryNode* node = NodeAt(choice);

    return node ? node->file : "";
}




bool ChapterMenu::CanEnterSelected() const
{
    if(graph == nullptr)
        return false;

    return graph->CanEnter(choice);
}
