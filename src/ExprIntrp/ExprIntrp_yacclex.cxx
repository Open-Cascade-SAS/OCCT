// Created on: 1993-12-14
// Created by: Arnaud BOUZY
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
  ExprIntrp_degree = strlen(ExprIntrptext);
}

int ExprIntrp_GetDegree()
{
  return ExprIntrp_degree;
}

int ExprIntrp_GetResult(char *s) 
{
  strcpy(s,ExprIntrp_curres);
  return strlen(ExprIntrp_curres);
}
