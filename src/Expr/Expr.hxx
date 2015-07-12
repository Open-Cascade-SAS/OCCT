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
class Expr_GeneralExpression;
class Expr_NumericValue;
class Expr_NamedExpression;
class Expr_NamedConstant;
class Expr_NamedUnknown;
class Expr_UnaryExpression;
class Expr_Absolute;
class Expr_ArcCosine;
class Expr_ArcSine;
class Expr_ArcTangent;
class Expr_ArgCosh;
class Expr_ArgSinh;
class Expr_ArgTanh;
class Expr_Cosh;
class Expr_Cosine;
class Expr_Exponential;
class Expr_LogOf10;
class Expr_LogOfe;
class Expr_Sign;
class Expr_Sine;
class Expr_Sinh;
class Expr_Square;
class Expr_SquareRoot;
class Expr_Tangent;
class Expr_Tanh;
class Expr_UnaryFunction;
class Expr_UnaryMinus;
class Expr_BinaryExpression;
class Expr_BinaryFunction;
class Expr_Difference;
class Expr_Division;
class Expr_Exponentiate;
class Expr_PolyExpression;
class Expr_PolyFunction;
class Expr_Product;
class Expr_Sum;
class Expr_UnknownIterator;
class Expr_GeneralRelation;
class Expr_SingleRelation;
class Expr_Different;
class Expr_Equal;
class Expr_GreaterThan;
class Expr_GreaterThanOrEqual;
class Expr_LessThan;
class Expr_LessThanOrEqual;
class Expr_SystemRelation;
class Expr_RelationIterator;
class Expr_RUIterator;
class Expr_GeneralFunction;
class Expr_NamedFunction;
class Expr_FunctionDerivative;


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
