#include <iostream>
#include <fstream>
#include <bits\stdc++.h>
#include "AddressInformation.h"
#include <string>
#include <sstream>
#include<vector>
#include<cstdlib>
#include <regex>
#include <map>
#include "ObjectCode.h"
#include <cmath>
#include "printing.h"
#include "Conversions.h"

using namespace std;

unordered_map<string, pair<int, string> > symTab1;
multimap <int, int> pass2_info1;
multimap <int,string> pass2_operand1;
map<string, int> registers1;
string operand;
bool using_base = false;
string base_operand;
map <string, string> ascii_map;
vector<pair<string,string>> object_code;
vector <string> modification;
bool undefined_symbol = false;
bool disp_fit = true;
bool illegal_address_expression = false;
bool print = true;
string nixbpe;
vector <string>  nixbpe_vect ;
vector<pair<bool,bool>> errors;
map<string, string> literals;
vector<vector<string>> literals_info;
bool relocatable = true;
bool exp_error=false;
int loc_counter;
void set_maps(multimap <int, int> m1, multimap <int,string> m2)
{
    pass2_info1 = m1;
    pass2_operand1 = m2;
}

void set_symbol_table(unordered_map<string, pair<int, string> > t1)
{
    symTab1 = t1;
}

void set_registers(map<string, int> r)
{
    registers1 = r;
}

void set_base(bool b, string op)
{
    using_base = b;
    base_operand = op;

}

string binary_to_hex(string bin)
{

    int result =0 ;

    for(size_t count = 0; count < bin.length() ; ++count)
    {
        result *=2;
        result += bin[count]=='1'? 1 :0;
    }

    stringstream ss;
    ss << hex << setw(8) << setfill('0')  << result;

    string hexVal(ss.str());
    return hexVal;
}

string char_to_string(char c)
{
    string r;
    stringstream ss;
    ss << c;
    ss >> r;
    return r;
}

string format_2(int opcode)
{
    char char1 = operand.at(0);
    int reg1 = registers1.find(char_to_string(char1))-> second;
    string temp1 = integer_to_string(reg1);

    string temp2 = "0";
    if(operand.length() > 2)
    {
        char char2 = operand.at(2);
        int reg2 = registers1.find(char_to_string(char2))-> second;
        temp2 = integer_to_string(reg2);
    }
    string hexa = decimal_to_hexadecimal(opcode);

    hexa = hexa + temp1 + temp2;
    return hexa;

}

string refactor_disp(int len,string disp)
{
    int diff = len - disp.size();
    int i ;
    string str = "" ;
    for(i=0; i<diff; i++)
    {
        str += "0";
    }
    disp = str + disp;
    return disp;
}

string calc_nix()
{
    string result = "";
    if(is_literal_info(operand))
    {
        result = "110";
        return result;
    }
    if(is_indirect(operand))
    {
        // 10
        result = "10";
        operand.erase(0,1);
    }
    else if(is_immediate(operand))
    {
        // 01
        result = "01";
        operand.erase(0,1);
    }
    else
    {
        // 11
        result = "11";
    }

    if(is_indexing(operand))
    {
        result = result + "1";
        operand.erase(operand.length()-2,2);
    }
    else
    {
        result = result + "0";
    }
    nixbpe = result;
    return result;
}

int iterate_symtab(string operand)
{
    int target;
    unordered_map<string, pair<int, string> > ::iterator it = symTab1.find(operand);
    if (it == symTab1.end())
    {
        target = -1;
    }
    else
    {
        target = it -> second.first;
    }

    return target;
}

pair <string, string> calc_bpe(int program_count, bool format4, int location_counter2)
{
    cout << operand <<endl;
    int target_address,disp;
    string bpe = "";
    string displacement = "";
    regex e ("[0-9]+");
    regex e1 ("[-][0-9]+");
    if(format4)
    {
        bpe = "001";
        if(regex_match(operand,e) || regex_match(operand,e1))
        {
            displacement = decimal_to_binary(atoi(operand.c_str()));

            if(displacement.length() > 20)
            {
                disp_fit = false;
                return make_pair("","");
            }
            else
            {
                displacement = refactor_disp(20, displacement);
            }

        }
        else
        {
            if(is_literal_info(operand))
            {
                target_address = hexa_to_int(literal_address());
            }
            else if (is_simple_expression(operand))
            {
                int val = calculate_expression(operand);
                if(exp_error)
                {
                    undefined_symbol = true;
                }
                else
                {
                    target_address = calculate_expression(operand);
                }
            }
            else
            {
                target_address = iterate_symtab(operand);
                if(target_address == -1)
                {
                    undefined_symbol = true;
                    return make_pair("","");
                }
            }
            disp = target_address;
            displacement = decimal_to_binary(disp);
            if(displacement.length() > 20)
            {
                disp_fit = false;
                return make_pair("","");
            }
            else
            {
                displacement = refactor_disp(20, displacement);
            }
        }
    }
    else
    {
        if(regex_match(operand,e) || regex_match(operand,e1))
        {
            bpe = "000";
            //  disp = operand;
            displacement = decimal_to_binary(atoi(operand.c_str()));
            if(displacement.length() > 12)
            {
                disp_fit = false;
                return make_pair("","");
            }
            else
            {
                displacement = refactor_disp(12, displacement);
            }
        }

        else
        {
            if(is_literal_info(operand))
            {
                target_address = hexa_to_int(literal_address());
            }
            else if (operand == "*")
            {
                target_address = location_counter2;
            }
            else if (is_simple_expression(operand))
            {
                undefined_symbol = true;
            }
            else
            {
                target_address = iterate_symtab(operand);
                if(target_address == -1)
                {
                    undefined_symbol = true;
                    return make_pair("","");
                }
            }
            disp = target_address - program_count;
            displacement = decimal_to_binary(disp);
            bpe = "010";
            if(disp > 2047 || disp < -2048 )
            {
                bpe = "100";
                int base = iterate_symtab(base_operand);
                if(base == -1)
                {
                    undefined_symbol = true;
                    return make_pair("","");
                }
                disp = target_address - base;
                if(disp > 4095)
                {
                    disp_fit = false;
                    return make_pair("","");
                }
                displacement = decimal_to_binary(disp);
                displacement = refactor_disp(12, displacement);
            }
            else
            {

                if(displacement.length() < 12)
                {
                    displacement = refactor_disp(12, displacement);
                }
                else if (displacement.length() > 12)
                {
                    displacement.erase(0,displacement.length()-12);
                }

            }

        }
    }
    nixbpe += bpe;

    nixbpe_vect.push_back(nixbpe);
    return make_pair(bpe, displacement);
}

string format_3_4(int op, int program, bool is_f4, int loc)
{
    if(op == 76)
    {
        cout << "4F0000"<<endl;
        return "4F0000";
    }
    string str = decimal_to_binary(op);
    str = refactor_disp(8,str);
    str.erase(6,2);
    nixbpe= calc_nix();
    str = str + nixbpe;
    pair<string, string> result;
    result = calc_bpe(program,is_f4,loc);
    if(result.first == "" && result.second == "")
    {
        undefined_symbol = true;
        return "";
    }
    str = str + result.first + result.second;
    str = binary_to_hex(str);
    if(str.length()>6&& !is_f4)
    {
        str.erase(0,str.length()-6);
    }
    else if (str.length()>8 && is_f4)
    {
        str.erase(0,str.length()-8);
    }
    transform(str.begin(), str.end(),str.begin(), ::toupper);
    return str;
}

string decimal_to_binary(unsigned int dec)
{

    char binary[33] = {0};

    int ix = 32; // subscript of current character
    do
    {
        binary[--ix] = '0' + dec % 2;
        dec /= 2;
    }
    while (dec);

    return  (binary+ix);    // ix is offset into char array where answer begins

}

string word_obcode()
{
    regex e ("[0-9]+");
    string obcode;
    if(regex_match(operand,e))
    {
        obcode = decimal_to_hexadecimal(atoi(operand.c_str()));
        obcode = refactor_disp(6,obcode);
    }
    else
    {
        unordered_map<string, pair<int, string> > ::iterator it = symTab1.find(operand);
        if (it == symTab1.end())
        {
            //generate error
            undefined_symbol = true;
            return "";
        }
        else
        {
            obcode = "000000";
        }
    }
    transform(obcode.begin(), obcode.end(),obcode.begin(), ::toupper);
    return obcode;
}

void load_ascii_table()
{
    string line;
    string ascii, character;
    ifstream myfile("ascii.txt");

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
                        ascii = word;
                    }
                    character = word;
                    i++;
                }
            }
            ascii_map.insert(make_pair(character,ascii));
        }
        myfile.close();
    }
    else cout << "Unable to open file";
}

string byte_obcode()
{
    regex e1("[xX][/'][a-fA-F0-9]+[/']");
    regex e2("[cC][/'][a-zA-Z0-9\\s!@#$%^&*()-*~/':;]+[/']");//----------------------------------> updated
    string obcode = "";
    if(regex_match(operand,e1))
    {
        operand.erase(0,2);
        operand.erase(operand.length()-1,1);
        obcode = operand;
    }
    else if(regex_match(operand,e2))
    {
        operand.erase(0,2);
        operand.erase(operand.length()-1,1);
        for(int i = 0; i < operand.length(); i++)
        {
            obcode+= ascii_map.find(char_to_string(operand.at(i)))-> second;
        }
    }

    transform(obcode.begin(), obcode.end(),obcode.begin(), ::toupper);
    return obcode;
}

void set_literals_info(map<string, string> l1, vector<vector<string>> v1)
{
    literals = l1;
    literals_info = v1;

}

string literal_obcode()
{
    string obcode;
    regex e1("[=][xX]['][a-fA-F0-9]{1,14}[']");
    regex e2("[=][cC][a-zA-Z0-9\\s!@#$%^&*()-*~/':;]{1,14}[']");
    regex e3("[=][wW]['][/-]?[0-9]{1,14}[']");

    if(regex_match(operand,e1))
    {
        operand.erase(0,3);
        operand.erase(operand.length()-1,1);
        obcode = operand;
    }
    else if(regex_match(operand,e2))
    {
        operand.erase(0,3);
        operand.erase(operand.length()-1,1);
        for(int i = 0; i < operand.length(); i++)
        {
            obcode+= ascii_map.find(char_to_string(operand.at(i)))-> second;
        }

    }

    else if(regex_match(operand,e3))
    {
        obcode = decimal_to_hexadecimal(atoi(operand.c_str()));
        obcode = refactor_disp(6,obcode);
    }

    return obcode;

}


pair<vector<pair<string,string>>, vector<string>> generate_object_code()
{
    int opcode;
    int format_num;
    int program_counter;
    int k = 0;

    regex e ("[0-9]+");
    for(multimap<int, int>::iterator it = pass2_info1.begin(); it != pass2_info1.end(); )
    {
        k = it -> first;
        pair<multimap<int, int>::iterator, multimap<int, int>::iterator> ret;
        ret = pass2_info1.equal_range((*it).first);
        multimap<int,string>:: iterator it2 = pass2_operand1.find(k);
        operand = it2 -> second;
        opcode = it -> second;
        it++;
        format_num = it -> second;
        it++;
        program_counter = it -> second;
        it++;
        loc_counter = it -> second;
        if(opcode == -1)
        {
            //directive
            nixbpe_vect.push_back(" ");
            switch(format_num)
            {
            case 1:
            {
                if(is_literal_info(operand))
                {

                    object_code.push_back(make_pair(refactor_disp(6, decimal_to_hexadecimal(loc_counter)),literal_obcode_info()));
                }
                else if(!undefined_symbol)
                {
                    object_code.push_back(make_pair(refactor_disp(6, decimal_to_hexadecimal(loc_counter)),word_obcode()));
                }
            }
            break;
            case 2:
            {
                if(is_literal_info(operand))
                {

                    object_code.push_back(make_pair(refactor_disp(6, decimal_to_hexadecimal(loc_counter)),literal_obcode_info()));
                }
                else
                {
                    object_code.push_back(make_pair(refactor_disp(6, decimal_to_hexadecimal(loc_counter)),byte_obcode()));
                }
            }
            break;
            case 3:
            {
                object_code.push_back(make_pair(refactor_disp(6, decimal_to_hexadecimal(loc_counter))," "));
            }
            break;
            case 4:
            {
                object_code.push_back(make_pair(refactor_disp(6, decimal_to_hexadecimal(loc_counter)),""));
            }
            break;
            case 5:
            {
                object_code.push_back(make_pair(refactor_disp(6, decimal_to_hexadecimal(loc_counter)),literal_obcode()));
            }
            break;
            default:
            {
                vector<pair<string,string>> v;
                v.push_back(make_pair("",""));
                vector<string> v2;
                v2.push_back("");
                pair<vector<pair<string,string>>, vector<string>>   pair_vect ;
                pair_vect = make_pair(v,v2);
                return pair_vect;
            }
            }
        }
        else
        {
            switch(format_num)
            {
            case 2:
            {
                nixbpe_vect.push_back(" ");
                object_code.push_back(make_pair(refactor_disp(6, decimal_to_hexadecimal(loc_counter)),format_2(opcode)));
            }
            break;

            case 3:
            {
                string s1 = format_3_4(opcode,program_counter,false, loc_counter);
                if(!undefined_symbol && disp_fit)
                {
                    object_code.push_back(make_pair(refactor_disp(6, decimal_to_hexadecimal(loc_counter)),s1));
                }
            }
            break;
            case 4:
            {
                string s2 = format_3_4(opcode,program_counter,true, loc_counter);
                if(!undefined_symbol && disp_fit)
                {
                    object_code.push_back(make_pair(refactor_disp(6, decimal_to_hexadecimal(loc_counter)),s2));
                }
                if (!regex_match(operand,e))
                {
                    modification.push_back(refactor_disp(6, decimal_to_hexadecimal(loc_counter+1)));
                }
                else
                {
                    calculate_expression(operand);
                    if(relocatable)
                    {
                        modification.push_back(refactor_disp(6, decimal_to_hexadecimal(loc_counter+1)));
                    }
                }
            }
            break;
            default:
            {
                vector<pair<string,string>> v;
                v.push_back(make_pair("",""));
                vector<string> v2;
                v2.push_back("");
                pair<vector<pair<string,string>>, vector<string>>   pair_vect ;
                pair_vect = make_pair(v,v2);
                return pair_vect;
            }
            }
        }
        if(undefined_symbol|| !disp_fit)
        {
            print = false;
        }
        errors.push_back(make_pair(undefined_symbol,disp_fit));
        undefined_symbol = false;
        disp_fit = true;
        it = pass2_info1.upper_bound(it->first);
    }
    pair<vector<pair<string,string>>, vector<string>>   pair_vect ;
    pair_vect = make_pair(object_code,modification);
    set_pass2_parameters(nixbpe_vect,errors);
    return pair_vect;
}

string literal_address()
{
    string address;
    for(int i = 0; i < literals_info.size(); i++)
    {
        if(literals_info[i][0] == operand)
        {
            address = literals_info[i][3];
            break;
        }
    }
    return address;
}

string literal_obcode_info()
{
    string address;
    for(int i = 0; i < literals_info.size(); i++)
    {
        if(literals_info[i][0] == operand)
        {
            address = literals_info[i][2];
            break;
        }
    }
    return address;
}

bool is_print ()
{
    return print;
}

int load_address(string label)
{
    auto search = symTab1.find(label);
    if(search != symTab1.end())
    {
        pair<int,string> temp=search->second  ;
        return temp.first;
    }
    return -1;

}

string load_type(string label)
{
    auto search = symTab1.find(label);

    pair<int,string> temp=search->second  ;
    return temp.second;


}

vector<string> split(const std::string &s, char delim)
{
    std::vector<string> elems;
    std::stringstream ss(s);
    std::string number;
    while(std::getline(ss, number, delim))
    {
        elems.push_back((number));
    }
    return elems;
}

bool is_absolute(string str)
{
    if(is_number(str))
    {
        return true;
    }
    if(load_address(str)!= -1)
    {
        if(case_ins_compare(load_type(str),"Absolute"))
        {
            return true;
        }

    }
    return false;
}

int abs_val(string str)
{
    if(is_number(str))
    {
        return string_to_integer(str);
    }
    if(load_address(str)!= -1)
    {
        if(case_ins_compare(load_type(str),"Absolute"))
        {
            return load_address(str);
        }

    }
    return -1;
}

bool is_relative(string str)
{
    if(is_number(str))
    {
        return false;
    }
    if(load_address(str)==-1 )
    {
        return false;
    }
    if(!case_ins_compare(load_type(str),"Relocatable"))
        return false;
    return true;
}

int calculate_expression(string e)
{
    exp_error=false;
    relocatable=true;
    illegal_address_expression = false;
    int val=-1;
    vector<string> elems;
    string op1;
    string op2;
    regex r1("[#|@]?[a-zA-Z0-9]+([+][a-zA-Z0-9]+)");
    regex r2("[#|@]?[a-zA-Z0-9]+([-][a-zA-Z0-9]+)");
    regex r3("[#|@]?[a-zA-Z0-9]+([*][a-zA-Z0-9]+)");
    regex r4("[#|@]?[a-zA-Z0-9]+([/][a-zA-Z0-9]+)");
    regex r5("[#|@]?[a-zA-Z0-9]+");
    regex r6("[*]");
    bool match1= regex_match(e,r1);
    bool match2= regex_match(e,r2);
    bool match3= regex_match(e,r3);
    bool match4= regex_match(e,r4);
    bool match5= regex_match(e,r5);
    bool match6= regex_match(e,r6);

    if(e.at(0)=='#')
    {
        e.erase(e.begin());
    }
    if(e.at(0)=='@')
    {
        e.erase(e.begin());
    }

    if(match1)
    {
        elems=split(e,'+');
        op1=elems.at(0);
        op2=elems.at(1);
        if(is_absolute(op1) && is_absolute(op2))
        {
            val=abs_val(op1)+abs_val(op2);
            relocatable=false;
        }
        else if (is_relative(op1) && is_absolute(op2))
        {
            val=load_address(op1) + abs_val(op2);
            relocatable=true;
        }
        else if (is_absolute(op1) && is_relative(op2))
        {

            val=abs_val(op1) + load_address(op2);
            relocatable=true;

        }
        else
        {
            exp_error=true;
            illegal_address_expression = true;
        }

    }
    else if(match2)
    {
        cout << "rrrrr";
        elems=split(e,'-');
        op1=elems.at(0);
        op2=elems.at(1);
        if(is_relative(op1) && is_relative(op2))
        {

            val=load_address(op1)- load_address(op2);
            relocatable=false;

        }
        else if (is_relative(op1) && is_absolute(op2))////reloc
        {
            val=load_address(op1) - abs_val(op2);
            if(val<0)
                exp_error=true;

        }
        else if(is_absolute(op1) && is_absolute(op2))
        {
            val=abs_val(op1)-abs_val(op2);
            relocatable=false;
        }
        else
        {

            exp_error=true;
            illegal_address_expression = true;
        }
    }
    else if (match3)
    {
        elems=split(e,'*');
        op1=elems.at(0);
        op2=elems.at(1);
        if(is_absolute(op1) && is_absolute(op2))
        {
            val=abs_val(op1)*abs_val(op2);
            relocatable=false;
        }
        else
        {
            exp_error=true;
            illegal_address_expression = true;
        }
    }
    else if(match4)
    {

        elems=split(e,'/');
        op1=elems.at(0);
        op2=elems.at(1);
        if(is_absolute(op1) && is_absolute(op2))
        {
            if(abs_val(op1)==0 && abs_val(op2)==0)
            {
                exp_error=true;
                //flags["Invalid, the two operands are both zeros"] = true;

            }
            else if(abs_val(op2)==0)
            {
                exp_error=true;
                //flags["Illegal division by zero"] = true;
            }
            else
            {
                val=abs_val(op1)/abs_val(op2);
                relocatable=false;
            }



        }
        else
        {
            exp_error=true;
            illegal_address_expression = true;
        }
    }
    else if(match5)
    {

        if(is_absolute(e))
        {
            val=abs_val(e);
            relocatable=false;
        }
        else if(is_relative(e))
        {
            val=load_address(e);
            relocatable=true;
        }
        else
        {
            exp_error=true;
        }
    }
    else if(match6)
    {
        val=loc_counter;
    }
    else
        exp_error=true;
    return val;
}

bool is_relocatable()
{
    return relocatable;
}

bool is_expression_error()
{
    return exp_error;
}

