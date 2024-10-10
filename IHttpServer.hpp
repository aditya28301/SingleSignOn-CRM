// IHttpServer.hpp
#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>
#include <objbase.h>

// {E0C0FFE0-7EEA-4EE5-8E4A-B7F6B6BDBBEC}
static const IID IID_IHttpServer =
{ 0xe0c0ffe0, 0x7eea, 0x4ee5, { 0x8e, 0x4a, 0xb7, 0xf6, 0xb6, 0xbd, 0xbb, 0xec } };

class IHttpServer : public IUnknown {
public:
    virtual HRESULT __stdcall StartServer(UINT16 port) = 0;
    virtual HRESULT __stdcall StopServer() = 0;
};
