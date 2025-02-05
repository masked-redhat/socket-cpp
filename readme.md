# Chat Server with Groups and Private Messages - CS425 Assignment 1

## 📌 Project Overview

This project implements a multi-threaded chat server that supports private messaging, group communication, and user authentication. It is designed as part of the **CS425: Computer Networks** course assignment. The server allows clients to _connect_, _authenticate_, and _interact_ via commands such as `/msg`, `/broadcast`, `/create_group`, `/join_group`, `/leave_group`, and `/group_msg`.

---

## 🔹 Features

## **Features**

1.  **User Authentication** :

    - Users are authenticated using a `users.txt` file containing usernames and passwords.
    - Clients must provide valid credentials to connect to the server.

2.  **Messaging Features** :

    - **Broadcast** : Send messages to all connected clients using `/broadcast <message>`.
    - **Private Messages** : Send direct messages to specific users using `/msg <username> <message>`.
    - **Group Messaging** : Send messages to all members of a group using `/group_msg <group name> <message>`.

3.  **Group Management** :

    - Create groups using `/create_group <group name>`.
    - Join groups using `/join_group <group name>`.
    - Leave groups using `/leave_group <group name>`.

4.  **Multi-threading** :

    - The server handles multiple concurrent client connections using threads.

5.  **Thread Safety** :

    - Shared resources (e.g., user lists, group memberships) are protected using mutex locks to ensure thread-safe access.

6.  **Cross Platform Support** :

    - Windows and Unix/Linux system supported
    - Application dockerized to remove system dependency

---

## 📦 Folder Structure

```
.
├── .dockerignore          # Files ignored by Docker
├── .gitignore             # Files ignored by Git
├── client.cpp             # Client-side implementation
|
├── constants/             # Constants and configurations
│   ├── db.h               # Database initialization
│   ├── env.h              # Environment variable loader
│   └── users.txt          # User credentials (username:password)
|
├── Dockerfile             # Docker configuration for deployment
|
├── handlers/              # Handlers for different commands
│   ├── broadcast.h        # Broadcast message handler
│   ├── group.h            # Group-related command handlers
│   └── msg.h              # Private message handler
|
├── headers/               # Header files for common definitions
│   ├── common.h           # Common includes and typedefs
│   ├── concurrency.h      # Concurrency utilities
│   ├── namespace.h        # Namespace declarations
│   ├── networking.h       # Networking-related includes
│   └── types.h            # Type definitions
|
├── Makefile               # Build script for compiling the server and client
├── server.cpp             # Server-side implementation
|
└── utils/                 # Utility functions and classes
    ├── connection.h       # Connection class for socket management
    ├── database.h         # Database class for managing users and groups
    ├── env.h              # Environment variable loader
    ├── file.h             # File utility functions
    └── utils.h            # General utility functions
```

---

## 🛠️ Installation & Setup

### **🔹 Windows (MinGW or MSVC)**

1. **Install MinGW** (if not installed):
   - Download from: [https://winlibs.com/](https://winlibs.com/)
   - Add `mingw32/bin` to `PATH`.
2. **Compile the Server and Client:**
   ```sh
   make windows
   ```
3. **Run the Server:**
   ```sh
   ./server.exe
   ```
4. **Run the Client:**
   ```sh
   ./client.exe
   ```

### **🔹 Linux (Ubuntu / MacOS)**

1. **Install g++ (if not installed):**
   ```sh
   sudo apt update && sudo apt install g++
   ```
2. **Compile the Server and Client:**
   ```sh
   make linux
   ```
3. **Run the Server:**
   ```sh
   ./server
   ```
4. **Run the Client:**
   ```sh
   ./client
   ```

### **🔹 Docker (Docker Desktop)**

1. **Install Docker Desktop** (if not installed):
   - Download from: [Docker Desktop](https://docs.docker.com/desktop/)
2. **Compile the Server and Client:**
   ```sh
   make docker
   ```
3. **Open another terminal and run:**
   ```sh
   make dclient
   ```
4. **Or you can run client.ext/client.out here on port 3000:**
   ```sh
   ./client.exe
   ```
5. **Stop the Docker Container**
   ```sh
   make dclean
   ```

---

## 💡 How to Use

### **Starting the Server**

```sh
./server
```

The server starts listening on **port 3000** or as available in **.env file** and waits for client connections.

### **Starting a Client**

```sh
./client
```

The client will prompt for authentication before allowing messaging.

### **Client Commands**

| Command                             | Description                         |
| ----------------------------------- | ----------------------------------- |
| `/msg <username> <message>`         | Send a private message to a user    |
| `/broadcast <message>`              | Send a message to all users         |
| `/create_group <group_name>`        | Create a new group                  |
| `/join_group <group_name>`          | Join an existing group              |
| `/leave_group <group_name>`         | Leave a group                       |
| `/group_msg <group_name> <message>` | Send a message to all group members |
| `/exit`                             | Disconnect from the server          |

### **Example Usage**

```
$ ./client
Connected to the server.
Enter username: alice
Enter password: password123
Welcome to the chat server!
/broadcast Hello everyone!
/msg bob Hey Bob, how are you?
/create_group CS425
/join_group CS425
/group_msg CS425 Hi team, let's start!
```

---

## ⚙️ Code Overview

### **1. `server.cpp` (Main Entry Point)**

- **Creates and configures the server socket.**
- **Accepts client connections.**
- **Handles authentication and message routing.**

### **2. `client.cpp` (Client Entry Point)**

- **Connects to the server.**
- **Handles authentication.**
- **Reads user input and sends commands to the server.**
- **Runs a background thread to receive messages from the server.**

### **3. `utils/connection.h` (Client Connection Class)**

- **Manages socket communication** (send, receive).
- **Handles broadcasting and direct messages.** (broadcast).
- **Ensures proper cleanup when disconnecting.** (close)

### **4. `utils/database.h` (User & Group Management)**

- **Stores authenticated users and active groups.**
- **Ensures thread-safe access using `std::mutex`.**
- **Allows concurrent access to different parts of the database efficiently.**
- **Handles authentication, client connections, and group management.**

### **5. `handlers/` (Message Handlers)**

- `msg.h`: Handles **private messages**.
- `broadcast.h`: Handles **broadcasts**.
- `group.h`: Handles **group-related actions**.

### **5. `constants/` (Config files and db initializations)**

- `db.h`: Creates **a simple memory database** for application.
- `env.h`: Loads **environment variables** like _port_, _user file location_.

---

## 🏆 Credits

This project is part of **CS425: Computer Networks** at IIT Kanpur, developed by **Kartikey's Team**.

### Team Members

- Kartikey Kumar [200493]
- Vijay Makwana [201113]
