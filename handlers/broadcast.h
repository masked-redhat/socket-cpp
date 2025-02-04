#include "../headers/common.h"
#include "../headers/networking.h"
#include "../headers/concurrency.h"
#include "../headers/namespace.h"
#include "../headers/setup.h"
#include "../headers/utils.h"

void handle_broadcasting(string data, string username, SOCKET &client_socket)
{
    lgm lock(client_mutex);
    string message = "[" + username + "] : " + data;
    broadcast(message, client_socket);
}