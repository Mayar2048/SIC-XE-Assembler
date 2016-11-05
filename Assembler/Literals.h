#ifndef LITERALS_H
#define LITERALS_H
#include <string>
#include <vector>
#include <map>

using namespace std;

extern void load_literals_information();

extern void load(string name, vector<string> &vec);

extern pair <bool,vector<string>> is_literal(string mnemonic, string operand, map<string, bool> &flags, bool format4);

extern string literal_operand_ascii_value(string operand);

extern string literal_operand_decimal_value(string operand);

extern bool check_usage(vector<string> &vec, string str);

extern int format_length(bool format4);

extern int operand_length(string operand);

extern bool warning();

#endif // LITERALS_H
