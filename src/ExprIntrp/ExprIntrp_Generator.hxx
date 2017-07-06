// Created on: 1992-07-20
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

#ifndef _ExprIntrp_Generator_HeaderFile
#define _ExprIntrp_Generator_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <ExprIntrp_SequenceOfNamedFunction.hxx>
#include <ExprIntrp_SequenceOfNamedExpression.hxx>
#include <Standard_Transient.hxx>
class Expr_NamedFunction;
class Expr_NamedExpression;
class TCollection_AsciiString;


class ExprIntrp_Generator;
DEFINE_STANDARD_HANDLE(ExprIntrp_Generator, Standard_Transient)

//! Implements general services for interpretation of
//! expressions.
class ExprIntrp_Generator : public Standard_Transient
{

public:

  
  Standard_EXPORT void Use (const Handle(Expr_NamedFunction)& func);
  
  Standard_EXPORT void Use (const Handle(Expr_NamedExpression)& named);
  
  Standard_EXPORT const ExprIntrp_SequenceOfNamedExpression& GetNamed() const;
  
  Standard_EXPORT const ExprIntrp_SequenceOfNamedFunction& GetFunctions() const;
  
  //! Returns NamedExpression with name <name> already
  //! interpreted if it exists. Returns a null handle if
  //! not.
  Standard_EXPORT Handle(Expr_NamedExpression) GetNamed (const TCollection_AsciiString& name) const;
  
  //! Returns NamedFunction with name <name> already
  //! interpreted if it exists. Returns a null handle if
  //! not.
  Standard_EXPORT Handle(Expr_NamedFunction) GetFunction (const TCollection_AsciiString& name) const;




  DEFINE_STANDARD_RTTIEXT(ExprIntrp_Generator,Standard_Transient)

protected:

  
  Standard_EXPORT ExprIntrp_Generator();



private:


  ExprIntrp_SequenceOfNamedFunction myFunctions;
  ExprIntrp_SequenceOfNamedExpression myNamed;


};







#endif // _ExprIntrp_Generator_HeaderFile
