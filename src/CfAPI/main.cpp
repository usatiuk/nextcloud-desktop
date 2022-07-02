/*
 *
 * Copyright (C) by Duncan Mac-Vicar P. <duncan@kde.org>
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

#include <Unknwn.h>
#include <winrt/base.h>

#include "ShellServices.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    MSG msg = {0};
    HRESULT hr;
    DWORD dwRegister;
   
     do {
        hr = CoInitialize(NULL);
        if (FAILED(hr)) {
            MessageBoxA(NULL, "Error. CoInitialize Failed.", "Tips", MB_OK | MB_ICONERROR);
            break;
        }

        ShellServices::InitAndStartServiceTask();

        while (GetMessage(&msg, NULL, 0, 0)) {
            DispatchMessage(&msg);
        }

        hr = CoRevokeClassObject(dwRegister);
        if (FAILED(hr)) {
            MessageBoxA(NULL, "Error. CoRevokeClassObject Failed.", "Tips", MB_OK | MB_ICONERROR);
            break;
        }

    } while (false);

    return msg.wParam;
}
