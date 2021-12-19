// Created on: 1991-07-18
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

#ifndef _ExprIntrp_HeaderFile
#define _ExprIntrp_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
class ExprIntrp_Generator;
class TCollection_AsciiString;


//! Describes an interpreter for GeneralExpressions,
//! GeneralFunctions, and GeneralRelations defined in
//! package Expr.
class ExprIntrp 
{
public:

  DEFINE_STANDARD_ALLOC

private:
  
  Standard_EXPORT static Standard_Boolean Parse (const Handle(ExprIntrp_Generator)& gen, const TCollection_AsciiString& str);

private:

  friend class ExprIntrp_GenExp;
  friend class ExprIntrp_GenFct;
  friend class ExprIntrp_GenRel;

};

#endif // _ExprIntrp_HeaderFile
