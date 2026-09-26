#include "StartMenu.h"



StartMenu::StartMenu()
{

    choice=0;


    items[0]="开始游戏";

    // [修改] 从"读取存档"改成"存档"，
    // 进入后是 LOAD 页面，同时支持 N/C/Delete/R 管理。
    items[1]="存档";

    items[2]="设置";

    items[3]="退出游戏";

}





void StartMenu::SetFontManager(
    FontManager* fm
)
{

    fontManager = fm;

}





void StartMenu::Render(
    Renderer& renderer
)
{

    // ============================================
    // 标题区（左上）
    // ============================================


    if(fontManager)
    {

        TTF_Font* titleFont =
            fontManager->GetFont(
                "resource/font/title.ttf",
                90
            );


        if(titleFont)
        {

            SDL_Color c;
            c.r = 255;
            c.g = 255;
            c.b = 255;
            c.a = 255;


            renderer.DrawText(
                "FourYears",
                180,
                80,
                titleFont,
                c
            );

        }




        TTF_Font* subFont =
            fontManager->GetFont(28);


        if(subFont)
        {

            SDL_Color c;
            c.r = 255;
            c.g = 255;
            c.b = 255;
            c.a = 255;


            renderer.DrawText(
                "四年 · 我们的青春",
                280,
                220,
                subFont,
                c
            );

        }

    }
    else
    {

        renderer.DrawText(
            "Four Years",
            80,
            60
        );

    }





    // ============================================
    // 菜单区（右侧竖排）
    // ============================================


    for(int i=0;i<ITEM_COUNT;i++)
    {

        std::string text;


        if(i==choice)
        {
            text = "> " + items[i];
        }
        else
        {
            text = items[i];
        }


        renderer.DrawText(

            text,

            MENU_X,

            MENU_Y + i * MENU_GAP

        );

    }

}





void StartMenu::HandleInput(
    int key
)
{

    // 上

    if(key==1)
    {

        choice--;


        if(choice<0)
        {
            choice=ITEM_COUNT-1;
        }

    }



    // 下

    else if(key==2)
    {

        choice++;


        if(choice>=ITEM_COUNT)
        {
            choice=0;
        }

    }

}





void StartMenu::HandleMouseMove(
    int x,
    int y
)
{

    for(int i=0;i<ITEM_COUNT;i++)
    {

        int ix = MENU_X;

        int iy = MENU_Y + i * MENU_GAP;


        if(
            x >= ix &&
            x <  ix + ITEM_W &&
            y >= iy &&
            y <  iy + ITEM_H
        )
        {

            choice = i;

            return;

        }

    }

}





MenuMouseResult StartMenu::HandleMouseClick(
    int x,
    int y
)
{

    // 主菜单没有返回按钮（已经在最顶层），
    // 只需要检测菜单项。

    for(int i=0;i<ITEM_COUNT;i++)
    {

        int ix = MENU_X;

        int iy = MENU_Y + i * MENU_GAP;


        if(
            x >= ix &&
            x <  ix + ITEM_W &&
            y >= iy &&
            y <  iy + ITEM_H
        )
        {

            choice = i;

            return MenuMouseResult::ACTIVATE;

        }

    }


    return MenuMouseResult::NONE;

}





int StartMenu::GetChoice() const
{

    return choice;

}





void StartMenu::Reset()
{

    choice=0;

}