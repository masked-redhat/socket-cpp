#include "../headers/types.h"
#include "../utils/env.h"

#ifndef ENV_H
#define ENV_H

mss envs = load_env();

string get_env(string key, string default_value = "")
{
    if (envs.find(key) != envs.end())
        return envs[key];
    else
        return default_value;
}

#endif