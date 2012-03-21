// Created on: 1992-05-07
// Created by: Jacques GOUSSARD
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


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
