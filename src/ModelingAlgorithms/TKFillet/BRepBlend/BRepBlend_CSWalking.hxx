// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2024 OPEN CASCADE SAS
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

#ifndef _BRepBlend_CSWalking_HeaderFile
#define _BRepBlend_CSWalking_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <BRepBlend_SequenceOfPointOnRst.hxx>
#include <Blend_Point.hxx>
#include <Blend_Status.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <math_Vector.hxx>

class BRepBlend_Line;
class Adaptor3d_TopolTool;
class StdFail_NotDone;
class Adaptor3d_HVertex;
class BRepBlend_HCurve2dTool;
class Adaptor3d_HSurfaceTool;
class BRepBlend_HCurveTool;
class BRepBlend_BlendTool;
class BRepBlend_PointOnRst;
class BRepBlend_Extremity;
class Blend_CSFunction;
class IntSurf_Transition;
class gp_Pnt;
class gp_Pnt2d;
class gp_Vec;
class gp_Vec2d;

class BRepBlend_CSWalking
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepBlend_CSWalking(const Handle(Adaptor3d_Curve)&     Curv,
                                      const Handle(Adaptor3d_Surface)&   Surf,
                                      const Handle(Adaptor3d_TopolTool)& Domain);

  Standard_EXPORT void Perform(Blend_CSFunction&      F,
                               const Standard_Real    Pdep,
                               const Standard_Real    Pmax,
                               const Standard_Real    MaxStep,
                               const Standard_Real    Tol3d,
                               const Standard_Real    TolGuide,
                               const math_Vector&     Soldep,
                               const Standard_Real    Fleche,
                               const Standard_Boolean Appro = Standard_False);

  Standard_EXPORT Standard_Boolean Complete(Blend_CSFunction& F, const Standard_Real Pmin);

  Standard_Boolean IsDone() const;

  const Handle(BRepBlend_Line)& Line() const;

private:
  Standard_EXPORT void InternalPerform(Blend_CSFunction&   F,
                                       math_Vector&        Sol,
                                       const Standard_Real Bound);

  Standard_EXPORT void Transition(const Handle(Adaptor2d_Curve2d)& A,
                                  const Standard_Real              Param,
                                  IntSurf_Transition&              TLine,
                                  IntSurf_Transition&              TArc);

  Standard_EXPORT void MakeExtremity(BRepBlend_Extremity&             Extrem,
                                     const Standard_Integer           Index,
                                     const Standard_Real              Param,
                                     const Standard_Boolean           IsVtx,
                                     const Handle(Adaptor3d_HVertex)& Vtx);

  Standard_EXPORT Blend_Status CheckDeflectionOnSurf(const gp_Pnt&   Psurf,
                                                     const gp_Pnt2d& Ponsurf,
                                                     const gp_Vec&   Tgsurf,
                                                     const gp_Vec2d& Tgonsurf);

  Standard_EXPORT Blend_Status CheckDeflectionOnCurv(const gp_Pnt&       Pcurv,
                                                     const Standard_Real Poncurv,
                                                     const gp_Vec&       Tgcurv);

  Standard_EXPORT Blend_Status TestArret(Blend_CSFunction&      F,
                                         const math_Vector&     Sol,
                                         const Standard_Boolean TestDeflection,
                                         const Blend_Status     State);

  Standard_Boolean              done;
  Handle(BRepBlend_Line)        line;
  Handle(Adaptor3d_Surface)     surf;
  Handle(Adaptor3d_Curve)       curv;
  Handle(Adaptor3d_TopolTool)   domain;
  Standard_Real                 tolpoint3d;
  Standard_Real                 tolgui;
  Standard_Real                 pasmax;
  Standard_Real                 fleche;
  Standard_Real                 param;
  Standard_Real                 firstparam;
  Handle(TColStd_HArray1OfReal) firstsol;
  Blend_Point                   previousP;
  Standard_Boolean              rebrou;
  Standard_Boolean              iscomplete;
  Standard_Boolean              comptra;
  Standard_Real                 sens;
};

#endif // _BRepBlend_CSWalking_HeaderFile
