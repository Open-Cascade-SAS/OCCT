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

#ifndef _Expr_NumericValue_HeaderFile
#define _Expr_NumericValue_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Expr_GeneralExpression.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
#include <Expr_Array1OfNamedUnknown.hxx>
#include <TColStd_Array1OfReal.hxx>
class Expr_NamedUnknown;
class TCollection_AsciiString;


class Expr_NumericValue;
DEFINE_STANDARD_HANDLE(Expr_NumericValue, Expr_GeneralExpression)

//! This class describes any reel value defined in an
//! expression.
class Expr_NumericValue : public Expr_GeneralExpression
{

public:

  
  Standard_EXPORT Expr_NumericValue(const Standard_Real val);
  
  Standard_EXPORT Standard_Real GetValue() const;
  
  Standard_EXPORT void SetValue (const Standard_Real val);
  
  //! Returns the number of sub-expressions contained
  //! in <me> ( >= 0)
  Standard_EXPORT Standard_Integer NbSubExpressions() const Standard_OVERRIDE;
  
  //! Returns the <I>-th sub-expression of <me>
  //! raises OutOfRange if <I> > NbSubExpressions(me)
  Standard_EXPORT const Handle(Expr_GeneralExpression)& SubExpression (const Standard_Integer I) const Standard_OVERRIDE;
  
  //! Returns a GeneralExpression after replacement of
  //! NamedUnknowns by an associated expression and after
  //! values computation.
  Standard_EXPORT Handle(Expr_GeneralExpression) Simplified() const Standard_OVERRIDE;
  
  //! Returns a GeneralExpression after a simplification
  //! of the arguments of <me>.
  Standard_EXPORT Handle(Expr_GeneralExpression) ShallowSimplified() const Standard_OVERRIDE;
  
  //! Returns a copy of <me> having the same unknowns and functions.
  Standard_EXPORT Handle(Expr_GeneralExpression) Copy() const Standard_OVERRIDE;
  
  //! Tests if <me> contains NamedUnknown.
  Standard_EXPORT Standard_Boolean ContainsUnknowns() const Standard_OVERRIDE;
  
  //! Tests if <exp> is contained in <me>.
  Standard_EXPORT Standard_Boolean Contains (const Handle(Expr_GeneralExpression)& exp) const Standard_OVERRIDE;
  
  //! Tests if <me> and <Other> define the same expression.
  //! This method does not include any simplification before
  //! testing.
  Standard_EXPORT Standard_Boolean IsIdentical (const Handle(Expr_GeneralExpression)& Other) const Standard_OVERRIDE;
  
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
  
  //! returns a string representing <me> in a readable way.
  Standard_EXPORT TCollection_AsciiString String() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Expr_NumericValue,Expr_GeneralExpression)

protected:




private:


  Standard_Real myValue;


};







#endif // _Expr_NumericValue_HeaderFile
