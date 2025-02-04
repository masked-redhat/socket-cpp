#include "../headers/common.h"
#include "../headers/networking.h"
#include "../headers/namespace.h"

void _send(string message, SOCKET socket)
{
    send(socket, message.c_str(), message.size(), 0);
}