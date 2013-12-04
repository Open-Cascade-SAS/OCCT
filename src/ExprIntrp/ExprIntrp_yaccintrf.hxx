// Created on: 1992-02-17
// Created by: Arnaud BOUZY
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and / or modify it
// under the terms of the GNU Lesser General Public version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#ifndef ExprIntrp_yaccintrf_HeaderFile
#define ExprIntrp_yaccintrf_HeaderFile

#include <TCollection_AsciiString.hxx>

extern "C" int ExprIntrpparse();
extern "C" void  ExprIntrp_start_string(const char* str);
extern "C" void  ExprIntrp_stop_string();
extern TCollection_AsciiString ExprIntrp_thestring;
extern Standard_Integer ExprIntrp_thecurchar;
int ExprIntrp_GetDegree();
int ExprIntrp_GetResult(char *s);

#endif



