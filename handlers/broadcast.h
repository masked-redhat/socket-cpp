#include "../headers/common.h"
#include "../headers/ds.h"
#include "../headers/networking.h"
#include "../headers/concurrency.h"
#include "../headers/namespace.h"

void handle_broadcasting(string data, string username, vector<SOCKET> &clients, mutex &clientMutex, SOCKET &clientSocket)
{
    lock_guard<mutex> lock(clientMutex);
    string message = "[ " + username + " ] : " + data;
    for (SOCKET client : clients)
    {
        if (client != clientSocket)
            _send(message, clientSocket);
    }
}