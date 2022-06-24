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

#include <new>

extern long dllReferenceCount;

HRESULT CfApiShellIntegrationClassFactory::CreateInstance(REFCLSID clsid, const CLASS_OBJECT_INIT *pClassObjectInits, size_t cClassObjectInits, REFIID riid, void **ppv)
{
    *ppv = NULL;
    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;
    for (size_t i = 0; i < cClassObjectInits; i++) {
        if (clsid == *pClassObjectInits[i].pClsid) {
            IClassFactory *pClassFactory = new (std::nothrow) CfApiShellIntegrationClassFactory(pClassObjectInits[i].pfnCreate);
            hr = pClassFactory ? S_OK : E_OUTOFMEMORY;
            if (SUCCEEDED(hr)) {
                hr = pClassFactory->QueryInterface(riid, ppv);
                pClassFactory->Release();
            }
            break; // match found
        }
    }
    return hr;
}

// IUnknown
IFACEMETHODIMP CfApiShellIntegrationClassFactory::QueryInterface(REFIID riid, void **ppv)
{
    HRESULT hResult = S_OK;

    if (IsEqualIID(IID_IUnknown, riid) || IsEqualIID(IID_IClassFactory, riid)) {
        *ppv = static_cast<IUnknown *>(this);
        AddRef();
    } else {
        hResult = E_NOINTERFACE;
        *ppv = nullptr;
    }

    return hResult;
}

IFACEMETHODIMP_(ULONG) CfApiShellIntegrationClassFactory::AddRef()
{
    return InterlockedIncrement(&_referenceCount);
}

IFACEMETHODIMP_(ULONG) CfApiShellIntegrationClassFactory::Release()
{
    long cRef = InterlockedDecrement(&_referenceCount);
    if (cRef == 0) {
        delete this;
    }
    return cRef;
}

// IClassFactory
IFACEMETHODIMP CfApiShellIntegrationClassFactory::CreateInstance(IUnknown *punkOuter, REFIID riid, void **ppv)
{
    return punkOuter ? CLASS_E_NOAGGREGATION : _pfnCreate(riid, ppv);
}

IFACEMETHODIMP CfApiShellIntegrationClassFactory::LockServer(BOOL fLock)
{
    if (fLock) {
        InterlockedIncrement(&dllReferenceCount);
    } else {
        InterlockedDecrement(&dllReferenceCount);
    }
    return S_OK;
}

CfApiShellIntegrationClassFactory::CfApiShellIntegrationClassFactory(PFNCREATEINSTANCE pfnCreate)
    : _referenceCount(1)
    , _pfnCreate(pfnCreate)
{
    InterlockedIncrement(&dllReferenceCount);
}

CfApiShellIntegrationClassFactory::~CfApiShellIntegrationClassFactory()
{
    InterlockedDecrement(&dllReferenceCount);
}