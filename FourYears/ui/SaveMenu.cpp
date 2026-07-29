#include "SaveMenu.h"

#include <iostream>



SaveMenu::SaveMenu()
{

    choice=0;

    page=SavePage::LOAD;

    back=false;

}







void SaveMenu::Refresh(
    SaveSystem& saveSystem
)
{

    saves =
        saveSystem.GetSaveList();


    if(choice >= (int)saves.size())
    {
        choice=0;
    }

}








void SaveMenu::Render(
    Renderer& renderer
)
{


    renderer.DrawText(
        "存档管理",
        80,
        60
    );



    if(page==SavePage::LOAD)
    {

        renderer.DrawText(
            "读取存档",
            80,
            120
        );

    }
    else
    {

        renderer.DrawText(
            "管理存档",
            80,
            120
        );

    }






    if(saves.empty())
    {

        renderer.DrawText(
            "暂无存档",
            100,
            250
        );


        renderer.DrawText(
            "ESC 返回",
            100,
            700
        );


        return;

    }






    int startX=80;

    int startY=200;



    for(
        int i=0;
        i<(int)saves.size();
        i++
    )
    {


        int x =
        startX
        +
        (i%3)*480;


        int y =
        startY
        +
        (i/3)*180;



        std::string box;



        if(i==choice)
        {

            box =
            "> ";

        }
        else
        {

            box="  ";

        }



        box +=
        saves[i].displayName;



        renderer.DrawText(
            box,
            x,
            y
        );



        renderer.DrawText(
            "章节:"
            +
            std::to_string(
                saves[i].chapter
            ),
            x,
            y+40
        );



        renderer.DrawText(
            saves[i].time,
            x,
            y+80
        );


    }






    renderer.DrawText(
        "Enter 选择",
        80,
        650
    );


    renderer.DrawText(
        "ESC 返回",
        80,
        700
    );



}









void SaveMenu::HandleInput(
    int key
)
{


    if(key==1)
    {

        choice--;


        if(choice<0)
        {
            choice =
            saves.empty()
            ?
            0
            :
            saves.size()-1;
        }

    }



    else if(key==2)
    {

        choice++;


        if(
            choice >=
            (int)saves.size()
        )
        {
            choice=0;
        }

    }



}









void SaveMenu::Confirm(
    SaveSystem& saveSystem
)
{


    if(
        saves.empty()
    )
    {

        return;

    }




    SaveData data =
        saves[choice];



    if(page==SavePage::LOAD)
    {

        int chapter;

        int index;



        if(
            saveSystem.LoadSave(
                data.filename,
                chapter,
                index
            )
        )
        {

            std::cout
            <<"读取存档:"
            <<data.filename
            <<std::endl;

        }

    }



    else
    {

        // 管理模式默认复制

        saveSystem.CopySave(
            data.filename
        );


        Refresh(
            saveSystem
        );

    }



}








bool SaveMenu::Back()
{

    return back;

}








int SaveMenu::GetChoice() const
{

    return choice;

}








SavePage SaveMenu::GetPage() const
{

    return page;

}