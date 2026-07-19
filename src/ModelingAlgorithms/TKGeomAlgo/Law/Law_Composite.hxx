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

#include <Law_Function.hxx>
#include <NCollection_List.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

//! Loi composite constituee d une liste de lois de
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
  Standard_EXPORT Law_Composite(const double First, const double Last, const double Tol);

  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals of continuity <S>.
  //! The array must provide enough room to accommodate for the parameters,
  //! i.e. T.Length() > NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape         S) const override;

  //! Returns the value at parameter X.
  Standard_EXPORT double Value(const double X) override;

  //! Returns the value and the first derivative at parameter X.
  Standard_EXPORT void D1(const double X, double& F, double& D) override;

  //! Returns the value, first and second derivatives
  //! at parameter X.
  Standard_EXPORT void D2(const double X, double& F, double& D, double& D2) override;

  //! Returns a law equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! It is usfule to determines the derivatives
  //! in these values <First> and <Last> if
  //! the Law is not Cn.
  Standard_EXPORT occ::handle<Law_Function> Trim(const double PFirst,
                                                 const double PLast,
                                                 const double Tol) const override;

  //! Returns the parametric bounds of the function.
  Standard_EXPORT void Bounds(double& PFirst, double& PLast) override;

  //! Returns the elementary function of the composite used
  //! to compute at parameter W.
  Standard_EXPORT occ::handle<Law_Function>& ChangeElementaryLaw(const double W);

  Standard_EXPORT NCollection_List<occ::handle<Law_Function>>& ChangeLaws();

  Standard_EXPORT bool IsPeriodic() const;

  Standard_EXPORT void SetPeriodic();

  DEFINE_STANDARD_RTTIEXT(Law_Composite, Law_Function)

private:
  //! Set the current function.
  Standard_EXPORT void Prepare(double& W);

  double                                      first;
  double                                      last;
  occ::handle<Law_Function>                   curfunc;
  NCollection_List<occ::handle<Law_Function>> funclist;
  bool                                        periodic;
  double                                      TFirst;
  double                                      TLast;
  double                                      PTol;
};

#endif // _Law_Composite_HeaderFile
