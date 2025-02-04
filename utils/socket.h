#include "../headers/common.h"
#include "../headers/concurrency.h"
#include "../headers/networking.h"
#include "../headers/setup.h"
#include "../headers/namespace.h"

#ifndef CONNECTION
#define CONNECTION

#define BUFFER_SIZE 1024

typedef pair<string, int> psi;

class Connection
{
public:
    SOCKET s; // connection socket
    string username;
    Connection(SOCKET socket) : s(socket) {}
    Connection(SOCKET socket, string username) : s(socket), username(username) {}

    void _send(string message)
    {
        send(s, message.c_str(), message.size(), 0);
    }

    void _send_to(string message, SOCKET socket)
    {
        send(socket, message.c_str(), message.size(), 0);
    }

    psi _recieve()
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer)); // reset buffer

        int bytes_recieved = recv(s, buffer, sizeof(buffer) - 1, 0);
        string rec_message = string(buffer);

        psi result(rec_message, bytes_recieved);

        return result;
    }

    void broadcast(string message, vS sockets = clients)
    {
        for (SOCKET client : sockets)
        {
            if (client != s)
                send(client, message.c_str(), message.size(), 0);
        }
    }

    void close()
    {
        {
            lgm lock(client_mutex);
            auto it = find(clients.begin(), clients.end(), s);
            clients.erase(it);

            users_socket.erase(username); // erase socket from username:socket map
        }
        closesocket(s);
    }

    void close(string message)
    {
        _send(message);
        {
            lgm lock(client_mutex);
            auto it = find(clients.begin(), clients.end(), s);
            clients.erase(it);

            users_socket.erase(username); // erase socket from username:socket map
        }
        closesocket(s);
    }
};

#endif