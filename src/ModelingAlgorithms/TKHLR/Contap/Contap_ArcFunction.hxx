// Created on: 1993-06-03
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

#ifndef _Contap_ArcFunction_HeaderFile
#define _Contap_ArcFunction_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_Surface.hxx>
#include <Contap_TFunction.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Sequence.hxx>
#include <IntSurf_Quadric.hxx>
#include <math_FunctionWithDerivative.hxx>

class Contap_ArcFunction : public math_FunctionWithDerivative
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT Contap_ArcFunction();

  Standard_EXPORT void Set(const occ::handle<Adaptor3d_Surface>& S);

  void Set(const gp_Dir& Direction);

  void Set(const gp_Dir& Direction, const double Angle);

  void Set(const gp_Pnt& Eye);

  void Set(const gp_Pnt& Eye, const double Angle);

  void Set(const occ::handle<Adaptor2d_Curve2d>& A);

  Standard_EXPORT bool Value(const double X, double& F) override;

  Standard_EXPORT bool Derivative(const double X, double& D) override;

  Standard_EXPORT bool Values(const double X, double& F, double& D) override;

  Standard_EXPORT int NbSamples() const;

  Standard_EXPORT int GetStateNumber() override;

  const gp_Pnt& Valpoint(const int Index) const;

  Standard_EXPORT const IntSurf_Quadric& Quadric() const;

  //! Returns mySurf field
  const occ::handle<Adaptor3d_Surface>& Surface() const;

  //! Returns the point, which has been computed
  //! while the last calling Value() method
  const gp_Pnt& LastComputedPoint() const;

private:
  occ::handle<Adaptor2d_Curve2d> myArc;
  occ::handle<Adaptor3d_Surface> mySurf;
  double                         myMean;
  Contap_TFunction               myType;
  gp_Dir                         myDir;
  double                         myCosAng;
  gp_Pnt                         myEye;
  gp_Pnt                         solpt;
  NCollection_Sequence<gp_Pnt>   seqpt;
  IntSurf_Quadric                myQuad;
};

#include <Contap_ArcFunction.lxx>

#endif // _Contap_ArcFunction_HeaderFile
