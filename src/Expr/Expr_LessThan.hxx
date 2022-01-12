// Created on: 1991-01-30
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

#ifndef _Expr_LessThan_HeaderFile
#define _Expr_LessThan_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Expr_SingleRelation.hxx>
class Expr_GeneralExpression;
class Expr_GeneralRelation;
class TCollection_AsciiString;


class Expr_LessThan;
DEFINE_STANDARD_HANDLE(Expr_LessThan, Expr_SingleRelation)


class Expr_LessThan : public Expr_SingleRelation
{

public:

  
  //! Creates the relation <exp1> < <exp2>.
  Standard_EXPORT Expr_LessThan(const Handle(Expr_GeneralExpression)& exp1, const Handle(Expr_GeneralExpression)& exp2);
  
  Standard_EXPORT Standard_Boolean IsSatisfied() const Standard_OVERRIDE;
  
  //! Returns a GeneralRelation after replacement of
  //! NamedUnknowns by an associated expression, and after
  //! values computation.
  Standard_EXPORT Handle(Expr_GeneralRelation) Simplified() const Standard_OVERRIDE;
  
  //! Replaces NamedUnknowns by associated expressions,
  //! and computes values in <me>.
  Standard_EXPORT void Simplify() Standard_OVERRIDE;
  
  //! Returns a copy of <me> having the same unknowns and functions.
  Standard_EXPORT Handle(Expr_GeneralRelation) Copy() const Standard_OVERRIDE;
  
  //! returns a string representing <me> in a readable way.
  Standard_EXPORT TCollection_AsciiString String() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Expr_LessThan,Expr_SingleRelation)

protected:




private:




};







#endif // _Expr_LessThan_HeaderFile
