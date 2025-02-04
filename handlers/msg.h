#include "../headers/common.h"
#include "../headers/ds.h"
#include "../headers/networking.h"
#include "../headers/namespace.h"
#include "../headers/utils.h"

void handle_private_msg(string &data, string &username, map<string, SOCKET> &users_socket, SOCKET &client_socket)
{
    string _username, msg;
    auto it = data.find(" ");
    if (it != string::npos)
    {
        _username = data.substr(0, it);
        msg = data.substr(it + 1);
        msg = "[" + username + "] : " + msg; // update message with the sender's username

        SOCKET rec_socket = users_socket[_username];

        _send(msg, rec_socket);
    }
    else
        _send("User does not exist", client_socket);
}