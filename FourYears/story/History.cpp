#include "History.h"



History::History()
{

}





void History::Add(

    const std::string& speaker,

    const std::string& text

)
{


    HistoryLine line;


    line.speaker = speaker;


    line.text = text;



    lines.push_back(
        line
    );


}







const std::vector<HistoryLine>& History::GetAll() const
{


    return lines;


}







void History::Clear()
{


    lines.clear();


}







int History::Size() const
{


    return lines.size();


}