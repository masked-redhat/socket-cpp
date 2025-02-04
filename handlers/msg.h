#include "../headers/common.h"
#include "../headers/networking.h"
#include "../headers/namespace.h"
#include "../headers/setup.h"
#include "../utils/socket.h"

void handle_private_msg(string &data, Connection&conn)
{
    string _username, msg;
    auto it = data.find(" ");
    if (it != string::npos)
    {
        _username = data.substr(0, it);
        msg = data.substr(it + 1);

        if (users_socket.find(_username) != users_socket.end())
        {
            msg = "[" + conn.username + "] : " + msg; // update message with the sender's username

            SOCKET rec_socket = users_socket[_username];

            conn._send_to(msg, rec_socket);
        }
        else
            conn._send("User does not exist, or not in connection");
    }
    else
        conn._send("Data not given in good format");
}