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
    if(currentIndex + 1 >= events.size())
    {
        return false;
    }

    currentIndex++;

    history.Add(
        events[currentIndex].name,
        events[currentIndex].text
    );

    return true;
}





StoryEvent Story::GetCurrentEvent()
{


    if(currentIndex>=events.size())
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

    StoryParser parser;


    return parser.Load(
        file,
        *this
    );

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