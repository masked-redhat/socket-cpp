#include "../headers/common.h"
#include "../headers/networking.h"
#include "../headers/concurrency.h"
#include "../headers/namespace.h"
#include "../headers/setup.h"
#include "../utils/socket.h"

void handle_broadcasting(string data, Connection &conn)
{
    lgm lock(client_mutex);
    string message = "[" + conn.username + "] : " + data;
    conn.broadcast(message);
}