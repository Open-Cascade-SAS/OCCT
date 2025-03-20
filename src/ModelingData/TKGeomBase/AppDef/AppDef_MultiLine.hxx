// Created on: 1991-12-02
// Created by: Laurent PAINNOT
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

#ifndef _AppDef_MultiLine_HeaderFile
#define _AppDef_MultiLine_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <AppDef_HArray1OfMultiPointConstraint.hxx>
#include <Standard_Integer.hxx>
#include <AppDef_Array1OfMultiPointConstraint.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <Standard_OStream.hxx>
class AppDef_MultiPointConstraint;

//! This class describes the organized set of points used in the
//! approximations. A MultiLine is composed of n
//! MultiPointConstraints.
//! The approximation of the MultiLine will be done in the order
//! of the given n MultiPointConstraints.
//!
//! Example of a MultiLine composed of MultiPointConstraints:
//!
//! P1______P2_____P3______P4________........_____PNbMult
//!
//! Q1______Q2_____Q3______Q4________........_____QNbMult
//! .                                               .
//! .                                               .
//! .                                               .
//! R1______R2_____R3______R4________........_____RNbMult
//!
//! Pi, Qi, ..., Ri are points of dimension 2 or 3.
//!
//! (P1, Q1, ...R1), ...(Pn, Qn, ...Rn) n= 1,...NbMult are
//! MultiPointConstraints.
//! There are NbPoints points in each MultiPointConstraint.
class AppDef_MultiLine
{
public:
  DEFINE_STANDARD_ALLOC

  //! creates an undefined MultiLine.
  Standard_EXPORT AppDef_MultiLine();

  //! given the number NbMult of MultiPointConstraints of this
  //! MultiLine , it initializes all the fields.SetValue must be
  //! called in order for the values of the multipoint
  //! constraint to be taken into account.
  //! An exception is raised if NbMult < 0.
  Standard_EXPORT AppDef_MultiLine(const Standard_Integer NbMult);

  //! Constructs a MultiLine with an array of MultiPointConstraints.
  Standard_EXPORT AppDef_MultiLine(const AppDef_Array1OfMultiPointConstraint& tabMultiP);

  //! The MultiLine constructed will have one line of
  //! 3d points without their tangencies.
  Standard_EXPORT AppDef_MultiLine(const TColgp_Array1OfPnt& tabP3d);

  //! The MultiLine constructed will have one line of
  //! 2d points without their tangencies.
  Standard_EXPORT AppDef_MultiLine(const TColgp_Array1OfPnt2d& tabP2d);

  //! returns the number of MultiPointConstraints of the
  //! MultiLine.
  Standard_EXPORT Standard_Integer NbMultiPoints() const;

  //! returns the number of Points from MultiPoints composing
  //! the MultiLine.
  Standard_EXPORT Standard_Integer NbPoints() const;

  //! Sets the value of the parameter for the
  //! MultiPointConstraint at position Index.
  //! Exceptions
  //! -   Standard_OutOfRange if Index is less
  //! than 0 or Index is greater than the number
  //! of Multipoint constraints in the MultiLine.
  Standard_EXPORT void SetParameter(const Standard_Integer Index, const Standard_Real U);

  //! It sets the MultiPointConstraint of range Index to the
  //! value MPoint.
  //! An exception is raised if Index < 0 or Index> MPoint.
  //! An exception is raised if the dimensions of the
  //! MultiPoints are different.
  Standard_EXPORT void SetValue(const Standard_Integer             Index,
                                const AppDef_MultiPointConstraint& MPoint);

  //! returns the MultiPointConstraint of range Index
  //! An exception is raised if Index<0 or Index>MPoint.
  Standard_EXPORT AppDef_MultiPointConstraint Value(const Standard_Integer Index) const;

  //! Prints on the stream o information on the current
  //! state of the object.
  //! Is used to redefine the operator <<.
  Standard_EXPORT void Dump(Standard_OStream& o) const;

protected:
  Handle(AppDef_HArray1OfMultiPointConstraint) tabMult;

private:
};

#endif // _AppDef_MultiLine_HeaderFile
