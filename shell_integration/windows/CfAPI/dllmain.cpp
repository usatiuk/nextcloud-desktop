/*
 * Copyright (C) by Oleksandr Zolotov <alex@nextcloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#include "cfapishellintegrationclassfactory.h"
#include "thumbnailprovider.h"
#include "customstateprovider.h"
#include "contextmenus.h"
#include <winrt/windows.storage.provider.h>
#include <string>
#include <Unknwn.h>

extern HRESULT ThumbnailProvider_CreateInstance(REFIID riid, void **ppv);
extern HRESULT CustomStateProvider_CreateInstance(REFIID riid, void **ppv);
extern HRESULT TestExplorerCommandHandler_CreateInstance(REFIID riid, void **ppv);

static const CLSID SZ_CLSID_THUMBHANDLER = __uuidof(ThumbnailProvider); 
static const CLSID SZ_CLSID_CUSTOMSTATEPOVIDER = __uuidof(winrt::CfApiShellExtensions::implementation::CustomStateProvider);
static const CLSID SZ_CLSID_TESTEXPLORERCOMMANDHANDLER = __uuidof(TestExplorerCommandHandler); 

// add classes supported by this module here
const CLASS_OBJECT_INIT c_rgClassObjectInit[] = {
    {&SZ_CLSID_THUMBHANDLER, ThumbnailProvider_CreateInstance},
    {&SZ_CLSID_CUSTOMSTATEPOVIDER, CustomStateProvider_CreateInstance},
    {&SZ_CLSID_TESTEXPLORERCOMMANDHANDLER, TestExplorerCommandHandler_CreateInstance}
};

template <typename T>
class ClassFactory : public winrt::implements<ClassFactory<T>, IClassFactory>
{
public:
    // IClassFactory
    IFACEMETHODIMP CreateInstance(_In_opt_ IUnknown *unkOuter, REFIID riid, _COM_Outptr_ void **object)
    {
        try {
            auto provider = winrt::make<T>();
            winrt::com_ptr<IUnknown> unkn{provider.as<IUnknown>()};
            winrt::check_hresult(unkn->QueryInterface(riid, object));
            return S_OK;
        } catch (...) {
            // winrt::to_hresult() will eat the exception if it is a result of winrt::check_hresult,
            // otherwise the exception will get rethrown and this method will crash out as it should
            return winrt::to_hresult();
        }
    }
    IFACEMETHODIMP LockServer(BOOL lock) { return S_OK; }
};

long dllReferenceCount = 0;

// Handle the the DLL's module
HINSTANCE instanceHandle = NULL;

// Standard DLL functions
STDAPI_(BOOL) DllMain(HINSTANCE hInstance, DWORD dwReason, void *)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        instanceHandle = hInstance;
        DisableThreadLibraryCalls(hInstance);
    }

    std::string strText = "DllMain\n";
    writeLog(strText);

    return TRUE;
}

STDAPI DllCanUnloadNow()
{
    // Only allow the DLL to be unloaded after all outstanding references have been released
    std::string strText = "DllCanUnloadNow dllReferenceCount: " + std::to_string(dllReferenceCount) + std::string("\n");
    writeLog(strText);
    return (dllReferenceCount == 0) ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID clsid, REFIID riid, void **ppv)
{
    char clsidGuid[40] = {0};
    sprintf(clsidGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", clsid.Data1, clsid.Data2, clsid.Data3,
        clsid.Data4[0], clsid.Data4[1], clsid.Data4[2], clsid.Data4[3], clsid.Data4[4], clsid.Data4[5], clsid.Data4[6],
        clsid.Data4[7]);

    char riidGuid[40] = {0};
    sprintf(riidGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3,
        riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6],
        riid.Data4[7]);

    // Write data to the file
    std::string strText = "DllGetClassObject clsidGuid: " + std::string(clsidGuid) + std::string(" riidGuid: ")
        + std::string(riidGuid) + std::string("\n"); // For C use LPSTR (char*) or LPWSTR (wchar_t*)

    writeLog(strText);

    return CfApiShellIntegrationClassFactory::CreateInstance(clsid, c_rgClassObjectInit, ARRAYSIZE(c_rgClassObjectInit), riid, ppv);
}

HRESULT ThumbnailProvider_CreateInstance(REFIID riid, void **ppv)
{
    char riidGuid[40] = {0};
    sprintf(riidGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3,
        riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6],
        riid.Data4[7]);
    std::string strText = "ThumbnailProvider_CreateInstance 1 riidGuid: " + std::string(riidGuid) + std::string("\n");
    writeLog(strText);

    *ppv = NULL;

    ThumbnailProvider *thumbnailProvider;
    try {
        thumbnailProvider = new ThumbnailProvider();
        strText = "ThumbnailProvider_CreateInstance 2 riidGuid: " + std::string(riidGuid) + std::string("\n");
        writeLog(strText);
    } catch (...) {
        strText = "ThumbnailProvider_CreateInstance 3 riidGuid: " + std::string(riidGuid) + std::string("\n");
        writeLog(strText);
        return E_OUTOFMEMORY;
    }

    if (!thumbnailProvider) {
        strText = "ThumbnailProvider_CreateInstance 4 riidGuid: " + std::string(riidGuid) + std::string("\n");
        writeLog(strText);
        return E_OUTOFMEMORY;
    }

    HRESULT res = thumbnailProvider->QueryInterface(riid, ppv);
    if (res != S_OK) {
        strText = "ThumbnailProvider_CreateInstance 5 riidGuid: " + std::string(riidGuid) + std::string("\n");
        writeLog(strText);
        delete thumbnailProvider;
    }
    strText = "ThumbnailProvider_CreateInstance 6 riidGuid: " + std::string(riidGuid) + std::string("\n");
    writeLog(strText);
    return res;
}

HRESULT CustomStateProvider_CreateInstance(REFIID riid, void **ppv)
{
    char riidGuid[40] = {0};
    sprintf(riidGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3,
        riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6],
        riid.Data4[7]);
    std::string strText = "CustomStateProvider_CreateInstance 1 riidGuid: " + std::string(riidGuid) + std::string("\n");
    writeLog(strText);
    ppv = NULL;
    return E_OUTOFMEMORY;
    //MessageBox(NULL, L"Attach to DLL", L"CustomStateProvider_CreateInstance", MB_OK);
    HRESULT hr = S_OK;
    DWORD cookie;
    try {
        IUnknown *pUnk = (IUnknown *)0xdeadbeef;
        const IID &customStateProviderIID = __uuidof(winrt::CfApiShellExtensions::implementation::CustomStateProvider);
        HRESULT hr2 = CoGetClassObject(customStateProviderIID, CLSCTX_INPROC_SERVER, NULL, IID_IUnknown, (void **)&pUnk);

        IUnknown *pUnk1 = (IUnknown *)0xdeadbeef;
        HRESULT hr3 = CoGetClassObject(customStateProviderIID, CLSCTX_INPROC_SERVER, NULL,
            ABI::Windows::Storage::Provider::IID_IStorageProviderItemPropertySource, (void **)&pUnk1);

        hr2 = pUnk->QueryInterface(riid, ppv);

        hr2 = pUnk->QueryInterface(ABI::Windows::Storage::Provider::IID_IStorageProviderItemPropertySource, ppv);

        winrt::CfApiShellExtensions::implementation::CustomStateProvider *customStateProvider = nullptr;
        hr = CoCreateInstance(__uuidof(winrt::CfApiShellExtensions::implementation::CustomStateProvider), NULL,
            CLSCTX_INPROC_SERVER, riid, ppv);

        if (SUCCEEDED(hr)) {
            hr = customStateProvider->QueryInterface(riid, ppv);
            //customStateProvider->Release();
        }
        return hr;
    } 
    catch (_com_error exc) {
        return exc.Error();
    }
    
    return cookie;
}

HRESULT TestExplorerCommandHandler_CreateInstance(REFIID riid, void **ppv)
{
    char riidGuid[40] = {0};
    sprintf(riidGuid, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}", riid.Data1, riid.Data2, riid.Data3,
        riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4], riid.Data4[5], riid.Data4[6],
        riid.Data4[7]);
    std::string strText = "TestExplorerCommandHandler_CreateInstance 1 riidGuid: " + std::string(riidGuid) + std::string("\n");
    writeLog(strText);

    *ppv = NULL;

    TestExplorerCommandHandler *testExplorerCommandHandler;
    try {
        testExplorerCommandHandler = new TestExplorerCommandHandler();
        strText =
            "TestExplorerCommandHandler_CreateInstance 2 riidGuid: " + std::string(riidGuid) + std::string("\n");
        writeLog(strText);
    } catch (...) {
        strText =
            "TestExplorerCommandHandler_CreateInstance 3 riidGuid: " + std::string(riidGuid) + std::string("\n");
        writeLog(strText);
        return E_OUTOFMEMORY;
    }

    if (!testExplorerCommandHandler) {
        strText =
            "TestExplorerCommandHandler_CreateInstance 4 riidGuid: " + std::string(riidGuid) + std::string("\n");
        writeLog(strText);
        return E_OUTOFMEMORY;
    }

    HRESULT res = testExplorerCommandHandler->QueryInterface(riid, ppv);
    if (res != S_OK) {
        strText =
            "TestExplorerCommandHandler_CreateInstance 5 riidGuid: " + std::string(riidGuid) + std::string("\n");
        writeLog(strText);
        delete testExplorerCommandHandler;
    }
    strText =
        "TestExplorerCommandHandler_CreateInstance 6 riidGuid: " + std::string(riidGuid) + std::string("\n");
    writeLog(strText);
    return res;
}