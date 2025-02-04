#include "../headers/common.h"
#include "../headers/networking.h"
#include "../headers/namespace.h"
#include "../headers/utils.h"
#include "../headers/setup.h"

void handle_private_msg(string &data, string &username, SOCKET &client_socket)
{
    string _username, msg;
    auto it = data.find(" ");
    if (it != string::npos)
    {
        _username = data.substr(0, it);
        msg = data.substr(it + 1);

        if (users_socket.find(_username) != users_socket.end())
        {
            msg = "[" + username + "] : " + msg; // update message with the sender's username

            SOCKET rec_socket = users_socket[_username];

            _send(msg, rec_socket);
        }
        else
            _send("User does not exist, or not in connection", client_socket);
    }
    else
        _send("Data not given in good format", client_socket);
}