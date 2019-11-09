#ifndef ENGSTR_H
#define ENGSTR_H

#include <string>


class EngStr
{
public:
    EngStr();
    static const std::string restr;
    static std::string doubleToStr(double, int);
    static double strToDouble(std::string);
};

#endif // ENGSTR_H
