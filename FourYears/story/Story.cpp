#include "Story.h"
#include "StoryParser.h"



Story::Story()
{

    currentIndex=0;

}



void Story::Add(
    const StoryEvent& event
)
{

    events.push_back(event);

}





bool Story::Next()
{

    // 情况 1：当前章节还没结束
    if(
        currentIndex + 1
        <
        (int)events.size()
    )
    {

        currentIndex++;


        history.Add(
            events[currentIndex].name,
            events[currentIndex].text
        );


        return true;

    }




    // 情况 2：当前章节已到末尾，且脚本声明了下一章
    if(
        !nextFile.empty()
    )
    {

        std::string file = nextFile;


        Story temp;

        StoryParser parser;


        if(
            parser.Load(
                file,
                temp
            )
        )
        {

            events.swap(
                temp.events
            );


            currentIndex = 0;


            // [新增] 更新当前脚本路径
            currentFile = file;


            nextFile =
                temp.GetNextFile();


            if(
                !events.empty()
            )
            {

                history.Add(
                    events[0].name,
                    events[0].text
                );

            }


            return true;

        }

    }




    // 情况 3：真的结束了
    return false;

}





StoryEvent Story::GetCurrentEvent()
{


    if(
        currentIndex
        >=
        (int)events.size()
    )
    {

        return StoryEvent();

    }


    return events[currentIndex];


}





bool Story::IsEnd() const
{

    return currentIndex>=events.size();

}





bool Story::Load(
    const std::string& file
)
{

    // [修改] 先加载到临时 Story，
    // 成功再接管。
    // 避免加载失败时把当前状态清空。

    Story temp;

    StoryParser parser;


    if(
        !parser.Load(
            file,
            temp
        )
    )
    {
        return false;
    }


    events.swap(
        temp.events
    );


    currentIndex = 0;


    currentFile = file;


    nextFile =
        temp.GetNextFile();


    return true;

}





Character& Story::GetCharacter(
    const std::string& name
)
{


    for(auto& c:characters)
    {

        if(c.GetName()==name)
        {

            return c;

        }

    }



    characters.push_back(

        Character(
            name,
            ""
        )

    );


    return characters.back();

}





History& Story::GetHistory()
{

    return history;

}





RouteManager& Story::GetRouteManager()
{

    return routeManager;

}





int Story::GetIndex() const
{

    return currentIndex;

}





void Story::SetIndex(
    int value
)
{

    currentIndex=value;

}





void Story::SetNextFile(
    const std::string& file
)
{

    nextFile = file;

}





const std::string& Story::GetNextFile() const
{

    return nextFile;

}





const std::string& Story::GetCurrentFile() const
{

    return currentFile;

}