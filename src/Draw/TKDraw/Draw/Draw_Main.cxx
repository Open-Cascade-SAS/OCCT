// Created on: 1999-12-30
// Created by: Roman LYGIN
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

#ifdef _WIN32
  #include <windows.h>
#endif

#include <Draw_Main.hxx>
#include <stdlib.h>
#include <Draw_Appli.hxx>
#include <TCollection_AsciiString.hxx>

#include <tcl.h>

#ifdef _WIN32
  #include <sys/stat.h>

extern Draw_Viewer dout;

// extern Standard_IMPORT Standard_Boolean Draw_Interprete(char* command); //for C21
Standard_IMPORT Standard_Boolean Draw_Interprete(const char* command); // for C30
// true if complete command

// necessary for WNT in C21 only
static FDraw_InitAppli theDraw_InitAppli; // pointer to the Draw_InitAppli
#endif

#ifdef _WIN32

//=======================================================================
// NOTE: OCC11
//     On Windows NT, both console (UNIX-like) and windowed (classical on
//     WNT, with three separated windows - input, output and graphic)
//     modes are supported.
//     Depending on compilation mode of executable (CONSOLE or WINDOWS),
//     either Draw_Main or Draw_WinMain becomes entry point;
//     the further different behaviour of DRAW is determined by variable
//     Draw_IsConsoleSubsystem which is set by Draw_Main only
//=======================================================================

extern Standard_Boolean Draw_IsConsoleSubsystem;

//=================================================================================================

Standard_Integer Draw_Main(int /*argc*/, char* argv[], const FDraw_InitAppli fDraw_InitAppli)
{
  Draw_IsConsoleSubsystem = Standard_True;
  theDraw_InitAppli       = fDraw_InitAppli;

  // Set console code page to UTF-8 so that input from cin and output to cout
  // pass Unicode symbols as expected
  SetConsoleCP(CP_UTF8);
  SetConsoleOutputCP(CP_UTF8);

  // MKV 01.02.05
  #if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4)))
  Tcl_FindExecutable(argv[0]);
  #endif

  int       aNbArgs  = 0;
  wchar_t** anArgVec = CommandLineToArgvW(GetCommandLineW(), &aNbArgs);
  Draw_Appli(::GetModuleHandleW(NULL), NULL, SW_SHOW, aNbArgs, anArgVec, fDraw_InitAppli);
  LocalFree(anArgVec);
  return 0;
}

//=================================================================================================

Standard_Integer Draw_WinMain(HINSTANCE hInstance,
                              HINSTANCE hPrevinstance,
                              LPSTR /*lpCmdLine*/,
                              int                   nCmdShow,
                              const FDraw_InitAppli fDraw_InitAppli)
{
  theDraw_InitAppli  = fDraw_InitAppli;
  int       aNbArgs  = 0;
  wchar_t** anArgVec = CommandLineToArgvW(GetCommandLineW(), &aNbArgs);
  Draw_Appli(hInstance, hPrevinstance, nCmdShow, aNbArgs, anArgVec, fDraw_InitAppli);
  LocalFree(anArgVec);
  return 0;
}

#else

//=================================================================================================

Standard_Integer Draw_Main(Standard_Integer      argc,
                           char*                 argv[],
                           const FDraw_InitAppli fDraw_InitAppli)
{
  // MKV 01.02.05
  #if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4)))
  Tcl_FindExecutable(argv[0]);
  #endif
  Draw_Appli(argc, argv, fDraw_InitAppli);
  return 0;
}

#endif
