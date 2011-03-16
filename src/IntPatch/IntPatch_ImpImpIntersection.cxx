// File:      IntPatch_ImpImpIntersection.cxx
// Created:   Thu May  7 08:47:45 1992
// Author:    Jacques GOUSSARD
// Copyright: OPEN CASCADE 1992

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
