#include "./headers/common.h"
#include "./headers/networking.h" // socket libs
#include "./utils/connection.h"   // socket utility fns
#include "./utils/utils.h"        // utility fns
#include "./constants/db.h"       // current db
#include "./handlers/msg.h"       // handlers private message
#include "./handlers/broadcast.h" // handlers broadcast message
#include "./handlers/group.h"     // handlers group message
#include "./headers/namespace.h"  // namespaces

typedef unordered_map<string, function<void(const string &, Connection &)>> ep;

// endpoints for this application
static const ep endpoints = {
    {"/msg", handle_private_msg},
    {"/broadcast", handle_broadcasting},
    {"/create_group", handle_create_group},
    {"/join_group", handle_join_group},
    {"/leave_group", handle_leave_group},
    {"/group_msg", handle_group_message},
};

bool authenticate(Connection &conn)
{
    conn.send_("Enter username: ");
    string username = conn.receive().first;

    if (db.is_connected(username))
    {
        cerr << "Client disconnected.\n";
        conn.close("User already connected.");
        return false;
    }

    conn.send_("Enter password: ");
    string password = conn.receive().first;

    if (!db.authenticate_user(username, password))
    {
        cerr << "Client disconnected.\n";
        conn.close("Authentication failed.");
        return false;
    }

    if (db.is_connected(username))
    {
        cerr << "Client disconnected.\n";
        conn.close("User already connected.");
        return false;
    }

    db.add_user(conn.s, username); // add user to database
    conn.username = username;

    conn.send_("Authentication success");
    conn.broadcast("[INFO] " + conn.username + " joined.");
    return true;
}

void handleClient(Connection conn)
{
    if (!authenticate(conn))
        return;

    while (true)
    {
        psi recieved = conn.receive();
        if (recieved.second <= 0) // bytes recieved <= 0
        {
            cerr << "Client disconnected.\n";
            conn.broadcast("[INFO] : " + conn.username + " left");
            conn.close();
            return;
        }

        string endpoint, data;
        separate_string(recieved.first, endpoint, data);

        // calls the function by matching from the above map
        auto it = endpoints.find(endpoint);
        if (it != endpoints.end())
        {
            it->second(data, conn);
            continue;
        }

        // Exit or Invalid endpoints
        if (endpoint == "/exit")
        {
            conn.close();
            return;
        }
        else
            conn.send_("[INFO] : No endpoint like that");
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
    serverAddr.sin_port = htons(3000);       // Port 3000
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

        // Add the client to the database
        db.add_user(client_socket);

        cout << "New client connected.\n";

        // Start a thread to handle the client
        thread(handleClient, Connection(client_socket)).detach();
    }

    // Clean up
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
