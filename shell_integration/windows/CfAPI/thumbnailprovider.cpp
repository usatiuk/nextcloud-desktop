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

#include "thumbnailprovider.h"

#include <Shlguid.h>
#include <string>

inline void throw_if_fail(HRESULT hr)
{
    if (FAILED(hr))
    {
        throw _com_error(hr);
    }
}

void writeLog(const std::wstring& log) {
    HANDLE hFile = CreateFile(
        L"D:\\work\\RecipeThumbProvider.log.txt",     // Filename
        FILE_APPEND_DATA,          // Desired access
        FILE_SHARE_READ,        // Share mode
        NULL,                   // Security attributes
        OPEN_ALWAYS,             // Creates a new file, only if it doesn't already exist
        FILE_ATTRIBUTE_NORMAL,  // Flags and attributes
        NULL);                  // Template file handle

    // Write data to the file
    DWORD bytesWritten;
    WriteFile(
        hFile,            // Handle to the file
        log.c_str(),  // Buffer to write
        log.size(),   // Buffer size
        &bytesWritten,    // Bytes written
        nullptr);         // Overlapped

     // Close the handle once we don't need it.
    CloseHandle(hFile);
}

ThumbnailProvider::ThumbnailProvider()
    : _referenceCount(1)
{
}

IFACEMETHODIMP ThumbnailProvider::QueryInterface(REFIID riid, void **ppv)
{
    static const QITAB qit[] = {
        QITABENT(ThumbnailProvider, IInitializeWithItem),
        QITABENT(ThumbnailProvider, IThumbnailProvider),
        {0},
    };
    return QISearch(this, qit, riid, ppv);
}

IFACEMETHODIMP_(ULONG) ThumbnailProvider::AddRef()
{
    return InterlockedIncrement(&_referenceCount);
}

IFACEMETHODIMP_(ULONG) ThumbnailProvider::Release()
{
    ULONG cRef = InterlockedDecrement(&_referenceCount);
    if (!cRef) {
        delete this;
    }
    return cRef;
}

IFACEMETHODIMP ThumbnailProvider::Initialize(_In_ IShellItem *item, _In_ DWORD mode)
{
    // MessageBox(NULL, L"Attach to DLL", L"Attach Now", MB_OK);

    try {
        throw_if_fail((item->QueryInterface(__uuidof(_itemDest), reinterpret_cast<void **>(&_itemDest))));

        LPWSTR pszName = NULL;
        throw_if_fail(_itemDest->GetDisplayName(SIGDN_FILESYSPATH, &pszName));

        std::wstring sourceItem = L"D:\\work\\cloud-sample\\server\\bird-g272205618_640.jpg";


        IShellItem *pShellItem = 0;

        throw_if_fail((SHCreateItemFromParsingName(
            sourceItem.data(), NULL, __uuidof(_itemSrc), reinterpret_cast<void **>(&_itemSrc))));

        writeLog(
            std::wstring(L"ThumbnailProvider::Initialize: pszName") + std::wstring(pszName) + std::wstring(L" \n"));
    } catch (_com_error exc) {
        return exc.Error();
    }

    return S_OK;
}

// IThumbnailProvider
IFACEMETHODIMP ThumbnailProvider::GetThumbnail(_In_ UINT width, _Out_ HBITMAP *bitmap, _Out_ WTS_ALPHATYPE *alphaType)
{
    // MessageBox(NULL, L"Attach to DLL", L"Attach Now", MB_OK);
    // Open a handle to the file
    writeLog(L"ThumbnailProvider::GetThumbnail...\n");
    // Retrieve thumbnails of the placeholders on demand by delegating to the thumbnail of the source items.
    *bitmap = nullptr;
    *alphaType = WTSAT_UNKNOWN;

    try {
        IThumbnailProviderPtr thumbnailProviderSource;
        throw_if_fail(_itemSrc->BindToHandler(NULL, BHID_ThumbnailHandler, __uuidof(thumbnailProviderSource),
            reinterpret_cast<void **>(&thumbnailProviderSource)));
        throw_if_fail(thumbnailProviderSource->GetThumbnail(width, bitmap, alphaType));
    } catch (_com_error exc) {
        return exc.Error();
    }

    writeLog(L"ThumbnailProvider::GetThumbnail success!\n");
    return S_OK;
}