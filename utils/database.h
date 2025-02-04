#include "../headers/common.h"
#include "../headers/concurrency.h"
#include "../headers/networking.h"
#include "../headers/types.h"
#include "./file.h"
#include "../headers/namespace.h"

#ifndef DATABASE
#define DATABASE

// users socket and usernames/groups database
// for this small socket application
class Database
{
private:
    // specific mutexs
    mutex users_mutex;
    mutex users_connected_mutex;
    mutex usernames_connected_mutex;
    mutex groups_mutex;

    mss users = load_users(); // username:password
    sS users_connected;       // connected users [sockets]
    msS usernames_connected;  // connected users [usernames] and thier sockets
    mssS groups;              // groups

public:
    Database() {}
    bool is_connected(SOCKET s)
    {
        lgm lock(users_connected_mutex);

        if (users_connected.count(s) == 0)
            return false;
        return true;
    }

    bool is_connected(string username)
    {
        lgm lock(usernames_connected_mutex);

        if (usernames_connected.count(username) == 0)
            return false;
        return true;
    }

    void add_user(SOCKET s)
    {
        lgm lock(users_connected_mutex);

        users_connected.insert(s);
    }

    void add_user(SOCKET s, string username)
    {
        scoped_lock lock(users_connected_mutex, usernames_connected_mutex);

        users_connected.insert(s);
        usernames_connected[username] = s;
    }

    void remove_user(SOCKET s)
    {
        lgm lock(users_connected_mutex);

        users_connected.erase(s);
    }

    void remove_user(string username)
    {
        lgm lock(usernames_connected_mutex);

        usernames_connected.erase(username);
    }

    void remove_user(SOCKET s, string username)
    {
        scoped_lock lock(users_connected_mutex, usernames_connected_mutex);

        users_connected.erase(s);
        usernames_connected.erase(username);
    }

    SOCKET get_socket(string username)
    {
        lgm lock(usernames_connected_mutex);

        return usernames_connected[username];
    }

    bool groups_empty()
    {
        lgm lock(groups_mutex);

        if (groups.size() == 0)
            return true;
        return false;
    }

    bool is_group_name_available(string group_name)
    {
        lgm lock(groups_mutex);

        if (groups.count(group_name) == 0)
            return true;
        return false;
    }

    bool is_group(string group_name)
    {
        lgm lock(groups_mutex);

        if (groups.count(group_name) == 0)
            return false;
        return true;
    }

    bool create_group(string group_name, SOCKET s)
    {
        lgm lock(groups_mutex);

        if (groups.count(group_name) != 0)
            return false;

        sS members{s};
        groups[group_name] = members;
        return true;
    }

    bool close_group(string group_name)
    {
        lgm lock(groups_mutex);

        if (groups.count(group_name) != 0)
            return false;

        groups.erase(group_name);
        return true;
    }

    bool add_to_group(string group_name, SOCKET s)
    {
        lgm lock(groups_mutex);

        if (groups.count(group_name) == 0)
            return false;

        if (groups[group_name].count(s) == 0)
            return false;

        groups[group_name].insert(s);
        return true;
    }

    bool leave_from_group(string group_name, SOCKET s)
    {
        lgm lock(groups_mutex);

        if (groups.count(group_name) == 0)
            return false;

        if (groups[group_name].count(s) == 0)
            return false;

        groups[group_name].erase(s);
        return true;
    }

    bool inside_group(string group_name, SOCKET s)
    {
        lgm lock(groups_mutex);

        if (groups.count(group_name) == 0)
            return false;

        if (groups[group_name].count(s) == 0)
            return false;

        return true;
    }

    sS get_group_members(string group_name)
    {
        lgm lock(groups_mutex);

        return groups[group_name];
    }

    sS get_clients()
    {
        lgm lock(users_connected_mutex);

        return users_connected;
    }

    bool authenticate_user(string username, string password)
    {
        lgm lock(users_mutex);

        if (users.count(username) == 0)
            return false;
        return users[username] == password;
    }

    msS get_connected_users()
    {
        lgm lock(usernames_connected_mutex);

        return usernames_connected;
    }
};

#endif