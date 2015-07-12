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

#include <Geom2dInt_TheProjPCurOfGInter.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter.hxx>
#include <Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter.hxx>
#include <Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter.hxx>
#include <gp_Pnt2d.hxx>
 

#define TheCurve Adaptor2d_Curve2d
#define TheCurve_hxx <Adaptor2d_Curve2d.hxx>
#define TheCurveTool Geom2dInt_Geom2dCurveTool
#define TheCurveTool_hxx <Geom2dInt_Geom2dCurveTool.hxx>
#define IntCurve_TheCurveLocator Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter
#define IntCurve_TheCurveLocator_hxx <Geom2dInt_TheCurveLocatorOfTheProjPCurOfGInter.hxx>
#define IntCurve_TheLocateExtPC Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter
#define IntCurve_TheLocateExtPC_hxx <Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter.hxx>
#define IntCurve_PCLocFOfTheLocateExtPC Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter
#define IntCurve_PCLocFOfTheLocateExtPC_hxx <Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter.hxx>
#define IntCurve_PCLocFOfTheLocateExtPC Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter
#define IntCurve_PCLocFOfTheLocateExtPC_hxx <Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter.hxx>
#define IntCurve_ProjPCurGen Geom2dInt_TheProjPCurOfGInter
#define IntCurve_ProjPCurGen_hxx <Geom2dInt_TheProjPCurOfGInter.hxx>
#include <IntCurve_ProjPCurGen.gxx>

