#ifndef HISTORY_H
#define HISTORY_H


#include <string>
#include <vector>



// ==========================================================
// HistoryLine
// ==========================================================
//
// 一条对话记录（说话人 + 内容）。

struct HistoryLine
{
    std::string speaker;
    std::string text;
};



// ==========================================================
// History
// ==========================================================
//
// 对话历史。
//
// 当前状态：
//   - Story::Next() 每次推进都会 Add 一条
//   - UI 层已改用好感度页面，不再显示历史
//   - 保留是为了将来可能做"剧情回退"功能
//
// 注意：
//   - 存档不保存历史，跨会话会丢
//   - 跨章节保留（切换脚本不清空）

class History
{

public:

    History();



    // 追加一条记录。
    void Add(
        const std::string& speaker,
        const std::string& text
    );



    // 获取全部记录（只读）。
    const std::vector<HistoryLine>& GetAll() const;



    // 清空。
    void Clear();



    // 记录条数。
    int Size() const;



private:

    std::vector<HistoryLine> lines;

};


#endif