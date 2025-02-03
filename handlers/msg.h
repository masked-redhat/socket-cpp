#include "../headers/common.h"
#include "../headers/ds.h"
#include "../headers/networking.h"
#include "../headers/namespace.h"

void handle_private_msg(string &data, string &username, map<string, SOCKET> &users_socket)
{
    string _username, msg;
    for (int i = 0; i < data.length(); i++)
    {
        if (data[i] == ' ')
        {
            _username = data.substr(0, i);
            msg = data.substr(i + 1);
            break;
        }
    }

    msg = "[ " + username + " ] " + msg; // update message with the sender's username

    SOCKET rec_socket = users_socket[_username];

    send(rec_socket, msg.c_str(), msg.size(), 0);
}