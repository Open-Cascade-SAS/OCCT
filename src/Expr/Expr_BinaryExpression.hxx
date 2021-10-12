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

#ifndef _Expr_BinaryExpression_HeaderFile
#define _Expr_BinaryExpression_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Expr_GeneralExpression.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Boolean.hxx>
class Expr_NamedUnknown;


class Expr_BinaryExpression;
DEFINE_STANDARD_HANDLE(Expr_BinaryExpression, Expr_GeneralExpression)

//! Defines all binary expressions. The order of the two
//! operands is significant.
class Expr_BinaryExpression : public Expr_GeneralExpression
{

public:

  
    const Handle(Expr_GeneralExpression)& FirstOperand() const;
  
    const Handle(Expr_GeneralExpression)& SecondOperand() const;
  
  //! Sets first operand of <me>
  //! Raises InvalidOperand if exp = me
  Standard_EXPORT void SetFirstOperand (const Handle(Expr_GeneralExpression)& exp);
  
  //! Sets second operand of <me>
  //! Raises InvalidOperand if <exp> contains <me>.
  Standard_EXPORT void SetSecondOperand (const Handle(Expr_GeneralExpression)& exp);
  
  //! returns the number of sub-expressions contained
  //! in <me> ( >= 0)
  Standard_EXPORT Standard_Integer NbSubExpressions() const Standard_OVERRIDE;
  
  //! returns the <I>-th sub-expression of <me>
  //! raises OutOfRange if <I> > NbSubExpressions(me)
  Standard_EXPORT const Handle(Expr_GeneralExpression)& SubExpression (const Standard_Integer I) const Standard_OVERRIDE;
  
  //! Does <me> contain NamedUnknown ?
  Standard_EXPORT Standard_Boolean ContainsUnknowns() const Standard_OVERRIDE;
  
  //! Tests if <me> contains <exp>.
  Standard_EXPORT Standard_Boolean Contains (const Handle(Expr_GeneralExpression)& exp) const Standard_OVERRIDE;
  
  //! Replaces all occurrences of <var> with <with> in <me>.
  //! Raises InvalidOperand if <with> contains <me>.
  Standard_EXPORT void Replace (const Handle(Expr_NamedUnknown)& var, const Handle(Expr_GeneralExpression)& with) Standard_OVERRIDE;
  
  //! Returns a GeneralExpression after replacement of
  //! NamedUnknowns by an associated expression and after
  //! values computation.
  Standard_EXPORT Handle(Expr_GeneralExpression) Simplified() const Standard_OVERRIDE;




  DEFINE_STANDARD_RTTIEXT(Expr_BinaryExpression,Expr_GeneralExpression)

protected:

  
  //! Sets first operand of <me>
  Standard_EXPORT void CreateFirstOperand (const Handle(Expr_GeneralExpression)& exp);
  
  //! Sets second operand of <me>
  //! Raises InvalidOperand if <exp> contains <me>.
  Standard_EXPORT void CreateSecondOperand (const Handle(Expr_GeneralExpression)& exp);



private:


  Handle(Expr_GeneralExpression) myFirstOperand;
  Handle(Expr_GeneralExpression) mySecondOperand;


};


#include <Expr_BinaryExpression.lxx>





#endif // _Expr_BinaryExpression_HeaderFile
