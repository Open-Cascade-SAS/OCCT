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

#ifndef _Expr_NamedConstant_HeaderFile
#define _Expr_NamedConstant_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Expr_NamedExpression.hxx>
#include <Standard_Integer.hxx>
#include <Expr_Array1OfNamedUnknown.hxx>
#include <TColStd_Array1OfReal.hxx>
class TCollection_AsciiString;
class Expr_GeneralExpression;
class Expr_NamedUnknown;


class Expr_NamedConstant;
DEFINE_STANDARD_HANDLE(Expr_NamedConstant, Expr_NamedExpression)

//! Describes any numeric constant known by a special name
//! (as PI, e,...).
class Expr_NamedConstant : public Expr_NamedExpression
{

public:

  
  //! Creates a constant value of name <name> and value <value>.
  Standard_EXPORT Expr_NamedConstant(const TCollection_AsciiString& name, const Standard_Real value);
  
    Standard_Real GetValue() const;
  
  //! returns the number of sub-expressions contained
  //! in <me> (always returns zero)
  Standard_EXPORT Standard_Integer NbSubExpressions() const Standard_OVERRIDE;
  
  //! returns the <I>-th sub-expression of <me>
  //! raises OutOfRange
  Standard_EXPORT const Handle(Expr_GeneralExpression)& SubExpression (const Standard_Integer I) const Standard_OVERRIDE;
  
  //! returns a GeneralExpression after replacement of
  //! NamedUnknowns by an associated expression and after
  //! values computation.
  Standard_EXPORT Handle(Expr_GeneralExpression) Simplified() const Standard_OVERRIDE;
  
  //! Returns a GeneralExpression after a simplification
  //! of the arguments of <me>.
  Standard_EXPORT Handle(Expr_GeneralExpression) ShallowSimplified() const Standard_OVERRIDE;
  
  //! Returns a copy of <me> having the same unknowns and functions.
  Standard_EXPORT Handle(Expr_GeneralExpression) Copy() const Standard_OVERRIDE;
  
  //! Tests if <me> contains NamedUnknown.
  //! (returns always False)
  Standard_EXPORT Standard_Boolean ContainsUnknowns() const Standard_OVERRIDE;
  
  //! Tests if <exp> is contained in <me>.
  Standard_EXPORT Standard_Boolean Contains (const Handle(Expr_GeneralExpression)& exp) const Standard_OVERRIDE;
  
  Standard_EXPORT Standard_Boolean IsLinear() const Standard_OVERRIDE;
  
  //! Returns the derivative on <X> unknown of <me>
  Standard_EXPORT Handle(Expr_GeneralExpression) Derivative (const Handle(Expr_NamedUnknown)& X) const Standard_OVERRIDE;
  
  //! Returns the <N>-th derivative on <X> unknown of <me>.
  //! Raises OutOfRange if <N> <= 0
  Standard_EXPORT virtual Handle(Expr_GeneralExpression) NDerivative (const Handle(Expr_NamedUnknown)& X, const Standard_Integer N) const Standard_OVERRIDE;
  
  //! Replaces all occurrences of <var> with <with> in <me>
  Standard_EXPORT void Replace (const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with) Standard_OVERRIDE;
  
  //! Returns the value of <me> (as a Real) by
  //! replacement of <vars> by <vals>.
  Standard_EXPORT Standard_Real Evaluate (const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Expr_NamedConstant,Expr_NamedExpression)

protected:




private:


  Standard_Real myValue;


};


#include <Expr_NamedConstant.lxx>





#endif // _Expr_NamedConstant_HeaderFile
