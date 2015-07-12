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

#include <HLRBRep_TheCSFunctionOfInterCSurf.hxx>

#include <HLRBRep_SurfaceTool.hxx>
#include <gp_Lin.hxx>
#include <HLRBRep_LineTool.hxx>
#include <math_Matrix.hxx>
#include <gp_Pnt.hxx>
 

#define ThePSurface Standard_Address
#define ThePSurface_hxx <Standard_Address.hxx>
#define ThePSurfaceTool HLRBRep_SurfaceTool
#define ThePSurfaceTool_hxx <HLRBRep_SurfaceTool.hxx>
#define TheCurve gp_Lin
#define TheCurve_hxx <gp_Lin.hxx>
#define TheCurveTool HLRBRep_LineTool
#define TheCurveTool_hxx <HLRBRep_LineTool.hxx>
#define IntImp_ZerCSParFunc HLRBRep_TheCSFunctionOfInterCSurf
#define IntImp_ZerCSParFunc_hxx <HLRBRep_TheCSFunctionOfInterCSurf.hxx>
#include <IntImp_ZerCSParFunc.gxx>

