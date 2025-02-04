#include "../headers/common.h"    // string
#include "../utils/socket.h"      // connection
#include "../constants/db.h"      // database
#include "../utils/utils.h"       // utility fns
#include "../headers/namespace.h" // namespace

void handle_create_group(const string &group_name, Connection &conn)
{
    if (db.is_group_name_available(group_name))
    {
        bool created = db.create_group(group_name, conn.s);

        if (created)
            conn.send_("Group created");
        else
            conn.send_("Some error occured while creating group");
    }
    else
        conn.send_("Group name not available");
}

void handle_join_group(const string &group_name, Connection &conn)
{
    if (db.is_group(group_name))
    {
        bool added = db.add_to_group(group_name, conn.s);
        if (added)
            conn.send_("you have joined the group");
        else
            conn.send_("You are already in this group");
    }
    else
        conn.send_("Group does not exist");
}

void handle_leave_group(const string &group_name, Connection &conn)
{
    if (db.is_group(group_name))
    {
        bool left = db.leave_from_group(group_name, conn.s);
        if (left)
            conn.send_("you have left the group");
        else
            conn.send_("You are not in the group");
    }
    else
        conn.send_("Group does not exist");
}

void handle_group_message(const string &data, Connection &conn)
{
    string group_name, msg;
    separate_string(data, group_name, msg);

    if (db.is_group(group_name))
    {
        if (db.inside_group(group_name, conn.s))
        {
            sS members = db.get_group_members(group_name);
            conn.broadcast_by(msg, members);
        }
        else
            conn.send_("You are not in the group");
    }
    else
        conn.send_("Group does not exist");
}