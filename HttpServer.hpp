// HttpServer.hpp
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h> 
#include <windows.h>
#include "IHttpServer.hpp"
#include <thread>
#include <string>
#include <map>

class HttpServer : public IHttpServer {
private:
    LONG m_refCount;
    SOCKET m_listenSocket;
    std::thread m_serverThread;
    bool m_running;

    void ServerLoop();
    void HandleClient(SOCKET clientSocket);
    std::string GetMimeType(const std::string& path);
    void Log(const std::string& message); // Logging method

public:
    HttpServer();
    ~HttpServer();

    // IUnknown methods
    HRESULT __stdcall QueryInterface(REFIID riid, void** ppvObject);
    ULONG __stdcall AddRef();
    ULONG __stdcall Release();

    // IHttpServer methods
    HRESULT __stdcall StartServer(UINT16 port);
    HRESULT __stdcall StopServer();
};
