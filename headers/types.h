#include "./common.h"
#include "./concurrency.h"
#include "./networking.h"
#include "./namespace.h"

#ifndef TYPES
#define TYPES

// typedefs
typedef unordered_set<SOCKET> sS;
typedef unordered_set<string> ss;
typedef unordered_map<string, SOCKET> msS;
typedef unordered_map<string, string> mss;
typedef unordered_map<string, unordered_set<SOCKET>> mssS;
typedef lock_guard<mutex> lgm; // lock guard
typedef pair<string, int> psi;

#endif