#include "Character.h"



Character::Character()
{

    name = "";

    image = "";

    affection = 0;

}







Character::Character(

    const std::string& name,

    const std::string& image

)
{


    this->name = name;


    this->image = image;


    affection = 0;


}








std::string Character::GetName() const
{

    return name;

}







std::string Character::GetImage() const
{

    return image;

}








int Character::GetAffection() const
{

    return affection;

}








void Character::AddAffection(

    int value

)
{

    affection += value;


}








void Character::SetAffection(

    int value

)
{

    affection=value;


}