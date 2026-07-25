#define SDL_MAIN_HANDLED


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>


#ifdef _WIN32
#include <windows.h>
#endif


#include "Story.h"
#include "StoryParser.h"
#include "GameState.h"


using namespace std;



//================================
// 菜单类型
//================================

enum class MenuType
{
    START,
    PAUSE
};




//================================
// 屏幕
//================================

const int SCREEN_WIDTH=1600;
const int SCREEN_HEIGHT=900;




//================================
// 根目录
//================================

string ROOT;



void InitRoot()
{

    ROOT=
    filesystem::current_path().string();


    if(
        filesystem::exists(
            ROOT+"/resource"
        )
    )
    {
        return;
    }



    if(
        filesystem::exists(
            ROOT+"/FourYears/resource"
        )
    )
    {

        ROOT+="/FourYears";

        return;

    }



    cout
    <<"找不到游戏目录"
    <<endl;


}




string Path(string p)
{

    return ROOT+"/"+p;

}






//================================
// 图片加载
//================================


SDL_Texture* LoadTexture(
    SDL_Renderer* renderer,
    string file
)
{

    SDL_Surface* surface=
    IMG_Load(
        Path(file).c_str()
    );


    if(!surface)
    {

        cout
        <<"图片失败:"
        <<Path(file)
        <<endl;


        return nullptr;

    }



    SDL_Texture* tex=
    SDL_CreateTextureFromSurface(
        renderer,
        surface
    );



    SDL_FreeSurface(surface);



    return tex;

}






//================================
// UTF8切割
//================================


vector<string> SplitUTF8(string text)
{

    vector<string> result;


    for(size_t i=0;i<text.size();)
    {

        unsigned char c=text[i];


        int len=1;


        if(c>=0xF0)
            len=4;

        else if(c>=0xE0)
            len=3;

        else if(c>=0xC0)
            len=2;



        result.push_back(
            text.substr(i,len)
        );


        i+=len;

    }


    return result;

}






//================================
// 创建文字
//================================


SDL_Texture* CreateText(
    SDL_Renderer* renderer,
    TTF_Font* font,
    string text
)
{

    SDL_Color white=
    {
        255,
        255,
        255,
        255
    };



    SDL_Surface* surface=
    TTF_RenderUTF8_Blended(
        font,
        text.c_str(),
        white
    );


    if(!surface)
    {
        return nullptr;
    }



    SDL_Texture* tex=
    SDL_CreateTextureFromSurface(
        renderer,
        surface
    );


    SDL_FreeSurface(surface);


    return tex;

}
//================================
// 打字系统
//================================


class TextSystem
{

public:


    SDL_Renderer* renderer;


    TTF_Font* simhei;


    vector<string> chars;


    int index=0;


    int speed=80;


    Uint32 timer=0;


    bool finished=true;


    SDL_Texture* texture=nullptr;



    TextSystem(
        SDL_Renderer* r,
        TTF_Font* f
    )
    {

        renderer=r;

        simhei=f;

    }




    void Clear()
    {

        if(texture)
        {

            SDL_DestroyTexture(
                texture
            );

            texture=nullptr;

        }

    }






    void SetText(
        string text
    )
    {

        Clear();


        chars=
        SplitUTF8(
            text
        );


        index=0;


        finished=false;


    }






    void Update()
    {

        if(finished)
            return;



        Uint32 now=
        SDL_GetTicks();



        if(now-timer>speed)
        {


            index++;



            if(index>=chars.size())
            {

                index=chars.size();

                finished=true;

            }


            UpdateTexture();


            timer=now;

        }

    }





    void UpdateTexture()
    {

        Clear();


        string show;



        for(
            int i=0;
            i<index;
            i++
        )
        {

            show+=chars[i];

        }



        if(show.empty())
            return;




        SDL_Color white=
        {
            255,
            255,
            255,
            255
        };



        SDL_Surface* surface=
        TTF_RenderUTF8_Blended_Wrapped(
            simhei,
            show.c_str(),
            white,
            1300
        );



        if(surface)
        {


            texture=
            SDL_CreateTextureFromSurface(
                renderer,
                surface
            );


            SDL_FreeSurface(
                surface
            );


        }


    }







    void Finish()
    {

        index=
        chars.size();


        finished=true;


        UpdateTexture();

    }






    void Render(
        SDL_Rect rect
    )
    {

        if(!texture)
            return;



        int w,h;


        SDL_QueryTexture(
            texture,
            nullptr,
            nullptr,
            &w,
            &h
        );



        SDL_Rect dst=
        {
            rect.x,
            rect.y,
            w,
            h
        };



        SDL_RenderCopy(
            renderer,
            texture,
            nullptr,
            &dst
        );


    }
    //================================
// 选择显示
//================================

void RenderChoice(
    vector<string> choices,
    int select,
    int x,
    int y
)
{

    int yy=y;


    for(
        int i=0;
        i<choices.size();
        i++
    )
    {


        string s;


        if(i==select)
        {
            s="> ";
        }
        else
        {
            s="  ";
        }


        s+=choices[i];



        SDL_Color white=
        {
            255,
            255,
            255,
            255
        };



        SDL_Surface* surface=
        TTF_RenderUTF8_Blended(
            simhei,
            s.c_str(),
            white
        );



        if(surface)
        {


            SDL_Texture* tex=
            SDL_CreateTextureFromSurface(
                renderer,
                surface
            );



            SDL_Rect r=
            {
                x,
                yy,
                surface->w,
                surface->h
            };



            SDL_RenderCopy(
                renderer,
                tex,
                nullptr,
                &r
            );



            SDL_DestroyTexture(
                tex
            );


            SDL_FreeSurface(
                surface
            );


        }


        yy+=50;


    }


}


};









//================================
// 资源管理
//================================


class ResourceManager
{

public:


    SDL_Renderer* renderer;


    map<string,SDL_Texture*> backgrounds;


    map<string,SDL_Texture*> characters;





    ResourceManager(
        SDL_Renderer* r
    )
    {

        renderer=r;

    }







    SDL_Texture* GetBackground(
        string name
    )
    {

        if(name=="")
            return nullptr;



        if(backgrounds.count(name))
        {

            return backgrounds[name];

        }



        SDL_Texture* tex=
        LoadTexture(
            renderer,
            "resource/bg/"+name
        );



        backgrounds[name]=tex;


        return tex;


    }








    SDL_Texture* GetCharacter(
        string name
    )
    {

        if(
            name.empty()
        )
        {

            return nullptr;

        }



        if(characters.count(name))
        {

            return characters[name];

        }



        SDL_Texture* tex=
        LoadTexture(
            renderer,
            "resource/character/"+name
        );


        characters[name]=tex;


        return tex;


    }







    void Clear()
    {


        for(auto& x:backgrounds)
        {

            if(x.second)
            {

                SDL_DestroyTexture(
                    x.second
                );

            }

        }





        for(auto& x:characters)
        {

            if(x.second)
            {

                SDL_DestroyTexture(
                    x.second
                );

            }

        }


    }


};

//================================
// main
//================================


int main()
{


#ifdef _WIN32

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

#endif



    InitRoot();



    cout
    <<"游戏目录:"
    <<ROOT
    <<endl;






    //==============================
    // SDL
    //==============================


    if(
        SDL_Init(
            SDL_INIT_VIDEO
        )
        !=0
    )
    {

        return -1;

    }



    IMG_Init(
        IMG_INIT_PNG
    );


    TTF_Init();








    SDL_Window* window=
    SDL_CreateWindow(

        "FourYears",

        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,

        SCREEN_WIDTH,
        SCREEN_HEIGHT,

        SDL_WINDOW_SHOWN

    );



    if(!window)
        return -1;






    SDL_Renderer* renderer=
    SDL_CreateRenderer(

        window,

        -1,

        SDL_RENDERER_ACCELERATED

    );



    if(!renderer)
        return -1;








    //==============================
    // 字体
    //==============================


    TTF_Font* simhei=
    TTF_OpenFont(

        Path(
        "resource/font/simhei.ttf"
        ).c_str(),

        32

    );



    TTF_Font* number=
    TTF_OpenFont(

        Path(
        "resource/font/number.ttf"
        ).c_str(),

        28

    );



    TTF_Font* title=
    TTF_OpenFont(

        Path(
        "resource/font/title.ttf"
        ).c_str(),

        48

    );



    if(
        !simhei ||
        !number ||
        !title
    )
    {

        cout
        <<"字体失败"
        <<endl;


        return -1;

    }









    //==============================
    //资源
    //==============================


    ResourceManager resource(
        renderer
    );









    //==============================
    //剧情
    //==============================


    Story story;


    StoryParser parser;



    vector<string> scripts=
    {

        "script/chapter01.txt",

        "script/chapter02.txt",

        "script/ending.txt"

    };





    for(auto& s:scripts)
    {

        if(
            !parser.Load(
                Path(s),
                story
            )
        )
        {

            cout
            <<"剧情读取失败:"
            <<s
            <<endl;


            return -1;

        }


    }








    if(story.events.empty())
    {

        cout
        <<"剧情为空"
        <<endl;


        return -1;

    }









    //==============================
    //剧情变量
    //==============================


    int current=0;


    string currentRoute="";


    bool choosing=false;


    int choiceIndex=0;






    TextSystem text(
        renderer,
        simhei
    );



    //注意：
    //这里不加载剧情
    //等待开始菜单


    bool running=true;





    //==============================
    //游戏状态
    //==============================


    GameState state=
    GameState::MENU;



    MenuType menuType=
    MenuType::START;



    int menuIndex=0;





    SDL_Event event;



    //下面进入游戏循环
//================================
// 游戏循环
//================================


while(running)
{


    while(SDL_PollEvent(&event))
    {



        if(event.type==SDL_QUIT)
        {

            running=false;

        }






        if(event.type==SDL_KEYDOWN)
        {


            SDL_Keycode key=
            event.key.keysym.sym;





            //================================
            // ESC
            //================================


            if(key==SDLK_ESCAPE)
            {


                if(
                    state==
                    GameState::PLAYING
                )
                {

                    state=
                    GameState::MENU;


                    menuType=
                    MenuType::PAUSE;


                    menuIndex=0;


                }


                else if(
                    state==
                    GameState::MENU
                    &&
                    menuType==
                    MenuType::PAUSE
                )
                {


                    state=
                    GameState::PLAYING;


                }


                continue;


            }







            //================================
            // 菜单
            //================================


            if(
                state==
                GameState::MENU
            )
            {



                if(key==SDLK_UP)
                {

                    menuIndex--;


                    if(menuIndex<0)
                    {

                        menuIndex=3;

                    }

                }





                else if(key==SDLK_DOWN)
                {


                    menuIndex++;


                    if(menuIndex>3)
                    {

                        menuIndex=0;

                    }


                }








                else if(
                    key==SDLK_RETURN
                    ||
                    key==SDLK_KP_ENTER
                )
                {



                    //========================
                    //开始菜单
                    //========================


                    if(
                        menuType==
                        MenuType::START
                    )
                    {


                        if(menuIndex==0)
                        {


                            state=
                            GameState::PLAYING;



                            current=0;



                            choosing=false;



                            text.SetText(
                                story.events[current].text
                            );


                        }




                        else if(menuIndex==3)
                        {

                            running=false;

                        }


                    }







                    //========================
                    //暂停菜单
                    //========================


                    else
                    {


                        if(menuIndex==0)
                        {

                            state=
                            GameState::PLAYING;

                        }



                        else if(menuIndex==3)
                        {

                            running=false;

                        }



                    }



                }


                continue;


            }









            //================================
            //剧情输入
            //================================


            if(
                state==
                GameState::PLAYING
            )
            {




                //========================
                //选择
                //========================


                if(choosing)
                {



                    if(key==SDLK_UP)
                    {

                        choiceIndex--;


                        if(choiceIndex<0)
                        {

                            choiceIndex=
                            story.events[current]
                            .choices.size()-1;

                        }


                    }




                    else if(key==SDLK_DOWN)
                    {


                        choiceIndex++;


                        if(
                            choiceIndex >=
                            story.events[current]
                            .choices.size()
                        )
                        {

                            choiceIndex=0;

                        }


                    }





                    else if(
                        key==SDLK_RETURN
                        ||
                        key==SDLK_KP_ENTER
                    )
                    {


                        string choice=
                        story.events[current]
                        .choices[choiceIndex];



                        choosing=false;



                        Story route;




                        if(
                            choice.find("陪伴")
                            !=string::npos
                        )
                        {


                            parser.Load(
                                Path(
                                "script/li_junhao_route.txt"
                                ),
                                route
                            );


                        }



                        else if(
                            choice.find("未来")
                            !=string::npos
                        )
                        {


                            parser.Load(
                                Path(
                                "script/zhang_hanyu_route.txt"
                                ),
                                route
                            );


                        }



                        else
                        {


                            parser.Load(
                                Path(
                                "script/alone_ending.txt"
                                ),
                                route
                            );


                        }





                        int oldSize=
                        story.events.size();




                        story.events.insert(
                            story.events.end(),
                            route.events.begin(),
                            route.events.end()
                        );




                        current=
                        oldSize;



                        text.SetText(
                            story.events[current].text
                        );



                    }


                }






                //========================
                //普通剧情
                //========================


                else
                {


                    if(
                        key==SDLK_SPACE
                        ||
                        key==SDLK_RETURN
                        ||
                        key==SDLK_KP_ENTER
                    )
                    {



                        if(!text.finished)
                        {

                            text.Finish();

                        }


                        else
                        {


                            current++;



                            if(
                                current<
                                story.events.size()
                            )
                            {



                                if(
                                    story.events[current]
                                    .isChoice
                                )
                                {

                                    choosing=true;

                                    choiceIndex=0;

                                    text.Clear();

                                }


                                else
                                {

                                    text.SetText(
                                        story.events[current].text
                                    );


                                }



                            }


                            else
                            {

                                running=false;

                            }


                        }



                    }


                }



            }





        }



    }
    
        //==============================
        // 更新文字
        //==============================

        text.Update();




        //==============================
        // 清屏
        //==============================


        SDL_SetRenderDrawColor(
            renderer,
            0,
            0,
            0,
            255
        );


        SDL_RenderClear(
            renderer
        );






        //================================
        // 游戏画面
        //================================


        if(
            state==
            GameState::PLAYING
        )
        {


            StoryEvent& now =
                story.events[current];

            //背景


            SDL_Texture* bg=
            resource.GetBackground(
                now.background
            );



            if(bg)
            {

                SDL_Rect dst=
                {
                    0,
                    0,
                    SCREEN_WIDTH,
                    SCREEN_HEIGHT
                };


                SDL_RenderCopy(
                    renderer,
                    bg,
                    nullptr,
                    &dst
                );

            }






            //人物


            SDL_Texture* character=
            resource.GetCharacter(
                now.character
            );



            if(character)
            {

                SDL_Rect dst=
                {
                    950,
                    110,
                    450,
                    800
                };


                SDL_RenderCopy(
                    renderer,
                    character,
                    nullptr,
                    &dst
                );


            }






            //==============================
            // 对话框
            //==============================


            SDL_SetRenderDrawBlendMode(
                renderer,
                SDL_BLENDMODE_BLEND
            );



            SDL_SetRenderDrawColor(
                renderer,
                0,
                0,
                0,
                170
            );



            SDL_Rect box=
            {
                50,
                700,
                1500,
                120
            };



            SDL_RenderFillRect(
                renderer,
                &box
            );







            //名字


            if(
                !choosing &&
                now.name!=""
            )
            {


                SDL_Texture* name=
                CreateText(
                    renderer,
                    title,
                    now.name
                );



                if(name)
                {

                    int nw,nh;

                    SDL_QueryTexture(
                        name,
                        nullptr,
                        nullptr,
                        &nw,
                        &nh
                    );

                    SDL_Rect r=
                    {
                        100,
                        650,
                        nw,
                        nh
                    };



                    SDL_RenderCopy(
                        renderer,
                        name,
                        nullptr,
                        &r
                    );


                    SDL_DestroyTexture(
                        name
                    );


                }


            }






            //正文


            
            if(!choosing)
            {

                SDL_Rect textRect=
                {
                    100,
                    710,
                    1300,
                    150
                };


                text.Render(
                    textRect
                );

            }







            //==============================
            //选择
            //==============================


            if(choosing)
            {


                text.RenderChoice(
                    now.choices,
                    choiceIndex,
                    180,
                    680
                );


            }



        }









        //================================
        // MENU
        //================================


        if(
            state==
            GameState::MENU
        )
        {



            //标题


            SDL_Texture* logo=
            CreateText(
                renderer,
                title,
                "FourYears"
            );



            if(logo)
            {


                int lw,lh;

                SDL_QueryTexture(
                    logo,
                    nullptr,
                    nullptr,
                    &lw,
                    &lh
                );

                SDL_Rect r=
                {
                    100,
                    120,
                    lw,
                    lh
                };



                SDL_RenderCopy(
                    renderer,
                    logo,
                    nullptr,
                    &r
                );



                SDL_DestroyTexture(
                    logo
                );


            }







            vector<string> menu;



            if(
                menuType==
                MenuType::START
            )
            {

                menu=
                {

                    "开始新游戏",

                    "读取存档",

                    "管理存档",

                    "退出游戏"

                };

            }


            else
            {

                menu=
                {

                    "继续游戏",

                    "保存游戏",

                    "读取存档",

                    "退出游戏"

                };

            }








            //暂停背景


            if(
                menuType==
                MenuType::PAUSE
            )
            {


                SDL_SetRenderDrawBlendMode(
                    renderer,
                    SDL_BLENDMODE_BLEND
                );



                SDL_SetRenderDrawColor(
                    renderer,
                    180,
                    180,
                    180,
                    210
                );



                SDL_Rect panel=
                {
                    520,
                    180,
                    560,
                    520
                };



                SDL_RenderFillRect(
                    renderer,
                    &panel
                );


            }







            //菜单文字


            for(
                int i=0;
                i<menu.size();
                i++
            )
            {


                string s;


                if(i==menuIndex)
                    s="> ";
                else
                    s="  ";



                s+=menu[i];



                SDL_Texture* t=
                CreateText(
                    renderer,
                    simhei,
                    s
                );



                if(t)
                {


                    SDL_Rect r;



                    if(
                        menuType==
                        MenuType::START
                    )
                    {


                        //左下角


                        int mw,mh;

                        SDL_QueryTexture(
                            t,
                            nullptr,
                            nullptr,
                            &mw,
                            &mh
                        );

                        r=
                        {
                            100,
                            680+i*55,
                            mw,
                            mh
                        };


                    }
                    else
                    {


                        //暂停中央


                        int mw,mh;

                        SDL_QueryTexture(
                            t,
                            nullptr,
                            nullptr,
                            &mw,
                            &mh
                        );

                        r=
                        {
                            SCREEN_WIDTH/2-mw/2,
                            350+i*60,
                            mw,
                            mh
                        };


                    }





                    SDL_RenderCopy(
                        renderer,
                        t,
                        nullptr,
                        &r
                    );



                    SDL_DestroyTexture(
                        t
                    );


                }


            }



        }






        SDL_RenderPresent(
            renderer
        );



        SDL_Delay(
            16
        );


    }








    //================================
    //释放
    //================================


    resource.Clear();



    text.Clear();



    TTF_CloseFont(
        simhei
    );


    TTF_CloseFont(
        number
    );


    TTF_CloseFont(
        title
    );



    SDL_DestroyRenderer(
        renderer
    );


    SDL_DestroyWindow(
        window
    );


    TTF_Quit();


    IMG_Quit();


    SDL_Quit();



    return 0;

}