#include "HistoryMenu.h"



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


        return;

    }




    const auto& list =
    history->GetAll();




    int y = 160;




    for(
        int i=offset;
        i<list.size()
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
        list[i].text;



        renderer.DrawText(

            line,

            80,

            y

        );


        y += 50;


    }



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