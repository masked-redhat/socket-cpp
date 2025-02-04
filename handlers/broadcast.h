#include "../headers/common.h"
#include "../utils/socket.h"

void handle_broadcasting(string data, Connection &conn)
{
    conn.broadcast(data);
}