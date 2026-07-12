#include "StoryParser.h"

#include <fstream>
#include <iostream>


using namespace std;



//================================
// 清理文本
//================================

string StoryParser::Clean(
    string text
)
{


    // UTF8 BOM

    if(
        text.size()>=3 &&
        (unsigned char)text[0]==0xEF &&
        (unsigned char)text[1]==0xBB &&
        (unsigned char)text[2]==0xBF
    )
    {

        text.erase(
            0,
            3
        );

    }



    // 删除\r和尾空格

    while(
        !text.empty() &&
        (
            text.back()=='\r' ||
            text.back()==' '
        )
    )
    {

        text.pop_back();

    }


    return text;

}






//================================
// 加载剧情
//================================

bool StoryParser::Load(
    string file,
    Story& story
)
{


    ifstream in(
        file
    );


    if(!in)
    {

        cout
        <<"无法打开:"
        <<file
        <<endl;


        return false;

    }




    string line;


    StoryEvent current;



    while(
        getline(in,line)
    )
    {


        line=
        Clean(line);



        if(line.empty())
            continue;





        //================================
        // 标签
        //================================


        if(
            line.front()=='[' &&
            line.back()==']'
        )
        {


            string tag=
            line.substr(
                1,
                line.size()-2
            );





            // 保存上一段

            if(
                !current.name.empty() &&
                (
                    !current.text.empty() ||
                    current.isChoice
                )
            )
            {

                story.Add(
                    current
                );


                current=
                StoryEvent();

            }







            //========================
            // 背景
            //========================

            if(tag=="背景")
            {


                string bg;


                getline(
                    in,
                    bg
                );


                current.background=
                Clean(bg);


                continue;

            }








            //========================
            // 立绘
            //========================


            if(tag=="立绘")
            {


                string c;


                getline(
                    in,
                    c
                );


                current.character=
                Clean(c);


                continue;

            }








            //========================
            // 选择
            //========================


            if(tag=="选择")
            {

                current.name=
                "选择";


                current.isChoice=
                true;


                continue;

            }








            //========================
            // 结果
            //========================


            if(tag=="结果")
            {

                string route;


                getline(
                    in,
                    route
                );


                current.choiceResult=
                Clean(route);


                continue;

            }









            //========================
            // 普通角色
            //========================


            current.name=
            tag;



            continue;


        }








        //================================
        // 选择内容
        //================================


        if(
            current.isChoice
        )
        {


            //格式:
            //
            //1. 去找他
            //2. 离开


            if(
                line.size()>2 &&
                line[0]>='0' &&
                line[0]<='9'
            )
            {


                string option=
                line.substr(
                    2
                );


                current.choices.push_back(
                    option
                );


            }



            continue;


        }








        //================================
        // 普通文本
        //================================


        if(
            !current.text.empty()
        )
        {

            current.text+="\n";

        }



        current.text+=line;



    }









    // 文件结束保存

    if(
        !current.name.empty() &&
        (
            !current.text.empty() ||
            current.isChoice
        )
    )
    {

        story.Add(
            current
        );

    }





    in.close();





    cout
    <<"读取完成:"
    <<file
    <<endl;


    cout
    <<"数量:"
    <<story.events.size()
    <<endl;




    return true;

}