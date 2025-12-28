// Created on: 1995-06-06
// Created by: Jean Yves LEBEY
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

#ifndef _BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox_HeaderFile
#define _BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntImp_ConstIsoparametric.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>
#include <NCollection_Array1.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
class StdFail_UndefinedDerivative;
class Standard_ConstructionError;
class BRepAdaptor_Surface;
class BRepApprox_SurfaceTool;
class math_Matrix;

class BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox
    : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox(
    const BRepAdaptor_Surface& S1,
    const BRepAdaptor_Surface& S2);

  Standard_EXPORT int NbVariables() const;

  Standard_EXPORT int NbEquations() const;

  Standard_EXPORT bool Value(const math_Vector& X, math_Vector& F);

  Standard_EXPORT bool Derivatives(const math_Vector& X, math_Matrix& D);

  Standard_EXPORT bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D);

  Standard_EXPORT void ComputeParameters(const IntImp_ConstIsoparametric ChoixIso,
                                         const NCollection_Array1<double>&     Param,
                                         math_Vector&                    UVap,
                                         math_Vector&                    BornInf,
                                         math_Vector&                    BornSup,
                                         math_Vector&                    Tolerance);

  //! returns somme des fi*fi
  double Root() const;

  gp_Pnt Point() const;

  Standard_EXPORT bool IsTangent(const math_Vector&         UVap,
                                             NCollection_Array1<double>&      Param,
                                             IntImp_ConstIsoparametric& BestChoix);

  gp_Dir Direction() const;

  gp_Dir2d DirectionOnS1() const;

  gp_Dir2d DirectionOnS2() const;

  const BRepAdaptor_Surface& AuxillarSurface1() const;

  const BRepAdaptor_Surface& AuxillarSurface2() const;

private:
  void*          surf1;
  void*          surf2;
  gp_Pnt                    pntsol1;
  gp_Pnt                    pntsol2;
  double             f[3];
  bool          compute;
  bool          tangent;
  double             tgduv[4];
  gp_Vec                    dpuv[4];
  IntImp_ConstIsoparametric chxIso;
  double             paramConst;
  double             ua0;
  double             va0;
  double             ua1;
  double             va1;
  double             ub0;
  double             vb0;
  double             ub1;
  double             vb1;
  double             ures1;
  double             ures2;
  double             vres1;
  double             vres2;
};

#define ThePSurface BRepAdaptor_Surface
#define ThePSurface_hxx <BRepAdaptor_Surface.hxx>
#define ThePSurfaceTool BRepApprox_SurfaceTool
#define ThePSurfaceTool_hxx <BRepApprox_SurfaceTool.hxx>
#define IntImp_ZerParFunc BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox
#define IntImp_ZerParFunc_hxx <BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox.hxx>

#include <IntImp_ZerParFunc.lxx>

#undef ThePSurface
#undef ThePSurface_hxx
#undef ThePSurfaceTool
#undef ThePSurfaceTool_hxx
#undef IntImp_ZerParFunc
#undef IntImp_ZerParFunc_hxx

#endif // _BRepApprox_TheFunctionOfTheInt2SOfThePrmPrmSvSurfacesOfApprox_HeaderFile
