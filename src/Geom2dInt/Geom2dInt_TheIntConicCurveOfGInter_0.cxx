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

#include <Geom2dInt_TheIntConicCurveOfGInter.hxx>

#include <IntCurve_IConicTool.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <Geom2dInt_TheProjPCurOfGInter.hxx>
#include <Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter.hxx>
#include <Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter.hxx>
#include <gp_Lin2d.hxx>
#include <IntRes2d_Domain.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Hypr2d.hxx>
 

#define TheImpTool IntCurve_IConicTool
#define TheImpTool_hxx <IntCurve_IConicTool.hxx>
#define ThePCurve Adaptor2d_Curve2d
#define ThePCurve_hxx <Adaptor2d_Curve2d.hxx>
#define ThePCurveTool Geom2dInt_Geom2dCurveTool
#define ThePCurveTool_hxx <Geom2dInt_Geom2dCurveTool.hxx>
#define TheProjPCur Geom2dInt_TheProjPCurOfGInter
#define TheProjPCur_hxx <Geom2dInt_TheProjPCurOfGInter.hxx>
#define IntCurve_TheIntersector Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter
#define IntCurve_TheIntersector_hxx <Geom2dInt_TheIntersectorOfTheIntConicCurveOfGInter.hxx>
#define IntCurve_MyImpParToolOfTheIntersector Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter
#define IntCurve_MyImpParToolOfTheIntersector_hxx <Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter.hxx>
#define IntCurve_MyImpParToolOfTheIntersector Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter
#define IntCurve_MyImpParToolOfTheIntersector_hxx <Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter.hxx>
#define IntCurve_IntConicCurveGen Geom2dInt_TheIntConicCurveOfGInter
#define IntCurve_IntConicCurveGen_hxx <Geom2dInt_TheIntConicCurveOfGInter.hxx>
#include <IntCurve_IntConicCurveGen.gxx>

