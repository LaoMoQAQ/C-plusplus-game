#include "StoryParser.h"

#include <fstream>
#include <iostream>


using namespace std;



string StoryParser::Clean(
    string text
)
{

    if(
        text.size()>=3
        &&
        (unsigned char)text[0]==0xEF
        &&
        (unsigned char)text[1]==0xBB
        &&
        (unsigned char)text[2]==0xBF
    )
    {
        text.erase(
            0,
            3
        );
    }



    while(
        !text.empty()
        &&
        (
            text.back()=='\r'
            ||
            text.back()==' '
        )
    )
    {
        text.pop_back();
    }


    return text;

}









bool StoryParser::Load(
    string file,
    Story& story
)
{

    ifstream in(file);



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




    string currentBackground;


    string currentCharacter;



    float currentWait=0.0f;




    auto SaveCurrent=[&]()
    {


        if(
            current.name.empty()
        )
        {
            return;

        }


        if(
            current.text.empty()
            &&
            !current.isChoice
        )
        {
            return;

        }



        if(
            current.background.empty()
        )
        {
            current.background=
            currentBackground;
        }



        if(
            current.character.empty()
        )
        {
            current.character=
            currentCharacter;
        }




        if(
            current.waitTime==0
        )
        {
            current.waitTime=
            currentWait;
        }



        currentWait=0;



        story.Add(
            current
        );



        current=
        StoryEvent();

    };









    while(
        getline(
            in,
            line
        )
    )
    {


        line=
        Clean(line);





        if(
            line.empty()
        )
        {
            continue;
        }







        //========================
        // 等待
        //========================

        if(
            line.front()=='<'
            &&
            line.back()=='>'
        )
        {

            try
            {

                currentWait=
                stof(
                    line.substr(
                        1,
                        line.size()-2
                    )
                );

            }
            catch(...)
            {

                currentWait=0;

            }


            continue;

        }









        //========================
        // 标签
        //========================


        if(
            line.front()=='['
            &&
            line.back()==']'
        )
        {


            string tag=
            line.substr(
                1,
                line.size()-2
            );





            // 背景

            if(
                tag=="背景"
            )
            {

                string bg;


                getline(
                    in,
                    bg
                );


                bg=
                Clean(bg);



                currentBackground=
                bg;



                continue;

            }








            // 立绘

            if(
                tag=="立绘"
            )
            {

                string c;


                getline(
                    in,
                    c
                );


                c=
                Clean(c);



                currentCharacter=
                c;



                continue;

            }








            // BGM预留

            if(
                tag=="BGM"
            )
            {

                string bgm;


                getline(
                    in,
                    bgm
                );


                bgm=
                Clean(bgm);



                continue;

            }








            // SE预留

            if(
                tag=="SE"
            )
            {

                string se;


                getline(
                    in,
                    se
                );


                se=
                Clean(se);



                continue;

            }








            // 选择

            if(
                tag=="选择"
            )
            {

                SaveCurrent();



                current.name=
                "选择";


                current.isChoice=true;



                continue;

            }








            // 普通人物

            SaveCurrent();



            current.name=
            tag;



            current.background=
            currentBackground;



            current.character=
            currentCharacter;



            continue;

        }












        //========================
        // 选择内容
        //========================


        if(
            current.isChoice
        )
        {


            if(
                line.size()>2
                &&
                line[0]>='0'
                &&
                line[0]<='9'
            )
            {

                current.choices.push_back(
                    line.substr(2)
                );

            }



            continue;

        }












        //========================
        // 普通文本
        //========================


        if(
            !current.text.empty()
        )
        {

            current.text+="\n";

        }



        current.text+=line;



    }









    SaveCurrent();





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