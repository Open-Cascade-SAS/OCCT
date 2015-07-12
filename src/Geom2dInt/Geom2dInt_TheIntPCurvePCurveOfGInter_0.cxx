// Created on: 1992-06-04
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

#include <Geom2dInt_TheIntPCurvePCurveOfGInter.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <Geom2dInt_TheProjPCurOfGInter.hxx>
#include <Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter.hxx>
#include <Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter.hxx>
#include <Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter.hxx>
#include <IntRes2d_Domain.hxx>
 

#define TheCurve Adaptor2d_Curve2d
#define TheCurve_hxx <Adaptor2d_Curve2d.hxx>
#define TheCurveTool Geom2dInt_Geom2dCurveTool
#define TheCurveTool_hxx <Geom2dInt_Geom2dCurveTool.hxx>
#define TheProjPCur Geom2dInt_TheProjPCurOfGInter
#define TheProjPCur_hxx <Geom2dInt_TheProjPCurOfGInter.hxx>
#define IntCurve_ThePolygon2d Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter
#define IntCurve_ThePolygon2d_hxx <Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_TheDistBetweenPCurves Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter
#define IntCurve_TheDistBetweenPCurves_hxx <Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_ExactIntersectionPoint Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter
#define IntCurve_ExactIntersectionPoint_hxx <Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_IntPolyPolyGen Geom2dInt_TheIntPCurvePCurveOfGInter
#define IntCurve_IntPolyPolyGen_hxx <Geom2dInt_TheIntPCurvePCurveOfGInter.hxx>
#include <IntCurve_IntPolyPolyGen.gxx>

