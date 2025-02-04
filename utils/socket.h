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

    // sends the message to connected client
    void _send(string message)
    {
        send(s, message.c_str(), message.size(), 0);
    }

    // sends a message from the connected client to target client
    void _send_to(string message, SOCKET target_client)
    {
        message = "[" + username + "] : " + message;
        send(target_client, message.c_str(), message.size(), 0);
    }

    // recieves from connected client
    psi _recieve()
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer)); // reset buffer

        int bytes_recieved = recv(s, buffer, sizeof(buffer) - 1, 0);
        string rec_message = string(buffer);

        psi result(rec_message, bytes_recieved);

        return result;
    }

    // broadcast from connected client to other connected clients
    void broadcast(string message, vS sockets = clients)
    {
        lgm lock(client_mutex);
        message = "[" + username + "] : " + message;
        for (SOCKET client : sockets)
        {
            if (client != s)
                send(client, message.c_str(), message.size(), 0);
        }
    }

    // close the connection
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

    // close the connection after sending a message to connected client
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