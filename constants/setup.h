#include "../headers/common.h"
#include "../headers/concurrency.h"
#include "../headers/ds.h"
#include "../headers/networking.h"
#include "../utils/file.h"
#include "../headers/namespace.h"

typedef vector<SOCKET> vS;
typedef map<string, SOCKET> msS;
typedef map<string, vector<SOCKET>> msvS;
typedef lock_guard<mutex> lgm; // lock guard

mutex client_mutex;        // mutex for locking
vS clients;               // To store connected client sockets
mss users = load_users(); // load users
msS users_socket;         // username: socket for private messaging
msvS groups;              // group management
