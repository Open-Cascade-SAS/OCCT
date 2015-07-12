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

#include <Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter.hxx>

#include <IntCurve_IConicTool.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
 

#define ImpTool IntCurve_IConicTool
#define ImpTool_hxx <IntCurve_IConicTool.hxx>
#define ParCurve Adaptor2d_Curve2d
#define ParCurve_hxx <Adaptor2d_Curve2d.hxx>
#define ParTool Geom2dInt_Geom2dCurveTool
#define ParTool_hxx <Geom2dInt_Geom2dCurveTool.hxx>
#define IntImpParGen_ImpParTool Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter
#define IntImpParGen_ImpParTool_hxx <Geom2dInt_MyImpParToolOfTheIntersectorOfTheIntConicCurveOfGInter.hxx>
#include <IntImpParGen_ImpParTool.gxx>

