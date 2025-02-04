#include "./headers/common.h"
#include "./headers/networking.h" // socket libs
#include "./utils/socket.h"       // socket utility fns
#include "./utils/utils.h"        // utility fns
#include "./constants/db.h"       // current db
#include "./handlers/msg.h"       // handlers private message
#include "./handlers/group.h"     // handlers group message
#include "./headers/namespace.h"  // namespaces

void handleClient(Connection conn)
{
    conn.send_("Enter username: ");
    string username = conn.receive().first;

    if (db.is_connected(username))
    {
        cerr << "Client disconnected.\n";
        conn.close("User already connected.");
        return;
    }

    conn.send_("Enter password: ");
    string password = conn.receive().first;

    if (!db.authenticate_user(username, password))
    {
        cerr << "Client disconnected.\n";
        conn.close("Authentication failed.");
        return;
    }

    if (db.is_connected(username))
    {
        cerr << "Client disconnected.\n";
        conn.close("User already connected.");
        return;
    }

    db.add_user(conn.s, username); // add user to database
    conn.username = username;

    conn.send_("Authentication success");
    conn.broadcast("[INFO] " + conn.username + " joined.");

    while (true)
    {
        psi recieved = conn.receive();
        if (recieved.second <= 0) // bytes recieved
        {
            cerr << "Client disconnected.\n";
            conn.broadcast("[INFO] : " + conn.username + " left");
            conn.close();
            return;
        }

        string endpoint, data;
        separate_string(recieved.first, endpoint, data);

        if (endpoint == "/msg")
            handle_private_msg(data, conn);
        else if (endpoint == "/broadcast")
            conn.broadcast_by(data, db.get_clients());
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

        // Add the client to the shared list
        db.add_user(client_socket);

        cout << "New client connected.\n";
        Connection conn = Connection(client_socket);

        // Start a thread to handle the client
        thread(handleClient, conn).detach();
    }

    // Clean up
    closesocket(serverSocket);
    WSACleanup();
    return 0;
}
