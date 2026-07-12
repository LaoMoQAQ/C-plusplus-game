#define SDL_MAIN_HANDLED


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>


#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>
#include <windows.h>


#include "Story.h"
#include "StoryParser.h"


using namespace std;



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





string Path(
    string p
)
{

    return ROOT+"/"+p;

}





//================================
// 图片
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


vector<string> SplitUTF8(
    string text
)
{

    vector<string> result;



    for(size_t i=0;i<text.size();)
    {


        unsigned char c=
        text[i];


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
// 打字机
//================================


class TextSystem
{

public:


    SDL_Renderer* renderer;
    
    //正文
    TTF_Font* simhei;

    //选择
    TTF_Font* number;

    // 标题
    TTF_Font* title;

    vector<string> chars;



    int index=0;


    int speed=80;


    Uint32 timer=0;



    bool finished=true;



    SDL_Texture* texture=nullptr;



    TextSystem(
        SDL_Renderer* r,
        TTF_Font* f,
        TTF_Font* option,
        TTF_Font* title
    )
    {

        renderer=r;

        simhei=f;

        number=option;

        title=title;

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



        if(show=="")
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
    // 选择文字显示
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
                s="> "+choices[i];
            }
            else
            {
                s="  "+choices[i];
            }



            SDL_Color color=
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
                color
            );



            if(surface)
            {

                SDL_Texture* t=
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
                    t,
                    nullptr,
                    &r
                );


                SDL_DestroyTexture(t);


                SDL_FreeSurface(
                    surface
                );

            }


            yy+=60;


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



    map<string, SDL_Texture*> backgrounds;


    map<string, SDL_Texture*> characters;





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
            name==""
            ||
            name=="none"
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
// 创建文字纹理
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

        cout
        <<"文字生成失败:"
        <<text
        <<endl;


        return nullptr;

    }







    SDL_Texture* texture=
    SDL_CreateTextureFromSurface(
        renderer,
        surface
    );



    SDL_FreeSurface(
        surface
    );



    return texture;


}
//================================
// main
//================================


int main()
{


    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);


    InitRoot();



    cout
    <<"游戏目录:"
    <<ROOT
    <<endl;





    //==============================
    // SDL初始化
    //==============================


    if(
        SDL_Init(SDL_INIT_VIDEO)
        !=0
    )
    {

        cout
        <<"SDL初始化失败"
        <<endl;

        return -1;

    }





    IMG_Init(
        IMG_INIT_PNG
    );


    TTF_Init();







    //==============================
    // 窗口
    //==============================


    SDL_Window* window=
    SDL_CreateWindow(

        "四年之后",

        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,

        SCREEN_WIDTH,
        SCREEN_HEIGHT,

        SDL_WINDOW_SHOWN

    );





    if(!window)
    {

        cout
        <<"窗口创建失败"
        <<endl;


        return -1;

    }









    SDL_Renderer* renderer=
    SDL_CreateRenderer(

        window,

        -1,

        SDL_RENDERER_ACCELERATED

    );








    if(!renderer)
    {

        cout
        <<"渲染器创建失败"
        <<endl;


        return -1;

    }









    //==============================
    // 字体
    //==============================


    TTF_Font* simhei=
    TTF_OpenFont(

        Path("resource/font/simhei.ttf").c_str(),

        32

    );



    TTF_Font* number=
    TTF_OpenFont(

        Path("resource/font/number.ttf").c_str(),

        24

    );



    TTF_Font* title=
    TTF_OpenFont(

        Path("resource/font/title.ttf").c_str(),

        48

    );


    if(
        !simhei ||
        !number ||
        !title
    )
    {

        cout
        <<"字体加载失败"
        <<endl;


        return -1;

    }









    //==============================
    // 资源
    //==============================


    ResourceManager resource(
        renderer
    );









//==============================
// 剧情读取
//==============================


Story story;


StoryParser parser;




//==============================
// 第一章
//==============================

if(
    !parser.Load(
        Path(
        "script/chapter01.txt"
        ),
        story
    )
)
{

    cout
    <<"第一章读取失败"
    <<endl;


    return -1;

}


cout
<<"第一章后:"
<<story.events.size()
<<endl;





//==============================
// 第二章
//==============================

if(
    !parser.Load(
        Path(
        "script/chapter02.txt"
        ),
        story
    )
)
{

    cout
    <<"第二章读取失败"
    <<endl;


    return -1;

}


cout
<<"第二章后:"
<<story.events.size()
<<endl;





//==============================
// 结局选择入口
//==============================

if(
    !parser.Load(
        Path(
        "script/ending.txt"
        ),
        story
    )
)
{

    cout
    <<"结局选择读取失败"
    <<endl;


    return -1;

}



cout
<<"结局选择后:"
<<story.events.size()
<<endl;






if(
    story.events.empty()
)
{

    cout
    <<"剧情为空"
    <<endl;


    return -1;

}


    //==============================
    // 初始剧情
    //==============================

    int current=0;


    // 当前路线
    // 空=普通剧情
    // li=李君浩路线
    // zhang=张瀚宇路线
    string currentRoute="";


    // 当前是否正在选择

    bool choosing=false;


    // 当前选择编号

    int choiceIndex=0;



    TextSystem text(
        renderer,
        simhei,
        number,
        title
    );



    text.SetText(
        story.events[current].text
    );



    bool running=true;


    SDL_Event event;




    //================================
    // 游戏循环
    //================================


    while(running)
    {


        while(SDL_PollEvent(&event))
        {


            //关闭窗口

            if(event.type==SDL_QUIT)
            {

                running=false;

            }


            



            //============================
            // 键盘
            //============================


            if(event.type==SDL_KEYDOWN)
            {


                SDL_Keycode key=
                event.key.keysym.sym;



                //正在选择

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
                            choiceIndex>=
                            story.events[current]
                            .choices.size()
                        )
                        {

                            choiceIndex=0;

                        }

                    }






                    else if(
    key==SDLK_RETURN ||
    key==SDLK_KP_ENTER
)
{


    cout
    <<"选择:"
    <<choiceIndex+1
    <<endl;



    string choice =
    story.events[current]
    .choices[choiceIndex];



    choosing=false;



    //================================
// 根据选择加载路线
//================================


Story route;




// 李君浩路线

if(
    choice.find("陪伴")
    !=string::npos
)
{

    if(
        !parser.Load(
            Path(
            "script/li_junhao_route.txt"
            ),
            route
        )
    )
    {

        cout
        <<"李君浩路线读取失败"
        <<endl;


        running=false;

    }

}




// 张瀚宇路线

else if(
    choice.find("未来")
    !=string::npos
)
{

    if(
        !parser.Load(
            Path(
            "script/zhang_hanyu_route.txt"
            ),
            route
        )
    )
    {

        cout
        <<"张瀚宇路线读取失败"
        <<endl;


        running=false;

    }

}





// 普通结局

else
{

    if(
        !parser.Load(
            Path(
            "script/alone_ending.txt"
            ),
            route
        )
    )
    {

        cout
        <<"普通结局读取失败"
        <<endl;


        running=false;

    }

}






//==============================
// 把路线接到当前剧情后面
//==============================


int oldSize =
story.events.size();



story.events.insert(
    story.events.end(),
    route.events.begin(),
    route.events.end()
);



//进入新路线第一段

current=oldSize;



currentRoute=
choice;







    if(
        current < story.events.size()
    )
    {

        if(
            story.events[current].isChoice
        )
        {


            choosing=true;
            choiceIndex=0;


            // 清除之前台词

            text.Clear();


        }
        else
        {


            text.SetText(
                story.events[current].text
            );

        }

    }



}


                }






                //普通剧情

                else
                {


                    if(
                        key==SDLK_SPACE ||
                        key==SDLK_RETURN ||
                        key==SDLK_KP_ENTER
                    )
                    {


                        //文字未显示完

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

                                cout
                                <<"全部剧情结束"
                                <<endl;


                                running=false;

                            }


                        }



                    }


                }



            }









            //============================
            // 鼠标
            //============================


            if(
                event.type==
                SDL_MOUSEBUTTONDOWN
            )
            {


                //选择时禁止鼠标跳过

                if(choosing)
                {

                    continue;

                }




                if(!text.finished)
                {

                    text.Finish();

                }

                else
                {

                    current++;


                    if(currentRoute=="li" &&
                       current < story.events.size() &&
                       story.events[current].choiceResult=="zhang")
                    {
                        running=false;
                    }


                    if(currentRoute=="zhang" &&
                       current < story.events.size() &&
                       story.events[current].choiceResult=="li")
                    {
                        running=false;
                    }



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

                        cout
                        <<"剧情结束"
                        <<endl;


                        running=false;

                    }


                }



            }



        }
                //================================
        // 更新文字
        //================================

        text.Update();





        //================================
        // 清屏
        //================================


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









        //当前剧情

        StoryEvent& now =
        story.events[current];








        //================================
        // 背景
        //================================


        SDL_Texture* bg =
        resource.GetBackground(
            now.background
        );



        if(bg)
        {


            SDL_Rect dst =
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









        //================================
        // 立绘
        //================================


        SDL_Texture* character =
        resource.GetCharacter(
            now.character
        );



        if(character)
        {


            SDL_Rect dst =
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









        //================================
        // 对话框
        //================================


        SDL_SetRenderDrawBlendMode(
            renderer,
            SDL_BLENDMODE_BLEND
        );



        SDL_SetRenderDrawColor(
            renderer,
            0,
            0,
            0,
            180
        );



        SDL_Rect box =
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









        //================================
        // 名字
        //================================


        if(
            now.name!="" &&
            !choosing
        )
        {


            SDL_Texture* nameTex =
            CreateText(
                renderer,
                title,
                now.name
            );



            if(nameTex)
            {


                int w,h;


                SDL_QueryTexture(
                    nameTex,
                    nullptr,
                    nullptr,
                    &w,
                    &h
                );



                SDL_Rect nameRect =
                {
                    100,
                    650,
                    w,
                    h
                };



                SDL_RenderCopy(
                    renderer,
                    nameTex,
                    nullptr,
                    &nameRect
                );



                SDL_DestroyTexture(
                    nameTex
                );


            }



        }









        //================================
        // 台词
        //================================


        SDL_Rect textRect =
        {
            100,
            710,
            1300,
            160
        };


        if(!choosing)
        {


            text.Render(
                textRect
            );


        }
        








        //================================
        // 选择显示
        //================================


        if(choosing)
{


    //提示词

    SDL_Texture* tipTex =
    CreateText(
        renderer,
        title,
        "请选择"
    );


    if(tipTex)
    {

        int w,h;


        SDL_QueryTexture(
            tipTex,
            nullptr,
            nullptr,
            &w,
            &h
        );


        SDL_Rect tipRect =
        {
            100,
            640,
            w,
            h
        };


        SDL_RenderCopy(
            renderer,
            tipTex,
            nullptr,
            &tipRect
        );


        SDL_DestroyTexture(
            tipTex
        );

    }





    //选项

    int y=700;



    for(
        int i=0;
        i<now.choices.size();
        i++
    )
    {


        string option;



        if(
            i==choiceIndex
        )
        {

            option="> ";

        }
        else
        {

            option="  ";

        }



        option +=
        now.choices[i];




        SDL_Texture* choiceTex =
        CreateText(
            renderer,
            number,
            option
        );



        if(choiceTex)
        {

            int w,h;


            SDL_QueryTexture(
                choiceTex,
                nullptr,
                nullptr,
                &w,
                &h
            );



            SDL_Rect r =
            {
                180,
                y,
                w,
                h
            };



            SDL_RenderCopy(
                renderer,
                choiceTex,
                nullptr,
                &r
            );


            SDL_DestroyTexture(
                choiceTex
            );


        }



        y+=40;


    }


}









        SDL_RenderPresent(
            renderer
        );



        SDL_Delay(16);



    }









    //================================
    // 清理
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