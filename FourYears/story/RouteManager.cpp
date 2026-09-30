#include "RouteManager.h"




RouteManager::RouteManager()
{

    currentRoute =
    RouteType::NONE;

}







void RouteManager::AddAffection(

    const std::string& character,

    int value

)
{


    affection[character]
    += value;


}







int RouteManager::GetAffection(

    const std::string& character

)
{


    if(
        affection.find(character)
        ==
        affection.end()
    )
    {

        return 0;

    }



    return affection[character];

}







void RouteManager::OpenRoute(

    RouteType route

)
{


    currentRoute =
    route;


}







RouteType RouteManager::GetCurrentRoute() const
{


    return currentRoute;


}







RouteType RouteManager::CheckRoute()
{

    int li =
    GetAffection(
        "李君浩"
    );



    int zhang =
    GetAffection(
        "张瀚宇"
    );



    if(li < 20 && zhang < 20)
    {
        return RouteType::NORMAL_END;
    }



    if(li == zhang)
    {
        return RouteType::NORMAL_END;
    }



    if(li > zhang)
    {
        return RouteType::LI_JUNHAO;
    }


    return RouteType::ZHANG_HANYU;

}







std::string RouteManager::RouteName()
{


    switch(
        currentRoute
    )
    {


    case RouteType::LI_JUNHAO:


        return "李君浩路线";



    case RouteType::ZHANG_HANYU:


        return "张瀚宇路线";



    case RouteType::NORMAL_END:


        return "普通结局";



    default:


        return "未开启路线";

    }


}





std::map<std::string, int>
RouteManager::GetAllAffection() const
{

    return affection;

}





// ==========================================================
// [新增] 整体替换
// ==========================================================

void RouteManager::SetAllAffection(
    const std::map<std::string, int>& data
)
{

    affection = data;

}