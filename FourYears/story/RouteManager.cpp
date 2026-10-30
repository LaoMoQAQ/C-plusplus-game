#include "RouteManager.h"



RouteManager::RouteManager()
{
}





// ==========================================================
// AddAffection
// ==========================================================
//
// 好感度范围 [0, MAX_AFFECTION]。
// 溢出自动截断。

void RouteManager::AddAffection(
    const std::string& character,
    int value,
    const std::string& source
)
{
    int current = affection[character];

    int newValue = current + value;

    // 上下限截断
    if(newValue < 0)
        newValue = 0;

    if(newValue > MAX_AFFECTION)
        newValue = MAX_AFFECTION;


    affection[character] = newValue;


    // 记录实际变化量（截断后）
    int actualDelta = newValue - current;

    AffectionRecord rec;
    rec.character = character;
    rec.delta = actualDelta;
    rec.source = source;

    records.push_back(rec);
}





int RouteManager::GetAffection(const std::string& character)
{
    auto it = affection.find(character);

    if(it == affection.end())
        return 0;

    return it->second;
}





RouteType RouteManager::CheckRoute()
{
    int li    = GetAffection("李君浩");
    int zhang = GetAffection("张瀚宇");


    if(li < CHECK_THRESHOLD && zhang < CHECK_THRESHOLD)
        return RouteType::NORMAL_END;


    if(li == zhang)
        return RouteType::NORMAL_END;


    if(li > zhang)
        return RouteType::LI_JUNHAO;


    return RouteType::ZHANG_HANYU;
}





std::map<std::string, int>
RouteManager::GetAllAffection() const
{
    return affection;
}





void RouteManager::SetAllAffection(
    const std::map<std::string, int>& data
)
{
    affection = data;
    records.clear();
}





const std::vector<AffectionRecord>&
RouteManager::GetRecords() const
{
    return records;
}