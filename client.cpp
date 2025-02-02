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

#define BUFFER_SIZE 1024

using namespace std;

mutex cout_mutex;

void handle_server_messages(SOCKET server_socket)
{
    char buffer[BUFFER_SIZE];

    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(server_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0)
        {
            lock_guard<mutex> lock(cout_mutex);
            cout << "Disconnected from server." << endl;
            closesocket(server_socket);
            WSACleanup();
            exit(0);
        }
        lock_guard<mutex> lock(cout_mutex);
        cout << buffer << endl;
    }
}

int main()
{
    WSADATA wsaData;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "WSAStartup failed.\n";
        return 1;
    }

    SOCKET client_socket;
    sockaddr_in server_address{};

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET)
    {
        cerr << "Error creating socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(3000);

    // Use inet_pton instead of inet_addr (inet_addr is deprecated)
    if (inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr) <= 0)
    {
        cerr << "Invalid address or address not supported.\n";
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // Connect to server
    if (connect(client_socket, (sockaddr *)&server_address, sizeof(server_address)) == SOCKET_ERROR)
    {
        cerr << "Error connecting to server: " << WSAGetLastError() << endl;
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    cout << "Connected to the server.\n";

    // Authentication
    string username, password;
    char buffer[BUFFER_SIZE];

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0); // Receive "Enter username: " message
    cout << buffer;
    getline(cin, username);
    send(client_socket, username.c_str(), username.size(), 0);

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0); // Receive "Enter password: " message
    cout << buffer;
    getline(cin, password);
    send(client_socket, password.c_str(), password.size(), 0);

    memset(buffer, 0, BUFFER_SIZE);
    recv(client_socket, buffer, BUFFER_SIZE, 0); // Receive authentication result
    cout << buffer << endl;

    if (string(buffer).find("Authentication failed") != string::npos)
    {
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

    // Start thread for receiving messages from server
    thread receive_thread(handle_server_messages, client_socket);
    receive_thread.detach(); // Run in the background

    // Send messages to the server
    while (true)
    {
        string message;
        getline(cin, message);

        if (message.empty())
            continue;

        send(client_socket, message.c_str(), message.size(), 0);

        if (message == "/exit")
        {
            closesocket(client_socket);
            WSACleanup();
            break;
        }
    }

    return 0;
}
