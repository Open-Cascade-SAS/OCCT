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

#ifndef _ExprIntrp_GenExp_HeaderFile
#define _ExprIntrp_GenExp_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <ExprIntrp_Generator.hxx>
class Expr_GeneralExpression;
class TCollection_AsciiString;


class ExprIntrp_GenExp;
DEFINE_STANDARD_HANDLE(ExprIntrp_GenExp, ExprIntrp_Generator)

//! This class permits, from a string, to create any
//! kind of expression of package Expr by using
//! built-in functions such as Sin,Cos, etc, and by
//! creating variables.
class ExprIntrp_GenExp : public ExprIntrp_Generator
{

public:

  
  Standard_EXPORT static Handle(ExprIntrp_GenExp) Create();
  
  //! Processes given string.
  Standard_EXPORT void Process (const TCollection_AsciiString& str);
  
  //! Returns false if any syntax error has occurred during
  //! process.
  Standard_EXPORT Standard_Boolean IsDone() const;
  
  //! Returns expression generated. Raises an exception if
  //! IsDone answers false.
  Standard_EXPORT Handle(Expr_GeneralExpression) Expression() const;




  DEFINE_STANDARD_RTTIEXT(ExprIntrp_GenExp,ExprIntrp_Generator)

protected:




private:

  
  //! Creates an empty generator
  Standard_EXPORT ExprIntrp_GenExp();

  Standard_Boolean done;
  Handle(Expr_GeneralExpression) myExpression;


};







#endif // _ExprIntrp_GenExp_HeaderFile
