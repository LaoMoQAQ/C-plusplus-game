食用流程:
1:
安装vs code 网址:  https://code.visualstudio.com/
2:
安装vs code 插件: C/C++  C/C++ Devtools   C/C++ Extension Pack
3:
环境部署: 
        https://github.com/msys2/msys2-installer/releases/  默认最新版
        如遇下载报404 可  sed -i "s#https\?://mirror.msys2.org/#https://mirrors.tuna.tsinghua.edu.cn/msys2/#g" /etc/pacman.d/mirrorlist*    pacman -Syyu
        接下来进msy64安装文件夹，进入uct64目录，进入bin  拷贝地址  开始菜单中搜编辑系统环境变量，选择环境变量，进入path，选新建，再将拷贝的地址粘贴下去，保存
4:
编译运行:
    在 vs code 中 :  选择main.cpp打开，按ctrl+shift+b编译，找到编译出的game.exe双击运行
    在 PowerShell 中 :  
                    cd F:\gal\FourYears
                    C:\msys64\ucrt64\bin\g++.exe main.cpp StoryParser.cpp -o game.exe -IC:/msys64/ucrt64/include/SDL2 -LC:/msys64/ucrt64/lib -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -lSDL2_ttf
                    .\game.exe


want to do:
    渐变对话框