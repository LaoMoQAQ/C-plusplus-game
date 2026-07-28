#ifndef ROUTE_MANAGER_H
#define ROUTE_MANAGER_H


#include <string>
#include <map>



enum class RouteType
{

    NONE,


    LI_JUNHAO,


    ZHANG_HANYU,


    NORMAL_END


};





class RouteManager
{


public:


    RouteManager();



    // 增加好感度
    void AddAffection(
        const std::string& character,
        int value
    );



    // 获取好感度
    int GetAffection(
        const std::string& character
    );



    // 开启路线
    void OpenRoute(
        RouteType route
    );



    // 当前路线
    RouteType GetCurrentRoute() const;



    // 根据条件判断路线
    RouteType CheckRoute();



    // 名称转换
    std::string RouteName();



private:


    std::map<std::string,int> affection;



    RouteType currentRoute;



};



#endif