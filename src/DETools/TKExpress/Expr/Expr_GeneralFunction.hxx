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
#include <TColStd_Array1OfReal.hxx>
class Expr_NamedUnknown;
class TCollection_AsciiString;

class Expr_GeneralFunction;
DEFINE_STANDARD_HANDLE(Expr_GeneralFunction, Standard_Transient)

//! Defines the general purposes of any function.
class Expr_GeneralFunction : public Standard_Transient
{

public:
  //! Returns the number of variables of <me>.
  Standard_EXPORT virtual Standard_Integer NbOfVariables() const = 0;

  //! Returns the variable denoted by <index> in <me>.
  //! Raises OutOfRange if index > NbOfVariables.
  Standard_EXPORT virtual Handle(Expr_NamedUnknown) Variable(
    const Standard_Integer index) const = 0;

  //! Returns a copy of <me> with the same form.
  Standard_EXPORT virtual Handle(Expr_GeneralFunction) Copy() const = 0;

  //! Returns Derivative of <me> for variable <var>.
  Standard_EXPORT virtual Handle(Expr_GeneralFunction) Derivative(
    const Handle(Expr_NamedUnknown)& var) const = 0;

  //! Returns Derivative of <me> for variable <var> with
  //! degree <deg>.
  Standard_EXPORT virtual Handle(Expr_GeneralFunction) Derivative(
    const Handle(Expr_NamedUnknown)& var,
    const Standard_Integer           deg) const = 0;

  //! Computes the value of <me> with the given variables.
  //! Raises NotEvaluable if <vars> does not match all variables
  //! of <me>.
  Standard_EXPORT virtual Standard_Real Evaluate(const Expr_Array1OfNamedUnknown& vars,
                                                 const TColStd_Array1OfReal&      vals) const = 0;

  //! Tests if <me> and <func> are similar functions (same
  //! name and same used expression).
  Standard_EXPORT virtual Standard_Boolean IsIdentical(
    const Handle(Expr_GeneralFunction)& func) const = 0;

  //! Tests if <me> is linear on variable on range <index>
  Standard_EXPORT virtual Standard_Boolean IsLinearOnVariable(
    const Standard_Integer index) const = 0;

  Standard_EXPORT virtual TCollection_AsciiString GetStringName() const = 0;

  DEFINE_STANDARD_RTTIEXT(Expr_GeneralFunction, Standard_Transient)

protected:
private:
};

#endif // _Expr_GeneralFunction_HeaderFile
