#include "../headers/common.h"    // string
#include "../constants/db.h"      // database
#include "../utils/socket.h"      // connection
#include "../utils/utils.h"       // utility fns
#include "../headers/namespace.h" // namespace

void handle_private_msg(const string &data, Connection &conn)
{
    string _username, msg;
    separate_string(data, _username, msg);
    if (db.is_connected(_username))
    {
        SOCKET rec_socket = db.get_socket(_username);
        conn.send_to(msg, rec_socket);
    }
    else
        conn.send_("User does not exist, or not in connection");
}