// Created on: 1995-11-15
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Law_BSpFunc_HeaderFile
#define _Law_BSpFunc_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Law_Function.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
class Law_BSpline;

//! Law Function based on a BSpline curve 1d. Package
//! methods and classes are implemented in package Law
//! to construct the basis curve with several
//! constraints.
class Law_BSpFunc : public Law_Function
{

public:
  Standard_EXPORT Law_BSpFunc();

  Standard_EXPORT Law_BSpFunc(const occ::handle<Law_BSpline>& C,
                              const double        First,
                              const double        Last);

  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  //! Returns the number of intervals for continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  //! Stores in <T> the parameters bounding the intervals of continuity <S>.
  //! The array must provide enough room to accommodate for the parameters, i.e. T.Length() >
  //! NbIntervals()
  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape   S) const override;

  Standard_EXPORT double Value(const double X) override;

  Standard_EXPORT void D1(const double X,
                          double&      F,
                          double&      D) override;

  Standard_EXPORT void D2(const double X,
                          double&      F,
                          double&      D,
                          double&      D2) override;

  //! Returns a law equivalent of <me> between
  //! parameters <First> and <Last>. <Tol> is used to
  //! test for 3d points confusion.
  //! It is usfule to determines the derivatives
  //! in these values <First> and <Last> if
  //! the Law is not Cn.
  Standard_EXPORT occ::handle<Law_Function> Trim(const double PFirst,
                                            const double PLast,
                                            const double Tol) const override;

  Standard_EXPORT void Bounds(double& PFirst, double& PLast) override;

  Standard_EXPORT occ::handle<Law_BSpline> Curve() const;

  Standard_EXPORT void SetCurve(const occ::handle<Law_BSpline>& C);

  DEFINE_STANDARD_RTTIEXT(Law_BSpFunc, Law_Function)

private:
  occ::handle<Law_BSpline> curv;
  double       first;
  double       last;
};

#endif // _Law_BSpFunc_HeaderFile
