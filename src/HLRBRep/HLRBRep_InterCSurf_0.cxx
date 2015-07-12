// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

#include <HLRBRep_InterCSurf.hxx>

#include <gp_Lin.hxx>
#include <HLRBRep_LineTool.hxx>
#include <HLRBRep_SurfaceTool.hxx>
#include <HLRBRep_ThePolygonOfInterCSurf.hxx>
#include <HLRBRep_ThePolygonToolOfInterCSurf.hxx>
#include <HLRBRep_ThePolyhedronOfInterCSurf.hxx>
#include <HLRBRep_ThePolyhedronToolOfInterCSurf.hxx>
#include <HLRBRep_TheInterferenceOfInterCSurf.hxx>
#include <HLRBRep_TheCSFunctionOfInterCSurf.hxx>
#include <HLRBRep_TheExactInterCSurf.hxx>
#include <HLRBRep_TheQuadCurvExactInterCSurf.hxx>
#include <HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Parab.hxx>
#include <gp_Hypr.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <Bnd_Box.hxx>
 

#define TheCurve gp_Lin
#define TheCurve_hxx <gp_Lin.hxx>
#define TheCurveTool HLRBRep_LineTool
#define TheCurveTool_hxx <HLRBRep_LineTool.hxx>
#define TheSurface Standard_Address
#define TheSurface_hxx <Standard_Address.hxx>
#define TheSurfaceTool HLRBRep_SurfaceTool
#define TheSurfaceTool_hxx <HLRBRep_SurfaceTool.hxx>
#define IntCurveSurface_ThePolygon HLRBRep_ThePolygonOfInterCSurf
#define IntCurveSurface_ThePolygon_hxx <HLRBRep_ThePolygonOfInterCSurf.hxx>
#define IntCurveSurface_ThePolygonTool HLRBRep_ThePolygonToolOfInterCSurf
#define IntCurveSurface_ThePolygonTool_hxx <HLRBRep_ThePolygonToolOfInterCSurf.hxx>
#define IntCurveSurface_ThePolyhedron HLRBRep_ThePolyhedronOfInterCSurf
#define IntCurveSurface_ThePolyhedron_hxx <HLRBRep_ThePolyhedronOfInterCSurf.hxx>
#define IntCurveSurface_ThePolyhedronTool HLRBRep_ThePolyhedronToolOfInterCSurf
#define IntCurveSurface_ThePolyhedronTool_hxx <HLRBRep_ThePolyhedronToolOfInterCSurf.hxx>
#define IntCurveSurface_TheInterference HLRBRep_TheInterferenceOfInterCSurf
#define IntCurveSurface_TheInterference_hxx <HLRBRep_TheInterferenceOfInterCSurf.hxx>
#define IntCurveSurface_TheCSFunction HLRBRep_TheCSFunctionOfInterCSurf
#define IntCurveSurface_TheCSFunction_hxx <HLRBRep_TheCSFunctionOfInterCSurf.hxx>
#define IntCurveSurface_TheExactInter HLRBRep_TheExactInterCSurf
#define IntCurveSurface_TheExactInter_hxx <HLRBRep_TheExactInterCSurf.hxx>
#define IntCurveSurface_TheQuadCurvExactInter HLRBRep_TheQuadCurvExactInterCSurf
#define IntCurveSurface_TheQuadCurvExactInter_hxx <HLRBRep_TheQuadCurvExactInterCSurf.hxx>
#define IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactInter HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf
#define IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactInter_hxx <HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf.hxx>
#define IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactInter HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf
#define IntCurveSurface_TheQuadCurvFuncOfTheQuadCurvExactInter_hxx <HLRBRep_TheQuadCurvFuncOfTheQuadCurvExactInterCSurf.hxx>
#define IntCurveSurface_Inter HLRBRep_InterCSurf
#define IntCurveSurface_Inter_hxx <HLRBRep_InterCSurf.hxx>
#include <IntCurveSurface_Inter.gxx>

