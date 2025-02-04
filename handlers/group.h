#include "../headers/common.h"
#include "../headers/ds.h"
#include "../headers/networking.h"
#include "../headers/concurrency.h"
#include "../headers/namespace.h"
#include "../headers/utils.h"
#include "../headers/setup.h"

void handle_create_group(string &group_name, SOCKET &client_socket)
{
    if (groups.find(group_name) == groups.end() || groups.size() == 0)
    {
        {
            lgm lock(client_mutex);
            vS members;
            members.push_back(client_socket);
            groups[group_name] = members;
        }

        _send("Group created", client_socket);
    }
    else
        _send("Group name not available", client_socket);
}

void handle_join_group(string &group_name, SOCKET &client_socket)
{

    if (groups.find(group_name) != groups.end())
    {
        vS members;
        {
            lgm lock(client_mutex);
            members = groups[group_name];

            for (auto x : members)
                if (x == client_socket)
                {
                    _send("You are already in this group", client_socket);
                    return;
                }

            members.push_back(client_socket);
            groups[group_name] = members;
        }

        _send("you have joined the group", client_socket);
    }
    else
        _send("Group does not exist", client_socket);
}

void handle_leave_group(string &group_name, SOCKET &client_socket)
{
    if (groups.find(group_name) != groups.end())
    {
        vS members;
        {
            lgm lock(client_mutex);
            members = groups[group_name];

            auto it = find(members.begin(), members.end(), client_socket);

            if (it == members.end())
            {
                _send("You are not in the group", client_socket);
                return;
            }

            members.erase(it);
            groups[group_name] = members;
        }

        _send("you have left the group", client_socket);
    }
    else
        _send("Group does not exist", client_socket);
}

void handle_group_message(string &data, string &username, SOCKET &client_socket)
{
    string group_name, msg;
    auto it = data.find(" ");
    if (it != string::npos)
    {
        group_name = data.substr(0, it);
        msg = data.substr(it + 1);
    }

    if (groups.find(group_name) != groups.end())
    {
        vS members;
        {
            lgm lock(client_mutex);
            members = groups[group_name];

            auto it = find(members.begin(), members.end(), client_socket);

            if (it != members.end())
            {
                string message = "[" + username + "] : " + msg;
                for (SOCKET client : members)
                {
                    if (client != client_socket)
                        _send(message, client);
                }
            }
            else
                _send("You are not in the group", client_socket);
        }
    }
    else
        _send("Group does not exist", client_socket);
}