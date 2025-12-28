// Created on: 1993-06-07
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

#ifndef _IntPatch_ArcFunction_HeaderFile
#define _IntPatch_ArcFunction_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <IntSurf_Quadric.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <math_FunctionWithDerivative.hxx>

class IntPatch_ArcFunction : public math_FunctionWithDerivative
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT IntPatch_ArcFunction();

  void SetQuadric(const IntSurf_Quadric& Q);

  void Set(const occ::handle<Adaptor2d_Curve2d>& A);

  void Set(const occ::handle<Adaptor3d_Surface>& S);

  Standard_EXPORT bool Value(const double X, double& F) override;

  Standard_EXPORT bool Derivative(const double X, double& D) override;

  Standard_EXPORT bool Values(const double X, double& F, double& D) override;

  Standard_EXPORT int NbSamples() const;

  Standard_EXPORT virtual int GetStateNumber() override;

  const gp_Pnt& Valpoint(const int Index) const;

  const IntSurf_Quadric& Quadric() const;

  const occ::handle<Adaptor2d_Curve2d>& Arc() const;

  const occ::handle<Adaptor3d_Surface>& Surface() const;

  //! Returns the point, which has been computed
  //! while the last calling Value() method
  const gp_Pnt& LastComputedPoint() const;

private:
  occ::handle<Adaptor2d_Curve2d> myArc;
  occ::handle<Adaptor3d_Surface> mySurf;
  IntSurf_Quadric                myQuad;
  gp_Pnt                         ptsol;
  NCollection_Sequence<gp_Pnt>   seqpt;
};

#include <IntPatch_ArcFunction.lxx>

#endif // _IntPatch_ArcFunction_HeaderFile
