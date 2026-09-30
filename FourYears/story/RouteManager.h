#ifndef ROUTE_MANAGER_H
#define ROUTE_MANAGER_H


#include <string>
#include <map>
#include <vector>



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



    void AddAffection(
        const std::string& character,
        int value
    );



    int GetAffection(
        const std::string& character
    );



    void OpenRoute(
        RouteType route
    );



    RouteType GetCurrentRoute() const;



    RouteType CheckRoute();



    std::string RouteName();



    std::map<std::string, int> GetAllAffection() const;


    // ==========================================================
    // [新增] 整体替换好感度
    // ==========================================================
    //
    // 读档时使用：先用存档里的数据覆盖当前所有好感度。

    void SetAllAffection(
        const std::map<std::string, int>& data
    );

    // ==========================================================



private:


    std::map<std::string,int> affection;



    RouteType currentRoute;



};



#endif