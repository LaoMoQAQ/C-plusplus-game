#pragma once


#include "Story.h"

#include <string>


using namespace std;



class StoryParser
{

public:


    bool Load(
        string file,
        Story& story
    );



private:


    //清理BOM、\r、空格

    string Clean(
        string text
    );


};