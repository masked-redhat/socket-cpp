#include "../headers/common.h"
#include "../headers/ds.h"
#include "../headers/networking.h"
#include "../headers/concurrency.h"
#include "../headers/namespace.h"
#include "../headers/setup.h"

void handle_broadcasting(string data, string username, SOCKET &client_socket)
{
    lgm lock(client_mutex);
    string message = "[" + username + "] : " + data;
    for (SOCKET client : clients)
    {
        if (client != client_socket)
            _send(message, client);
    }
}