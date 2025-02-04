#include "../headers/common.h"
#include "../headers/networking.h"
#include "../headers/namespace.h"

#ifndef RECIEVE_FN
#define RECIEVE_FN

#define BUFFER_SIZE 1024

typedef pair<string, int> psi;

psi _recieve(SOCKET &socket)
{
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer)); // reset buffer

    int bytes_recieved = recv(socket, buffer, sizeof(buffer) - 1, 0);
    string rec_message = string(buffer);

    psi result(rec_message, bytes_recieved);

    return result;
}

#endif