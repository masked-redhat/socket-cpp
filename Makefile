# Compiler and flags
CXX = g++
CXXFLAGS_WINDOWS = -lws2_32 -pthread -std=c++20
CXXFLAGS = -pthread -std=c++20

# Targets
SERVER_SRC = server.cpp
CLIENT_SRC = client.cpp
SERVER_BIN = server
CLIENT_BIN = client

# Default target
all:
	$(CXX) $(SERVER_SRC) -o $(SERVER_BIN) $(CXXFLAGS)
	$(CXX) $(CLIENT_SRC) -o $(CLIENT_BIN) $(CXXFLAGS)

windows:
	$(CXX) $(SERVER_SRC) -o $(SERVER_BIN) $(CXXFLAGS_WINDOWS)
	$(CXX) $(CLIENT_SRC) -o $(CLIENT_BIN) $(CXXFLAGS_WINDOWS)

linux:
	$(CXX) $(SERVER_SRC) -o $(SERVER_BIN) $(CXXFLAGS)
	$(CXX) $(CLIENT_SRC) -o $(CLIENT_BIN) $(CXXFLAGS)

# Clean build artifacts
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

# windows clean
wclean: 
	del "$(SERVER_BIN)"
	del "$(CLIENT_BIN)"

