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

#ifndef _Expr_Difference_HeaderFile
#define _Expr_Difference_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Expr_BinaryExpression.hxx>
#include <Standard_Boolean.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Expr_Array1OfNamedUnknown.hxx>
#include <TColStd_Array1OfReal.hxx>
class Expr_GeneralExpression;
class Expr_NamedUnknown;
class TCollection_AsciiString;


class Expr_Difference;
DEFINE_STANDARD_HANDLE(Expr_Difference, Expr_BinaryExpression)


class Expr_Difference : public Expr_BinaryExpression
{

public:

  
  //! Creates the difference <exp1> - <exp2>.
  Standard_EXPORT Expr_Difference(const Handle(Expr_GeneralExpression)& exp1, const Handle(Expr_GeneralExpression)& exp2);
  
  //! Returns a GeneralExpression after a simplification
  //! of the arguments of <me>.
  Standard_EXPORT Handle(Expr_GeneralExpression) ShallowSimplified() const Standard_OVERRIDE;
  
  //! Returns a copy of <me> having the same unknowns and functions.
  Standard_EXPORT Handle(Expr_GeneralExpression) Copy() const Standard_OVERRIDE;
  
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
  
  //! Returns the value of <me> (as a Real) by
  //! replacement of <vars> by <vals>.
  //! Raises NotEvaluable if <me> contains NamedUnknown not
  //! in <vars> or NumericError if result cannot be computed.
  Standard_EXPORT Standard_Real Evaluate (const Expr_Array1OfNamedUnknown& vars, const TColStd_Array1OfReal& vals) const Standard_OVERRIDE;
  
  //! returns a string representing <me> in a readable way.
  Standard_EXPORT TCollection_AsciiString String() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Expr_Difference,Expr_BinaryExpression)

protected:




private:




};







#endif // _Expr_Difference_HeaderFile
