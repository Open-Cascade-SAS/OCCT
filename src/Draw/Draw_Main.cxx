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

// *******************************************************************
//    RLN  06 January 2000
// *******************************************************************
// This file is built from former NTMain.pxx duplicated in many executables
// extending DRAW Test Harness.
// Now is implemented only in one instance.

// Differences between CAS.CADE versions:
//  - in C21 Draw_Appli is defined in Draw.cxx (DRAW UL) and calls externally
// defined Draw_InitAppli. Moreover, on WNT Draw_Appli was not declared as 
// Standard_EXPORT and therefore it has to be duplicated explicitly in this
// code as in Draw.cxx.
//  - in C30 Draw_Appli accepts Draw_InitAppli as parameter which is given to
// it in each executable. Draw_Appli is declared as Standard_EXPORT and
// therefore it needs not to be duplicated.

// To have only one instance of this file and to call it from all the executables
// thereare defined macros in .hxx that replace main/WinMain functions and which
// calls _main_/_WinMain_ defined in this file with specified Draw_InitAppli.
// To avoid Unresolved symbols on WNT, Draw_InitAppli is explicitly defined in this
// is initialized by main/_WinMain_.  

// WARNING: Although versions C21 and C30 are synchronised as much as they can,
// there are two versions of this file for both configurations. This is explained by:
// - Standard_IMPOR is differently defined,
// - Draw_Appli is differently declared.

#ifdef _WIN32
#include <windows.h> /* SW_SHOW */
#endif

#include <Draw_Main.hxx>
#include <stdlib.h>
#include <string.h>
#include <Draw_Appli.hxx>
#include <OSD.hxx>

#include <tcl.h>

#ifdef _WIN32
#include <sys/stat.h>
#include <Draw_Window.hxx>

#include <stdio.h>
#include <io.h>
#include <fcntl.h>

extern Draw_Viewer dout;

static char* ColorNames[MAXCOLOR] = {
  "White","Red","Green","Blue","Cyan","Gold","Magenta",
  "Maroon","Orange","Pink","Salmon","Violet","Yellow","Khaki","Coral"
  };

static   Standard_Boolean XLoop;

// extern Standard_IMPORT Standard_Boolean Draw_Interprete(char* command); //for C21
Standard_IMPORT Standard_Boolean Draw_Interprete(const char* command); //for C30
// true if complete command

// necessary for WNT in C21 only
static FDraw_InitAppli theDraw_InitAppli; //pointer to the Draw_InitAppli
#endif

#ifdef _WIN32
//=======================================================================
//NOTE: OCC11
//     On Windows NT, both console (UNIX-like) and windowed (classical on 
//     WNT, with three separated windows - input, output and graphic)
//     modes are supported.
//     Depending on compilation mode of executable (CONSOLE or WINDOWS),
//     either _main_ or _WinMain_ becomes entry point;
//     the further different behaviour of DRAW is determined by variable 
//     Draw_IsConsoleSubsystem which is set by _main_ only
//=======================================================================

  
extern Standard_Boolean Draw_IsConsoleSubsystem;

//=======================================================================
//function : _main_
//purpose  : 
//=======================================================================

Standard_Integer _main_ (int /*argc*/, char* argv[], char* /*envp*/[], const FDraw_InitAppli fDraw_InitAppli)
{
  Draw_IsConsoleSubsystem = Standard_True;
  //return _WinMain_(::GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOW, fDraw_InitAppli);
  theDraw_InitAppli = fDraw_InitAppli;
  //ParseCommandLine(GetCommandLine());

  // MKV 01.02.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4)))
  Tcl_FindExecutable(argv[0]);
#endif

  Draw_Appli(::GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOW, fDraw_InitAppli);
  return 0;

}

//=======================================================================
//function : _WinMain_
//purpose  : 
//=======================================================================

Standard_Integer _WinMain_ (HINSTANCE /*hInstance*/, HINSTANCE /*hPrevinstance*/, LPSTR /*lpCmdLine*/, int /*nCmdShow*/, const FDraw_InitAppli /*fDraw_InitAppli*/)
{
//  theDraw_InitAppli = fDraw_InitAppli;
//  ParseCommandLine (lpCmdLine);
//  Draw_Appli(hInstance, hPrevinstance, lpCmdLine, nCmdShow, Draw_InitAppli); // for C30;
  return 0;
}
#else
//=======================================================================
//function : _main_
//purpose  : 
//=======================================================================

Standard_Integer _main_ (Standard_Integer argc, char* argv[], const FDraw_InitAppli fDraw_InitAppli)
{
  // MKV 01.02.05
#if ((TCL_MAJOR_VERSION > 8) || ((TCL_MAJOR_VERSION == 8) && (TCL_MINOR_VERSION >= 4)))
  Tcl_FindExecutable(argv[0]);
#endif
  Draw_Appli(argc, argv, fDraw_InitAppli);
  return 0;
}
#endif

