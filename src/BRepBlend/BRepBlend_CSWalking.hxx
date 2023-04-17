// Created on: 1993-12-06
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

#ifndef _BRepBlend_CSWalking_HeaderFile
#define _BRepBlend_CSWalking_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Adaptor3d_Surface.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <Blend_Point.hxx>
#include <BRepBlend_SequenceOfPointOnRst.hxx>
#include <math_Vector.hxx>
#include <Blend_Status.hxx>

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

  
  Standard_EXPORT BRepBlend_CSWalking(const Handle(Adaptor3d_Curve)& Curv, const Handle(Adaptor3d_Surface)& Surf, const Handle(Adaptor3d_TopolTool)& Domain);
  
  Standard_EXPORT void Perform (Blend_CSFunction& F, const Standard_Real Pdep, const Standard_Real Pmax, const Standard_Real MaxStep, const Standard_Real Tol3d, const Standard_Real TolGuide, const math_Vector& Soldep, const Standard_Real Fleche, const Standard_Boolean Appro = Standard_False);
  
  Standard_EXPORT Standard_Boolean Complete (Blend_CSFunction& F, const Standard_Real Pmin);
  
    Standard_Boolean IsDone() const;
  
    const Handle(BRepBlend_Line)& Line() const;

private:

  
  Standard_EXPORT void InternalPerform (Blend_CSFunction& F, math_Vector& Sol, const Standard_Real Bound);
  
  Standard_EXPORT void Transition (const Handle(Adaptor2d_Curve2d)& A, const Standard_Real Param, IntSurf_Transition& TLine, IntSurf_Transition& TArc);
  
  Standard_EXPORT void MakeExtremity (BRepBlend_Extremity& Extrem, const Standard_Integer Index, const Standard_Real Param, const Standard_Boolean IsVtx, const Handle(Adaptor3d_HVertex)& Vtx);
  
  Standard_EXPORT Blend_Status CheckDeflectionOnSurf (const gp_Pnt& Psurf, const gp_Pnt2d& Ponsurf, const gp_Vec& Tgsurf, const gp_Vec2d& Tgonsurf);
  
  Standard_EXPORT Blend_Status CheckDeflectionOnCurv (const gp_Pnt& Pcurv, const Standard_Real Poncurv, const gp_Vec& Tgcurv);
  
  Standard_EXPORT Blend_Status TestArret (Blend_CSFunction& F, const math_Vector& Sol, const Standard_Boolean TestDeflection, const Blend_Status State);


  Standard_Boolean done;
  Handle(BRepBlend_Line) line;
  Handle(Adaptor3d_Surface) surf;
  Handle(Adaptor3d_Curve) curv;
  Handle(Adaptor3d_TopolTool) domain;
  Standard_Real tolpoint3d;
  Standard_Real tolgui;
  Standard_Real pasmax;
  Standard_Real fleche;
  Standard_Real param;
  Standard_Real firstparam;
  Handle(TColStd_HArray1OfReal) firstsol;
  Blend_Point previousP;
  Standard_Boolean rebrou;
  Standard_Boolean iscomplete;
  Standard_Boolean comptra;
  Standard_Real sens;


};

#define TheVertex Handle(Adaptor3d_HVertex)
#define TheVertex_hxx <Adaptor3d_HVertex.hxx>
#define TheArc Handle(Adaptor2d_Curve2d)
#define TheArc_hxx <Adaptor2d_Curve2d.hxx>
#define TheSurface Handle(Adaptor3d_Surface)
#define TheSurface_hxx <Adaptor3d_Surface.hxx>
#define TheCurve Handle(Adaptor3d_Curve)
#define TheCurve_hxx <Adaptor3d_Curve.hxx>
#define TheVertexTool Standard_Integer
#define TheVertexTool_hxx <Standard_Integer.hxx>
#define TheArcTool BRepBlend_HCurve2dTool
#define TheArcTool_hxx <BRepBlend_HCurve2dTool.hxx>
#define TheSurfaceTool Adaptor3d_HSurfaceTool
#define TheSurfaceTool_hxx <Adaptor3d_HSurfaceTool.hxx>
#define TheCurveTool BRepBlend_HCurveTool
#define TheCurveTool_hxx <BRepBlend_HCurveTool.hxx>
#define Handle_TheTopolTool Handle(Adaptor3d_TopolTool)
#define TheTopolTool Adaptor3d_TopolTool
#define TheTopolTool_hxx <Adaptor3d_TopolTool.hxx>
#define TheBlendTool BRepBlend_BlendTool
#define TheBlendTool_hxx <BRepBlend_BlendTool.hxx>
#define ThePointOnRst BRepBlend_PointOnRst
#define ThePointOnRst_hxx <BRepBlend_PointOnRst.hxx>
#define TheSeqPointOnRst BRepBlend_SequenceOfPointOnRst
#define TheSeqPointOnRst_hxx <BRepBlend_SequenceOfPointOnRst.hxx>
#define TheExtremity BRepBlend_Extremity
#define TheExtremity_hxx <BRepBlend_Extremity.hxx>
#define Handle_TheLine Handle(BRepBlend_Line)
#define TheLine BRepBlend_Line
#define TheLine_hxx <BRepBlend_Line.hxx>
#define Blend_CSWalking BRepBlend_CSWalking
#define Blend_CSWalking_hxx <BRepBlend_CSWalking.hxx>

#undef TheVertex
#undef TheVertex_hxx
#undef TheArc
#undef TheArc_hxx
#undef TheSurface
#undef TheSurface_hxx
#undef TheCurve
#undef TheCurve_hxx
#undef TheVertexTool
#undef TheVertexTool_hxx
#undef TheArcTool
#undef TheArcTool_hxx
#undef TheSurfaceTool
#undef TheSurfaceTool_hxx
#undef TheCurveTool
#undef TheCurveTool_hxx
#undef Handle_TheTopolTool
#undef TheTopolTool
#undef TheTopolTool_hxx
#undef TheBlendTool
#undef TheBlendTool_hxx
#undef ThePointOnRst
#undef ThePointOnRst_hxx
#undef TheSeqPointOnRst
#undef TheSeqPointOnRst_hxx
#undef TheExtremity
#undef TheExtremity_hxx
#undef Handle_TheLine
#undef TheLine
#undef TheLine_hxx
#undef Blend_CSWalking
#undef Blend_CSWalking_hxx




#endif // _BRepBlend_CSWalking_HeaderFile
