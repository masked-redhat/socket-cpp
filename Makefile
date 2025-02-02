# Compiler and flags
CXX = g++
CXXFLAGS = -lws2_32 -std=c++17

# Targets
SERVER_SRC = server.cpp
CLIENT_SRC = client.cpp
SERVER_BIN = server
CLIENT_BIN = client

# Default target
all: $(SERVER_BIN) $(CLIENT_BIN)

# Compile server
$(SERVER_BIN): $(SERVER_SRC)
	$(CXX) $(SERVER_SRC) -o $(SERVER_BIN) $(CXXFLAGS)

# Compile client
$(CLIENT_BIN): $(CLIENT_SRC)
	$(CXX) $(CLIENT_SRC) -o $(CLIENT_BIN) $(CXXFLAGS)

# Clean build artifacts
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

