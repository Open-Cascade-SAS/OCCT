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

#ifndef _Law_Constant_HeaderFile
#define _Law_Constant_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Real.hxx>
#include <Law_Function.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>

//! Loi constante
class Law_Constant : public Law_Function
{

public:
  Standard_EXPORT Law_Constant();

  //! Set the radius and the range of the constant Law.
  Standard_EXPORT void Set(const double Radius, const double PFirst, const double PLast);

  //! Returns GeomAbs_CN
  Standard_EXPORT GeomAbs_Shape Continuity() const override;

  //! Returns 1
  Standard_EXPORT int NbIntervals(const GeomAbs_Shape S) const override;

  Standard_EXPORT void Intervals(NCollection_Array1<double>& T,
                                 const GeomAbs_Shape         S) const override;

  //! Returns the value at parameter X.
  Standard_EXPORT double Value(const double X) override;

  //! Returns the value and the first derivative at parameter X.
  Standard_EXPORT void D1(const double X, double& F, double& D) override;

  //! Returns the value, first and second derivatives
  //! at parameter X.
  Standard_EXPORT void D2(const double X, double& F, double& D, double& D2) override;

  Standard_EXPORT occ::handle<Law_Function> Trim(const double PFirst,
                                                 const double PLast,
                                                 const double Tol) const override;

  //! Returns the parametric bounds of the function.
  Standard_EXPORT void Bounds(double& PFirst, double& PLast) override;

  DEFINE_STANDARD_RTTIEXT(Law_Constant, Law_Function)

private:
  double radius;
  double first;
  double last;
};

#endif // _Law_Constant_HeaderFile
