// Created on: 1992-04-07
// Created by: Remi LEQUETTE
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef Draw_Appli_HeaderFile
#define Draw_Appli_HeaderFile

#include <Draw_Viewer.hxx>
#include <Draw.hxx>

typedef void (*FDraw_InitAppli)(Draw_Interpretor&);

#ifdef _WIN32
  #include <windows.h>
Standard_EXPORT void Draw_Appli(HINSTANCE,
                                HINSTANCE,
                                int,
                                int                   argc,
                                wchar_t**             argv,
                                const FDraw_InitAppli Draw_InitAppli);
#else
extern void Draw_Appli(int argc, char** argv, const FDraw_InitAppli Draw_InitAppli);
#endif

#ifndef _WIN32
extern Draw_Viewer      dout;
extern Standard_Boolean Draw_Batch;
#endif

#endif
