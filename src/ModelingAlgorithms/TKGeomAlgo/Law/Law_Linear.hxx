// Created on: 1995-01-12
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

#ifndef _Law_Linear_HeaderFile
#define _Law_Linear_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Law_Function.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

//! Describes an linear evolution law.
class Law_Linear : public Law_Function
{

public:
  //! Constructs an empty linear evolution law.
  Standard_EXPORT Law_Linear();

  //! Defines this linear evolution law by assigning both:
  //! -   the bounds Pdeb and Pfin of the parameter, and
  //! -   the values Valdeb and Valfin of the function at these
  //! two parametric bounds.
  Standard_EXPORT void Set(const double Pdeb,
                           const double Valdeb,
                           const double Pfin,
                           const double Valfin);

  //! Returns GeomAbs_CN
  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  //! Returns 1
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape   S) const override;

  //! Returns the value of this function at the point of parameter X.
  Standard_EXPORT double Value(const double X) override;

  //! Returns the value F and the first derivative D of this
  //! function at the point of parameter X.
  Standard_EXPORT void D1(const double X,
                          double&      F,
                          double&      D) override;

  //! Returns the value, first and second derivatives
  //! at parameter X.
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

  //! Returns the parametric bounds of the function.
  Standard_EXPORT void Bounds(double& PFirst, double& PLast) override;

  DEFINE_STANDARD_RTTIEXT(Law_Linear, Law_Function)

private:
  double valdeb;
  double valfin;
  double pdeb;
  double pfin;
};

#endif // _Law_Linear_HeaderFile
