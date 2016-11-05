#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string>
#include"Printing.h"
#include <iostream>
#include <fstream>
#include <bits\stdc++.h>
#include <iomanip>
using namespace std;

vector<string>nixbpe_vector;
vector<pair<bool,bool>> errors_flags;
vector<pair<string,string>> code;

void set_pass2_parameters(vector<string>nixbpe,vector<pair<bool,bool>> error)
{
    nixbpe_vector = nixbpe;
    errors_flags = error;
}
void print_listing_file(vector<string> list_file,unordered_map<string, pair<int, string> > table, bool flag)
{
    bool print ;
    int u ;
    ofstream myfile ("list.txt");
    if (myfile.is_open())
    {
        myfile<< "  Source Program statements with value of LC indicated" << endl;
        myfile<<endl;
        int i;
        for(i = 0; i< list_file.size(); i++)
        {
            myfile << list_file.at(i)<<endl ;
        }
        myfile<< endl;
        if(!flag)
        {
            myfile << ">>    e n d    o f   p a s s   1" <<endl ;
            myfile<< endl;
            myfile<<">>   *****************************************************"<<endl;
            myfile<<">>    s y m b o l     t a b l e   (values in decimal)"<<endl;
            myfile<<"        "<<left<< setw(11)<<"name"<<right<<setw(12)<<" value"<<setw(16)<<"Absol/Reloc"<<endl;
            myfile<<"        ---------------------------------------"<<endl;

            for (auto address_entry : table )
            {

                myfile<<"        "<<left<< setw(11)<< address_entry.first;

                myfile<< right<< setw(12) << address_entry.second.first << setw(16) << address_entry.second.second << endl;

            }
            myfile<<">>   *****************************************************"<<endl;

            myfile << ">>   S t a r t   o f    P a s s   I I " <<endl;

            myfile << ">>   A s s e m b l e d    p r o g r a m     l i s t i n g"<<endl;

            myfile << " LC           Source Statement                                        code"<<endl;
            for( u = 0 ; u<nixbpe_vector.size(); u++)
            {
                cout << nixbpe_vector.at(u) << "yaraaaaaaaaaaaaaaaaab"<<endl;
                if(nixbpe_vector.at(u) != " ")
                {

                    myfile << "                             " << "n=" << nixbpe_vector.at(u).at(0) << " " << "i=" << nixbpe_vector.at(u).at(1) << " "
                           << "x=" << nixbpe_vector.at(u).at(2) << "  " << "b=" << nixbpe_vector.at(u).at(3) << " " << "p=" << nixbpe_vector.at(u).at(4) << " "
                           "p=" << nixbpe_vector.at(u).at(5) << endl;

                }

                myfile <<left<<setw(70)<< list_file.at(u) <<left << setw(30)<< code.at(u).second<<endl;

                if(errors_flags.at(u).first == true)
                {
                    print = false;
                    myfile << " ****** undefined symbol in operand" << endl;
                }

                if(errors_flags.at(u).second == false)
                {
                    print = false;
                    myfile << " ****** displacement does not fit the 12 bits" << endl;
                }


            }
//            string c ;
//              for(int o = u ; o<list_file.size()-1; o++)
//            {
//                if(o<code.size())
//                {
//                    c = code.at(o).second;
//                }
//
//
//                  myfile <<left<<setw(70)<< list_file.at(o) <<left << setw(30)<< c <<endl;
//                    if(errors_flags.at(o).first == true)
//                {
//                    print = false;
//                    myfile << " ****** undefined symbol in operand" << endl;
//                }
//
//                if(errors_flags.at(o).second == false)
//                {
//                    print = false;
//                    myfile << " ****** displacement does not fit the 12 bits" << endl;
//                }
//
//
//            }
            if(!print)
            {
                myfile << ">>    s u c c e s s f u l    a s s e m b l y"<<endl;
            }
            else
            {
                myfile << ">>    i n c o m p l e t e    a s s e m b l y" << endl;
            }

        }
        else
        {

            myfile <<">>    i n c o m p l e t e    a s s e m b l y"<<endl;
        }

    }
    myfile.close();

}

string refactor(int len,string disp, bool b)
{
    int diff = len - disp.size();
    int i ;
    string str = "" ;
    for(i=0; i<diff; i++)
    {
        if(b)
        {
            str += " ";
        }
        else
        {
            str += "0";
        }
    }

    if(b)
    {
        disp = disp + str;
    }
    else
    {
        disp = str + disp;
    }
    return disp;
}


string decimal_to_hexa(int number)
{
    stringstream sstream;
    sstream << hex << number;
    string result = sstream.str();
    transform(result.begin(), result.end(),result.begin(), ::toupper);
    return result;

}

void print_object_code(vector<pair<string,string>> objcode, string name, string length,string starting_add, vector<string> modi, bool print)
{
    ofstream myfile ("obcode.txt");
    string s, len;
    code = objcode;
    if (myfile.is_open())
    {
        myfile << "H^" << refactor(6,name,true) << "^" << starting_add << "^" << length << endl;
        if(!print)
        {
            return;
        }

        int j =0;
        int i = 0;
        for(i = 0 ; i  < objcode.size() ; i+=j)
        {

            s = "";
            for(j = 0; j < 10 ; j++)
            {

                if(objcode.size() <= i+j)
                {
                    break;

                }
                if(objcode.at(i+j).second == " ")
                {
                    cout << "radwa" << endl;
                    //myfile<< "   " <<endl;
                    j++;
                    break;

                }
                else if (objcode.at(i+j).second.length() != 0)
                {
                    s = s+ objcode.at(i+j).second;
                    len = refactor(2,decimal_to_hexa(s.size() / 2),false);
                }

            }
            if(!len.empty())
            {
                string  str = objcode.at(i).first ;
                if(i == 0)
                {
                    if(str == "FFFFFFFF")
                    {
                        str = objcode.at(i+1).first ;
                    }
                }
                myfile << "T^"<<str<<"^" << len<< "^"  << s <<endl;
                len = "";
            }

        }
        for(int k = 0  ; k < modi.size(); k++)
        {
            myfile << "M^"<<modi.at(k)<<"^05"<<endl;
        }

        myfile << "E^" << starting_add << endl;

    }
    myfile.close();

}
