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

#ifndef _Expr_GeneralFunction_HeaderFile
#define _Expr_GeneralFunction_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Real.hxx>
#include <Expr_Array1OfNamedUnknown.hxx>
#include <NCollection_Array1.hxx>
class Expr_NamedUnknown;
class TCollection_AsciiString;

//! Defines the general purposes of any function.
class Expr_GeneralFunction : public Standard_Transient
{

public:
  //! Returns the number of variables of <me>.
  Standard_EXPORT virtual int NbOfVariables() const = 0;

  //! Returns the variable denoted by <index> in <me>.
  //! Raises OutOfRange if index > NbOfVariables.
  Standard_EXPORT virtual occ::handle<Expr_NamedUnknown> Variable(const int index) const = 0;

  //! Returns a copy of <me> with the same form.
  Standard_EXPORT virtual occ::handle<Expr_GeneralFunction> Copy() const = 0;

  //! Returns Derivative of <me> for variable <var>.
  Standard_EXPORT virtual occ::handle<Expr_GeneralFunction> Derivative(
    const occ::handle<Expr_NamedUnknown>& var) const = 0;

  //! Returns Derivative of <me> for variable <var> with
  //! degree <deg>.
  Standard_EXPORT virtual occ::handle<Expr_GeneralFunction> Derivative(
    const occ::handle<Expr_NamedUnknown>& var,
    const int                             deg) const = 0;

  //! Computes the value of <me> with the given variables.
  //! Raises NotEvaluable if <vars> does not match all variables
  //! of <me>.
  Standard_EXPORT virtual double Evaluate(
    const NCollection_Array1<occ::handle<Expr_NamedUnknown>>& vars,
    const NCollection_Array1<double>&                         vals) const = 0;

  //! Tests if <me> and <func> are similar functions (same
  //! name and same used expression).
  Standard_EXPORT virtual bool IsIdentical(const occ::handle<Expr_GeneralFunction>& func) const = 0;

  //! Tests if <me> is linear on variable on range <index>
  Standard_EXPORT virtual bool IsLinearOnVariable(const int index) const = 0;

  Standard_EXPORT virtual TCollection_AsciiString GetStringName() const = 0;

  DEFINE_STANDARD_RTTIEXT(Expr_GeneralFunction, Standard_Transient)
};

#endif // _Expr_GeneralFunction_HeaderFile
