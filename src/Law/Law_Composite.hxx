// Created on: 1996-03-29
// Created by: Laurent BOURESCHE
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Law_Composite_HeaderFile
#define _Law_Composite_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Law_Laws.hxx>
#include <Standard_Boolean.hxx>
#include <Law_Function.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
class Law_Function;
class Standard_OutOfRange;


class Law_Composite;
DEFINE_STANDARD_HANDLE(Law_Composite, Law_Function)

//! Loi  composite constituee  d une liste  de lois de
//! ranges consecutifs.
//! Cette implementation un peu lourde permet de reunir
//! en une seule loi des portions de loi construites de
//! facon independantes (par exemple en interactif) et
//! de lancer le walking d un coup a l echelle d une
//! ElSpine.
//! CET OBJET REPOND DONC A UN PROBLEME D IMPLEMENTATION
//! SPECIFIQUE AUX CONGES!!!
class Law_Composite : public Law_Function
{

public:


  //! Construct an empty Law
  Standard_EXPORT Law_Composite();

  //! Construct an empty, trimmed Law
  Standard_EXPORT Law_Composite(const Standard_Real First, const Standard_Real Last, const Standard_Real Tol);

  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;

  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT Standard_Integer NbIntervals (const GeomAbs_Shape S) const Standard_OVERRIDE;

  //! Stores in <T> the  parameters bounding the intervals of continuity <S>.
  //! The array must provide enough room to accommodate for the parameters,
  //! i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals (TColStd_Array1OfReal& T, const GeomAbs_Shape S) const Standard_OVERRIDE;

  //! Returns the value at parameter X.
  Standard_EXPORT Standard_Real Value (const Standard_Real X) Standard_OVERRIDE;

  //! Returns the value and the first derivative at parameter X.
  Standard_EXPORT void D1 (const Standard_Real X, Standard_Real& F, Standard_Real& D) Standard_OVERRIDE;

  //! Returns the value, first and second derivatives
  //! at parameter X.
  Standard_EXPORT void D2 (const Standard_Real X, Standard_Real& F, Standard_Real& D, Standard_Real& D2) Standard_OVERRIDE;

  //! Returns a  law equivalent of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! It is usfule to determines the derivatives
  //! in these values <First> and <Last> if
  //! the Law is not Cn.
  Standard_EXPORT Handle(Law_Function) Trim (const Standard_Real PFirst, const Standard_Real PLast, const Standard_Real Tol) const Standard_OVERRIDE;

  //! Returns the parametric bounds of the function.
  Standard_EXPORT void Bounds (Standard_Real& PFirst, Standard_Real& PLast) Standard_OVERRIDE;

  //! Returns the elementary  function of the composite used
  //! to compute at parameter W.
  Standard_EXPORT Handle(Law_Function)& ChangeElementaryLaw (const Standard_Real W);

  Standard_EXPORT Law_Laws& ChangeLaws();

  Standard_EXPORT Standard_Boolean IsPeriodic() const;

  Standard_EXPORT void SetPeriodic();

  DEFINE_STANDARD_RTTIEXT(Law_Composite,Law_Function)

private:

  //! Set the current function.
  Standard_EXPORT void Prepare (Standard_Real& W);

  Standard_Real first;
  Standard_Real last;
  Handle(Law_Function) curfunc;
  Law_Laws funclist;
  Standard_Boolean periodic;
  Standard_Real TFirst;
  Standard_Real TLast;
  Standard_Real PTol;

};

#endif // _Law_Composite_HeaderFile
