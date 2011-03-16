// Copyright: 	Matra-Datavision 1992
// File:	ExprIntrp_yaccintrf.hxx
// Created:	Mon Feb 17 11:14:09 1992
// Author:	Arnaud BOUZY
//		<adn>


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



