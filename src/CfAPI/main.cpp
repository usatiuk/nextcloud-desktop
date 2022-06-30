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

#include <QtCore/QCoreApplication>

int main(int argc, char **argv)
{
    winrt::init_apartment();

    ShellServices::InitAndStartServiceTask();

    QCoreApplication a(argc, argv);
    return a.exec();
}
