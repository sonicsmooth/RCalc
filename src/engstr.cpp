#include "engstr.h"

#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <cassert>

EngStr::EngStr()
{

}

std::string EngStr::doubleToStr(double x, int prec) {
    // Convert x to string with prec digits
    double lx = log10(x);
    assert(lx >= -12);
    assert(lx <= 12);
    int flx = int(floor(lx/3)) + 4;
    std::string suffixes[] = {"pico","nano","micro","milli","","kilo","mega","giga","tera"};
    std::string suffix = suffixes[flx];
    double norm = x / (pow(10.0, 3*(flx-4)));
    std::stringstream ss;
    ss << std::fixed << std::setprecision(prec) << norm << " " << suffix;
    return ss.str();   
    


}
