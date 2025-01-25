// Created on: 1993-12-24
// Created by: Jacques GOUSSARD
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _Law_Function_HeaderFile
#define _Law_Function_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
#include <GeomAbs_Shape.hxx>
#include <Standard_Integer.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <Standard_Real.hxx>

class Law_Function;
DEFINE_STANDARD_HANDLE(Law_Function, Standard_Transient)

//! Root class for evolution laws.
class Law_Function : public Standard_Transient
{

public:
  Standard_EXPORT virtual GeomAbs_Shape Continuity() const = 0;

  //! Returns  the number  of  intervals for  continuity
  //! <S>. May be one if Continuity(me) >= <S>
  Standard_EXPORT virtual Standard_Integer NbIntervals(const GeomAbs_Shape S) const = 0;

  //! Stores in <T> the parameters bounding the intervals of continuity <S>.
  //! The array must provide enough room to accommodate for the parameters,
  //! i.e. T.Length() > NbIntervals()
  Standard_EXPORT virtual void Intervals(TColStd_Array1OfReal& T, const GeomAbs_Shape S) const = 0;

  //! Returns the value of the function at the point of parameter X.
  Standard_EXPORT virtual Standard_Real Value(const Standard_Real X) = 0;

  //! Returns the value F and the first derivative D of the
  //! function at the point of parameter X.
  Standard_EXPORT virtual void D1(const Standard_Real X, Standard_Real& F, Standard_Real& D) = 0;

  //! Returns the value, first and seconde derivatives
  //! at parameter X.
  Standard_EXPORT virtual void D2(const Standard_Real X,
                                  Standard_Real&      F,
                                  Standard_Real&      D,
                                  Standard_Real&      D2) = 0;

  //! Returns a  law equivalent of  <me>  between
  //! parameters <First>  and <Last>. <Tol>  is used  to
  //! test for 3d points confusion.
  //! It is usfule to determines the derivatives
  //! in these values <First> and <Last> if
  //! the Law is not Cn.
  Standard_EXPORT virtual Handle(Law_Function) Trim(const Standard_Real PFirst,
                                                    const Standard_Real PLast,
                                                    const Standard_Real Tol) const = 0;

  //! Returns the parametric bounds of the function.
  Standard_EXPORT virtual void Bounds(Standard_Real& PFirst, Standard_Real& PLast) = 0;

  DEFINE_STANDARD_RTTIEXT(Law_Function, Standard_Transient)

protected:
private:
};

#endif // _Law_Function_HeaderFile
