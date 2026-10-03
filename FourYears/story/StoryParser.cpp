#include "StoryParser.h"

#include <fstream>
#include <iostream>
#include <sstream>


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
        text.erase(0, 3);
    }

    while(
        !text.empty()
        &&
        (text.back()=='\r' || text.back()==' ')
    )
    {
        text.pop_back();
    }

    return text;
}





static string Trim(const string& s)
{
    size_t a = s.find_first_not_of(" \t");
    if(a == string::npos) return "";
    size_t b = s.find_last_not_of(" \t");
    return s.substr(a, b - a + 1);
}





static void ParseAffection(
    const string& str,
    vector<AffectionChange>& out
)
{
    stringstream ss(str);
    string item;

    while(getline(ss, item, ','))
    {
        item = Trim(item);
        if(item.empty()) continue;

        size_t sp = item.find_last_of(" \t");
        if(sp == string::npos) continue;

        string name = Trim(item.substr(0, sp));
        string val  = Trim(item.substr(sp + 1));

        if(name.empty() || val.empty()) continue;

        try
        {
            int delta = stoi(val);

            AffectionChange ch;
            ch.character = name;
            ch.delta = delta;

            out.push_back(ch);
        }
        catch(...)
        {
        }
    }
}





static void ParseChoiceLine(
    const string& raw,
    string& text,
    vector<AffectionChange>& affection,
    string& target
)
{
    string s = raw;

    size_t arrow = s.find("->");
    if(arrow != string::npos)
    {
        target = Trim(s.substr(arrow + 2));
        s = s.substr(0, arrow);
    }

    size_t bar = s.find('|');
    if(bar != string::npos)
    {
        string affstr = s.substr(bar + 1);
        ParseAffection(affstr, affection);
        s = s.substr(0, bar);
    }

    text = Trim(s);
}








bool StoryParser::Load(
    string file,
    Story& story
)
{
    ifstream in(file);

    if(!in)
    {
        cout << "无法打开:" << file << endl;
        return false;
    }





    string line;

    StoryEvent current;

    string currentBackground;

    // 三个立绘位置的当前状态
    string currentLeft;
    string currentCenter;
    string currentRight;

    // [新增] 当前 BGM 状态，会延续到后续事件
    string currentBgm;

    float currentWait = 0.0f;




    auto SaveCurrent = [&]()
    {
        if(current.name.empty())
            return;

        if(
            current.text.empty()
            &&
            !current.isChoice
            &&
            !current.isEndingBranch
        )
        {
            return;
        }


        if(current.background.empty())
            current.background = currentBackground;

        // 立绘三个位置分别继承
        if(current.characterLeft.empty())
            current.characterLeft = currentLeft;

        if(current.characterCenter.empty())
            current.characterCenter = currentCenter;

        if(current.characterRight.empty())
            current.characterRight = currentRight;

        // [新增] BGM 继承
        if(current.bgm.empty())
            current.bgm = currentBgm;

        if(current.waitTime == 0)
            current.waitTime = currentWait;

        currentWait = 0;

        story.Add(current);

        current = StoryEvent();
    };



    // ==========================================================
    // 统一参数获取
    // ==========================================================
    //
    // 优先用行内参数（[标签] xxx 的单行写法），
    // 没有的话读下一行（两行写法）。

    auto GetArg = [&](const string& inlineArg) -> string
    {
        if(!inlineArg.empty())
            return inlineArg;

        string v;

        if(getline(in, v))
            return Clean(v);

        return "";
    };



    while(getline(in, line))
    {
        line = Clean(line);

        if(line.empty())
            continue;




        //========================
        // 等待
        //========================

        if(line.front()=='<' && line.back()=='>')
        {
            try
            {
                currentWait = stof(
                    line.substr(1, line.size()-2)
                );
            }
            catch(...)
            {
                currentWait = 0;
            }

            continue;
        }






        //========================
        // 标签
        //========================

        if(line.front()=='[')
        {
            size_t close = line.find(']');

            if(close == string::npos)
            {
                goto NORMAL_TEXT;
            }


            string tag = line.substr(1, close - 1);

            string inlineArg;

            if(close + 1 < line.size())
            {
                inlineArg = Trim(line.substr(close + 1));
            }




            // 背景

            if(tag=="背景")
            {
                string bg = GetArg(inlineArg);

                currentBackground = bg;

                if(
                    !current.name.empty()
                    &&
                    current.background.empty()
                )
                {
                    current.background = bg;
                }

                continue;
            }






            // 立绘
            //
            // 支持：
            //   [立绘] xxx.png          右位置
            //   [立绘 左] xxx.png
            //   [立绘 中] xxx.png
            //   [立绘 右] xxx.png
            //
            // 特殊值 "clear" 清除该位置。

            // 先把 tag 按第一个空格拆成"主标签 + 位置参数"
            string tagName = tag;
            string tagArg;

            {
                size_t sp = tag.find(' ');

                if(sp != string::npos)
                {
                    tagName = tag.substr(0, sp);
                    tagArg  = Trim(tag.substr(sp + 1));
                }
            }


            if(tagName == "立绘")
            {
                // 位置：默认右
                string slot = tagArg;

                if(slot.empty())
                    slot = "右";


                // 文件名：优先行内参数，其次下一行
                string path = GetArg(inlineArg);


                // 更新"当前状态"，供后续事件继承
                if(slot == "左")      currentLeft   = path;
                else if(slot == "中") currentCenter = path;
                else                  currentRight  = path;


                // 立即应用到当前未保存的事件
                if(!current.name.empty())
                {
                    if(slot == "左")      current.characterLeft   = path;
                    else if(slot == "中") current.characterCenter = path;
                    else                  current.characterRight  = path;
                }

                continue;
            }






            // [新增] BGM
            //
            // 语法：
            //   [BGM] school_daily.mp3
            //   [BGM] stop
            //
            // 会延续到后续所有事件，直到下一条 [BGM]。

            if(tag=="BGM")
            {
                string bgm = GetArg(inlineArg);

                currentBgm = bgm;

                if(!current.name.empty())
                {
                    current.bgm = bgm;
                }

                continue;
            }






            // [新增] SE
            //
            // 语法：
            //   [SE] typing.wav
            //
            // 一次性音效，只贴到当前事件上。

            if(tag=="SE")
            {
                string se = GetArg(inlineArg);

                if(!current.name.empty())
                {
                    current.se = se;
                }

                continue;
            }






            // 选择

            if(tag=="选择")
            {
                SaveCurrent();

                current.name = "选择";
                current.isChoice = true;

                continue;
            }






            // 标签

            if(tag=="标签")
            {
                SaveCurrent();

                string name = GetArg(inlineArg);

                if(!name.empty())
                {
                    story.AddLabel(
                        name,
                        (int)story.events.size()
                    );
                }

                continue;
            }






            // 跳转

            if(tag=="跳转")
            {
                SaveCurrent();

                string target = GetArg(inlineArg);

                if(!target.empty() && target[0] == '#')
                {
                    target = target.substr(1);
                }

                if(!target.empty())
                {
                    StoryEvent ev;

                    ev.isGoto = true;
                    ev.gotoLabel = target;

                    story.Add(ev);
                }

                continue;
            }






            // 结局分支

            if(tag=="结局分支")
            {
                SaveCurrent();

                current.name = "结局分支";
                current.isEndingBranch = true;

                int read = 0;

                while(read < 3 && getline(in, line))
                {
                    line = Clean(line);

                    if(line.empty())
                        continue;

                    size_t arrow = line.find("->");

                    if(arrow == string::npos)
                        continue;

                    string key = Trim(line.substr(0, arrow));
                    string val = Trim(line.substr(arrow + 2));

                    if(key == "李君浩")
                    {
                        current.branchLiJunhao = val;
                        read++;
                    }
                    else if(key == "张瀚宇")
                    {
                        current.branchZhangHanyu = val;
                        read++;
                    }
                    else if(key == "单人")
                    {
                        current.branchNormal = val;
                        read++;
                    }
                }

                continue;
            }






            // 下一章

            if(tag=="下一章")
            {
                string next = GetArg(inlineArg);

                story.SetNextFile(next);

                continue;
            }






            // 普通人物

            SaveCurrent();

            current.name = tag;
            current.background = currentBackground;

            // 立绘三个位置
            current.characterLeft   = currentLeft;
            current.characterCenter = currentCenter;
            current.characterRight  = currentRight;

            continue;
        }




    NORMAL_TEXT:





        //========================
        // 选择内容
        //========================

        if(current.isChoice)
        {
            if(
                line.size()>2
                &&
                line[0]>='0'
                && line[0]<='9'
            )
            {
                string item = line.substr(2);

                string text;
                vector<AffectionChange> aff;
                string target;

                ParseChoiceLine(item, text, aff, target);

                current.choices.push_back(text);
                current.choiceAffection.push_back(aff);
                current.choiceTargets.push_back(target);
            }

            continue;
        }







        //========================
        // 普通文本
        //========================

        if(!current.text.empty())
        {
            current.text += "\n";
        }

        current.text += line;
    }





    SaveCurrent();

    in.close();

    cout << "读取完成:" << file << endl;
    cout << "数量:" << story.events.size() << endl;

    return true;
}