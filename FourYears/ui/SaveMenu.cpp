#include "SaveMenu.h"

#include <iostream>



SaveMenu::SaveMenu()
{

    choice=0;

    page=SavePage::LOAD;

}





void SaveMenu::SetPage(
    SavePage value
)
{

    page=value;

    choice=0;

}







void SaveMenu::Refresh(
    SaveSystem& saveSystem
)
{

    saves.clear();

    saveDataList.clear();



    saveDataList =
        saveSystem.GetSaveList();



    for(auto& data : saveDataList)
    {

        saves.push_back(
            data.displayName
        );

    }



    if(
        saves.empty()
    )
    {

        saves.push_back(
            "没有存档"
        );

    }



    choice=0;

}







void SaveMenu::Render(
    Renderer& renderer
)
{


    if(
        page==SavePage::LOAD
    )
    {

        renderer.DrawText(
            "读取存档",
            600,
            100
        );

    }

    else
    {

        renderer.DrawText(
            "存档管理",
            600,
            100
        );

    }





    for(
        int i=0;
        i<(int)saves.size();
        i++
    )
    {

        std::string text;


        if(
            i==choice
        )
        {

            text=
            "> "
            +
            saves[i];

        }
        else
        {

            text=
            saves[i];

        }



        renderer.DrawText(
            text,
            500,
            200+i*50
        );

    }




    if(
        page==SavePage::MANAGE
    )
    {

        renderer.DrawText(
            "N 新建  C复制  Delete删除  R重命名",
            350,
            750
        );

    }

}







void SaveMenu::HandleInput(
    int key
)
{


    if(
        saves.empty()
    )
    {
        return;
    }




    // 上

    if(
        key==1
    )
    {

        choice--;


        if(
            choice<0
        )
        {

            choice=
            saves.size()-1;

        }

    }




    // 下

    else if(
        key==2
    )
    {

        choice++;


        if(
            choice>=
            (int)saves.size()
        )
        {

            choice=0;

        }

    }


}







int SaveMenu::GetChoice() const
{

    return choice;

}







void SaveMenu::Reset()
{

    choice=0;

}








SaveData SaveMenu::GetCurrentSave()
{

    if(
        choice>=0
        &&
        choice<(int)saveDataList.size()
    )
    {

        return saveDataList[choice];

    }



    return SaveData();

}









void SaveMenu::Create(
    SaveSystem& saveSystem
)
{


    saveSystem.CreateSave(
        "新的存档",
        1,
        0
    );



    Refresh(
        saveSystem
    );


}








void SaveMenu::Delete(
    SaveSystem& saveSystem
)
{


    SaveData data=
        GetCurrentSave();



    if(
        data.filename.empty()
    )
    {
        return;
    }



    saveSystem.DeleteSave(
        data.filename
    );



    Refresh(
        saveSystem
    );


}









void SaveMenu::Copy(
    SaveSystem& saveSystem
)
{


    SaveData data=
        GetCurrentSave();



    if(
        data.filename.empty()
    )
    {
        return;
    }



    saveSystem.CopySave(
        data.filename
    );



    Refresh(
        saveSystem
    );


}









void SaveMenu::Rename(
    SaveSystem& saveSystem,
    const std::string& name
)
{


    SaveData data=
        GetCurrentSave();



    if(
        data.filename.empty()
    )
    {
        return;
    }




    saveSystem.RenameSave(
        data.filename,
        name
    );



    Refresh(
        saveSystem
    );


}






SavePage SaveMenu::GetPage() const
{

    return page;

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



    if(
        page==SavePage::LOAD
    )
    {

        SaveData data =
            GetCurrentSave();



        if(
            data.filename.empty()
        )
        {
            return;
        }



        int chapter=0;

        int index=0;



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

}