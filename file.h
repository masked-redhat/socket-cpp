#include <fstream>
#include <iostream>
#include <map>
#include <string>

using namespace std;

typedef map<string, string> mss;

mss load_users()
{
    ifstream f("users.txt");

    mss users;

    string user_info;
    while (getline(f, user_info))
    {
        if (user_info.length() > 1)
        {
            int loc = user_info.find(':');
            string username = user_info.substr(0, loc);
            string password = user_info.substr(loc + 1);
            users[username] = password;
        }
    }

    return users;
}