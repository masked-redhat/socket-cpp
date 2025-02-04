#include "../headers/common.h"
#include "../headers/networking.h"
#include "../headers/setup.h"
#include "../headers/namespace.h"
#include "./send_message.h"

void broadcast(string message, SOCKET &client_socket, vS clients = clients)
{
    for (SOCKET client : clients)
    {
        if (client != client_socket)
            _send(message, client);
    }
}