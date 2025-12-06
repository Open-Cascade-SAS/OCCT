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

#ifndef _HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter_HeaderFile
#define _HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter_HeaderFile

#include <Extrema_GCurveLocator.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <gp_Pnt2d.hxx>
#include <HLRBRep_CurveTool.hxx>
#include <Standard_TypeDef.hxx>

//! Type alias for curve locator using HLRBRep_CurveTool.
using HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter =
  Extrema_GCurveLocator<Standard_Address, HLRBRep_CurveTool, Extrema_POnCurv2d, gp_Pnt2d>;

#endif // _HLRBRep_TheCurveLocatorOfTheProjPCurOfCInter_HeaderFile
