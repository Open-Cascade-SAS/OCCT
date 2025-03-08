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
#include <TColStd_Array1OfReal.hxx>

class Law_Linear;
DEFINE_STANDARD_HANDLE(Law_Linear, Law_Function)

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
  Standard_EXPORT void Set(const Standard_Real Pdeb,
                           const Standard_Real Valdeb,
                           const Standard_Real Pfin,
                           const Standard_Real Valfin);

  //! Returns GeomAbs_CN
  Standard_EXPORT GeomAbs_Shape Continuity() const Standard_OVERRIDE;

  //! Returns  1
  Standard_EXPORT Standard_Integer NbIntervals(const GeomAbs_Shape S) const Standard_OVERRIDE;

  Standard_EXPORT void Intervals(TColStd_Array1OfReal& T,
                                 const GeomAbs_Shape   S) const Standard_OVERRIDE;

  //! Returns the value of this function at the point of parameter X.
  Standard_EXPORT Standard_Real Value(const Standard_Real X) Standard_OVERRIDE;

  //! Returns the value F and the first derivative D of this
  //! function at the point of parameter X.
  Standard_EXPORT void D1(const Standard_Real X,
                          Standard_Real&      F,
                          Standard_Real&      D) Standard_OVERRIDE;

  //! Returns the value, first and second derivatives
  //! at parameter X.
  Standard_EXPORT void D2(const Standard_Real X,
                          Standard_Real&      F,
                          Standard_Real&      D,
                          Standard_Real&      D2) Standard_OVERRIDE;

  //! Returns a  law equivalent of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! It is usfule to determines the derivatives
  //! in these values <First> and <Last> if
  //! the Law is not Cn.
  Standard_EXPORT Handle(Law_Function) Trim(const Standard_Real PFirst,
                                            const Standard_Real PLast,
                                            const Standard_Real Tol) const Standard_OVERRIDE;

  //! Returns the parametric bounds of the function.
  Standard_EXPORT void Bounds(Standard_Real& PFirst, Standard_Real& PLast) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Law_Linear, Law_Function)

protected:
private:
  Standard_Real valdeb;
  Standard_Real valfin;
  Standard_Real pdeb;
  Standard_Real pfin;
};

#endif // _Law_Linear_HeaderFile
