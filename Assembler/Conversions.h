#ifndef CONVERSIONS_H
#define CONVERSIONS_H
#include <string>

using namespace std;

extern string integer_to_string(int num);

extern int string_to_integer(string str);

extern string decimal_to_hexadecimal(int number);

extern int hexa_to_int(string code);

extern bool is_number(string s3);

extern bool case_ins_compare(const string& s1, const string& s2);

extern inline bool case_ins_char_compare(char a, char b);

#endif // CONVERSIONS_H
