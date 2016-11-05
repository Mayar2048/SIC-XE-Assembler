
#include "Literals.h"
#include"Conversions.h"
#include <algorithm>
#include <regex>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;

vector<string> not_allowed;

vector<string> restricted;

bool literal_warning = false;

void load_literals_information()
{
    load("NotAllowed.txt",not_allowed);
    load("restrictedLiterals.txt",restricted);
}

void load(string name, vector<string> &vec)
{
    string line;
    ifstream myfile(name);

    if (myfile.is_open())
    {
        while (!myfile.eof() )
        {
            getline (myfile,line);
            vec.push_back(line);
        }
        myfile.close();
    }
    else cout << "Unable to open file";
}

pair <bool,vector<string>> is_literal(string mnemonic, string operand, map<string, bool> &flags, bool format4)
{
    vector<string> result;
    bool format = format4;
    literal_warning = false;
    smatch group;

    regex e1("[=][xX]['][a-fA-F0-9]{1,14}[']");
    regex e2("[=][cC]['][a-zA-Z0-9\\s!@#$%^&*()-*~/':;]{1,14}[']");
    regex e3("[=][wW]['][/-]?[0-9]{1,14}[']");

    bool matche1 = regex_match(operand,e1);
    bool matche2 = regex_match(operand,e2);
    bool matche3 = regex_match(operand,e3);

    if(matche1)
    {
        result.push_back(operand);
        result.push_back(operand.substr(3,operand.length()-4));
        regex_search(operand,group,e1);
        if(group[0].length() != operand.length())
        {
            flags["extra characters at end of statement"] = true;
            result.push_back(integer_to_string(format_length(format4)));
        }
        else
        {
            if(mnemonic == "BYTE")
            {
                if((operand_length(operand) % 2) != 0)
                {
                    flags["odd length of hex string"] = true;
                    result.push_back(integer_to_string(0));
                }
                else
                {
                    literal_warning = true;
                    result.push_back(integer_to_string(operand_length(operand)/2));
                }
            }
            else if(mnemonic == "WORD")
            {
                flags["illegal operand"] = true;
                result.push_back(integer_to_string(0));
            }
            else if(check_usage(not_allowed, mnemonic))
            {
                flags["wrong use of a literal (used as destination)"] = true;
                result.push_back(integer_to_string(0));
            }
            else if(check_usage(restricted, mnemonic))
            {
                if(operand_length(operand) != 6)
                {
                    flags["incorrect length for literal"] = true;
                    result.push_back(integer_to_string(0));
                }
                else
                {
                    result.push_back(integer_to_string(operand_length(operand)/2));
                    literal_warning = true;
                }
            }
            else
            {
                if(operand_length(operand) != 2)
                {
                    flags["incorrect length for literal"] = true;
                    result.push_back(integer_to_string(0));
                }
                else
                {
                    literal_warning = true;
                    result.push_back(integer_to_string(operand_length(operand)/2));
                }
            }
        }
        result.push_back("");
        return make_pair(true,result);
    }
    else if(matche2)
    {
        result.push_back(operand);
        result.push_back(literal_operand_ascii_value(operand));
        regex_search(operand,group,e2);
        if(group[0].length() != operand.length())
        {
            flags["extra characters at end of statement"] = true;
            result.push_back(integer_to_string(0));
        }
        else
        {
            if(mnemonic == "BYTE")
            {
                literal_warning = true;
                result.push_back(integer_to_string(operand_length(operand)));
            }
            else if(mnemonic == "WORD")
            {
                flags["illegal operand"] = true;
                result.push_back(integer_to_string(0));
            }
            else if(check_usage(not_allowed, mnemonic))
            {
                flags["wrong use of a literal (used as destination)"] = true;
                result.push_back(integer_to_string(0));
            }
            else if(check_usage(restricted, mnemonic))
            {
                if(operand_length(operand) != 3)
                {
                    flags["incorrect length for literal"] = true;
                    result.push_back(integer_to_string(0));
                }
                else
                {
                    literal_warning = true;
                    result.push_back(integer_to_string(operand_length(operand)));
                }
            }
            else
            {
                if(operand_length(operand) != 1)
                {
                    flags["incorrect length for literal"] = true;
                    result.push_back(integer_to_string(0));
                }
                else
                {
                    literal_warning = true;
                    result.push_back(integer_to_string(operand_length(operand)));
                }
            }
        }
        result.push_back("");
        return make_pair(true,result);
    }
    else if(matche3)
    {
        result.push_back(operand);
        int num = string_to_integer(operand.substr(3,operand.length()-4));
        result.push_back(literal_operand_decimal_value(decimal_to_hexadecimal(num)));
        regex_search(operand,group,e3);
        if(group[0].length() != operand.length())
        {
            flags["extra characters at end of statement"] = true;
            result.push_back(integer_to_string(0));
        }
        else
        {
            if(mnemonic == "BYTE")
            {
                flags["illegal operand"] = true;
                result.push_back(integer_to_string(0));
            }
            else if(mnemonic == "WORD")
            {
                literal_warning = true;
                result.push_back(integer_to_string(3));
            }
            else if(check_usage(not_allowed, mnemonic))
            {
                flags["wrong use of a literal (used as destination)"] = true;
                result.push_back(integer_to_string(0));
            }
            else if(check_usage(restricted, mnemonic))
            {
                // some check
                literal_warning = true;
                result.push_back(integer_to_string(3));
            }
            else
            {
                flags["illegal operand"] = true;
                result.push_back(integer_to_string(3));
            }
        }
        result.push_back("");
        return make_pair(true,result);
    }
    else
    {
        return make_pair(false,result);
    }
}

string literal_operand_ascii_value(string operand)
{
    string result = "";
    for(int i = 3; i < operand.length()-2; i++)
    {
        int value = int(operand.at(i));
        stringstream stream;
        stream << hex << value;
        result += stream.str();
    }
    return result;
}

string literal_operand_decimal_value(string operand)
{
    int x;
    stringstream convert(operand);
    convert >> x;

    stringstream sstream;
    sstream << hex << x;
    string result = sstream.str();
    return result ;
}

bool check_usage(vector<string> &vec, string str)
{
    if(find(vec.begin(),vec.end(), str)!= vec.end())
    {
        return true;
    }
    return false;
}

int format_length(bool format4)
{
    if(format4)
    {
        return 4;
    }
    else
    {
        return 3;
    }
}

int operand_length(string operand)
{
    return operand.length()- 4;
}

bool warning()
{
    return literal_warning;
}
