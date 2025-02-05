#include "../headers/common.h"
#include "../headers/types.h"
#include "../headers/namespace.h"

#ifndef LOAD_ENV
#define LOAD_ENV

mss load_env(string env_filename = ".env")
{
    ifstream f(env_filename); // load env file

    mss vars;

    string line;
    while (getline(f, line))
    {
        if (line.length() > 1)
        {
            int loc = line.find('=');
            string key = line.substr(0, loc);
            string value = line.substr(loc + 1);
            vars[key] = value;
        }
    }

    return vars;
}

#endif