#ifndef CHARACTER_H
#define CHARACTER_H


#include <string>



class Character
{


public:


    Character();



    Character(
        const std::string& name,
        const std::string& image
    );



    std::string GetName() const;



    std::string GetImage() const;




    int GetAffection() const;



    void AddAffection(
        int value
    );



    void SetAffection(
        int value
    );




private:


    std::string name;



    std::string image;



    int affection;



};



#endif