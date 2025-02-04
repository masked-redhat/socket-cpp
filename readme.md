# Chat Server with Groups and Private Messages - CS425 Assignment 1

## 📌 Project Overview

This project implements a **multi-threaded chat server and client** that allows users to:

- **Authenticate** using a `users.txt` file in `*./constants*` directory.
- **Send private messages** to specific users.
- **Broadcast messages** to all connected users.
- **Create and manage groups** for group messaging.
- **Handle multiple clients simultaneously** using multithreading.
- **Handles Error** by giving specific messages to clients.
- **Support both Windows and Linux platforms**.

This chat server and client are part of **CS425: Computer Networks - Assignment 1**.

---

## 🔹 Features

- ✅ **User Authentication** (based on `users.txt`)
- ✅ **Private Messaging** (`/msg <username> <message>`)
- ✅ **Broadcast Messaging** (`/broadcast <message>`)
- ✅ **Group Management**
  - `/create_group <group_name>` - Create a group.
  - `/join_group <group_name>` - Join a group.
  - `/leave_group <group_name>` - Leave a group.
  - `/group_msg <group_name> <message>` - Send a message to a group.
- ✅ **Thread-safe Client Management** using `std::mutex`
- ✅ **Cross-platform Support** (Windows & Linux)

---

## 📦 Folder Structure

```
chat-server/
│── headers/               # Header files
│   ├── common.h           # Common includes
│   ├── networking.h       # Cross-platform networking
│   ├── types.h            # Type definitions
|
│── utils/                 # Utility classes and functions
│   ├── connection.h       # Connection class for sockets
│   ├── database.h         # Database for users and groups
│   ├── file.h             # File handling (loading users.txt)
│   ├── utils.h            # String processing utilities
|
│── handlers/              # Message handling functions
│   ├── msg.h              # Private message handling
│   ├── broadcast.h        # Broadcast message handling
│   ├── group.h            # Group message handling
|
│── constants/             # Constants and predefined data
│   ├── db.h               # Global database object
│   ├── users.txt          # List of usernames and passwords
|
│── server.cpp             # Main server implementation
│── client.cpp             # Client implementation
│── Makefile               # Compilation commands
│── .gitignore             # Ignored files for Git
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

---

## 💡 How to Use

### **Starting the Server**

```sh
./server
```

The server starts listening on **port 3000** and waits for client connections.

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

---

## 🤝 Contribution

Feel free to contribute by opening issues or pull requests!

---

## 🏆 Credits

This project is part of **CS425: Computer Networks** at IIT Kanpur, developed by **Kartikey's Team**.

### Team Members

- Kartikey Kumar [200493]
- Vijay Makwana [201113]
