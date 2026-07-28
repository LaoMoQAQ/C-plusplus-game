#ifndef HISTORY_H
#define HISTORY_H


#include <string>
#include <vector>



struct HistoryLine
{

    std::string speaker;


    std::string text;


};





class History
{


public:


    History();



    // 添加一条历史记录
    void Add(
        const std::string& speaker,
        const std::string& text
    );



    // 获取全部历史
    const std::vector<HistoryLine>& GetAll() const;



    // 清空历史
    void Clear();



    // 获取数量
    int Size() const;




private:


    std::vector<HistoryLine> lines;



};



#endif