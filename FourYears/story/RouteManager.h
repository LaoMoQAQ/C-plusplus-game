#ifndef ROUTE_MANAGER_H
#define ROUTE_MANAGER_H


#include <string>
#include <map>
#include <vector>



// ==========================================================
// RouteType
// ==========================================================
//
// 路线类型。
// 由 CheckRoute() 根据好感度判定。

enum class RouteType
{
    NONE,          // 未开启
    LI_JUNHAO,     // 李君浩线
    ZHANG_HANYU,   // 张瀚宇线
    NORMAL_END     // 单人结局
};



// ==========================================================
// RouteManager
// ==========================================================
//
// 好感度与路线管理。
//
// 数据流：
//   - 选择事件触发 AddAffection(name, delta)
//   - 结局事件调用 CheckRoute() 决定走哪条线
//   - 存档时 GetAllAffection() 快照，读档时 SetAllAffection() 恢复
//
// 判定规则（CheckRoute）：
//   1. 两人都 < 阈值 -> 单人结局
//   2. 两人相等     -> 单人结局
//   3. 谁高走谁
//
// 阈值当前是 20，写在 CheckRoute 里。

class RouteManager
{

public:

    RouteManager();



    // 好感度增加（可以为负）。
    void AddAffection(
        const std::string& character,
        int value
    );



    // 查询好感度。没记录返回 0。
    int GetAffection(const std::string& character);



    // 手动设置当前路线（一般不用，CheckRoute 会自动判定）。
    void OpenRoute(RouteType route);

    // 当前路线。
    RouteType GetCurrentRoute() const;



    // 根据好感度判定应该走哪条线。
    RouteType CheckRoute();



    // 路线名的中文显示。
    std::string RouteName();



    // 获取全部好感度（用于存档）。
    std::map<std::string, int> GetAllAffection() const;



    // 整体替换好感度（用于读档）。
    void SetAllAffection(
        const std::map<std::string, int>& data
    );



private:

    std::map<std::string, int> affection;  // 角色名 -> 好感度

    RouteType currentRoute;

};


#endif