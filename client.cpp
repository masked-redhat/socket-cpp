#include "./headers/common.h"
#include "./headers/networking.h" // socket libs
#include "./headers/types.h"      // types (psi)
#include "./utils/connection.h"   // connection class for socket
#include "./headers/namespace.h"  // namespaces

mutex cout_mutex;

void handle_server_messages(Connection &conn)
{
    while (true)
    {
        psi received = conn.receive();
        if (received.second <= 0) // bytes read <= 0
        {
            lock_guard<mutex> lock(cout_mutex);
            cout << "Disconnected from server." << endl;
            conn.close();
            exit(0);
        }
        lock_guard<mutex> lock(cout_mutex);
        cout << received.first << endl;
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
    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (client_socket == INVALID_SOCKET)
    {
        cerr << "Error creating socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return -1;
    }

#else

    // create socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1)
    {
        cerr << "Error creating socket.\n";
        return -1;
    }

#endif

    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;   // IPv4
    server_address.sin_port = htons(3000); // Port 3000

// connect to server
#ifdef _WIN32

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

#else

    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (connect(client_socket, (sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        cerr << "Error connecting to server." << endl;
        return 1;
    }

#endif

    cout << "Connected to the server.\n";

    // connection
    Connection conn = Connection(client_socket);

    // Authentication
    string username, password;

    string message = conn.receive().first; // Receive "Enter username: " message
    cout << message;
    getline(cin, username);
    conn.send_(username);

    message = conn.receive().first; // Receive "Enter password: " message
    cout << message;
    getline(cin, password);
    conn.send_(password);

    message = conn.receive().first; // Receive authentication result
    cout << message << endl;

// authentication failed
#ifdef _WIN32

    if (message.find("Authentication failed") != string::npos)
    {
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }

#else

    if (message.find("Authentication failed") != string::npos)
    {
        close(client_socket);
        return 1;
    }

#endif

    // Start thread for receiving messages from server
    thread receive_thread(handle_server_messages, ref(conn));
    receive_thread.detach(); // Run in the background

    // Send messages to the server
    while (true)
    {
        string message;
        getline(cin, message);

        if (message.empty())
            continue;

        conn.send_(message);

        if (message == "/exit")
        {
            conn.close();
            break;
        }
    }

    return 0;
}
