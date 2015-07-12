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

#include <BRepBlend_CSWalking.hxx>

#include <BRepBlend_Line.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <StdFail_NotDone.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <BRepBlend_HCurve2dTool.hxx>
#include <Adaptor3d_HSurfaceTool.hxx>
#include <BRepBlend_HCurveTool.hxx>
#include <BRepBlend_BlendTool.hxx>
#include <BRepBlend_PointOnRst.hxx>
#include <BRepBlend_Extremity.hxx>
#include <Blend_CSFunction.hxx>
#include <IntSurf_Transition.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
 

#define TheVertex Handle(Adaptor3d_HVertex)
#define TheVertex_hxx <Adaptor3d_HVertex.hxx>
#define TheArc Handle(Adaptor2d_HCurve2d)
#define TheArc_hxx <Adaptor2d_HCurve2d.hxx>
#define TheSurface Handle(Adaptor3d_HSurface)
#define TheSurface_hxx <Adaptor3d_HSurface.hxx>
#define TheCurve Handle(Adaptor3d_HCurve)
#define TheCurve_hxx <Adaptor3d_HCurve.hxx>
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
#include <Blend_CSWalking.gxx>

