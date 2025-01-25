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

#include <Geom2dInt_GInter.hxx>

#include <Standard_ConstructionError.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <Geom2dInt_TheProjPCurOfGInter.hxx>
#include <Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter.hxx>
#include <Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter.hxx>
#include <Geom2dInt_TheIntConicCurveOfGInter.hxx>
#include <Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter.hxx>
#include <Geom2dInt_IntConicCurveOfGInter.hxx>
#include <Geom2dInt_TheIntPCurvePCurveOfGInter.hxx>
#include <Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter.hxx>
#include <Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter.hxx>
#include <Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter.hxx>
#include <IntRes2d_Domain.hxx>

#define TheCurve Adaptor2d_Curve2d
#define TheCurve_hxx <Adaptor2d_Curve2d.hxx>
#define TheCurveTool Geom2dInt_Geom2dCurveTool
#define TheCurveTool_hxx <Geom2dInt_Geom2dCurveTool.hxx>
#define IntCurve_TheProjPCur Geom2dInt_TheProjPCurOfGInter
#define IntCurve_TheProjPCur_hxx <Geom2dInt_TheProjPCurOfGInter.hxx>
#define IntCurve_TheCurveLocatorOfTheProjPCur Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter
#define IntCurve_TheCurveLocatorOfTheProjPCur_hxx                                                  \
  <Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter.hxx>
#define IntCurve_TheLocateExtPCOfTheProjPCur Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter
#define IntCurve_TheLocateExtPCOfTheProjPCur_hxx                                                   \
  <Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter.hxx>
#define IntCurve_TheCurveLocatorOfTheProjPCur Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter
#define IntCurve_TheCurveLocatorOfTheProjPCur_hxx                                                  \
  <Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter.hxx>
#define IntCurve_TheLocateExtPCOfTheProjPCur Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter
#define IntCurve_TheLocateExtPCOfTheProjPCur_hxx                                                   \
  <Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter.hxx>
#define IntCurve_TheIntConicCurve Geom2dInt_TheIntConicCurveOfGInter
#define IntCurve_TheIntConicCurve_hxx <Geom2dInt_TheIntConicCurveOfGInter.hxx>
#define IntCurve_TheIntersectorOfTheIntConicCurve Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter
#define IntCurve_TheIntersectorOfTheIntConicCurve_hxx                                              \
  <Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter.hxx>
#define IntCurve_TheIntersectorOfTheIntConicCurve Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter
#define IntCurve_TheIntersectorOfTheIntConicCurve_hxx                                              \
  <Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter.hxx>
#define IntCurve_IntConicCurve Geom2dInt_IntConicCurveOfGInter
#define IntCurve_IntConicCurve_hxx <Geom2dInt_IntConicCurveOfGInter.hxx>
#define IntCurve_TheIntPCurvePCurve Geom2dInt_TheIntPCurvePCurveOfGInter
#define IntCurve_TheIntPCurvePCurve_hxx <Geom2dInt_TheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve_hxx                                              \
  <Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve                                         \
  Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve_hxx                                     \
  <Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve                                        \
  Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve_hxx                                    \
  <Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter
#define IntCurve_ThePolygon2dOfTheIntPCurvePCurve_hxx                                              \
  <Geom2dInt_ThePolygon2dOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve                                         \
  Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter
#define IntCurve_TheDistBetweenPCurvesOfTheIntPCurvePCurve_hxx                                     \
  <Geom2dInt_TheDistBetweenPCurvesOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve                                        \
  Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter
#define IntCurve_ExactIntersectionPointOfTheIntPCurvePCurve_hxx                                    \
  <Geom2dInt_ExactIntersectionPointOfTheIntPCurvePCurveOfGInter.hxx>
#define IntCurve_IntCurveCurveGen Geom2dInt_GInter
#define IntCurve_IntCurveCurveGen_hxx <Geom2dInt_GInter.hxx>
#include "../IntCurve/IntCurve_IntCurveCurveGen.gxx"
