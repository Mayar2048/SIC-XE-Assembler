#include <iostream>
#include <fstream>
#include <bits\stdc++.h>
#include "Reading.h"
#include "Printing.h"
#include "ObjectCode.h"
#include "AddressInformation.h"
#include <string>
#include <sstream>
#include<vector>
#include<cstdlib>
#include <regex>
#include <map>
#include "Literals.h"
#include"Conversions.h"

using namespace std;
string name;
int location_counter = 0;
int program_counter;
int counter = 0;
unordered_map<string, pair<int, string> > symTab;
multimap<string,int> my_appendix;
map<string, int> registers;
map < string, bool > flags;
vector <string> listing_file ;
vector <int> loc ;
string source_file_name = "";
bool error = false;
int first_location;
bool progEnd=false;
bool pass2 = true;
multimap <int, int> pass2_info ;
multimap <int,string> pass2_operand;
vector<vector<string>> literal_table;
map<string, string> found;
bool literal = false;
//-----------DIRECTIVES----------//
string startName;
string  dir[11];
bool start = false;
bool base = false;
string base_op ;
//-----------DIRECTIVES----------//

void set_flags ()
{
    flags.insert(pair<string,bool>("duplicate label definition",false));
    flags.insert(pair<string,bool>("extra length",false));
    flags.insert(pair<string,bool>("invalid operation code",false));
    flags.insert(pair<string,bool>("unrecognized operation code",false));
    flags.insert(pair<string,bool>("missing or misplaced operand field",false));
    flags.insert(pair<string,bool>("extra characters at the end of the statement",false));
    flags.insert(pair<string,bool>("illegal address for a register",false));
    flags.insert(pair<string,bool>("this statement can not have an operand",false));
    flags.insert(pair<string,bool>("missing comma in operand field",false));
    flags.insert(pair<string,bool>("can not be format 4 instruction",false));
    flags.insert(pair<string,bool>("extra operand",false));
    flags.insert(pair<string,bool>("wrong use of a literal (used as destination)",false));
    //-----------DIRECTIVES----------//
    flags.insert(pair<string,bool>("illegal operand",false));
    flags.insert(pair<string,bool>("odd length of hex string",false));
    flags.insert(pair<string,bool>("too long hex-decimal string",false));
    flags.insert(pair<string,bool>("too long character string",false));
    flags.insert(pair<string,bool>("duplicate or misplaced start statement",false));
    flags.insert(pair<string,bool>("undefined symbol in operand",false));
    flags.insert(pair<string,bool>("unhandled directive in pass 1",false));
    flags.insert(pair<string,bool>("not a hexadecimal string",false));
    flags.insert(pair<string,bool>("address out of range",false));
    flags.insert(pair<string,bool>("address out of range",false));
    flags.insert(pair<string,bool>("statement should not follow END statement",false));

    //-----------LITERALS----------//
    flags.insert(pair<string,bool>("wrong use of a literal (used as destination)",false));
    flags.insert(pair<string,bool>("incorrect length for literal",false));
    flags.insert(pair<string,bool>("illegal operand",false));
}

//---------DIRECTIVES START----------//
bool directive_in_symtab(string label)
{
    unordered_map<string, pair<int, string> >::iterator it = symTab.find(label);
    if (it == symTab.end())
        return false;
    return true;
}



void load_directives()
{
    string fileName = "directives.txt";
    ifstream file;
    file.open(fileName);
    if (!file.is_open())
    {
        exit(EXIT_FAILURE);
    }
    char directives[11];
    int i = 0;
    file >> directives;
    while (file.good())
    {
        dir[i] = directives;
        i++;
        file >> directives;
    }
}

bool is_directive(string s2)
{
    int i;
    for (i = 0; i < 11; i++)
    {
        if (case_ins_compare(s2, dir[i]))
            return true;
    }
    return false;
}

int increment_num(string s3, int len) ///mod
{
    if (isalpha(s3[0]))
    {
        flags["illegal operand"] = true;
        return 0;
    }
    if (isspace(s3[0]))
    {
        flags["missing or misplaced operand field"] = true;
        return 0;
    }
    if (s3.length() > len)
    {
        s3.erase(len, s3.length());
        flags["extra characters at end of statement"] = true;
        //flags["address out of range"] = true;
    }
    return string_to_integer(s3);
}



bool check_directive(string s1, string s2, string s3)
{
    literal = false;
    set_flags();
    if (!is_directive(s2))
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////
    if(case_ins_compare(s2, "LTORG"))
    {
        pass2_operand.insert(pair<int,string>(counter,s3));
        literal = true;
        pass2_info.insert(pair<int,int>(counter,-1));
        pass2_info.insert(pair<int,int>(counter,4)); // ay 7aga
        pass2_info.insert(pair<int,int>(counter,location_counter));
        pass2_info.insert(pair<int,int>(counter,program_counter));
        counter++;
    }
    else if (case_ins_compare(s2, "START"))
    {
        name = s1;
        pass2_operand.insert(pair<int,string>(counter,s3));
        if (!start)//no start before
        {
            if(loc.size() != 0)
            {
                flags["duplicate or misplaced start statement"] = true;
            }
            else
            {
                start = true;
                location_counter =  hexa_to_int(integer_to_string(increment_num(s3, 4)));
                program_counter = location_counter;
                first_location = location_counter;
            }
        }
        else
        {
            if(!(start && location_counter == first_location))
            {
                flags["duplicate or misplaced start statement"] = true;
            }
            else
            {
                location_counter =  hexa_to_int(integer_to_string(increment_num(s3, 4)));
                program_counter = location_counter;
                first_location = location_counter;
            }
        }
        pass2_info.insert(pair<int,int>(counter,-1));
        pass2_info.insert(pair<int,int>(counter,4)); // ay 7aga
        pass2_info.insert(pair<int,int>(counter,location_counter));
        pass2_info.insert(pair<int,int>(counter,program_counter));
        counter++;

    }
    /////////////////////////////////////////////////////////////////////////////
    else if (case_ins_compare(s2, "END"))
    {
        literal = true;
        pass2_operand.insert(pair<int,string>(counter,s3));
        if(!progEnd)
        {
            progEnd=true;
            if(!s3.empty())
            {
                startName = s3;
                if (!(directive_in_symtab(s3)))
                {
                    flags["undefined symbol in operand"] = true;
                }
            }
        }
        else
            flags["statement should not follow END statement"] = true;

        pass2_info.insert(pair<int,int>(counter,-1));
        pass2_info.insert(pair<int,int>(counter,4)); // ay 7aga
        pass2_info.insert(pair<int,int>(counter,location_counter));
        pass2_info.insert(pair<int,int>(counter,program_counter));
        counter++;
    }
    ////////////////////////////////////////////////////////////////////////////////
    else if (case_ins_compare(s2, "WORD"))
    {
        pass2_operand.insert(pair<int,string>(counter,s3));
        regex word("([-]*[0-9]+[a-zA-Z]+)"); //----------------------------------> updated


        if(s3.empty())
        {
            flags["missing or misplaced operand field"]=true;
        }
        else
        {
            pair<bool,vector<string>> result = is_literal("WORD",s3,flags,false);
            if(result.first)
            {
                map<string,string>::iterator iter = found.find(s3);
                if (iter == found.end())
                {
                    found.insert(make_pair(s3,"WORD"));
                    // literal_table.push_back(result.second);
                }
            }

            else if ((is_number(s3) && s3.length() > 4) || s3.length()>8|| regex_match(s3,word))
            {
                flags["extra characters at end of statement"] = true;
            }
            location_counter = location_counter + 3;
        }
        if(!s1.empty())
        {
            symTab.insert(make_pair(s1, make_pair(location_counter, "Relocatable")));
        }

        pass2_info.insert(pair<int,int>(counter,-1));
        pass2_info.insert(pair<int,int>(counter,1)); // ay 7aga
        pass2_info.insert(pair<int,int>(counter,location_counter));
        pass2_info.insert(pair<int,int>(counter,program_counter));
        counter++;

    }
    //////////////////////////////////////////////////////////////////////////////////
    else if (case_ins_compare(s2, "BYTE"))
    {
        pass2_operand.insert(pair<int,string>(counter,s3));
        pair<bool,vector<string>> result = is_literal("BYTE",s3,flags,false);
        if(result.first)
        {
            map<string,string>::iterator iter = found.find(s3);
            if (iter == found.end())
            {
                found.insert(make_pair(s3,"BYTE"));
                location_counter += string_to_integer(result.second[2]);
                // literal_table.push_back(result.second);
            }
        }
        else
        {
            regex byte1("[cC][/'][a-zA-Z0-9\\s!@#$%^&*()-*~/':;]+[/']");//----------------------------------> updated
            regex byte2("[xX][/'][a-fA-F0-9]+[/']");
            regex byte3("[xX][/'](.*)");
            if (std::regex_match(s3, byte1))
            {
                s3.erase(s3.begin());
                s3.erase(s3.begin());
                s3.erase(s3.length() - 1);
                if (s3.length() > 15)
                {
                    flags["too long character string"] = true;
                }
                else
                {
                    symTab.insert(make_pair(s1, make_pair(location_counter, "Relocatable")));
                    location_counter = location_counter + s3.length();
                }
            }
            else if (std::regex_match(s3, byte3))
            {
                if (std::regex_match(s3, byte2))
                {
                    s3.erase(s3.begin());
                    s3.erase(s3.begin());
                    s3.erase(s3.length() - 1);
                    if (!((s3.length() % 2) == 0))
                    {
                        flags["odd length of hex string"] = true;
                    }
                    else if (s3.length() > 14)
                    {
                        flags["too long hex-decimal string"] = true;
                    }
                    else
                    {
                        symTab.insert(make_pair(s1, make_pair(location_counter, "Relocatable")));
                        location_counter = location_counter + (s3.length() / 2);
                    }
                }
                else
                {
                    flags["not a hexadecimal string"] = true;
                }
            }
            else if(s3.empty())
            {
                flags["missing or misplaced operand field"]=true;
            }
            else
            {
                flags["illegal operand"] = true;
            }
        }
        pass2_info.insert(pair<int,int>(counter,-1));
        pass2_info.insert(pair<int,int>(counter,2)); // ay 7aga
        pass2_info.insert(pair<int,int>(counter,location_counter));
        pass2_info.insert(pair<int,int>(counter,program_counter));
        counter++;

    }
    /////////////////////////////////////////////////////////////////////////////////////
    else if (case_ins_compare(s2, "RESW"))
    {
        pass2_operand.insert(pair<int,string>(counter,s3));
        if ((is_number(s3)))
        {
            if (string_to_integer(s3) < 0)
                flags["illegal operand"] = true;
            else
            {
                location_counter = location_counter + 3 * increment_num(s3, 4);
            }

        }
        else if(s3.empty())
        {
            flags["missing or misplaced operand field"] = true;
        }
        else
            flags["illegal operand"] = true;
        if(!s1.empty())
            symTab.insert(make_pair(s1, make_pair(location_counter, "Relocatable")));

        pass2_info.insert(pair<int,int>(counter,-1));
        pass2_info.insert(pair<int,int>(counter,3)); // ay 7aga
        pass2_info.insert(pair<int,int>(counter,location_counter));
        pass2_info.insert(pair<int,int>(counter,program_counter));
        counter++;
    }

    else if (case_ins_compare(s2, "RESB"))
    {
        pass2_operand.insert(pair<int,string>(counter,s3));

        if ((is_number(s3)))
        {
            if (string_to_integer(s3) < 0)
                flags["illegal operand"] = true;
            else
            {
                location_counter = location_counter +increment_num(s3, 4);
            }

        }
        else if(s3.empty())
        {
            flags["missing or misplaced operand field"] = true;
        }
        else
            flags["illegal operand"] = true;
        if(!s1.empty())
            symTab.insert(make_pair(s1, make_pair(location_counter, "Relocatable")));

        pass2_info.insert(pair<int,int>(counter,-1));
        pass2_info.insert(pair<int,int>(counter,3)); // ay 7aga
        pass2_info.insert(pair<int,int>(counter,location_counter));
        pass2_info.insert(pair<int,int>(counter,program_counter));
        counter++;
    }
    else if(case_ins_compare(s2, "BASE"))
    {
        pass2_operand.insert(pair<int,string>(counter,s3));
        base = true;
        base_op = s3;

        pass2_info.insert(pair<int,int>(counter,-1));
        pass2_info.insert(pair<int,int>(counter,4)); // ay 7aga
        pass2_info.insert(pair<int,int>(counter,location_counter));
        pass2_info.insert(pair<int,int>(counter,program_counter));
        counter++;
    }

    else if(case_ins_compare(s2, "NOBASE"))
    {
        pass2_operand.insert(make_pair(counter,s3));
        base = false;

        pass2_info.insert(pair<int,int>(counter,-1));
        pass2_info.insert(pair<int,int>(counter,4)); // ay 7aga
        pass2_info.insert(pair<int,int>(counter,location_counter));
        pass2_info.insert(pair<int,int>(counter,program_counter));
        counter++;
    }

    //------------------------------------------------------------------------------------------------------------------
    else if (case_ins_compare(s2, "ORG"))
    {
        pass2_operand.insert(pair<int,string>(counter,s3));
        if (!s1.empty())
        {
            flags["this statement can not have an operand"] = true;
        }
        else if(!is_simple_expression(s3))
        {
            flags["illegal operand"] = true;
        }
        else
        {
            int temp = calculate_expression(s3);
            if (is_expression_error())
            {
                //location_counter = 0;
                flags["undefined symbol in operand"] = true;
            }
            else if(!is_relocatable())
            {
                flags["address expression is not relocatable"] = true;
            }
            else
            {
                location_counter = temp;
            }
        }
        pass2_info.insert(pair<int,int>(counter,-1));
        pass2_info.insert(pair<int,int>(counter,4)); // ay 7aga
        pass2_info.insert(pair<int,int>(counter,location_counter));
        pass2_info.insert(pair<int,int>(counter,program_counter));
        counter++;
    }
    else if (case_ins_compare(s2, "EQU"))
    {
        pass2_operand.insert(pair<int,string>(counter,s3));
        if(!is_simple_expression(s3))
        {
            flags["illegal operand"] = true;
            symTab.erase(s1);
        }
        else if(s1.empty())
        {
            flags["this statement requires a label"] = true;
        }

        else
        {
            int temp = calculate_expression(s3);

            if (is_expression_error())
            {
                flags["undefined symbol in operand"] = true;
            }
            else
            {
                if (is_relocatable())
                {
                    symTab[s1]=make_pair(temp, "Relocatable");
                }
                else
                {
                    symTab[s1]=make_pair(temp, "Absolute");

                }
            }
        }
        pass2_info.insert(pair<int,int>(counter,-1));
        pass2_info.insert(pair<int,int>(counter,4)); // ay 7aga
        pass2_info.insert(pair<int,int>(counter,location_counter));
        pass2_info.insert(pair<int,int>(counter,program_counter));
        counter++;
    }
    return true;
}

//-----------DIRECTIVES END----------//
int is_register(string regName) // return register code
{
    map<string, int>::iterator it1 = registers.find(regName);
    if (it1 == registers.end())
    {
        return -1;
    }
    return it1 -> second;
}

int add_to_symtab(string label)
//returns -1 if not found in SYMTAB
// else sets duplicate symbol flag & returns symbol's location
{
    unordered_map<string,pair<int, string> >::iterator it = symTab.find(label);
    if (it == symTab.end())
    {
        symTab.insert(make_pair(label,make_pair(location_counter,"Relocatable")));
        return -1;
    }
    map<string, bool>::iterator it2 = flags.find("duplicate label definition");
    string str = it2 -> first;
    if (it2 != flags.end())
        it2 -> second = true;
    return (it->second).first;
}

void check_memory(string operand, string mnemonic)
{

    regex e ("[a-zA-Z][a-zA-Z$0-9]*");
    regex e1("[@][a-zA-Z][a-zA-Z0-9$]*");//----------------------------------> updated
    // regex e1 ("[//@][a-zA-Z]+");
    regex e2 ("[/#][a-zA-Z0-9][a-zA-Z0-9$]*");//----------------------------------> updated
    regex e13 ("[/#][/-][0-9]+");
    regex e3 ("[a-zA-Z]+[/,][xX]");
    regex e4 ("[0-9]+");
    regex e5 ("[=][0-9]+");//----------------------------------> Mayar
    regex e6 ("[/*]");
    regex e7 ("[/=][/'][a-zA-Z0-9]+[/']");//----------------------------------> Mayar
    regex e8 ("[a-zA-Z]+[/+//-/*//][a-zA-Z0-9]+");//----------------------------------> updated
    regex e9 ("[a-zA-Z]+[/+//-/*//][0-9]+");//----------------------------------> updated
    regex e10 ("[0-9]+[/+//-/*//][0-9]+");//----------------------------------> updated
    regex e11 ("[-][0-9]+");
    regex e12 ("[0-9]+[a-zA-Z]+");//----------------------------------> updated

    bool match = regex_match(operand,e) || regex_match(operand,e1) || regex_match(operand,e2) || regex_match(operand,e3)
                 || regex_match(operand,e4) || regex_match(operand,e5) || regex_match(operand,e6)
                 || regex_match(operand,e7) || regex_match(operand,e8) || regex_match(operand,e9)
                 || regex_match(operand,e10) || regex_match(operand,e11)||regex_match(operand,e13);

    if(operand.size() == 0)
    {
        flags["missing or misplaced operand field"] = true;
    }
    else
    {
        if(case_ins_compare(mnemonic,"J"))
        {
            if(operand.at(0) == '*' && operand.size()>1)
            {
                flags["extra characters at end of statement "] = true;
            }
        }
        if(operand.at(0) == '=')
        {
            if(!regex_match(operand,e5) || !regex_match(operand,e7))
            {

                flags["wrong use of a literal (used as destination)"] = true;
            }
        }
        if(regex_match(operand,e12))
        {
            flags["extra characters at end of statement "] = true;
        }
        else if(!match)
        {
            flags["illegal operand"] = true;
        }

    }
}

void check_operand_logical (int operandType, string operand, string mnemonic, bool format4)
{
    switch (operandType)
    {
    case 0:
    {
        if (operand.length()!=0)
        {
            map<string, bool>::iterator it = flags.find("this statement can not have an operand");
            string str = it -> first;
            if (it != flags.end())
                it->second = true;
            cout << it -> second ;
        }
    }
    break;
    case 1:
    {
        pair<bool,vector<string>> result = is_literal(mnemonic,operand,flags,format4);
        if(result.first)
        {
            map<string,string>::iterator iter = found.find(operand);
            if (iter == found.end() && warning())
            {
                found.insert(make_pair(operand,"Instruction"));
                literal_table.push_back(result.second);
            }
        }
        else if(is_simple_expression(operand))

        {
           break;

        }
        else
        {
            //check expression
            check_memory(operand,mnemonic);
        }
    }
    break;
    case 2:
    {
        if (is_register(operand) == -1)
        {
            map<string, bool>::iterator it2 =
                flags.find("illegal address for a register");
            if (it2 != flags.end())
            {
                it2 -> second = true;
            }
        }
    }
    break;
    case 3:
    {
        //split operand
        //if result.length == 2
        //isRegister(register1)
        //isRegister(register2)
        if (operand.length() == 0)
        {
            map<string, bool>::iterator it2 =
                flags.find("illegal address for a register");
            string str = it2 -> first;
            if (it2 != flags.end())
                it2 -> second = true;
            return;
        }
        int flag = 0;
        for (int i=0; i<operand.length(); i++)
        {
            if (operand[i] == ',')
            {
                flag++;
                operand[i] = ' ';
            }
        }
        if(flag != 1)
        {
            map<string, bool>::iterator it2 =
                flags.find("illegal address for a register");
            string str = it2 -> first;
            if (it2 != flags.end())
                it2 -> second = true;
            return;
        }
        vector<string> tempArray;
        stringstream ss(operand);
        string temp;
        while (ss >> temp && temp != "")
        {
            tempArray.push_back(temp);
        }
        int length = tempArray.size();

        if (tempArray.size() == 1)
        {
            map<string,int>::iterator it = registers.find(tempArray[0]);
            if (it == registers.end())
            {
                map<string, bool>::iterator it2 =
                    flags.find("illegal address for a register");
                string str = it2 -> first;
                if (it2 != flags.end())
                    it2 -> second = true;
                return;
            }
            map<string, bool>::iterator it2 =
                flags.find("missing comma in operand field");
            string str = it2 -> first;
            if (it2 != flags.end())
                it2 -> second = true;
            return;
        }
        if (tempArray.size() == 2)
        {
            if (is_register(tempArray[0]) == -1
                    || is_register(tempArray[1]) == -1)
            {
                //cout << "check 2 registers" << endl;
                map<string, bool>::iterator it2 =
                    flags.find("illegal address for a register");
                string str = it2 -> first;
                if (it2 != flags.end())
                    it2 -> second = true;
                return;
            }
            // name of one register is wrong.
            // warning has been already set in isRegister fn.
//                return false;
//                isRegister(tempArray[0]);
//                isRegister(tempArray[1]);
            return;
        }
        if (tempArray.size() == 0)
        {
            map<string, bool>::iterator it2 =
                flags.find("missing comma in operand field");
            string str = it2 -> first;
            if (it2 != flags.end())
                it2 -> second = true;
            return;
        }
        if (tempArray.size() > 2)
        {
            map<string, bool>::iterator it2 =
                flags.find("extra characters at the end of the statement");
            string str = it2 -> first;
            if (it2 != flags.end())
                it2 -> second = true;
            return;
        }
    }
    break;
    case 4:
    {
        // shiftl, shiftr
        regex e51("[AaXxTtFfSsBbLl],[0-9]+");
        if(!regex_match(operand,e51))
        {
            flags["illegal operand"] = true;
        }
    }
    break;
    default:
        // or return true;
        return;
    }
}

int change_location_counter(int format, bool format4)
{
    if (format4)
    {
        if (format == 3)
        {
            return 4;
        }
        map<string, bool>::iterator it2 =
            flags.find("can not be format 4 instruction");
        string str = it2 -> first;
        if (it2 != flags.end())
            it2 -> second = true;
        return 0;
    }
    return format;
}

bool check_instruction(string label, string mnemonic, string operand)
{
    literal = false;
    int operandType;
    int formatType;
    int opCode;
    bool format4 = false;

    if (mnemonic.length()!=0 && mnemonic[0]=='+')
    {
        format4 = true;
        mnemonic.erase(0,1);
    }
    transform(mnemonic.begin(), mnemonic.end(),mnemonic.begin(), ::toupper);
    multimap<string,int>::iterator it = my_appendix.find(mnemonic);
    if (it == my_appendix.end())
    {
        return false;
    }

    pair <multimap<string,int>::iterator, multimap<string,int>::iterator> ret;
    ret = my_appendix.equal_range((*it).first);
    operandType = it -> second;
    it++;
    formatType = it -> second;
    it++;
    opCode = it -> second;
    pass2_info.insert(pair<int,int>(counter,opCode));
    if(format4)
    {
        pass2_info.insert(pair<int,int>(counter,formatType+1));
    }
    else
    {
        pass2_info.insert(pair<int,int>(counter,formatType));
    }

    check_operand_logical(operandType, operand, mnemonic,format4);
    int temp = change_location_counter(formatType,format4);

    if (temp == 0)
    {
        return false;
    }
    location_counter += temp;
    pass2_info.insert(pair<int,int>(counter,location_counter));
    pass2_info.insert(pair<int,int>(counter,program_counter));
    pass2_operand.insert(pair<int,string>(counter,operand));
    counter++;
    return true;
}

void remove_space(string &str)
{
    str.erase(remove( str.begin(), str.end(), ' ' ), str.end());
}

string get_mnemonic(string instruction)
{
    string mnemonic = instruction.substr(0,9);
    remove_space(mnemonic);
    return mnemonic;
}

int operand_kind(string operand)
{
    switch(operand.length())
    {
    case 0:
        return 0;   // RSUB
    case 1:
        return 1;   // MEMORY
    case 2:
        return 2;   // ONE REGISTER
    case 4:
        return 4;   // SHIFTL, SHIFTR
    case 5:
        return 3;   // TWO REGISTERS
    default:
        return -1;
    }
}

int get_operand(string instruction)
{
    string operand = instruction.substr(9,9);
    remove_space(operand);
    return operand_kind(operand);
}

int format_length(string format)
{
    return atoi(format.c_str());
}

int get_format(string instruction)
{
    string format = instruction.substr(18,6);
    remove_space(format);
    return format_length(format);
}

int get_opcode(string instruction)
{
    string opcode = instruction.substr(24,2);
    return hexa_to_int(opcode);
}
void load_instruction_appendix()
{
    string line;
    ifstream myfile("InstructionAppendix.txt");
    if (myfile.is_open())
    {
        int i = 0;
        while (! myfile.eof() )
        {
            getline (myfile,line);
            my_appendix.insert(make_pair(get_mnemonic(line),get_operand(line)));
            my_appendix.insert(make_pair(get_mnemonic(line),get_format(line)));
            my_appendix.insert(make_pair(get_mnemonic(line),get_opcode(line)));
            i++;
        }
        myfile.close();
    }
    else cout << "Unable to open file";
}

void load_registers_names()
{
    string line;
    string name, number;
    ifstream myfile("RegistersNames.txt");

    if (myfile.is_open())
    {
        int i = 0;
        while (! myfile.eof() )
        {
            getline (myfile,line);
            istringstream iss(line);
            while (iss)
            {
                string word;
                iss >> word;
                if (word != "")
                {
                    if (i%2 == 0)
                    {
                        name = word;
                    }
                    number = word;
                    i++;
                }
            }
            registers.insert(make_pair(name,atoi(number.c_str())));
        }
        myfile.close();
    }
    else cout << "Unable to open file";
}
bool check_label(string label)
{
    regex e1 ("[^aAbBxXfFlLtTsS]");
    regex e2 ("[a-zA-Z][a-zA-Z$$0-9]+");
    if(label.size() == 0)
    {
        return true;
    }
    else if(label.size() == 1)
    {
        if(!regex_match(label,e1))  //check label syntax
        {
            flags["unrecognized operation code"] = true;
            return false;
        }
    }
    else
    {
        if(!regex_match(label,e2))  //check label syntax
        {
            flags["unrecognized operation code"] = true;
            return false;
        }
    }
    return true;
}

bool check_opcode(string opcode)
{
    regex e2("[/+a-zA-Z]+");//----------------------------------> updated
    if(!regex_match(opcode,e2))  //check opcode syntax
    {
        flags["unrecognized operation code"] = true;
        return false;
    }
    return true;
}

bool check_operand(string operand)
{
    regex e ("[a-zA-Z][a-zA-Z$$0-9]*");
    regex e1 ("[/=][/'][a-zA-Z0-9]+[/']");
    regex e2 ("[//@][a-zA-Z]+");
    regex e3 ("[/#][a-zA-Z0-9]+");
    regex e4 ("[a-zA-Z]+[/,][a-zA-Z]");
    regex e5 ("[0-9]+");
    regex e6 ("[a-zA-Z]+[/-][a-zA-Z0-9]+");
    regex e7 ("[a-zA-Z]+[/+][0-9]+");
    regex e8 ("[0-9]+[/+][0-9]+");
    regex e9 ("[cC](.*)");
    regex e10("[xX](.*)");
    regex e11("[=][0-9]+");
    regex e12 ("[/*]");
    regex e13 ("[-][0-9]+");

    bool match = regex_match(operand,e)||regex_match(operand,e1)||regex_match(operand,e2)
                 ||regex_match(operand,e3)||regex_match(operand,e4)||regex_match(operand,e5)||regex_match(operand,e6)
                 ||regex_match(operand,e7)||regex_match(operand,e8)||regex_match(operand,e9)||regex_match(operand,e10)
                 ||regex_match(operand,e11)||regex_match(operand,e12)||regex_match(operand,e13);
    if(operand.size() != 0)
    {
        if(!match)  //check operand syntax
        {
            flags["unrecognized operation code"] = true;
            return false;
        }
    }
    return true;
}

bool is_comment ( string s)
{
    if( s.at(0)== '.')
    {
        return true;
    }
    return false;
}

string to_hexa(int number)
{
    stringstream sstream;
    sstream << hex << number;
    string result = sstream.str();
    int diff = 6 - result.size();
    int i ;
    string str = "" ;
    for(i=0; i<diff; i++)
    {
        str += "0";
    }
    result = str + result ;
    transform(result.begin(), result.end(),result.begin(), ::toupper);
    return result ;
}

vector <string>check_warnings()
{
    vector<string> warnings;
    for ( auto address_entry : flags )
    {
        if(address_entry.second == true)
        {
            warnings.push_back(address_entry.first);
            pass2 = false;
        }
    }
    return warnings;
}

void add_to_map (string s )
{

    if(!is_comment(s))
    {
        string line = to_hexa(program_counter);
        line = line + " " + s;
        listing_file.push_back(line);
        loc.push_back(location_counter);
    }
    else
    {
        listing_file.push_back(s);
    }

    vector<string> warnings = check_warnings();
    int i;

    for(i = 0; i<warnings.size(); i++)
    {
        listing_file.push_back(" ****** " + warnings.at(i));
        flags[warnings.at(i)] = false;
    }
}

void parse_fixed_format(string s)
{
    string label;
    string opcode;
    string operand;
    bool valid = true;
    bool format4 = false;
    program_counter= location_counter;
    regex e ("\\s*");

    if(!s.empty() && !regex_match(s,e))  //empty lines is accepted!
    {
        //  transform(s.begin(), s.end(),s.begin(), ::toupper);
        if(is_comment(s))  //check for comments
        {
            add_to_map(s);
            pass2_operand.insert(pair<int,string>(counter,""));
            pass2_info.insert(pair<int,int>(counter,-1));
            pass2_info.insert(pair<int,int>(counter,4)); // ay 7aga
            pass2_info.insert(pair<int,int>(counter,location_counter));
            pass2_info.insert(pair<int,int>(counter,program_counter));
            counter++;
            return;
        }
        if(s.size()>8)
        {
            if(s.at(8) != ' ')
            {
                if(s.at(8) == '+')
                {
                    format4 = true;
                }
                else
                {
                    flags["unrecognized operation code"] = true;
                    add_to_map(s);
                    return;
                }
            }
        }
        if((s.size() > 17) && (s.at(15) != ' ' || s.at(16) != ' '))
        {
            flags["unrecognized operation code"] = true;
            add_to_map(s);
            return;
        }

        if(s.size() > 65)  //length can't exceed 65
        {
            flags["extra length"] = true;
            valid = false;
        }
        if(s.size() > 0)
        {
            label = s.substr(0,8);
        }
        int i;

        for(i = 0; i < label.size(); i++) //count the length of the label
        {
            if(label.at(i) == ' ')
            {
                break;
            }
        }

        label = label.substr(0,i);

        if(valid)
        {
            valid = check_label(label);
            if(valid && label.size() != 0)
            {
                add_to_symtab(label);
            }
        }

        int j;
        for(j = label.size(); j<s.size()&& j<=8 ; j++)
        {
            if(format4 && j == 8)
            {
                break;
            }
            if(s.at(j) != ' ')
            {
                //set error
                flags["unrecognized operation code"] = true;
                valid = false;
            }
        }

        if(format4)
        {
            if(s.size() > 8)
            {
                opcode = s.substr(8,6);
            }
        }
        else
        {
            if(s.size() > 9)
            {
                opcode = s.substr(9,6);
            }

            if(opcode.at(0) == '+')
            {
                flags["unrecognized operation code"] = true;
                valid = false;
            }
        }

        for(j = 0; j < opcode.size(); j++) //count the length of the opcode
        {
            if(opcode.at(j) == ' ')
            {
                break;
            }
        }

        opcode = opcode.substr(0,j);

        if(valid)
        {
            valid = check_opcode(opcode);
        }
        if(opcode.size() >= 1)
        {
            for(j = (9+opcode.size()); j<s.size() && j<=14; j++)
            {
                if(s.at(j) != ' ')
                {
                    //set error
                    flags["unrecognized operation code"] = true;
                    valid = false;
                }
            }
        }
        else
        {
            valid = false;
        }
        if(s.size() > 17)
        {
            operand = s.substr(17,18);
        }
        int k;
        regex e("[cC][/'][a-zA-Z0-9\\s!@#$%^&*//-/+/*//]+[/']\\s*");//----------------------------------> updated
        regex e1("[xX][/'][a-fA-F0-9\\s]+[/']");
        bool flag2 = false;
        if(regex_match(operand, e) || regex_match(operand, e1))
        {
            for(k = 0; k < operand.size(); k++) //count the length of the label
            {
                flag2 = false;
                if(operand.at(k) == '\'')
                {
                    flag2 = true;
                }
                if(operand.at(k) == ' ' && k>1 && flag2)
                {
                    break;
                }
            }
        }
        else
        {
            for(k = 0; k < operand.size(); k++) //count the length of the label
            {
                if(operand.at(k) == ' ')
                {
                    break;
                }
            }
        }

        operand = operand.substr(0,k);

        /* if(valid)
         {
             valid = check_operand(operand);
         }*/
        for(j = (17+operand.size()); j<= 34 ; j++)
        {
            if(j>= s.size())
            {
                break;
            }
            if(s.at(j) != ' ' && valid)
            {
                //set error
                flags["extra characters at the end of the statement"] = true;
            }
        }
        if(valid)
        {
            //call directives
            //call instructions
            // xor between the two functions
            //cout<<   check_instruction(label, opcode, operand);

            bool flag = true;
            if(!check_directive(label, opcode, operand))
            {
                flag = check_instruction(label, opcode, operand);
            }

            if(!flag)
            {
                flags["unrecognized operation code"] = true;
            }
        }
        else
        {
            error = true;
        }
        add_to_map(s);

        if(literal)
        {
            for(int i = 0; i < literal_table.size(); i++)
            {
                if(literal_table[i].back() == "")
                {

                    pass2_operand.insert(pair<int,string>(counter,literal_table[i][0]));
                    pass2_info.insert(pair<int,int>(counter,-1));
                    pass2_info.insert(pair<int,int>(counter,5)); // ay 7aga
                    pass2_info.insert(pair<int,int>(counter,location_counter));
                    pass2_info.insert(pair<int,int>(counter,program_counter));
                    counter++;
                    string s1 = "*       " + literal_table[i][0];
                    program_counter = location_counter;
                    literal_table[i][3] = to_hexa(location_counter);
                    cout << "--------------------------------" << endl;
                    cout << location_counter << endl;
                    cout << "--------------------------------" << endl;
                    location_counter += string_to_integer(literal_table[i][2]);
                    add_to_map(s1);
                }
            }
        }
    }
}
void parse_free_format(string input)
{

    regex  e("\\s*(\\S+)\\s+(\\S+)\\s+(\\S+\\s*,\\s*\\S+\\s*)");
    regex e1("\\s*(\\S+)\\s+(\\S+\\s*,\\s*\\S+\\s*)");
    regex e2 ("\\s*(\\S+)\\s+(\\S+)\\s+([@=#]\\s*.*)");
    regex e3 ("\\s*(\\S+)\\s+([@=#]\\s*.*)");
    //  regex e4 ("\\s*(\\S+)\\s+(\\S+)\\s+([XC].*)");
    regex e4("\\s*(\\S+)\\s+(\\S+)\\s+([XC])\\s*([/'].*[/'])\\s*");
    regex e5 ("\\s*(\\S+)\\s+([XC])\\s*([/'].*[/'])\\s*");

    string label;
    string opcode;
    string operand;
    string operand2;
    bool valid = true;
    smatch result ;
    program_counter= location_counter;
    regex e66 ("\\s*");

    if(!input.empty() && !regex_match(input,e66))  //empty lines is accepted!
    {
        // transform(input.begin(), input.end(),input.begin(), ::toupper);

        bool valid1 = regex_match(input,e);
        bool valid2 = regex_match(input,e1);
        bool valid3 = regex_match(input,e2);
        bool valid4 = regex_match(input,e3);
        bool valid5 = regex_match(input,e4);
        bool valid6 = regex_match(input,e5);

        if(valid1|| valid3||valid5)
        {

            if(valid1) regex_search(input,result,e);
            if(valid3) regex_search(input,result,e2);
            if(valid5)
            {
                regex_search(input,result,e4);
                operand2 = result[4];

            }

            label= result[1];
            opcode= result[2];
            operand = result[3];
            string::iterator end_pos = remove(operand.begin(), operand.end(), ' ');
            operand.erase(end_pos, operand.end());
            operand= operand+operand2;
            input = label+" "+opcode+" "+operand;
            if(valid5)
            {
                input= input +operand2;
            }

            valid = check_label(label) && check_opcode(opcode);
        }
        else if (valid2||valid4||valid6)
        {


            if(valid2) regex_search(input,result,e1);
            if(valid4)regex_search(input,result,e3);
            if(valid6)
            {
                regex_search(input,result,e4);
                operand2 = result[3];

            }

            opcode = result[1];
            operand = result[2];
            string::iterator end_pos = remove(operand.begin(), operand.end(), ' ');
            operand.erase(end_pos, operand.end());
            operand = operand +operand2 ;
            input =  opcode +" "+operand;
            if(valid6)
            {
                input = input + operand2;
            }
            valid =  check_opcode(opcode);
        }


        else
        {
            if(input.at(0) == '.')  //check for comments
            {
                add_to_map(input);
                return;
            }

            vector<string> results;
            stringstream ss(input);
            string temp;

            while (ss >> temp && temp != "")
            {
                results.push_back(temp);
            }

            int size = results.size();

            if(size > 3)
            {
                label  = results.at(0);
                opcode = results.at(1);

                for(int y = 2; y<size; y++)
                {
                    operand += results.at(y);
                }

                valid = check_label(label) && check_opcode(opcode)/* && check_operand(operand)*/;
            }
            if(size == 3)
            {

                label = results.at(0);
                opcode = results.at(1);
                operand = results.at(2);

                valid = check_label(label) && check_opcode(opcode) /*&& check_operand(operand)*/;
            }
            if(size == 2)
            {
                string s = results.at(1);
                //format one
                if(case_ins_compare(s,"FIX")||case_ins_compare(s,"FLOAT")|| case_ins_compare(s,"HIO")||
                        case_ins_compare(s,"NORM")|| case_ins_compare(s,"SIO")||case_ins_compare(s,"TIO")|| case_ins_compare(s,"RSUB"))
                {
                    label = results.at(0);
                    opcode = results.at(1);
                    valid = check_label(label) && check_opcode(opcode);
                }
                else
                {
                    opcode = results.at(0);
                    operand = results.at(1);
                    valid = check_opcode(opcode) /*&& check_operand(operand)*/;
                }
            }
            if(size == 1)
            {
                opcode = results.at(0);
                valid = check_opcode(opcode);
            }
        }
        if(label.size() > 8 || opcode.size() > 6 || operand.size() > 18)
        {
            flags["unrecognized operation code"] = true;
            valid = false;
        }
        if(label.size() != 0 && check_label(label))
        {
            add_to_symtab(label);
        }
        if(valid)
        {
            //call directives
            //call instructions
            // xor between the two functions
            //cout<<   check_instruction(label, opcode, operand);
            bool flag = true;
            if(!check_directive(label, opcode, operand))
            {
                flag = check_instruction(label, opcode, operand);
            }

            if(!flag)
            {
                flags["unrecognized operation code"] = true;
            }
        }
        else
        {
            error = true;
        }
        string temp2 = label;

        for (int k = 0 ; k < 8- label.size(); k++)
        {
            temp2 += " ";
        }
        temp2 += opcode;
        for (int k = 0 ; k < 6- opcode.size(); k++)
        {
            temp2 += " ";
        }
        temp2 += operand ;
        add_to_map(temp2);

              if(literal)
        {
            for(int i = 0; i < literal_table.size(); i++)
            {
                if(literal_table[i].back() == "")
                {

                    pass2_operand.insert(pair<int,string>(counter,literal_table[i][0]));
                    pass2_info.insert(pair<int,int>(counter,-1));
                    pass2_info.insert(pair<int,int>(counter,5)); // ay 7aga
                    pass2_info.insert(pair<int,int>(counter,location_counter));
                    pass2_info.insert(pair<int,int>(counter,program_counter));
                    counter++;
                    string s1 = "*       " + literal_table[i][0];
                    program_counter = location_counter;
                    literal_table[i][3] = to_hexa(location_counter);
                    location_counter += string_to_integer(literal_table[i][2]);
                    add_to_map(s1);
                }
            }
        }
    }
}

int main(int argc, char *argv[])
{
    string source_file_name;
    vector<string> contents;

    set_flags();
    load_instruction_appendix();
    load_registers_names();
    load_directives();
    load_literals_information();

    switch(argc)
    {
    case 1:
        std::cout << "Insufficient Number of Arguments" << endl;
        break;
    case 2:
    {
        //FIXED FORMAT (DEFAULT)
        source_file_name = argv[1];
        contents =  read_from_file(source_file_name);
        for(int i = 0; i < contents.size(); i++)
        {
            parse_fixed_format(contents.at(i));
        }
        if(error)
        {
            print_listing_file(listing_file,symTab,true);
        }
        else
        {
            print_listing_file(listing_file,symTab,false);
        }
    }
    break;
    case 3:
    {
        source_file_name = argv[1];
        std::stringstream convert(argv[2]);
        int myint;
        if (!(convert >> myint))
        {
            std::cout << argv[2] << " is Invalid Argument" << endl;
            return 0;
        }
        else
        {
            if(myint == 1)
            {
                // FIXED FORMAT
                contents =  read_from_file(source_file_name);
                for(int i = 0; i < contents.size(); i++)
                {
                    parse_fixed_format(contents.at(i));
                }
                if(error)
                {
                    print_listing_file(listing_file,symTab,true);
                }
                else
                {
                    print_listing_file(listing_file,symTab,false);
                }
            }
            else if(myint == 2)
            {
                //  FREE FORMAT
                contents =  read_from_file(source_file_name);
                for(int i = 0; i < contents.size(); i++)
                {
                    parse_free_format(contents.at(i));
                }
                if(error)
                {
                    print_listing_file(listing_file,symTab,true);
                }
                else
                {
                    print_listing_file(listing_file,symTab,false);
                }
            }
            else
            {
                std::cout << argv[2] << " is Invalid Argument" << endl;
                return 0;
            }
        }
    }

    break;
    default:
        break;
    }


    if(pass2)
    {
        set_maps(pass2_info, pass2_operand);
        set_symbol_table(symTab);
        set_registers(registers);
        set_base(base,base_op);
        load_ascii_table();
        set_literals_info(found,literal_table);
        pair<vector<pair<string,string>>, vector<string>> code = generate_object_code();
        print_object_code(code.first,name,to_hexa(loc.at(loc.size()-1) - loc.at(0)),to_hexa(loc.at(0)),code.second,true);
    }

    if(error)
    {
        print_listing_file(listing_file,symTab,true);
    }
    else
    {
        print_listing_file(listing_file,symTab,false);
    }

    return 0;
}

/*
int main(int argc, char *argv[])
{
    set_flags(); //set the keys of the map of the flags
    load_instruction_appendix();
    load_registers_names();
    load_directives();

    //-----------------LITERAL---------------------//
    load_literals_information();
    // source_file_name = argv[1];
    //// vector<string> contents =  read_from_file(source_file_name);
    vector<string> contents =  read_from_file("pass1.txt");
    for(int i = 0; i < contents.size(); i++)
    {
        parse_fixed_format(contents.at(i));
    }
    if(pass2)
    {
        set_maps(pass2_info, pass2_operand);
        set_symbol_table(symTab);
        set_registers(registers);
        set_base(base,base_op);
        load_ascii_table();
        set_literals_info(found,literal_table);
        pair<vector<pair<string,string>>, vector<string>> code = generate_object_code();
        print_object_code(code.first,name,to_hexa(loc.at(loc.size()-1) - loc.at(0)),to_hexa(loc.at(0)),code.second,is_print());
    }
    if(error)
    {
        print_listing_file(listing_file,symTab,true);
    }
    else
    {
        print_listing_file(listing_file,symTab,false);
    }
    return 0;
}
*/
