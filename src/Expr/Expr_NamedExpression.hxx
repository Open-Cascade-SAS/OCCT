// Created on: 1991-01-10
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

#ifndef _Expr_NamedExpression_HeaderFile
#define _Expr_NamedExpression_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TCollection_AsciiString.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Standard_Boolean.hxx>


class Expr_NamedExpression;
DEFINE_STANDARD_HANDLE(Expr_NamedExpression, Expr_GeneralExpression)

//! Describe an expression used  by its name (as constants
//! or variables). A single reference is made to a
//! NamedExpression in every Expression (i.e. a
//! NamedExpression is shared).
class Expr_NamedExpression : public Expr_GeneralExpression
{

public:

  
  Standard_EXPORT const TCollection_AsciiString& GetName() const;
  
  Standard_EXPORT void SetName (const TCollection_AsciiString& name);
  
  //! Tests if <me> can be shared by one or more expressions
  //! or must be copied. This method redefines to a True
  //! value the GeneralExpression method.
  Standard_EXPORT virtual Standard_Boolean IsShareable() const Standard_OVERRIDE;
  
  //! Tests if <me> and <Other> define the same expression.
  //! This method does not include any simplification before
  //! testing.
  Standard_EXPORT Standard_Boolean IsIdentical (const Handle(Expr_GeneralExpression)& Other) const Standard_OVERRIDE;
  
  //! returns a string representing <me> in a readable way.
  Standard_EXPORT TCollection_AsciiString String() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Expr_NamedExpression,Expr_GeneralExpression)

protected:




private:


  TCollection_AsciiString myName;


};







#endif // _Expr_NamedExpression_HeaderFile
