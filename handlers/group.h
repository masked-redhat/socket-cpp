#include "../headers/common.h"
#include "../headers/ds.h"
#include "../headers/networking.h"
#include "../headers/concurrency.h"
#include "../headers/namespace.h"
#include "../headers/utils.h"

void handle_create_group(map<string, vector<SOCKET>> &groups, string &group_name, SOCKET &clientSocket, mutex &clientMutex)
{
    if (groups.find(group_name) == groups.end() || groups.size() == 0)
    {
        {
            lock_guard<mutex> lock(clientMutex);
            vector<SOCKET> members;
            members.push_back(clientSocket);
            groups[group_name] = members;
        }

        string message = "Group created";
        _send(message, clientSocket);
    }
    else
    {
        string message = "Group name not available";
        _send(message, clientSocket);
    }
}

void handle_join_group(map<string, vector<SOCKET>> &groups, string &group_name, SOCKET &clientSocket, mutex &clientMutex)
{

    if (groups.find(group_name) != groups.end())
    {
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
        _send(message, clientSocket);
    }
    else
    {
        string message = "Group does not exist";
        _send(message, clientSocket);
    }
}

void handle_leave_group(map<string, vector<SOCKET>> &groups, string &group_name, SOCKET &clientSocket, mutex &clientMutex)
{
    if (groups.find(group_name) != groups.end())
    {
        vector<SOCKET> members;
        {
            lock_guard<mutex> lock(clientMutex);
            members = groups[group_name];

            auto it = find(members.begin(), members.end(), clientSocket);

            if (it == members.end())
            {
                string message = "You are not in the group";
                _send(message, clientSocket);
                return;
            }

            members.erase(it);
            groups[group_name] = members;
        }

        string message = "you have left the group";
        _send(message, clientSocket);
    }
    else
    {
        string message = "Group does not exist";
        _send(message, clientSocket);
    }
}

void handle_group_message(map<string, vector<SOCKET>> &groups, string &data, string &username, SOCKET &clientSocket, mutex &clientMutex)
{
    string group_name, msg;
    for (int i = 0; i < data.length(); i++)
        if (data[i] == ' ')
        {
            group_name = data.substr(0, i);
            msg = data.substr(i + 1);
            break;
        }

    if (groups.find(group_name) != groups.end())
    {
        vector<SOCKET> members;
        {
            lock_guard<mutex> lock(clientMutex);
            members = groups[group_name];

            auto it = find(members.begin(), members.end(), clientSocket);

            if (it != members.end())
            {
                string message = "[ " + username + " ] : " + msg;
                for (SOCKET client : members)
                {
                    if (client != clientSocket)
                        _send(message, clientSocket);
                }
            }
            else
            {
                string message = "You are not in the group";
                _send(message, clientSocket);
            }
        }
    }
    else
    {
        string message = "Group does not exist";
        _send(message, clientSocket);
    }
}