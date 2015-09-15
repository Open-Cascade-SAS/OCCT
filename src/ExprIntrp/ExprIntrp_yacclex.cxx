// Created on: 1993-12-14
// Created by: Arnaud BOUZY
// Copyright (c) 1993-1999 Matra Datavision
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

#include <string.h>
#include <ExprIntrp_yaccintrf.hxx>

static TCollection_AsciiString ExprIntrp_curres;
static int ExprIntrp_degree;

#ifndef _WIN32
extern char* ExprIntrptext;
#else
extern "C" char* ExprIntrptext;
#endif  // _WIN32


extern "C" void ExprIntrp_SetResult()
{
  ExprIntrp_curres = ExprIntrptext;
}

extern "C" void ExprIntrp_SetDegree()
{
  ExprIntrp_degree = (int)strlen(ExprIntrptext);
}

int ExprIntrp_GetDegree()
{
  return ExprIntrp_degree;
}

const TCollection_AsciiString& ExprIntrp_GetResult ()
{
  return ExprIntrp_curres;
}
