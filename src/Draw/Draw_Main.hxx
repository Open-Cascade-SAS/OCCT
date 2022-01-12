// Created on: 1999-12-30
// Created by: data exchange team
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Defines common framework for declaration of main/WinMain functions 
// for executables on UNIX and WNT that extends DRAW Test Harness.

// In order to create executable in DRAW environment, in the executable
// the following line should be added:
// DRAW_MAIN

#ifndef Draw_Main_HeaderFile
#define Draw_Main_HeaderFile

#include <Draw_Appli.hxx>
#include <Standard_PCharacter.hxx>

typedef void (*FDraw_InitAppli)(Draw_Interpretor&);

Standard_EXPORT Standard_Integer Draw_Main (Standard_Integer argc, Standard_PCharacter argv[], const FDraw_InitAppli Draw_InitAppli);

// Declarations of macros DRAW_MAIN to be used in executables instead of explicit main/WinMain
#ifdef _WIN32

#include <windows.h>

Standard_EXPORT Standard_Integer Draw_WinMain (HINSTANCE hInstance,
                                               HINSTANCE hPrevinstance, 
                                               LPSTR lpCmdLine,
                                               Standard_Integer nCmdShow,
					       const FDraw_InitAppli Draw_InitAppli);

// WinMain() and main()
#define DRAW_MAIN Standard_Integer PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevinstance, LPSTR lpCmdLine, Standard_Integer nCmdShow) \
{ return Draw_WinMain (hInstance, hPrevinstance, lpCmdLine, nCmdShow, Draw_InitAppli); } \
\
int main (int argc, char* argv[]) \
{ return Draw_Main (argc, argv, Draw_InitAppli); }

#else

// main()
#define DRAW_MAIN int main (Standard_Integer argc, char* argv[]) \
{ return Draw_Main (argc, argv, Draw_InitAppli); }

#endif

#endif
