#include "./headers/common.h"
#include "./headers/concurrency.h" // mutex and thread
#include "./headers/networking.h"  // socket libs
#include "./headers/handlers.h"    // handlers
#include "./headers/namespace.h"   // namespaces
#include "./utils/socket.h"        // socket utility fns
#include "./headers/setup.h"       // constant variables

bool authenticate(Connection &conn)
{
    conn._send("Enter username: ");
    string username = conn._recieve().first;

    if (users_socket[username] != 0)
    {
        conn.close("User already exist in connection");
        return;
    }

    conn._send("Enter password: ");
    string password = conn._recieve().first;
    if (users[username] != password)
    {
        conn.close("Authentication failed");
        return false;
    }
    else
    {
        conn.username = username;
        conn._send("Authentication success");
        {
            lgm lock(client_mutex);
            users_socket[username] = conn.s;
        }
        conn.broadcast("[INFO] : " + username + " joined");
    }

    return true;
}

void handleClient(SOCKET client_socket)
{
    Connection conn = Connection(client_socket);

    bool authenticated = authenticate(conn);

    if (!authenticated)
        return;

    while (true)
    {
        psi recieved = conn._recieve();
        if (recieved.second <= 0) // bytes recieved
        {
            cerr << "Client disconnected.\n";
            conn.broadcast("[INFO] : " + conn.username + " left");
            conn.close();
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
            handle_private_msg(data, conn);
        else if (endpoint == "/broadcast")
            handle_broadcasting(data, conn);
        else if (endpoint == "/create_group")
            handle_create_group(data, conn);
        else if (endpoint == "/join_group")
            handle_join_group(data, conn);
        else if (endpoint == "/leave_group")
            handle_leave_group(data, conn);
        else if (endpoint == "/group_msg")
            handle_group_message(data, conn);
        else if (endpoint == "/exit")
        {
            conn.close();
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

        SOCKET client_socket = accept(serverSocket, (sockaddr *)&clientAddr, &clientAddrLen);
        if (client_socket == INVALID_SOCKET)
        {
            cerr << "Accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

        // Add the client to the shared list
        {
            lgm lock(client_mutex);
            clients.push_back(client_socket);
        }

        cout << "New client connected.\n";

        // Start a thread to handle the client
        thread(handleClient, client_socket).detach();
    }

    // Clean up
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
