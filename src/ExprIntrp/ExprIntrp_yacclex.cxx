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

static char ExprIntrp_curres[255];
static int ExprIntrp_degree;

#ifndef WNT
extern char* ExprIntrptext;
#else
extern "C" char* ExprIntrptext;
#endif  // WNT


extern "C" void ExprIntrp_SetResult()
{
  strcpy(ExprIntrp_curres,ExprIntrptext);
}

extern "C" void ExprIntrp_SetDegree()
{
  ExprIntrp_degree = (int)strlen(ExprIntrptext);
}

int ExprIntrp_GetDegree()
{
  return ExprIntrp_degree;
}

int ExprIntrp_GetResult(char *s) 
{
  strcpy(s,ExprIntrp_curres);
  return (int)strlen(ExprIntrp_curres);
}
