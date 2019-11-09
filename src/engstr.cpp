#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <cassert>
#include <regex>
#include <algorithm>
#include "engstr.h"

EngStr::EngStr() {} 

std::string EngStr::doubleToStr(double x, int prec) {
    // Convert x to string with prec digits
    if (std::isnan(x))
        return std::string("nan");
    if (x == 0.0) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(prec) << 0.0;
        return ss.str();
    }
    // eg we want indices to be computed as follows
    // x       lx  flx
    // 1e-12  -12  0
    // 1e0      0  4
    // 1e12    12  8
    double lx = log10(abs(x));
    int flx = std::min(std::max(int(floor(lx/3)), -4), 4) + 4;
    double engx = x / pow(10.0, 3 * (flx - 4));
    //std::string suffixes[] = {"pico","nano","micro","milli","","kilo","mega","giga","tera"};
    std::string suffixes[] = {"p","n","u","m","","k","M","G","T"};
    std::string suffix = suffixes[flx];
    std::stringstream ss;
    ss << std::fixed << std::setprecision(prec) << engx << suffix;
    return ss.str();   
}

double EngStr::strToDouble(std::string str) {
    using namespace std::regex_constants;
    // Parses scientific and engineering notation, eg "1.234e6p" as 1.234e-3
    std::regex pattern(
        "((?:[+-]?(?:\\d+(?:\\.\\d*)?|\\.\\d+))(?:[eE][+-]?\\d+)?)\\s*(?:(?:([pP])|([nN])|([uU])|([m])|([k])|([M])|([gG])|([tT]))[a-zA-Z]*)?");
    std::smatch match;
    double base = 0.0;
    double exp = 0;
    double result = std::numeric_limits<double>::quiet_NaN();
    if (std::regex_match(str, match, pattern)) {
        base = double(std::stod(match[1].str()));
        for (size_t i=1; i < match.size(); i++) {
            if (match[i].length() > 0 && i > 1) {
                if (i<6) exp = (int(i)-6) * 3;
                else     exp = (int(i)-5) * 3;
            }   
        }
        result = base * pow(10.0, exp);
    } 
    return result;
}
