// Created on: 1992-05-07
// Created by: Jacques GOUSSARD
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

#include <IntPatch_ImpImpIntersection.ixx>

#include <Standard_ConstructionError.hxx>
#include <IntPatch_SequenceOfLine.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_SequenceOfReal.hxx>
#include <TColgp_SequenceOfPnt.hxx>

#include <IntSurf_Quadric.hxx>

#include <ElCLib.hxx>
#include <ElSLib.hxx>
#include <gp.hxx>
#include <IntAna_Quadric.hxx>
#include <IntAna_QuadQuadGeo.hxx>
#include <IntAna_IntQuadQuad.hxx>
#include <gp_Pln.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Cone.hxx>
#include <IntSurf.hxx>

#include <Adaptor3d_HVertex.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <IntPatch_HInterTool.hxx>
#include <IntPatch_ArcFunction.hxx>
#include <IntPatch_GLine.hxx>
#include <IntPatch_ALine.hxx>
#include <IntPatch_RLine.hxx>
#include <IntPatch_ThePathPointOfTheSOnBounds.hxx>
#include <IntPatch_TheSegmentOfTheSOnBounds.hxx>

#include <IntPatch_ImpImpIntersection_0.gxx>
#include <IntPatch_ImpImpIntersection_1.gxx>
#include <IntPatch_ImpImpIntersection_2.gxx>
#include <IntPatch_ImpImpIntersection_3.gxx>
#include <IntPatch_ImpImpIntersection_4.gxx>
#include <IntPatch_ImpImpIntersection_5.gxx>
#include <IntPatch_ImpImpIntersection_6.gxx>
