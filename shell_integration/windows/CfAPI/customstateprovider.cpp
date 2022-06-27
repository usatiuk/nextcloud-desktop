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

#include "customstateprovider.h"

#include <Shlguid.h>
#include <string>
#include <locale>
#include <codecvt>

#pragma push_macro("MIDL_CONST_ID")
#if !defined(_MSC_VER) || (_MSC_VER >= 1910)
#define MIDL_CONST_ID constexpr const
#else
#define MIDL_CONST_ID const __declspec(selectany)
#endif

MIDL_CONST_ID IID &IID_IStorageProviderItemPropertySource = {};

IFACEMETHODIMP CustomStateProvider::QueryInterface(REFIID riid, void **ppv)
{
    MessageBox(NULL, L"Attach to DLL", L"CustomStateProvider::QueryInterface", MB_OK);
    /*static const QITAB qit[] = {
        QITABENT(CustomStateProvider, IID_IStorageProviderItemPropertySource),
        {0},
    };*/
    return 1;
}

IFACEMETHODIMP_(ULONG) CustomStateProvider::AddRef()
{
    return InterlockedIncrement(&_referenceCount);
}

IFACEMETHODIMP_(ULONG) CustomStateProvider::Release()
{
    ULONG cRef = InterlockedDecrement(&_referenceCount);
    if (!cRef) {
        delete this;
    }
    return cRef;
}

winrt::Windows::Foundation::Collections::IIterable<winrt::Windows::Storage::Provider::StorageProviderItemProperty>
CustomStateProvider::GetItemProperties(_In_ winrt::hstring const &itemPath)
{
    std::hash<std::wstring> hashFunc;
    auto hash = hashFunc(itemPath.c_str());

    std::vector<winrt::Windows::Storage::Provider::StorageProviderItemProperty> properties;

    if ((hash & 0x1) != 0) {
        winrt::Windows::Storage::Provider::StorageProviderItemProperty itemProperty;
        itemProperty.Id(2);
        itemProperty.Value(L"Value2");
        // This icon is just for the sample. You should provide your own branded icon here
        itemProperty.IconResource(L"shell32.dll,-14");
        properties.push_back(std::move(itemProperty));
    }

    return winrt::single_threaded_vector(std::move(properties));
}