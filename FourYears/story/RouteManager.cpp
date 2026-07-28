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



    if(li>=20)
    {

        return RouteType::LI_JUNHAO;

    }



    if(zhang>=20)
    {

        return RouteType::ZHANG_HANYU;

    }



    return RouteType::NORMAL_END;


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