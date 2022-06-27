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

IFACEMETHODIMP CustomStateProvider::QueryInterface(REFIID riid, void **ppv)
{
    static const QITAB qit[] = {
        QITABENT(CustomStateProvider, GetGuid("8f6f9c3e-f632-4a9b-8d99-d2d7a11df56a")),
        {0},
    };
    return QISearch(this, qit, riid, ppv);
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