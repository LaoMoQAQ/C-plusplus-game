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



// ==========================================================
// AffectionRecord
// ==========================================================

struct AffectionRecord
{
    std::string character;
    int delta;
    std::string source;
};



// ==========================================================
// RouteManager
// ==========================================================
//
// 好感度与路线管理。
//
// 阈值：CHECK_THRESHOLD = 70
//   好感度封顶 100，阈值 70 意味着"走了大半条线"触发对应线。
//   混着选可能两边都不到 70，走单人结局。

class RouteManager
{

public:

    // 触发结局所需的好感度阈值
    static constexpr int CHECK_THRESHOLD = 70;

    // 好感度上限
    static constexpr int MAX_AFFECTION = 100;


    RouteManager();

    void AddAffection(
        const std::string& character,
        int value,
        const std::string& source = ""
    );

    int GetAffection(const std::string& character);

    void OpenRoute(RouteType route);
    RouteType GetCurrentRoute() const;
    RouteType CheckRoute();
    std::string RouteName();

    std::map<std::string, int> GetAllAffection() const;

    void SetAllAffection(
        const std::map<std::string, int>& data
    );

    const std::vector<AffectionRecord>& GetRecords() const;



private:

    std::map<std::string, int> affection;

    std::vector<AffectionRecord> records;

    RouteType currentRoute;

};


#endif