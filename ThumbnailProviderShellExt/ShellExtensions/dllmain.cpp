// dllmain.cpp : Defines the entry point for the DLL application.
#include <wrl/module.h>
#include <wrl/implements.h>
#include <wrl/client.h>
#include <shobjidl_core.h>
#include <wil\resource.h>
#include <string>
#include <vector>
#include <sstream>
#include <thumbcache.h>
#include <shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

using namespace Microsoft::WRL;

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    // Open a handle to the file
    HANDLE hFile = CreateFile(L"D:\\work\\ShellExtensions.log.txt", // Filename
        FILE_APPEND_DATA, // Desired access
        FILE_SHARE_READ, // Share mode
        NULL, // Security attributes
        OPEN_ALWAYS, // Creates a new file, only if it doesn't already exist
        FILE_ATTRIBUTE_NORMAL, // Flags and attributes
        NULL); // Template file handle

    // Write data to the file
    std::string strText = "ShellExtensionThumbnailProvider::DllMain\n";
    DWORD bytesWritten;
    WriteFile(hFile, // Handle to the file
        strText.c_str(), // Buffer to write
        strText.size(), // Buffer size
        &bytesWritten, // Bytes written
        nullptr); // Overlapped

    // Close the handle once we don't need it.
    CloseHandle(hFile);

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

class __declspec(uuid("70F03B6A-EA1C-401E-9D8D-614C57938740")) ShellExtensionThumbnailProvider final
    : public RuntimeClass<RuntimeClassFlags<ClassicCom>, IInitializeWithStream, IThumbnailProvider>
{
public:
    ShellExtensionThumbnailProvider()
        : _cRef(1)
        , _pStream(NULL)
    {
    }

    virtual ~ShellExtensionThumbnailProvider()
    {
        if (_pStream) {
            _pStream->Release();
        }
    }

    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv)
    {
        static const QITAB qit[] = {
            QITABENT(ShellExtensionThumbnailProvider, IInitializeWithStream),
            QITABENT(ShellExtensionThumbnailProvider, IThumbnailProvider),
            {0},
        };
        return QISearch(this, qit, riid, ppv);
    }

    IFACEMETHODIMP_(ULONG) AddRef() { return InterlockedIncrement(&_cRef); }

    IFACEMETHODIMP_(ULONG) Release()
    {
        ULONG cRef = InterlockedDecrement(&_cRef);
        if (!cRef) {
            delete this;
        }
        return cRef;
    }

    // IInitializeWithStream
    IFACEMETHODIMP Initialize(IStream *pStream, DWORD grfMode)
    {
        HRESULT hr = E_UNEXPECTED; // can only be inited once
        if (_pStream == NULL) {
            // take a reference to the stream if we have not been inited yet
            hr = pStream->QueryInterface(&_pStream);
        }
        return hr;
    }

    // IThumbnailProvider
    IFACEMETHODIMP GetThumbnail(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha)
    {
        // Open a handle to the file
        HANDLE hFile = CreateFile(L"D:\\work\\ShellExtensions.log.txt", // Filename
            FILE_APPEND_DATA, // Desired access
            FILE_SHARE_READ, // Share mode
            NULL, // Security attributes
            OPEN_ALWAYS, // Creates a new file, only if it doesn't already exist
            FILE_ATTRIBUTE_NORMAL, // Flags and attributes
            NULL); // Template file handle

        // Write data to the file
        std::string strText = "ShellExtensionThumbnailProvider::GetThumbnail\n"; 
        DWORD bytesWritten;
        WriteFile(hFile, // Handle to the file
            strText.c_str(), // Buffer to write
            strText.size(), // Buffer size
            &bytesWritten, // Bytes written
            nullptr); // Overlapped

        // Close the handle once we don't need it.
        CloseHandle(hFile);

        BOOL br = FALSE;
        HDC hdc = ::GetDC(NULL);
        HDC hDrawDC = CreateCompatibleDC(hdc);
        if (hDrawDC != NULL) {
            void *bits = 0;
            RECT rcBounds;
            SetRect(&rcBounds, 0, 0, cx, cx);

            BITMAPINFO bi = {0};
            bi.bmiHeader.biWidth = cx;
            bi.bmiHeader.biHeight = cx;
            bi.bmiHeader.biPlanes = 1;
            bi.bmiHeader.biBitCount = 32;
            bi.bmiHeader.biSizeImage = 0;
            bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bi.bmiHeader.biClrUsed = 0;
            bi.bmiHeader.biClrImportant = 0;

            HBITMAP hBmp = CreateDIBSection(hdc, &bi, DIB_RGB_COLORS, &bits, NULL, 0);
            if (hBmp != NULL) {
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDrawDC, hBmp);
                HBRUSH hDrawBrush = CreateSolidBrush(RGB(255, 0, 255)); // pink
                FillRect(hDrawDC, &rcBounds, hDrawBrush);
                SelectObject(hDrawDC, hOldBitmap);
                *phbmp = hBmp;
                br = TRUE;
            }
            DeleteDC(hDrawDC);
        }

        ReleaseDC(NULL, hdc);
        return S_OK;
    }

private:
    long _cRef;
    IStream *_pStream; // provided during initialization.
};

CoCreatableClass(ShellExtensionThumbnailProvider)

CoCreatableClassWrlCreatorMapInclude(ShellExtensionThumbnailProvider)


STDAPI DllGetActivationFactory(_In_ HSTRING activatableClassId, _COM_Outptr_ IActivationFactory** factory)
{
    return Module<ModuleType::InProc>::GetModule().GetActivationFactory(activatableClassId, factory);
}

STDAPI DllCanUnloadNow()
{
    return Module<InProc>::GetModule().GetObjectCount() == 0 ? S_OK : S_FALSE;
}

STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _COM_Outptr_ void** instance)
{
    return Module<InProc>::GetModule().GetClassObject(rclsid, riid, instance);
}