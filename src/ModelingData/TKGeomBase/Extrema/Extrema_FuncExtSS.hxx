// Created on: 1996-01-09
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

#ifndef _Extrema_FuncExtSS_HeaderFile
#define _Extrema_FuncExtSS_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <Extrema_POnSurf.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>
class Adaptor3d_Surface;
class math_Matrix;
class Extrema_POnSurf;

//! Function to find extrema of the
//! distance between two surfaces.
class Extrema_FuncExtSS : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Extrema_FuncExtSS();

  Standard_EXPORT Extrema_FuncExtSS(const Adaptor3d_Surface& S1, const Adaptor3d_Surface& S2);

  //! sets the field mysurf of the function.
  Standard_EXPORT void Initialize(const Adaptor3d_Surface& S1, const Adaptor3d_Surface& S2);

  Standard_EXPORT int NbVariables() const override;

  Standard_EXPORT int NbEquations() const override;

  //! Calculate Fi(U,V).
  Standard_EXPORT bool Value(const math_Vector& UV, math_Vector& F) override;

  //! Calculate Fi'(U,V).
  Standard_EXPORT bool Derivatives(const math_Vector& UV, math_Matrix& DF) override;

  //! Calculate Fi(U,V) and Fi'(U,V).
  Standard_EXPORT bool Values(const math_Vector& UV, math_Vector& F, math_Matrix& DF) override;

  //! Save the found extremum.
  Standard_EXPORT int GetStateNumber() override;

  //! Return the number of found extrema.
  Standard_EXPORT int NbExt() const;

  //! Return the value of the Nth distance.
  Standard_EXPORT double SquareDistance(const int N) const;

  //! Return the Nth extremum on S1.
  Standard_EXPORT const Extrema_POnSurf& PointOnS1(const int N) const;

  //! Renvoie le Nieme extremum sur S2.
  Standard_EXPORT const Extrema_POnSurf& PointOnS2(const int N) const;

private:
  const Adaptor3d_Surface*              myS1;
  const Adaptor3d_Surface*              myS2;
  gp_Pnt                                myP1;
  gp_Pnt                                myP2;
  double                                myU1;
  double                                myV1;
  double                                myU2;
  double                                myV2;
  NCollection_Sequence<double>          mySqDist;
  NCollection_Sequence<Extrema_POnSurf> myPoint1;
  NCollection_Sequence<Extrema_POnSurf> myPoint2;
  bool                                  myS1init;
  bool                                  myS2init;
};

#endif // _Extrema_FuncExtSS_HeaderFile
