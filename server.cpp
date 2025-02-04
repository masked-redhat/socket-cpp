#include "./headers/common.h"
#include "./headers/networking.h" // socket libs
#include "./utils/connection.h"   // connection class for socket
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
        conn.close_("User already connected.");
        return false;
    }

    conn.send_("Enter password: ");
    string password = conn.receive().first;

    if (!db.authenticate_user(username, password))
    {
        cerr << "Client disconnected.\n";
        conn.close_("Authentication failed.");
        return false;
    }

    if (db.is_connected(username))
    {
        cerr << "Client disconnected.\n";
        conn.close_("User already connected.");
        return false;
    }

    db.add_user(conn.s, username); // add user to database
    conn.username = username;

    conn.send_("Welcome to the chat server !");
    conn.broadcast(conn.username + " has joined the chat.");
    return true;
}

void handle_client(Connection conn)
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
            conn.close_();
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
            conn.close_();
            return;
        }
        else
            conn.send_("[INFO] : No endpoint like that");
    }
}

int main()
{

// create socket
#ifdef _WIN32

    // winsock initialization (windows)
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0)
    {
        cerr << "WSAStartup failed.\n";
        return -1;
    }

    // create socket
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket == INVALID_SOCKET)
    {
        cerr << "Error creating socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }

#else

    // create socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1)
    {
        cerr << "Error creating socket.\n";
        return -1;
    }

#endif

    // configure the server address
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;         // IPv4
    server_addr.sin_port = htons(3000);       // Port 3000
    server_addr.sin_addr.s_addr = INADDR_ANY; // Bind to all interfaces

// bind and listen
#ifdef _WIN32

    // bind the socket to the address and port
    if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR)
    {
        cerr << "Bind failed: " << WSAGetLastError() << "\n";
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

    // start listening for connections
    if (listen(server_socket, SOMAXCONN) == SOCKET_ERROR)
    {
        cerr << "Listen failed: " << WSAGetLastError() << "\n";
        closesocket(server_socket);
        WSACleanup();
        return -1;
    }

#else

    // bind the socket to the address and port
    if (bind(server_socket, (sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        cerr << "Bind failed.\n";
        close(server_socket);
        return -1;
    }

    // start listening for connections
    if (listen(server_socket, SOMAXCONN) == -1)
    {
        cerr << "Listen failed.\n";
        close(server_socket);
        return -1;
    }

#endif

    cout << "Server is listening on port 3000...\n";

    // accept and handle client connections
    while (true)
    {
        sockaddr_in client_addr{};

// client socket
#ifdef _WIN32

        int client_addr_len = sizeof(client_addr);
        SOCKET client_socket = accept(server_socket, (sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == INVALID_SOCKET)
        {
            cerr << "Accept failed: " << WSAGetLastError() << "\n";
            continue;
        }

#else

        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (sockaddr *)&client_addr, &client_addr_len);
        if (client_socket == -1)
        {
            cerr << "Accept failed.\n";
            continue;
        }

#endif

        // add the client to the database
        db.add_user(client_socket);

        cout << "New client connected.\n";

        // start a thread to handle the client
        thread(handle_client, Connection(client_socket)).detach();
    }

// clean up
#ifdef _WIN32

    closesocket(server_socket);
    WSACleanup();

#else

    close(server_socket);

#endif

    return 0;
}
