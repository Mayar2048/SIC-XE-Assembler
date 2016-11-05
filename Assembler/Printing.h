#include<unordered_map>
#ifndef PRINTING_H_INCLUDED
#define PRINTING_H_INCLUDED
using namespace std;

extern void print_listing_file(vector<string> list_file,unordered_map<string,pair<int, string> > table , bool flag);
extern void print_object_code(vector<pair<string,string>> objcode,string name, string length,string starting_add,vector<string> modi, bool print);
extern void set_pass2_parameters(vector<string>nixbpe,vector<pair<bool,bool>> error);
#endif // PRINTING_H_INCLUDED


