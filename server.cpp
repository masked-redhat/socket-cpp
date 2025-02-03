#include "./headers/common.h";
#include "./headers/ds.h";
#include "./headers/concurrency.h"; // mutex and thread
#include "./headers/networking.h";  // socket libs

// load users
#include "file.h"

#define BUFFER_SIZE 1024

using namespace std;

mutex clientMutex;
vector<SOCKET> clients;            // To store connected client sockets
mss users = load_users();          // load users
map<string, SOCKET> users_socket;  // username: socket for private messaging
map<string, vector<SOCKET>> group; // group management

void removeClient(SOCKET clientSocket)
{
    vector<SOCKET>::iterator first = clients.begin();
    vector<SOCKET>::iterator end = clients.end();

    while (first != end)
    {
        if (*first == clientSocket)
        {
            clients.erase(first, first);
            break;
        }
        first++;
    }
}

void handleClient(SOCKET clientSocket)
{
    char buffer[BUFFER_SIZE];
    string username;
    string message = "Enter username: ";

    memset(buffer, 0, BUFFER_SIZE);
    send(clientSocket, message.c_str(), message.size(), 0);
    recv(clientSocket, buffer, BUFFER_SIZE, 0);
    username = buffer;

    message = "Enter password: ";
    memset(buffer, 0, BUFFER_SIZE);
    send(clientSocket, message.c_str(), message.size(), 0);
    recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (users[username] != buffer)
    {
        message = "Authentication failed";
        users_socket[username] = clientSocket;
    }
    else
        message = "Authentication success";

    send(clientSocket, message.c_str(), message.size(), 0);

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0)
        {
            cerr << "Client disconnected.\n";

            // Remove client from the list
            {
                lock_guard<mutex> lock(clientMutex);
                removeClient(clientSocket);

                string message = "[INFO] " + username + " left";
                memset(buffer, 0, sizeof(buffer));
                for (int i = 0; i < message.length(); i++)
                    buffer[i] = message[i];
                for (SOCKET client : clients)
                {
                    if (client != clientSocket)
                    { // Don't echo back to the sender
                        send(client, buffer, sizeof(buffer), 0);
                    }
                }
            }
            closesocket(clientSocket);
            return;
        }

        cout << "Received: " << buffer << "\n";

        string message = buffer;
        string endpoint, data;
        for (int i = 0; buffer[i] != '\0'; i++)
        {
            if (buffer[i] == ' ')
            {
                endpoint = message.substr(0, i);
                data = message.substr(i + 1);
                break;
            }
        }

        if (endpoint == "/msg")
        {
        }

        // // Echo the message to all connected clients
        // lock_guard<mutex> lock(clientMutex);
        // string message = "[" + username + "] : " + buffer;
        // memset(buffer, 0, sizeof(buffer));
        // for (int i = 0; i < message.length(); i++)
        //     buffer[i] = message[i];
        // for (SOCKET client : clients)
        // {
        //     if (client != clientSocket)
        //     { // Don't echo back to the sender
        //         send(client, buffer, sizeof(buffer), 0);
        //     }
        // }
    }
}

int main()
{
    WSADATA wsaData;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "WSAStartup failed.\n";
        return -1;
    }

    // Create the server socket
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        cerr << "Error creating socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }

    // Configure the server address
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;         // IPv4
    serverAddr.sin_port = htons(3000);       // Port 12345
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces

    // Bind the socket to the address and port
    if (bind(serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerr << "Bind failed: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    // Start listening for connections
    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        cerr << "Listen failed: " << WSAGetLastError() << "\n";
        closesocket(serverSocket);
        WSACleanup();
        return -1;
    }

    cout << "Server is listening on port 3000...\n";

    // Accept and handle client connections
    while (true)
    {
        sockaddr_in clientAddr{};
        int clientAddrLen = sizeof(clientAddr);

        SOCKET clientSocket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket == INVALID_SOCKET)
        {
            cerr << "Accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

        // Add the client to the shared list
        {
            lock_guard<mutex> lock(clientMutex);
            clients.push_back(clientSocket);
        }

        cout << "New client connected.\n";

        // Start a thread to handle the client
        thread(handleClient, clientSocket).detach();
    }

    // Clean up
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
