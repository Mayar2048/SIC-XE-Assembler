#ifndef ADDRESSINFORMATION_H_INCLUDED
#define ADDRESSINFORMATION_H_INCLUDED
#include<string>
using namespace std;

extern bool is_indirect(string operand);

extern bool is_immediate(string operand);

extern bool is_indexing(string operand);

extern bool is_literal_info(string opernad);

extern bool is_simple_expression(string operand);

#endif // ADDRESSINFORMATION_H_INCLUDED
