#include "HistoryMenu.h"


#include <string>




// ==========================================================
// 历史记录文本清理
// ==========================================================
//
// 历史记录保存的是 StoryEvent.text 原文，
// 包含 '…' 和 '\n'。
// SDL_ttf 找不到这两个字形时会画方框，
// 所以显示前先替换：
//   '…'  -> '.'
//   '\n' -> ' '

namespace
{

    std::string CleanHistoryText(
        const std::string& text
    )
    {

        std::string result;


        for(size_t i=0;i<text.size();)
        {

            if(text[i]=='\n')
            {
                result += ' ';
                i++;
                continue;
            }


            unsigned char c =
                (unsigned char)text[i];

            size_t len = 1;

            if((c & 0x80) == 0)
            {
                len = 1;
            }
            else if((c & 0xE0) == 0xC0)
            {
                len = 2;
            }
            else if((c & 0xF0) == 0xE0)
            {
                len = 3;
            }
            else if((c & 0xF8) == 0xF0)
            {
                len = 4;
            }


            std::string ch =
                text.substr(i, len);


            if(ch == "…")
            {
                result += '.';
            }
            else
            {
                result += ch;
            }


            i += len;

        }


        return result;

    }

}

// ==========================================================




HistoryMenu::HistoryMenu()
{

    history = nullptr;

    offset = 0;

}





void HistoryMenu::SetHistory(
    History* history
)
{

    this->history = history;

}





void HistoryMenu::Render(
    Renderer& renderer
)
{


    renderer.DrawText(
        "历史记录",
        500,
        80
    );



    if(history==nullptr)
    {

        renderer.DrawText(
            "暂无记录",
            500,
            200
        );

        renderer.DrawText(
            "返回 [ESC]",
            UILayout::BACK_X,
            UILayout::BACK_Y
        );

        return;

    }




    const auto& list =
    history->GetAll();




    int y = 160;




    for(
        int i=offset;
        i<(int)list.size()
        &&
        i<offset+8;
        i++
    )
    {


        std::string line;


        line =
        list[i].speaker
        +
        "："
        +
        // [修改] 清理方框字符
        CleanHistoryText(
            list[i].text
        );



        renderer.DrawText(

            line,

            80,

            y

        );

        y += 50;

    }




    renderer.DrawText(
        "返回 [ESC]",
        UILayout::BACK_X,
        UILayout::BACK_Y
    );

}





void HistoryMenu::HandleInput(
    int key
)
{


    if(history==nullptr)
    {
        return;
    }



    int size =
    history->Size();



    if(key==1)
    {


        offset--;

        if(offset<0)
        {

            offset=0;

        }


    }



    else if(key==2)
    {


        offset++;

        if(offset>=size)
        {

            offset=size-1;

        }


    }

}





void HistoryMenu::Reset()
{

    offset=0;

}





MenuMouseResult HistoryMenu::HandleMouseClick(
    int x,
    int y
)
{

    if(
        x >= UILayout::BACK_X &&
        x <  UILayout::BACK_X + UILayout::BACK_W &&
        y >= UILayout::BACK_Y &&
        y <  UILayout::BACK_Y + UILayout::BACK_H
    )
    {
        return MenuMouseResult::BACK;
    }


    return MenuMouseResult::NONE;

}