#include "../headers/common.h"
#include "../headers/types.h"
#include "../constants/env.h"
#include "../headers/namespace.h"

#ifndef LOADFILE
#define LOADFILE

mss load_users()
{
    string file_loc = get_env("FILE_LOC", "constants/users.txt");
    ifstream f(file_loc); // load file

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

#endif