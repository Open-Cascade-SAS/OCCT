// Created on: 1996-01-22
// Created by: Laurent PAINNOT
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

#ifndef _Extrema_FuncExtCS_HeaderFile
#define _Extrema_FuncExtCS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_POnSurf.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>
class Adaptor3d_Curve;
class Adaptor3d_Surface;
class math_Matrix;
class Extrema_POnCurv;
class Extrema_POnSurf;

//! Function to find extrema of the
//! distance between a curve and a surface.
class Extrema_FuncExtCS : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Extrema_FuncExtCS();

  Standard_EXPORT Extrema_FuncExtCS(const Adaptor3d_Curve& C, const Adaptor3d_Surface& S);

  //! sets the field mysurf of the function.
  Standard_EXPORT void Initialize(const Adaptor3d_Curve& C, const Adaptor3d_Surface& S);

  Standard_EXPORT int NbVariables() const override;

  Standard_EXPORT int NbEquations() const override;

  //! Calculation of Fi(U,V).
  Standard_EXPORT bool Value(const math_Vector& UV, math_Vector& F) override;

  //! Calculation of Fi'(U,V).
  Standard_EXPORT bool Derivatives(const math_Vector& UV, math_Matrix& DF) override;

  //! Calculation of Fi(U,V) and Fi'(U,V).
  Standard_EXPORT bool Values(const math_Vector& UV, math_Vector& F, math_Matrix& DF) override;

  //! Save the found extremum.
  Standard_EXPORT virtual int GetStateNumber() override;

  //! Return the number of found extrema.
  Standard_EXPORT int NbExt() const;

  //! Return the value of the Nth distance.
  Standard_EXPORT double SquareDistance(const int N) const;

  //! Returns the Nth extremum on C.
  Standard_EXPORT const Extrema_POnCurv& PointOnCurve(const int N) const;

  //! Return the Nth extremum on S.
  Standard_EXPORT const Extrema_POnSurf& PointOnSurface(const int N) const;

  //! Change Sequence of SquareDistance
  NCollection_Sequence<double>& SquareDistances() { return mySqDist; }

  //! Change Sequence of PointOnCurv
  NCollection_Sequence<Extrema_POnCurv>& PointsOnCurve() { return myPoint1; }

  //! Change Sequence of PointOnSurf
  NCollection_Sequence<Extrema_POnSurf>& PointsOnSurf() { return myPoint2; }

private:
  const Adaptor3d_Curve*                myC;
  const Adaptor3d_Surface*              myS;
  gp_Pnt                                myP1;
  gp_Pnt                                myP2;
  double                                myt;
  double                                myU;
  double                                myV;
  NCollection_Sequence<double>          mySqDist;
  NCollection_Sequence<Extrema_POnCurv> myPoint1;
  NCollection_Sequence<Extrema_POnSurf> myPoint2;
  bool                                  myCinit;
  bool                                  mySinit;
};

#endif // _Extrema_FuncExtCS_HeaderFile
