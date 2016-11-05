#ifndef OBJECTCODE_H_INCLUDED
#define OBJECTCODE_H_INCLUDED
#include <map>
#include<cmath>
using namespace std;

extern void set_maps(multimap <int, int> m1, multimap <int,string> m2);

extern void set_symbol_table(unordered_map<string, pair<int, string> > t1);

extern pair<vector<pair<string,string>>, vector<string>> generate_object_code();

extern string decimal_to_binary(unsigned int dec);

extern void set_registers(map<string, int> r);

extern string int_to_string(int num);

extern string dec_to_hexa(int number);

extern string format_2(int opcode);

extern  string format_3_4(int op, int program,bool f4,int location);

extern pair < string, string > calc_bpe(int program_count , bool f4 , int location);

extern string calc_nix();

extern string refactor_disp(int len,string disp);

extern string binary_to_hex(string bin);

extern int iterate_symtab(string operand);

extern void set_base(bool b, string op);

extern string word_obcode();

extern string byte_obcode();

extern void load_ascii_table();

extern string char_to_string(char c);

extern vector<string> modification_record();

extern bool is_print ();

extern void set_literals_info(map<string, string> l1, vector<vector<string>> v1);

extern string literal_address();

extern string literal_obcode();

extern string literal_obcode_info();

extern int calculate_expression(string e);

extern bool is_relative(string str);

extern int abs_val(string str);

extern bool is_absolute(string str);

extern vector<string> split(const std::string &s, char delim);

extern string load_type(string label);

extern int load_address(string label);

extern bool is_relocatable();

extern bool is_expression_error();

#endif // OBJECTCODE_H_INCLUDED
