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


        // [新增] 跳过 [跳转] 链
        int guard = 0;

        while(
            currentIndex < (int)events.size() &&
            events[currentIndex].isGoto &&
            guard < 1000
        )
        {

            auto it = labels.find(
                events[currentIndex].gotoLabel
            );

            if(
                it == labels.end() ||
                it->second < 0 ||
                it->second >= (int)events.size()
            )
            {
                break;
            }

            currentIndex = it->second;

            guard++;

        }


        if(!events[currentIndex].isGoto)
        {
            history.Add(
                events[currentIndex].name,
                events[currentIndex].text
            );
        }


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

            labels.swap(
                temp.labels
            );


            currentIndex = 0;


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

    // [新增] 标签也要换
    labels.swap(
        temp.labels
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





// ==========================================================
// [新增] 标签系统
// ==========================================================

void Story::AddLabel(
    const std::string& name,
    int eventIndex
)
{

    labels[name] = eventIndex;

}





bool Story::HasLabel(
    const std::string& name
) const
{

    return labels.find(name) != labels.end();

}





int Story::GetLabel(
    const std::string& name
) const
{

    auto it = labels.find(name);

    if(it == labels.end())
    {
        return -1;
    }

    return it->second;

}





bool Story::JumpToLabel(
    const std::string& name
)
{

    auto it = labels.find(name);

    if(it == labels.end())
    {
        return false;
    }


    int target = it->second;

    if(
        target < 0 ||
        target >= (int)events.size()
    )
    {
        return false;
    }


    currentIndex = target;


    // 跳过跳转链
    int guard = 0;

    while(
        currentIndex < (int)events.size() &&
        events[currentIndex].isGoto &&
        guard < 1000
    )
    {

        auto it2 = labels.find(
            events[currentIndex].gotoLabel
        );

        if(
            it2 == labels.end() ||
            it2->second < 0 ||
            it2->second >= (int)events.size()
        )
        {
            break;
        }

        currentIndex = it2->second;

        guard++;

    }


    if(
        currentIndex < (int)events.size() &&
        !events[currentIndex].isGoto
    )
    {
        history.Add(
            events[currentIndex].name,
            events[currentIndex].text
        );
    }


    return true;

}