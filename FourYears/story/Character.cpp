#include "Character.h"



Character::Character()
{
    name  = "";
    image = "";
}





Character::Character(
    const std::string& name,
    const std::string& image
)
{
    this->name  = name;
    this->image = image;
}





std::string Character::GetName() const
{
    return name;
}





std::string Character::GetImage() const
{
    return image;
}