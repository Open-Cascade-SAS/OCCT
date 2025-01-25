// Created on: 1991-01-14
// Created by: Arnaud BOUZY
// Copyright (c) 1991-1999 Matra Datavision
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

#ifndef _Expr_InvalidOperand_HeaderFile
#define _Expr_InvalidOperand_HeaderFile

#include <Standard_Type.hxx>
#include <Standard_DefineException.hxx>
#include <Standard_SStream.hxx>
#include <Expr_ExprFailure.hxx>

class Expr_InvalidOperand;
DEFINE_STANDARD_HANDLE(Expr_InvalidOperand, Expr_ExprFailure)

#if !defined No_Exception && !defined No_Expr_InvalidOperand
  #define Expr_InvalidOperand_Raise_if(CONDITION, MESSAGE)                                         \
    if (CONDITION)                                                                                 \
      throw Expr_InvalidOperand(MESSAGE);
#else
  #define Expr_InvalidOperand_Raise_if(CONDITION, MESSAGE)
#endif

DEFINE_STANDARD_EXCEPTION(Expr_InvalidOperand, Expr_ExprFailure)

#endif // _Expr_InvalidOperand_HeaderFile
