#include "Conversions.h"
#include <sstream>
#include <string>
#include<cstdlib>
#include <iostream>
#include <regex>

string integer_to_string(int num)
{
    stringstream ss;
    ss << num;
    string str = ss.str();
    return str;
}

int string_to_integer(string str)
{
    int x;
    stringstream convert(str);
    convert >> x;
    return x;
}

string decimal_to_hexadecimal(int number)
{
    stringstream sstream;
    sstream << hex << number;
    string result = sstream.str();
    transform(result.begin(), result.end(),result.begin(), ::toupper);
    return result;
}

int hexa_to_int(string code)
{
    unsigned int y;
    stringstream stream;
    stream << hex << code;
    stream >> y;
    return y;
}

bool is_number(string s3)
{
    regex e4 ("[0-9]+");
    //return str.find_first_not_of("0123456789") == std::string::npos;
    return (std::regex_match(s3, e4));
}

bool case_ins_compare(const string& s1, const string& s2)
{
    return((s1.size() == s2.size()) &&
           equal(s1.begin(), s1.end(), s2.begin(), case_ins_char_compare));
}

inline bool case_ins_char_compare(char a, char b)
{
    return(toupper(a) == toupper(b));
}


