#include "./headers/common.h"
#include "./headers/ds.h"
#include "./headers/concurrency.h" // mutex and thread
#include "./headers/networking.h"  // socket libs
#include "./headers/handlers.h"    // handlers
#include "./headers/namespace.h"   // namespaces
#include "./headers/utils.h"       // utility functions
#include "file.h"                  // load users

mutex clientMutex;
vector<SOCKET> clients;             // To store connected client sockets
mss users = load_users();           // load users
map<string, SOCKET> users_socket;   // username: socket for private messaging
map<string, vector<SOCKET>> groups; // group management

void removeClient(SOCKET &clientSocket, string &username)
{
    auto it = find(clients.begin(), clients.end(), clientSocket);
    clients.erase(it);

    users_socket.erase(username); // erase socket from username:socket map
}

void handleClient(SOCKET clientSocket)
{
    string username;
    string message = "Enter username: ";

    _send(message, clientSocket);
    username = _recieve(clientSocket).first;

    if (users_socket[username] != 0)
    {
        message = "Username already exist in connection";
        _send(message, clientSocket);
        {
            lock_guard<mutex> lock(clientMutex);
            removeClient(clientSocket, username);
        }
        closesocket(clientSocket);
        return;
    }

    message = "Enter password: ";
    _send(message, clientSocket);
    string password = _recieve(clientSocket).first;
    if (users[username] != password)
        message = "Authentication failed";
    else
    {
        message = "Authentication success";
        {
            lock_guard<mutex> lock(clientMutex);
            users_socket[username] = clientSocket;
        }
        handle_broadcasting(username + " joined", "INFO", clients, clientMutex, clientSocket);
    }

    _send(message, clientSocket);

    while (true)
    {
        psi recieved = _recieve(clientSocket);
        int bytes_read = recieved.second;
        if (bytes_read <= 0)
        {
            cerr << "Client disconnected.\n";
            {
                lock_guard<mutex> lock(clientMutex);
                removeClient(clientSocket, username);
                handle_broadcasting(username + " left", "INFO", clients, clientMutex, clientSocket);
            }
            closesocket(clientSocket);
            return;
        }

        string message = recieved.first;
        string endpoint, data;
        auto it = message.find(" ");
        if (it != string::npos)
        {
            endpoint = message.substr(0, it);
            data = message.substr(it + 1);
        }

        if (endpoint == "/msg")
            handle_private_msg(data, username, users_socket);
        else if (endpoint == "/broadcast")
            handle_broadcasting(data, username, clients, clientMutex, clientSocket);
        else if (endpoint == "/create_group")
            handle_create_group(groups, data, clientSocket, clientMutex);
        else if (endpoint == "/join_group")
            handle_join_group(groups, data, clientSocket, clientMutex);
        else if (endpoint == "/leave_group")
            handle_leave_group(groups, data, clientSocket, clientMutex);
        else if (endpoint == "/group_msg")
            handle_group_message(groups, data, username, clientSocket, clientMutex);
        else if (endpoint == "/exit")
        {
            {
                lock_guard<mutex> lock(clientMutex);
                removeClient(clientSocket, username);
            }
            closesocket(clientSocket);
            return;
        }
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
