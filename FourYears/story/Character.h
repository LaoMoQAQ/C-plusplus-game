#ifndef CHARACTER_H
#define CHARACTER_H


#include <string>



// ==========================================================
// Character
// ==========================================================
//
// 角色数据。
//
// 目前只用来记录名字和立绘路径。
// 好感度由 RouteManager 统一管理，不在这里。
//
// Story::GetCharacter(name) 会按需创建。

class Character
{

public:

    Character();

    Character(
        const std::string& name,
        const std::string& image
    );



    std::string GetName()  const;
    std::string GetImage() const;



private:

    std::string name;      // 角色名，如"李君浩"
    std::string image;     // 立绘路径

};


#endif