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

namespace winrt {
using namespace winrt::Windows::Storage::Provider;
}

namespace winrt::CfApiShellExtensions::implementation {
Windows::Foundation::Collections::IIterable<Windows::Storage::Provider::StorageProviderItemProperty>
CustomStateProvider::GetItemProperties(hstring const &itemPath)
{
    std::hash<std::wstring> hashFunc;
    auto hash = hashFunc(itemPath.c_str());

    std::vector<winrt::StorageProviderItemProperty> properties;

    if ((hash & 0x1) != 0) {
        winrt::StorageProviderItemProperty itemProperty;
        itemProperty.Id(2);
        itemProperty.Value(L"Value2");
        // This icon is just for the sample. You should provide your own branded icon here
        itemProperty.IconResource(L"shell32.dll,-14");
        properties.push_back(std::move(itemProperty));
    }

    return winrt::single_threaded_vector(std::move(properties));
}
}