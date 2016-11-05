#include "AddressInformation.h"
#include <string>
#include <regex>

using namespace std;
bool is_indirect(string operand)
{
    regex e ("[//@][a-zA-Z][a-zA-Z$$0-9]+");
    regex e1 ("[//@][0-9]+");
    if(regex_match(operand,e) || regex_match(operand,e1))
    {
        return true;
    }
    return false;

}

bool is_immediate(string operand)
{
    regex e ("[/#][0-9]+");
    regex e1 ("[/#][a-zA-Z][a-zA-Z$$0-9]+");
    if(regex_match(operand,e)||regex_match(operand,e1))
    {
        return true;
    }
    return false;
}

bool is_indexing(string operand)
{
    regex e ("[a-zA-Z][a-zA-Z$$0-9]+[/,][xX]");
    if(regex_match(operand,e))
    {
        return true;
    }
    return false;
}

bool is_literal_info(string operand)
{
    regex e1("[=][xX]['][a-fA-F0-9]{1,14}[']");
    regex e2("[=][cC]['][a-zA-Z0-9]{1,14}[']");
    regex e3("[=][wW]['][/-]?[0-9]{1,14}[']");

    if(regex_match(operand,e1) || regex_match(operand,e2) || regex_match(operand,e3) )
    {
        return true;
    }else{
        return false;
    }
}

bool is_simple_expression(string operand)
{
    regex r1("[#|@]?[a-zA-Z0-9]+([+|-|/|*][a-zA-Z0-9]+)*");
    regex r2("[*]");
    regex r3("(#@)[a-zA-Z0-9]+([+|-|/|*][a-zA-Z0-9]+)*");
    bool match1= regex_match(operand,r1);
    bool match2= regex_match(operand,r2);
    bool match3= regex_match(operand,r3);
    if(match3)
        return false;
    return (match1 || match2);
}
