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

#pragma once

#include <shlwapi.h>
#include <thumbcache.h>
#include <comdef.h>
#include <unknwn.h>
#include "WinShellExtConstants.h"
#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/windows.foundation.collections.h>
#include <winrt/windows.storage.provider.h>

class __declspec(uuid(APPX_MANIFEST_CUSTOM_STATE_HANDLER_CLASS_ID)) CustomStateProvider
    : public winrt::implements<CustomStateProvider,
          winrt::Windows::Storage::Provider::IStorageProviderItemPropertySource>
{
public:
    CustomStateProvider() = default;
    virtual ~CustomStateProvider() = default;
    winrt::Windows::Foundation::Collections::IIterable<winrt::Windows::Storage::Provider::StorageProviderItemProperty>
    GetItemProperties(_In_ winrt::hstring const &itemPath);

    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);

    IFACEMETHODIMP_(ULONG) AddRef();

    IFACEMETHODIMP_(ULONG) Release();

private:
    long _referenceCount;
};