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

#ifndef _Expr_HeaderFile
#define _Expr_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
class Expr_GeneralExpression;
class Expr_GeneralRelation;


//! This package describes  the data structure  of any
//! expression, relation or function used in mathematics.
//! It also describes the assignment of variables. Standard
//! mathematical functions are implemented such as
//! trigonometrics, hyperbolics, and log functions.
class Expr 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT static Handle(Expr_GeneralExpression) CopyShare (const Handle(Expr_GeneralExpression)& exp);
  
  Standard_EXPORT static Standard_Integer NbOfFreeVariables (const Handle(Expr_GeneralExpression)& exp);
  
  Standard_EXPORT static Standard_Integer NbOfFreeVariables (const Handle(Expr_GeneralRelation)& exp);
  
  Standard_EXPORT static Standard_Real Sign (const Standard_Real val);




protected:





private:




friend class Expr_GeneralExpression;
friend class Expr_NumericValue;
friend class Expr_NamedExpression;
friend class Expr_NamedConstant;
friend class Expr_NamedUnknown;
friend class Expr_UnaryExpression;
friend class Expr_Absolute;
friend class Expr_ArcCosine;
friend class Expr_ArcSine;
friend class Expr_ArcTangent;
friend class Expr_ArgCosh;
friend class Expr_ArgSinh;
friend class Expr_ArgTanh;
friend class Expr_Cosh;
friend class Expr_Cosine;
friend class Expr_Exponential;
friend class Expr_LogOf10;
friend class Expr_LogOfe;
friend class Expr_Sign;
friend class Expr_Sine;
friend class Expr_Sinh;
friend class Expr_Square;
friend class Expr_SquareRoot;
friend class Expr_Tangent;
friend class Expr_Tanh;
friend class Expr_UnaryFunction;
friend class Expr_UnaryMinus;
friend class Expr_BinaryExpression;
friend class Expr_BinaryFunction;
friend class Expr_Difference;
friend class Expr_Division;
friend class Expr_Exponentiate;
friend class Expr_PolyExpression;
friend class Expr_PolyFunction;
friend class Expr_Product;
friend class Expr_Sum;
friend class Expr_UnknownIterator;
friend class Expr_GeneralRelation;
friend class Expr_SingleRelation;
friend class Expr_Different;
friend class Expr_Equal;
friend class Expr_GreaterThan;
friend class Expr_GreaterThanOrEqual;
friend class Expr_LessThan;
friend class Expr_LessThanOrEqual;
friend class Expr_SystemRelation;
friend class Expr_RelationIterator;
friend class Expr_RUIterator;
friend class Expr_GeneralFunction;
friend class Expr_NamedFunction;
friend class Expr_FunctionDerivative;

};







#endif // _Expr_HeaderFile
