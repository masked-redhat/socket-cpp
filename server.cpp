#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <sstream>
#include <cstring>
#include <cstdlib>

// For Windows and Linux
#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#endif

using namespace std;

mutex clientMutex;
vector<SOCKET> clients; // To store connected client sockets

void handleClient(SOCKET clientSocket)
{
    char buffer[1024];
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
                clients.erase(clients.begin(), clients.end());
            }
            closesocket(clientSocket);
            return;
        }

        cout << "Received: " << buffer << "\n";

        // Echo the message to all connected clients
        lock_guard<mutex> lock(clientMutex);
        for (SOCKET client : clients)
        {
            if (client != clientSocket)
            { // Don't echo back to the sender
                send(client, buffer, bytesRead, 0);
            }
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
