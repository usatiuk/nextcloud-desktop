// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
// PARTICULAR PURPOSE.
//
// Copyright (c) Microsoft Corporation. All rights reserved

#include <shlwapi.h>
#include <Wincrypt.h>   // For CryptStringToBinary.
#include <thumbcache.h> // For IThumbnailProvider.
#include <wincodec.h>   // Windows Imaging Codecs
#include <msxml6.h>
#include <Shlguid.h>
#include <new>
#include <string>
#include <comip.h>
#include <comdef.h>

#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "msxml6.lib")

_COM_SMARTPTR_TYPEDEF(IShellItem2, IID_IShellItem2);
_COM_SMARTPTR_TYPEDEF(IThumbnailProvider, IID_IThumbnailProvider);

// this thumbnail provider implements IInitializeWithStream to enable being hosted
// in an isolated process for robustness

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

class CRecipeThumbProvider : public IInitializeWithItem,
                             public IThumbnailProvider
{
public:
    CRecipeThumbProvider() : _cRef(1), _pStream(NULL)
    {
    }

    virtual ~CRecipeThumbProvider()
    {
        if (_pStream)
        {
            _pStream->Release();
        }
    }

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv)
    {
       
        static const QITAB qit[] =
        {
            QITABENT(CRecipeThumbProvider, IInitializeWithItem),
            QITABENT(CRecipeThumbProvider, IThumbnailProvider),
            { 0 },
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) AddRef()
    {
        return InterlockedIncrement(&_cRef);
    }

    IFACEMETHODIMP_(ULONG) Release()
    {
        ULONG cRef = InterlockedDecrement(&_cRef);
        if (!cRef)
        {
            delete this;
        }
        return cRef;
    }

    // IInitializeWithStream
    IFACEMETHODIMP Initialize(IStream *pStream, DWORD grfMode);

    IFACEMETHODIMP Initialize(_In_ IShellItem* item, _In_ DWORD mode);

    // IThumbnailProvider
    IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha);

private:
    HRESULT _LoadXMLDocument( IXMLDOMDocument **ppXMLDoc);
    HRESULT _GetBase64EncodedImageString(UINT cx, PWSTR *ppszResult);
    HRESULT _GetStreamFromString(PCWSTR pszImageName, IStream **ppStream);

    long _cRef;
    IStream *_pStream;     // provided during initialization.

    IShellItem2Ptr _itemDest;
    IShellItem2Ptr _itemSrc;
};

HRESULT CRecipeThumbProvider_CreateInstance(REFIID riid, void **ppv)
{
    CRecipeThumbProvider *pNew = new (std::nothrow) CRecipeThumbProvider();
    HRESULT hr = pNew ? S_OK : E_OUTOFMEMORY;
    if (SUCCEEDED(hr))
    {
        hr = pNew->QueryInterface(riid, ppv);
        pNew->Release();
    }
    return hr;
}

// IInitializeWithStream
IFACEMETHODIMP CRecipeThumbProvider::Initialize(IStream *pStream, DWORD)
{
    HRESULT hr = E_UNEXPECTED;  // can only be inited once
    if (_pStream == NULL)
    {
        // take a reference to the stream if we have not been inited yet
        hr = pStream->QueryInterface(&_pStream);
    }
    return hr;
}

IFACEMETHODIMP CRecipeThumbProvider::Initialize(_In_ IShellItem* item, _In_ DWORD mode)
{
    //MessageBox(NULL, L"Attach to DLL", L"Attach Now", MB_OK);

    try
    {
        throw_if_fail((item->QueryInterface(__uuidof(_itemDest), reinterpret_cast<void**>(&_itemDest))));

        LPWSTR pszName = NULL;
        throw_if_fail(_itemDest->GetDisplayName(SIGDN_FILESYSPATH, &pszName));

        std::wstring sourceItem = L"D:\\work\\cloud-sample\\server\\bird-g272205618_640.jpg";


        IShellItem* pShellItem = 0;

        throw_if_fail((SHCreateItemFromParsingName(sourceItem.data(), NULL, __uuidof(_itemSrc), reinterpret_cast<void**>(&_itemSrc))));

        writeLog(std::wstring(L"CRecipeThumbProvider::Initialize: pszName") + std::wstring(pszName) + std::wstring(L" \n"));
    }
    catch (_com_error exc)
    {
        return exc.Error();
    }

    return S_OK;
}

// IThumbnailProvider
IFACEMETHODIMP CRecipeThumbProvider::GetThumbnail(_In_ UINT width, _Out_ HBITMAP* bitmap, _Out_ WTS_ALPHATYPE* alphaType)
{
    //MessageBox(NULL, L"Attach to DLL", L"Attach Now", MB_OK);
    // Open a handle to the file
    writeLog(L"CRecipeThumbProvider::GetThumbnail...\n");
    // Retrieve thumbnails of the placeholders on demand by delegating to the thumbnail of the source items.
    *bitmap = nullptr;
    *alphaType = WTSAT_UNKNOWN;

    try
    {
        IThumbnailProviderPtr thumbnailProviderSource;
        throw_if_fail(_itemSrc->BindToHandler(NULL, BHID_ThumbnailHandler, __uuidof(thumbnailProviderSource), reinterpret_cast<void**>(&thumbnailProviderSource)));
        throw_if_fail(thumbnailProviderSource->GetThumbnail(width, bitmap, alphaType));
    }
    catch (_com_error exc)
    {
        return exc.Error();
    }

    writeLog(L"CRecipeThumbProvider::GetThumbnail success!\n");
    return S_OK;
}
