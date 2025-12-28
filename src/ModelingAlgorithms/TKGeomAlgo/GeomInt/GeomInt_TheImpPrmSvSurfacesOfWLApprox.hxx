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

#ifndef _GeomInt_TheImpPrmSvSurfacesOfWLApprox_HeaderFile
#define _GeomInt_TheImpPrmSvSurfacesOfWLApprox_HeaderFile

#include <Adaptor3d_Surface.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Vec.hxx>
#include <GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox.hxx>
#include <ApproxInt_SvSurfaces.hxx>

class Adaptor3d_HSurfaceTool;
class IntSurf_Quadric;
class IntSurf_QuadricTool;
class IntPatch_WLine;
class GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox;

class GeomInt_TheImpPrmSvSurfacesOfWLApprox : public ApproxInt_SvSurfaces
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT GeomInt_TheImpPrmSvSurfacesOfWLApprox(const occ::handle<Adaptor3d_Surface>& Surf1,
                                                        const IntSurf_Quadric& Surf2);

  Standard_EXPORT GeomInt_TheImpPrmSvSurfacesOfWLApprox(
    const IntSurf_Quadric&                Surf1,
    const occ::handle<Adaptor3d_Surface>& Surf2);

  //! returns True if Tg,Tguv1 Tguv2 can be computed.
  Standard_EXPORT bool Compute(double&   u1,
                               double&   v1,
                               double&   u2,
                               double&   v2,
                               gp_Pnt&   Pt,
                               gp_Vec&   Tg,
                               gp_Vec2d& Tguv1,
                               gp_Vec2d& Tguv2);

  Standard_EXPORT void Pnt(const double u1,
                           const double v1,
                           const double u2,
                           const double v2,
                           gp_Pnt&      P);

  Standard_EXPORT bool SeekPoint(const double     u1,
                                 const double     v1,
                                 const double     u2,
                                 const double     v2,
                                 IntSurf_PntOn2S& Point);

  Standard_EXPORT bool Tangency(const double u1,
                                const double v1,
                                const double u2,
                                const double v2,
                                gp_Vec&      Tg);

  Standard_EXPORT bool TangencyOnSurf1(const double u1,
                                       const double v1,
                                       const double u2,
                                       const double v2,
                                       gp_Vec2d&    Tg);

  Standard_EXPORT bool TangencyOnSurf2(const double u1,
                                       const double v1,
                                       const double u2,
                                       const double v2,
                                       gp_Vec2d&    Tg);

  bool FillInitialVectorOfSolution(const double u1,
                                   const double v1,
                                   const double u2,
                                   const double v2,
                                   const double binfu,
                                   const double bsupu,
                                   const double binfv,
                                   const double bsupv,
                                   math_Vector& X,
                                   double&      TranslationU,
                                   double&      TranslationV);

private:
  gp_Pnt2d                                             MyParOnS1;
  gp_Pnt2d                                             MyParOnS2;
  gp_Pnt                                               MyPnt;
  gp_Vec2d                                             MyTguv1;
  gp_Vec2d                                             MyTguv2;
  gp_Vec                                               MyTg;
  bool                                                 MyIsTangent;
  bool                                                 MyHasBeenComputed;
  gp_Pnt2d                                             MyParOnS1bis;
  gp_Pnt2d                                             MyParOnS2bis;
  gp_Pnt                                               MyPntbis;
  gp_Vec2d                                             MyTguv1bis;
  gp_Vec2d                                             MyTguv2bis;
  gp_Vec                                               MyTgbis;
  bool                                                 MyIsTangentbis;
  bool                                                 MyHasBeenComputedbis;
  bool                                                 MyImplicitFirst;
  GeomInt_TheZerImpFuncOfTheImpPrmSvSurfacesOfWLApprox MyZerImpFunc;
};

#endif // _GeomInt_TheImpPrmSvSurfacesOfWLApprox_HeaderFile
