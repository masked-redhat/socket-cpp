#include "../headers/common.h"
#include "../headers/ds.h"
#include "../headers/networking.h"
#include "../headers/concurrency.h"
#include "../headers/namespace.h"
#include <algorithm>

void handle_create_group(map<string, vector<SOCKET>> &groups, string &group_name, SOCKET &clientSocket, mutex &clientMutex)
{

    if (groups.find(group_name) == groups.end())
    {
        string message = "Group name not available";
        send(clientSocket, message.c_str(), message.size(), 0);
        return;
    }

    {
        lock_guard<mutex> lock(clientMutex);
        vector<SOCKET> members;
        members.push_back(clientSocket);
        groups[group_name] = members;
    }

    string message = "Group created";
    send(clientSocket, message.c_str(), message.size(), 0);
}

void handle_join_group(map<string, vector<SOCKET>> &groups, string &group_name, SOCKET &clientSocket, mutex &clientMutex)
{
    if (groups.find(group_name) == groups.end())
    {
        string message = "Group does not exist";
        send(clientSocket, message.c_str(), message.size(), 0);
        return;
    }

    vector<SOCKET> members;
    {
        lock_guard<mutex> lock(clientMutex);
        members = groups[group_name];

        for (auto x : members)
            if (x == clientSocket)
            {
                string message = "You are already in this group";
                send(clientSocket, message.c_str(), message.size(), 0);
                return;
            }

        members.push_back(clientSocket);
        groups[group_name] = members;
    }

    string message = "you have joined the group";
    send(clientSocket, message.c_str(), message.size(), 0);
}

void handle_leave_group(map<string, vector<SOCKET>> &groups, string &group_name, SOCKET &clientSocket, mutex &clientMutex)
{
    if (groups.find(group_name) == groups.end())
    {
        string message = "Group does not exist";
        send(clientSocket, message.c_str(), message.size(), 0);
        return;
    }

    vector<SOCKET> members;
    {
        lock_guard<mutex> lock(clientMutex);
        members = groups[group_name];

        auto it = find(members.begin(), members.end(), clientSocket);

        if (it == members.end())
        {
            string message = "You are not in the group";
            send(clientSocket, message.c_str(), message.size(), 0);
            return;
        }

        members.erase(it);
        groups[group_name] = members;
    }

    string message = "you have left the group";
    send(clientSocket, message.c_str(), message.size(), 0);
}