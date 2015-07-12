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

#include <Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <gp_Pnt2d.hxx>
 

#define Curve1 Adaptor2d_Curve2d
#define Curve1_hxx <Adaptor2d_Curve2d.hxx>
#define Tool1 Geom2dInt_Geom2dCurveTool
#define Tool1_hxx <Geom2dInt_Geom2dCurveTool.hxx>
#define Curve2 Adaptor2d_Curve2d
#define Curve2_hxx <Adaptor2d_Curve2d.hxx>
#define Tool2 Geom2dInt_Geom2dCurveTool
#define Tool2_hxx <Geom2dInt_Geom2dCurveTool.hxx>
#define POnC Extrema_POnCurv2d
#define POnC_hxx <Extrema_POnCurv2d.hxx>
#define Pnt gp_Pnt2d
#define Pnt_hxx <gp_Pnt2d.hxx>
#define Extrema_CurveLocator Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter
#define Extrema_CurveLocator_hxx <Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter.hxx>
#include <Extrema_CurveLocator.gxx>

