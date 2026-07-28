#ifndef CONFIG_MENU_H
#define CONFIG_MENU_H


#include "../core/Renderer.h"
#include "../core/Config.h"



class ConfigMenu
{


public:


    ConfigMenu();



    // 设置配置对象
    void SetConfig(
        Config* config
    );



    // 绘制
    void Render(
        Renderer& renderer
    );



    // 输入
    void HandleInput(
        int key
    );



    // 保存设置
    void Save();



    // 获取当前选项
    int GetChoice() const;



private:


    Config* config;


    int choice;


    std::string items[6];


};



#endif