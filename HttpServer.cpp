// HttpServer.cpp

// Add this define at the very top of the file, before any includes
#define _WIN32_WINNT 0x0600 // Targeting Windows Vista or later

// Include necessary headers
#include <winsock2.h>
#include <ws2tcpip.h> // For InetNtop and other advanced Winsock functions
#include <windows.h>
#include "HttpServer.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

// Mutex for thread-safe logging
std::mutex logMutex;

HttpServer::HttpServer() : m_refCount(1), m_listenSocket(INVALID_SOCKET), m_running(false) {
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        Log("WSAStartup failed with error: " + std::to_string(result));
    }
}

HttpServer::~HttpServer() {
    StopServer();
    WSACleanup();
}

// IUnknown methods
HRESULT __stdcall HttpServer::QueryInterface(REFIID riid, void** ppvObject) {
    if (riid == IID_IUnknown || riid == IID_IHttpServer) {
        *ppvObject = static_cast<IHttpServer*>(this);
        AddRef();
        return S_OK;
    }
    *ppvObject = nullptr;
    return E_NOINTERFACE;
}

ULONG __stdcall HttpServer::AddRef() {
    return InterlockedIncrement(&m_refCount);
}

ULONG __stdcall HttpServer::Release() {
    ULONG res = InterlockedDecrement(&m_refCount);
    if (res == 0) {
        delete this;
    }
    return res;
}

// IHttpServer methods
HRESULT __stdcall HttpServer::StartServer(UINT16 port) {
    if (m_running) {
        Log("Server is already running.");
        return E_FAIL;
    }

    m_listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (m_listenSocket == INVALID_SOCKET) {
        int err = WSAGetLastError();
        Log("Socket creation failed with error: " + std::to_string(err));
        return E_FAIL;
    }

    sockaddr_in service{};
    service.sin_family = AF_INET;
    service.sin_addr.s_addr = INADDR_ANY;
    service.sin_port = htons(port);

    if (bind(m_listenSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        Log("Bind failed with error: " + std::to_string(err));
        closesocket(m_listenSocket);
        return E_FAIL;
    }

    if (listen(m_listenSocket, SOMAXCONN) == SOCKET_ERROR) {
        int err = WSAGetLastError();
        Log("Listen failed with error: " + std::to_string(err));
        closesocket(m_listenSocket);
        return E_FAIL;
    }

    Log("Server is listening on port " + std::to_string(port) + ".");

    m_running = true;
    m_serverThread = std::thread(&HttpServer::ServerLoop, this);
    return S_OK;
}

HRESULT __stdcall HttpServer::StopServer() {
    if (!m_running) {
        Log("Server is not running.");
        return E_FAIL;
    }

    m_running = false;
    closesocket(m_listenSocket);
    if (m_serverThread.joinable()) {
        m_serverThread.join();
    }
    return S_OK;
}

void HttpServer::ServerLoop() {
    while (m_running) {
        SOCKET clientSocket = accept(m_listenSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            int err = WSAGetLastError();
            Log("Accept failed with error: " + std::to_string(err));
            continue;
        }

        // Handle client in a separate thread
        std::thread clientThread(&HttpServer::HandleClient, this, clientSocket);
        clientThread.detach();
    }
}

void HttpServer::HandleClient(SOCKET clientSocket) {
    // Get client IP address
    sockaddr_storage clientInfo;
    int addrsize = sizeof(clientInfo);
    int result = getpeername(clientSocket, (sockaddr*)&clientInfo, &addrsize);
    if (result == SOCKET_ERROR) {
        int err = WSAGetLastError();
        Log("getpeername failed with error: " + std::to_string(err));
        closesocket(clientSocket);
        return;
    }

    char clientIP[NI_MAXHOST];
    result = getnameinfo((sockaddr*)&clientInfo, addrsize, clientIP, sizeof(clientIP), NULL, 0, NI_NUMERICHOST);
    if (result != 0) {
        int err = WSAGetLastError();
        Log("getnameinfo failed with error: " + std::to_string(err));
        closesocket(clientSocket);
        return;
    }

    Log("Client connected: " + std::string(clientIP));

    Log("Client connected: " + std::string(clientIP));

    const int bufferSize = 8192;
    char recvBuffer[bufferSize];
    int recvResult;

    std::string request;

    // Receive data until the end of the HTTP headers
    do {
        recvResult = recv(clientSocket, recvBuffer, bufferSize - 1, 0);
        if (recvResult > 0) {
            recvBuffer[recvResult] = '\0';
            request += recvBuffer;

            // Check if we have received all the headers
            if (request.find("\r\n\r\n") != std::string::npos) {
                break;
            }
        }
        else if (recvResult == 0) {
            // Connection closing
            break;
        }
        else {
            int err = WSAGetLastError();
            Log("Receive failed with error: " + std::to_string(err));
            closesocket(clientSocket);
            return;
        }
    } while (recvResult > 0);

    // Log the request
    Log("Received request from " + std::string(clientIP) + ":\n" + request);

    // Parse the request
    std::istringstream requestStream(request);
    std::string method;
    std::string uri;
    std::string httpVersion;

    requestStream >> method >> uri >> httpVersion;

    // For security, prevent accessing files outside the server directory
    if (uri.find("..") != std::string::npos) {
        // Send 403 Forbidden
        std::string response =
            "HTTP/1.1 403 Forbidden\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 9\r\n"
            "\r\n"
            "Forbidden";
        send(clientSocket, response.c_str(), (int)response.length(), 0);

        Log("Sent response to " + std::string(clientIP) + ":\n" + response);

        closesocket(clientSocket);
        return;
    }

    // Default to index.html if root is requested
    if (uri == "/") {
        uri = "/index.html";
    }

    std::string response;

    if (uri == "/getEmployee/") {
        // Generate dynamic content
        // Simulate random employee list
        std::string employeePage = R"(
<!DOCTYPE html>
<html>
<head>
    <title>Employee List</title>
    <style>
        body { font-family: Arial, sans-serif; }
        .employee { margin: 10px; padding: 10px; border: 1px solid #ccc; }
        .employee:hover { background-color: #f0f0f0; }
        .animation { animation: fadeIn 2s; }
        @keyframes fadeIn {
            from { opacity: 0; }
            to { opacity: 1; }
        }
    </style>
</head>
<body>
    <h1>Employee List</h1>
    <div id="employees"></div>
    <script>
        const employees = [
            { name: 'John Doe', position: 'Manager' },
            { name: 'Jane Smith', position: 'Developer' },
            { name: 'Alice Johnson', position: 'Designer' },
            { name: 'Bob Brown', position: 'Tester' },
            { name: 'Carol White', position: 'Support' }
        ];

        function shuffle(array) {
            array.sort(() => Math.random() - 0.5);
        }

        shuffle(employees);

        const container = document.getElementById('employees');
        employees.forEach(emp => {
            const div = document.createElement('div');
            div.className = 'employee animation';
            div.innerHTML = '<strong>' + emp.name + '</strong><br>' + emp.position;
            container.appendChild(div);
        });
    </script>
</body>
</html>
)";
        std::ostringstream oss;
        oss << "HTTP/1.1 200 OK\r\n"
            << "Content-Type: text/html\r\n"
            << "Content-Length: " << employeePage.length() << "\r\n"
            << "\r\n"
            << employeePage;
        response = oss.str();
    }
    else {
        // Get the executable's directory
        char exePath[MAX_PATH];
        GetModuleFileNameA(NULL, exePath, MAX_PATH);
        std::string exeDir(exePath);
        exeDir = exeDir.substr(0, exeDir.find_last_of("\\/"));

        // Build the file path
        std::string filePath = exeDir + uri;
        Log("Attempting to open file: " + filePath);

        // Try to open the requested file
        std::ifstream file(filePath, std::ios::in | std::ios::binary);
        if (file) {
            // Read file content
            std::ostringstream ss;
            ss << file.rdbuf();
            std::string fileContent = ss.str();
            file.close();

            // Get MIME type based on file extension
            std::string mimeType = GetMimeType(filePath);

            std::ostringstream oss;
            oss << "HTTP/1.1 200 OK\r\n"
                << "Content-Type: " << mimeType << "\r\n"
                << "Content-Length: " << fileContent.length() << "\r\n"
                << "\r\n"
                << fileContent;

            response = oss.str();
        }
        else {
            // File not found, send 404
            response =
                "HTTP/1.1 404 Not Found\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 9\r\n"
                "\r\n"
                "Not Found";
        }
    }

    // Send the response
    int sendResult = send(clientSocket, response.c_str(), (int)response.length(), 0);
    if (sendResult == SOCKET_ERROR) {
        int err = WSAGetLastError();
        Log("Send failed with error: " + std::to_string(err));
    }
    else {
        // Log the response
        Log("Sent response to " + std::string(clientIP) + ":\n" + response);
    }

    closesocket(clientSocket);
}

std::string HttpServer::GetMimeType(const std::string& path) {
    // Simple mapping of file extensions to MIME types
    static std::map<std::string, std::string> mimeTypes = {
        { ".html", "text/html" },
        { ".htm", "text/html" },
        { ".css", "text/css" },
        { ".js", "application/javascript" },
        { ".json", "application/json" },
        { ".png", "image/png" },
        { ".jpg", "image/jpeg" },
        { ".jpeg", "image/jpeg" },
        { ".gif", "image/gif" },
        { ".ico", "image/x-icon" },
        { ".svg", "image/svg+xml" },
        { ".txt", "text/plain" },
        { ".woff", "font/woff" },
        { ".woff2", "font/woff2" },
        { ".ttf", "font/ttf" },
        { ".eot", "application/vnd.ms-fontobject" },
        { ".otf", "font/otf" },
        { ".wasm", "application/wasm" },
        // Add more MIME types as needed
    };

    size_t dotPos = path.find_last_of(".");
    if (dotPos != std::string::npos) {
        std::string ext = path.substr(dotPos);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        auto it = mimeTypes.find(ext);
        if (it != mimeTypes.end()) {
            return it->second;
        }
    }
    return "application/octet-stream"; // Default binary type
}

void HttpServer::Log(const std::string& message) {
    std::lock_guard<std::mutex> guard(logMutex);
    // Get current time
    SYSTEMTIME time;
    GetLocalTime(&time);
    char timeStr[100];
    sprintf_s(timeStr, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
        time.wYear, time.wMonth, time.wDay,
        time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);

    // Output the timestamped message
    std::cout << "[" << timeStr << "] " << message << std::endl;
}
