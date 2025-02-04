#include "../headers/common.h"
#include "../headers/networking.h"
#include "../headers/namespace.h"

#ifndef SEND_FN
#define SEND_FN

void _send(string message, SOCKET socket)
{
    send(socket, message.c_str(), message.size(), 0);
}

#endif