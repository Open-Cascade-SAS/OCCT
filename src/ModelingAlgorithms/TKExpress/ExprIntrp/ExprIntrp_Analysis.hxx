// Created on: 1992-02-21
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

#ifndef _ExprIntrp_Analysis_HeaderFile
#define _ExprIntrp_Analysis_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Expr_GeneralExpression.hxx>
#include <NCollection_List.hxx>
#include <Expr_GeneralRelation.hxx>
#include <Expr_GeneralFunction.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Integer.hxx>
#include <Expr_NamedFunction.hxx>
#include <NCollection_Sequence.hxx>
#include <Expr_NamedExpression.hxx>
class ExprIntrp_Generator;
class Expr_GeneralExpression;
class Expr_GeneralRelation;
class TCollection_AsciiString;
class Expr_GeneralFunction;
class Expr_NamedFunction;
class Expr_NamedExpression;

class ExprIntrp_Analysis
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT ExprIntrp_Analysis();

  Standard_EXPORT void SetMaster(const occ::handle<ExprIntrp_Generator>& agen);

  Standard_EXPORT void Push(const occ::handle<Expr_GeneralExpression>& exp);

  Standard_EXPORT void PushRelation(const occ::handle<Expr_GeneralRelation>& rel);

  Standard_EXPORT void PushName(const TCollection_AsciiString& name);

  Standard_EXPORT void PushValue(const int degree);

  Standard_EXPORT void PushFunction(const occ::handle<Expr_GeneralFunction>& func);

  Standard_EXPORT occ::handle<Expr_GeneralExpression> Pop();

  Standard_EXPORT occ::handle<Expr_GeneralRelation> PopRelation();

  Standard_EXPORT TCollection_AsciiString PopName();

  Standard_EXPORT int PopValue();

  Standard_EXPORT occ::handle<Expr_GeneralFunction> PopFunction();

  Standard_EXPORT bool IsExpStackEmpty() const;

  Standard_EXPORT bool IsRelStackEmpty() const;

  Standard_EXPORT void ResetAll();

  Standard_EXPORT void Use(const occ::handle<Expr_NamedFunction>& func);

  Standard_EXPORT void Use(const occ::handle<Expr_NamedExpression>& named);

  Standard_EXPORT occ::handle<Expr_NamedExpression> GetNamed(const TCollection_AsciiString& name);

  Standard_EXPORT occ::handle<Expr_NamedFunction> GetFunction(const TCollection_AsciiString& name);

private:
  NCollection_List<occ::handle<Expr_GeneralExpression>>   myGEStack;
  NCollection_List<occ::handle<Expr_GeneralRelation>>     myGRStack;
  NCollection_List<occ::handle<Expr_GeneralFunction>>     myGFStack;
  NCollection_List<TCollection_AsciiString>               myNameStack;
  NCollection_List<int>                                   myValueStack;
  NCollection_Sequence<occ::handle<Expr_NamedFunction>>   myFunctions;
  NCollection_Sequence<occ::handle<Expr_NamedExpression>> myNamed;
  occ::handle<ExprIntrp_Generator>                        myMaster;
};

#endif // _ExprIntrp_Analysis_HeaderFile
