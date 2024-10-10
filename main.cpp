// main.cpp
#include "HttpServer.hpp"
#include <iostream>

int main() {
    IHttpServer* server = new HttpServer();

    if (server->StartServer(8081) == S_OK) {
        std::cout << "Server started on port 8081." << std::endl;
    }
    else {
        std::cout << "Failed to start server." << std::endl;
        server->Release();
        return -1;
    }

    std::cout << "Press Enter to stop the server..." << std::endl;
    std::cin.get();

    server->StopServer();
    server->Release();

    return 0;
}
