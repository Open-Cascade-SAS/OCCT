// Created on: 1995-01-27
// Created by: Jacques GOUSSARD
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

#ifndef _GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox_HeaderFile
#define _GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir2d.hxx>
#include <math_FunctionSetWithDerivatives.hxx>
#include <math_Vector.hxx>

class Adaptor3d_HSurfaceTool;
class IntSurf_Quadric;
class IntSurf_QuadricTool;
class math_Matrix;

class GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox : public math_FunctionSetWithDerivatives
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox();

  Standard_EXPORT GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox(
    const occ::handle<Adaptor3d_Surface>& PS,
    const IntSurf_Quadric&                IS);

  Standard_EXPORT GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox(const IntSurf_Quadric& IS);

  void Set(const occ::handle<Adaptor3d_Surface>& PS);

  void SetImplicitSurface(const IntSurf_Quadric& IS);

  void Set(const double Tolerance);

  Standard_EXPORT int NbVariables() const override;

  Standard_EXPORT int NbEquations() const override;

  Standard_EXPORT bool Value(const math_Vector& X, math_Vector& F) override;

  Standard_EXPORT bool Derivatives(const math_Vector& X, math_Matrix& D) override;

  Standard_EXPORT bool Values(const math_Vector& X, math_Vector& F, math_Matrix& D) override;

  double Root() const;

  //! Returns the value Tol so that if std::abs(Func.Root())<Tol
  //! the function is considered null.
  double Tolerance() const;

  const gp_Pnt& Point() const;

  Standard_EXPORT bool IsTangent();

  const gp_Vec& Direction3d();

  const gp_Dir2d& Direction2d();

  const occ::handle<Adaptor3d_Surface>& PSurface() const;

  const IntSurf_Quadric& ISurface() const;

private:
  void*    surf;
  void*    func;
  double   u;
  double   v;
  double   tol;
  gp_Pnt   pntsol;
  double   valf;
  bool     computed;
  bool     tangent;
  double   tgdu;
  double   tgdv;
  gp_Vec   gradient;
  bool     derived;
  gp_Vec   d1u;
  gp_Vec   d1v;
  gp_Vec   d3d;
  gp_Dir2d d2d;
};

#define ThePSurface occ::handle<Adaptor3d_Surface>
#define ThePSurface_hxx <Adaptor3d_Surface.hxx>
#define ThePSurfaceTool Adaptor3d_HSurfaceTool
#define ThePSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define TheISurface IntSurf_Quadric
#define TheISurface_hxx <IntSurf_Quadric.hxx>
#define TheISurfaceTool IntSurf_QuadricTool
#define TheISurfaceTool_hxx <IntSurf_QuadricTool.hxx>
#define IntImp_ZerImpFunc GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox
#define IntImp_ZerImpFunc_hxx <GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox.hxx>

#include <IntImp_ZerImpFunc.lxx>

#undef ThePSurface
#undef ThePSurface_hxx
#undef ThePSurfaceTool
#undef ThePSurfaceTool_hxx
#undef TheISurface
#undef TheISurface_hxx
#undef TheISurfaceTool
#undef TheISurfaceTool_hxx
#undef IntImp_ZerImpFunc
#undef IntImp_ZerImpFunc_hxx

#endif // _GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox_HeaderFile
