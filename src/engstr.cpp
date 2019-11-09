#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <cassert>
#include <regex>
#include "engstr.h"

EngStr::EngStr() {} 

std::string EngStr::doubleToStr(double x, int prec) {
    // Convert x to string with prec digits
    if (std::isnan(x))
        return std::string("nan");
    bool isNeg = x < 0;
    double lx = abs(log10(x));
    //assert(lx >= -12);
    //assert(lx <= 12);
    int flx = int(floor(lx/3)) + 4;
    std::string suffixes[] = {"pico","nano","micro","milli","","kilo","mega","giga","tera"};
    std::string suffix = (flx >= -12 && flx <= 12) ? suffixes[flx] : std::string("toomuch");
    double norm = x / (pow(10.0, 3*(flx-4)));
    if(isNeg) norm *= -1.0;
    std::stringstream ss;
    ss << std::fixed << std::setprecision(prec) << norm << " " << suffix;
    return ss.str();   
}

double EngStr::strToDouble(std::string str) {
    using namespace std::regex_constants;
    // Parses scientific and engineering notation, eg 1.234e6p as 1.234 x 10^-3
    //std::regex pattern("^[-+]?[0-9]*\\.?[0-9]+([eE][-+]?[0-9]+)?$");
    //std::regex pattern("[-+]?([0-9]*[.])?[0-9]+([eE][-+]?\\d+)?"); \\s*?(p|n|u|m|k|Meg|g|t)?
    std::regex pattern(
        "((?:[+-]?\\d+(?:\\.\\d*)?|\\.\\d+)(?:[eE][+-]?\\d+)?)\\s*(?:(?:([pP])|([nN])|([uU])|([m])|([k])|([M])|([gG])|([tT]))[a-zA-Z]*)?");
    std::smatch match;
    double base = 0.0;
    double exp = 0;
    double result = std::numeric_limits<double>::quiet_NaN();
    if (std::regex_match(str, match, pattern)) {
        base = double(std::stod(match[1].str()));
        for (int i=1; i < match.size(); i++) {
            if (match[i].length() > 0 && i > 1) {
                if (i<6) exp = (i-6) * 3;
                else     exp = (i-5) * 3;
            }   
        }
        result = base * pow(10.0, exp);
        //std::cout  << result << std::endl;
    } 
    return result;
}
