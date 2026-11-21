#include "StoryGraph.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <map>
#include <queue>


namespace fs = std::filesystem;


// ==========================================================
// 章节顺序（线性解锁按这个来）
// ==========================================================
//
// 一个章节一个脚本文件。顺序按阅读顺序排；
// 没在这个表里的文件会被排到最后（方便以后加新章节又不改代码）。

static const char* CHAPTER_ORDER[] =
{
    "script/chapter01.txt",
    "script/chapter02.txt",
    "script/chapter03.txt",
    "script/chapter04.txt",
    "script/ending.txt",
    "script/li_junhao_route.txt",
    "script/zhang_hanyu_route.txt",
    "script/alone_ending.txt",
    "script/afterstory.txt",
};


static int OrderOf(const std::string& file)
{
    // 排序规则：
    //   1. 主线 chapterNN 按数字升序（0..99）
    //   2. 结局 / 角色线 / 后日谈排在主线之后（100 + 字母序）
    //
    // 原来是硬编码 CHAPTER_ORDER 查表，只覆盖特定几个文件名，
    // 新增章节会全部落到同一个兜底值，排序就乱了。
    // 这里改成按数字解析，chapter01~chapter99 都能正确排序。

    {
        size_t pos = file.find("chapter");

        if(pos != std::string::npos)
        {
            std::string num;

            for(size_t k = pos + 7; k < file.size(); k++)
            {
                if(file[k] >= '0' && file[k] <= '9')
                    num += file[k];
                else
                    break;
            }

            if(!num.empty())
            {
                int n = std::atoi(num.c_str());

                if(n >= 1 && n <= 99)
                    return n - 1;
            }
        }
    }

    // 非主线：固定顺序，保证每次启动排列一致
    if(file.find("li_junhao_route") != std::string::npos) return 100;
    if(file.find("zhang_hanyu_route") != std::string::npos) return 101;
    if(file.find("alone_ending") != std::string::npos) return 102;
    if(file.find("afterstory") != std::string::npos) return 103;
    if(file.find("ending") != std::string::npos) return 104;

    return 200;
}



std::string StoryGraph::TitleFor(
    const std::string& file
)
{
    // 主线章节：chapterNN -> 第N章
    // 用数字解析而不是硬编码查表，新增章节不用改这里。
    {
        size_t pos = file.find("chapter");

        if(pos != std::string::npos)
        {
            std::string num;

            for(size_t k = pos + 7; k < file.size(); k++)
            {
                if(file[k] >= '0' && file[k] <= '9')
                    num += file[k];
                else
                    break;
            }

            if(!num.empty())
            {
                int n = std::atoi(num.c_str());

                if(n > 0)
                    return "第" + std::to_string(n) + "章";
            }
        }
    }

    if(file.find("li_junhao_route") != std::string::npos) return "李君浩线";
    if(file.find("zhang_hanyu_route") != std::string::npos) return "张瀚宇线";
    if(file.find("alone_ending") != std::string::npos) return "单人结局";
    if(file.find("afterstory") != std::string::npos) return "后日谈";
    if(file.find("ending") != std::string::npos) return "结局";

    // 兜底：直接用文件名
    std::string name = file;

    size_t slash = name.find_last_of('/');

    if(slash != std::string::npos)
        name = name.substr(slash + 1);

    size_t dot = name.find_last_of('.');

    if(dot != std::string::npos)
        name = name.substr(0, dot);

    return name;
}



// 章节副标题（进场标题卡用）
// ==========================================================

std::string StoryGraph::SubtitleFor(
    const std::string& file
)
{
    static const char* subtitles[] = {
        "九月 · 初见",
        "十月 · 靠近",
        "十一月 · 心事",
        "十二月 · 选择",
        "一月 · 寒假",
        "二月 · 归途",
        "三月 · 重逢",
        "四月 · 樱花",
        "五月 · 告白",
        "六月 · 离别"
    };

    {
        size_t pos = file.find("chapter");

        if(pos != std::string::npos)
        {
            std::string num;

            for(size_t k = pos + 7; k < file.size(); k++)
            {
                if(file[k] >= '0' && file[k] <= '9')
                    num += file[k];
                else
                    break;
            }

            if(!num.empty())
            {
                int n = std::atoi(num.c_str());

                if(n >= 1 && n <= 10)
                    return subtitles[n - 1];
            }
        }
    }

    if(file.find("li_junhao_route") != std::string::npos) return "未说出口的话";
    if(file.find("zhang_hanyu_route") != std::string::npos) return "并肩的那年";
    if(file.find("alone_ending") != std::string::npos) return "一个人的成长";
    if(file.find("afterstory") != std::string::npos) return "四年之后";
    if(file.find("ending") != std::string::npos) return "尾声";

    return "";
}



// ==========================================================
// 从一行文本里找出所有 "script/xxx.txt" 形式的引用
static void CollectRefs(
    const std::string& line,
    std::vector<std::string>& out
)
{
    const std::string prefix = "script/";

    size_t pos = 0;

    while(true)
    {
        size_t at = line.find(prefix, pos);

        if(at == std::string::npos)
        {
            return;
        }

        size_t end = at;

        while(end < line.size())
        {
            char c = line[end];

            if(c == ' ' || c == '\t' || c == '\r' || c == '\n')
            {
                break;
            }

            end++;
        }

        std::string ref = line.substr(at, end - at);

        if(ref.size() > 4 && ref.compare(ref.size() - 4, 4, ".txt") == 0)
        {
            out.push_back(ref);
        }

        pos = end;
    }
}



void StoryGraph::Build(
    const std::string& scriptDir,
    const std::string& rootFile
)
{
    nodes.clear();
    directory = scriptDir;

    std::error_code ec;

    if(!fs::is_directory(scriptDir, ec))
    {
        return;
    }


    // 1. 收集所有剧本文件
    std::vector<std::string> files;

    for(auto& entry : fs::directory_iterator(scriptDir, ec))
    {
        if(!entry.is_regular_file())
        {
            continue;
        }

        if(entry.path().extension() != ".txt")
        {
            continue;
        }

        files.push_back(
            "script/" + entry.path().filename().string()
        );
    }

    std::sort(files.begin(), files.end());


    // 2. 每个文件一个节点，解析出它的下游文件
    std::map<std::string, int> index;

    for(auto& f : files)
    {
        StoryNode node;
        node.file = f;
        node.title = TitleFor(f);
        node.order = OrderOf(f);

        std::ifstream in(scriptDir + "/" + fs::path(f).filename().string());

        std::string line;

        std::vector<std::string> refs;

        while(std::getline(in, line))
        {
            CollectRefs(line, refs);
        }

        std::sort(refs.begin(), refs.end());
        refs.erase(std::unique(refs.begin(), refs.end()), refs.end());

        for(auto& r : refs)
        {
            if(r == f)
            {
                continue;
            }

            node.children.push_back(r);
        }

        index[f] = (int)nodes.size();
        nodes.push_back(node);
    }


    // 3. 从根节点 BFS 排层级，顺便去掉指向不存在文件的边
    for(auto& node : nodes)
    {
        std::vector<std::string> keep;

        for(auto& c : node.children)
        {
            if(index.count(c))
            {
                keep.push_back(c);
            }
        }

        node.children.swap(keep);
    }

    const int UNVISITED = -1;

    for(auto& node : nodes)
    {
        node.depth = UNVISITED;
    }

    std::queue<std::string> q;

    if(index.count(rootFile))
    {
        nodes[index[rootFile]].depth = 0;
        q.push(rootFile);
    }
    else if(!nodes.empty())
    {
        nodes[0].depth = 0;
        q.push(nodes[0].file);
    }

    while(!q.empty())
    {
        std::string cur = q.front();
        q.pop();

        int curDepth = nodes[index[cur]].depth;

        for(auto& c : nodes[index[cur]].children)
        {
            if(nodes[index[c]].depth == UNVISITED)
            {
                nodes[index[c]].depth = curDepth + 1;
                q.push(c);
            }
        }
    }


    // 4. 没连上的孤立节点排到最后
    int fallback = 0;

    for(auto& node : nodes)
    {
        if(node.depth == UNVISITED)
        {
            node.depth = 1;
            fallback++;
        }
    }

    (void)fallback;


    // 5. 按章节顺序排（线性解锁就是按这个顺序解锁的），
    //    depth 仍然用来做缩进，让人看得出分支
    std::stable_sort(
        nodes.begin(),
        nodes.end(),
        [](const StoryNode& a, const StoryNode& b)
        {
            if(a.order != b.order)
            {
                return a.order < b.order;
            }

            return a.title < b.title;
        }
    );
}



const std::vector<StoryNode>& StoryGraph::GetNodes() const
{
    return nodes;
}



int StoryGraph::IndexOf(
    const std::string& file
) const
{
    for(int i = 0; i < (int)nodes.size(); i++)
    {
        if(nodes[i].file == file)
        {
            return i;
        }
    }

    return -1;
}



void StoryGraph::MarkVisited(
    const std::string& file
)
{
    if(file.empty())
    {
        return;
    }

    visited.insert(file);

    int idx = IndexOf(file);

    if(idx >= 0)
    {
        nodes[idx].visited = true;
    }
}



bool StoryGraph::IsVisited(
    const std::string& file
) const
{
    return visited.count(file) > 0;
}



int StoryGraph::FurthestVisitedOrder() const
{
    int furthest = -1;

    for(auto& node : nodes)
    {
        if(IsVisited(node.file) && node.order > furthest)
        {
            furthest = node.order;
        }
    }

    return furthest;
}



int StoryGraph::UnlockedUpTo() const
{
    // 什么都没看过 -> 只解锁第 1 章
    // 看过第 N 章 -> 解锁到第 N+1 章
    return FurthestVisitedOrder() + 1;
}



bool StoryGraph::CanEnter(
    int nodeIndex
) const
{
    if(nodeIndex < 0 || nodeIndex >= (int)nodes.size())
    {
        return false;
    }

    return nodes[nodeIndex].order <= UnlockedUpTo();
}



void StoryGraph::LoadProgress(
    const std::string& path
)
{
    visited.clear();

    std::ifstream in(path);

    if(!in)
    {
        return;
    }

    std::string line;

    while(std::getline(in, line))
    {
        if(!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }

        if(!line.empty())
        {
            visited.insert(line);
        }
    }

    for(auto& node : nodes)
    {
        node.visited = IsVisited(node.file);
    }
}



void StoryGraph::SaveProgress(
    const std::string& path
) const
{
    std::ofstream out(path, std::ios::trunc);

    if(!out)
    {
        return;
    }

    for(auto& f : visited)
    {
        out << f << "\n";
    }
}
