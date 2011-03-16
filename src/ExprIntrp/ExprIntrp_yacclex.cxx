//static const char* sccsid = "@(#)ExprIntrp_yacclex.cxx	3.2 95/01/10"; // Do not delete this line. Used by sccs.
// Copyright: 	Matra-Datavision 1993
// File:	ExprIntrp_yacclex.cxx
// Created:	Tue Dec 14 10:40:17 1993
// Author:	Arnaud BOUZY
//		<adn>

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
