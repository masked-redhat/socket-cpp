#include "../headers/common.h"
#include "../headers/concurrency.h"
#include "../headers/networking.h"
#include "../headers/types.h"
#include "../constants/db.h"
#include "../headers/namespace.h"

#ifndef CONNECTION
#define CONNECTION

#define BUFFER_SIZE 1024

// socket connection class
// containing socket and username information
class Connection
{
public:
    SOCKET s; // connection socket
    string username;
    Connection(SOCKET socket) : s(socket) {}
    Connection(SOCKET socket, string username) : s(socket), username(username) {}

    // sends the message to connected client
    void send_(string message)
    {
        send(s, message.c_str(), message.size(), 0);
    }

    // sends a message from the connected client to target client
    void send_to(string message, SOCKET target_client)
    {
        message = "[" + username + "] : " + message;
        send(target_client, message.c_str(), message.size(), 0);
    }

    // recieves from connected client
    psi receive()
    {
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, sizeof(buffer)); // reset buffer

        int bytes_recieved = recv(s, buffer, sizeof(buffer) - 1, 0);
        string rec_message = string(buffer);

        psi result(rec_message, bytes_recieved);

        return result;
    }

    // broadcast from connected client to other connected clients
    void broadcast_by(string message, sS sockets = db.get_clients())
    {
        message = "[" + username + "] : " + message;
        for (SOCKET client : sockets)
        {
            if (client != s)
                send(client, message.c_str(), message.size(), 0);
        }
    }

    // broadcast to other connected clients
    void broadcast(string message, sS sockets = db.get_clients())
    {
        for (SOCKET client : sockets)
        {
            if (client != s)
                send(client, message.c_str(), message.size(), 0);
        }
    }

    // close the connection
    void close()
    {
        db.remove_user(s);
        db.remove_user(username);
        closesocket(s);
    }

    // close the connection after sending a message to connected client
    void close(string message)
    {
        send_(message);
        db.remove_user(s);
        db.remove_user(username);
        closesocket(s);
    }
};

#endif