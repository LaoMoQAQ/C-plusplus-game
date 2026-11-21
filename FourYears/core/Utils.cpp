#include "Utils.h"


#include <fstream>

#include <sstream>

#include <chrono>

#include <ctime>



namespace Utils
{


bool FileExists(

    const std::string& path

)
{


    std::ifstream file(
        path
    );


    return file.good();


}








std::string GetFileName(

    const std::string& path

)
{


    size_t pos =
    path.find_last_of(
        "/\\"
    );



    if(pos==std::string::npos)
    {

        return path;

    }



    return path.substr(
        pos+1
    );


}








std::string GetDirectory(

    const std::string& path

)
{


    size_t pos =
    path.find_last_of(
        "/\\"
    );



    if(pos==std::string::npos)
    {

        return "";

    }



    return path.substr(
        0,
        pos
    );


}








std::vector<std::string> Split(

    const std::string& str,

    char delimiter

)
{


    std::vector<std::string> result;


    std::stringstream ss(
        str
    );


    std::string item;



    while(
        std::getline(
            ss,
            item,
            delimiter
        )
    )
    {


        result.push_back(
            item
        );


    }



    return result;


}








std::string Replace(

    std::string str,

    const std::string& from,

    const std::string& to

)
{


    size_t pos=0;



    while(
        (
            pos =
            str.find(
                from,
                pos
            )
        )
        !=
        std::string::npos
    )
    {


        str.replace(
            pos,
            from.length(),
            to
        );


        pos += to.length();


    }



    return str;


}








std::string GetTime()

{


    auto now =
    std::chrono::system_clock::to_time_t(
        std::chrono::system_clock::now()
    );



    std::string result =
    std::ctime(
        &now
    );



    if(!result.empty())
    {

        result.pop_back();

    }



    return result;


}



}