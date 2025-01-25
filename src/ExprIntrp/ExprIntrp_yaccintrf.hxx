// Created on: 1992-02-17
// Created by: Arnaud BOUZY
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

#ifndef ExprIntrp_yaccintrf_HeaderFile
#define ExprIntrp_yaccintrf_HeaderFile

#ifdef __cplusplus
extern "C"
{
#endif

  int  ExprIntrpparse();
  void ExprIntrperror(char* msg);

  void ExprIntrp_start_string(const char* str);
  void ExprIntrp_stop_string();

  void ExprIntrp_SetResult();
  void ExprIntrp_SetDegree();

  int ExprIntrplex(void);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

  #include <TCollection_AsciiString.hxx>

const TCollection_AsciiString& ExprIntrp_GetResult();
int                            ExprIntrp_GetDegree();

#endif

#endif
