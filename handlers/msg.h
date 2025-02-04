#include "../headers/common.h"    // string
#include "../constants/db.h"      // database
#include "../utils/socket.h"      // connection
#include "../headers/namespace.h" // namespace

void handle_private_msg(string &data, Connection &conn)
{
    string _username, msg;
    auto it = data.find(" ");
    if (it != string::npos)
    {
        _username = data.substr(0, it); // set target username
        msg = data.substr(it + 1);      // set msg to send

        if (db.is_connected(_username))
        {
            SOCKET rec_socket = db.get_socket(_username);
            conn.send_to(msg, rec_socket);
        }
        else
            conn.send_("User does not exist, or not in connection");
    }
    else
        conn.send_("Data not given in good format");
}