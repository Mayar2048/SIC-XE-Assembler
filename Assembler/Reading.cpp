#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <string>
#include"Reading.h"

using namespace std;
 vector<string> read_from_file(string file_name)
{
    ifstream file(file_name);
    string str;
    string file_contents;
    vector<string> contents;
    while (getline(file, str))
    {
        cout << str << endl;
        contents.push_back(str);

    }

   // cout << contents.size();
    return contents;

}



