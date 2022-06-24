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

#include <string>

extern HRESULT ThumbnailProvider_CreateInstance(REFIID riid, void **ppv);

static const CLSID SZ_CLSID_THUMBHANDLER = __uuidof(ThumbnailProvider); 

// add classes supported by this module here
const CLASS_OBJECT_INIT c_rgClassObjectInit[] = {{&SZ_CLSID_THUMBHANDLER, ThumbnailProvider_CreateInstance}};


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

    // Open a handle to the file
    HANDLE hFile = CreateFile(L"D:\\work\\RecipeThumbProvider.log.txt", // Filename
        FILE_APPEND_DATA, // Desired access
        FILE_SHARE_READ, // Share mode
        NULL, // Security attributes
        OPEN_ALWAYS, // Creates a new file, only if it doesn't already exist
        FILE_ATTRIBUTE_NORMAL, // Flags and attributes
        NULL); // Template file handle

    // Write data to the file
    std::string strText = "DllMain\n"; // For C use LPSTR (char*) or LPWSTR (wchar_t*)
    DWORD bytesWritten;
    WriteFile(hFile, // Handle to the file
        strText.c_str(), // Buffer to write
        strText.size(), // Buffer size
        &bytesWritten, // Bytes written
        nullptr); // Overlapped

    // Close the handle once we don't need it.
    CloseHandle(hFile);

    return TRUE;
}

STDAPI DllCanUnloadNow()
{
    // Only allow the DLL to be unloaded after all outstanding references have been released
    return (dllReferenceCount == 0) ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(REFCLSID clsid, REFIID riid, void **ppv)
{
    return CfApiShellIntegrationClassFactory::CreateInstance(clsid, c_rgClassObjectInit, ARRAYSIZE(c_rgClassObjectInit), riid, ppv);
}

HRESULT ThumbnailProvider_CreateInstance(REFIID riid, void **ppv)
{
    ThumbnailProvider *thumbnailProvider = new (std::nothrow) ThumbnailProvider();
    HRESULT hr = thumbnailProvider ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr)) {
        hr = thumbnailProvider->QueryInterface(riid, ppv);
        thumbnailProvider->Release();
    }
    return hr;
}