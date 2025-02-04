#include "../headers/common.h"    // string
#include "../utils/connection.h"  // connection
#include "../headers/namespace.h" // namespace

void handle_broadcasting(const string &data, Connection &conn)
{
    conn.broadcast_by(data);
}