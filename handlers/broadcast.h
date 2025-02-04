#include "../headers/common.h"    // string
#include "../utils/socket.h"      // connection
#include "../headers/namespace.h" // namespace

void handle_broadcasting(string &data, Connection &conn)
{
    conn.broadcast_by(data);
}