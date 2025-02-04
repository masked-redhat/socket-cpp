#include "../headers/common.h"
#include "../headers/namespace.h"

#ifndef UTILS_H
#define UTILS_H

// separates string by space and put the values in two different strings
void separate_string(const string &data, string &string1, string &string2)
{
    auto it = data.find(" ");
    if (it == string::npos)
        return;

    string1 = data.substr(0, it);
    string2 = data.substr(it + 1);
}

#endif