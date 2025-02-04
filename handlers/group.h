#include "../headers/common.h"
#include "../headers/concurrency.h"
#include "../headers/setup.h"
#include "../utils/socket.h"
#include "../headers/namespace.h"

void handle_create_group(string &group_name, Connection &conn)
{
    if (groups.find(group_name) == groups.end() || groups.size() == 0)
    {
        {
            lgm lock(client_mutex);
            vS members;
            members.push_back(conn.s);
            groups[group_name] = members;
        }

        conn._send("Group created");
    }
    else
        conn._send("Group name not available");
}

void handle_join_group(string &group_name, Connection &conn)
{

    if (groups.find(group_name) != groups.end())
    {
        {
            lgm lock(client_mutex);
            vS members = groups[group_name];

            auto it = find(members.begin(), members.end(), conn.s);

            if (it != members.end())
            {
                members.push_back(conn.s);    // add client to members
                groups[group_name] = members; // update members
            }
            else
            {
                conn._send("You are already in this group");
                return;
            }
        }

        conn._send("you have joined the group");
    }
    else
        conn._send("Group does not exist");
}

void handle_leave_group(string &group_name, Connection &conn)
{
    if (groups.find(group_name) != groups.end())
    {
        {
            lgm lock(client_mutex);
            vS members = groups[group_name];

            auto it = find(members.begin(), members.end(), conn.s);

            if (it != members.end())
            {
                members.erase(it);            // remove client from members
                groups[group_name] = members; // update members
            }
            else
            {
                conn._send("You are not in the group");
                return;
            }
        }

        conn._send("you have left the group");
    }
    else
        conn._send("Group does not exist");
}

void handle_group_message(string &data, Connection &conn)
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
        }

        auto it = find(members.begin(), members.end(), conn.s);

        if (it != members.end())
            conn.broadcast(msg, members);
        else
            conn._send("You are not in the group");
    }
    else
        conn._send("Group does not exist");
}