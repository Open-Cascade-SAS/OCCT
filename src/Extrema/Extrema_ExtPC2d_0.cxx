// Created on: 1991-02-26
// Created by: Isabelle GRIGNON
// Copyright (c) 1991-1999 Matra Datavision
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

#include <Extrema_ExtPC2d.hxx>

#include <StdFail_NotDone.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Extrema_Curve2dTool.hxx>
#include <Extrema_ExtPElC2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <Extrema_EPCOfExtPC2d.hxx>
#include <Extrema_PCFOfEPCOfExtPC2d.hxx>
 

#define TheCurve Adaptor2d_Curve2d
#define TheCurve_hxx <Adaptor2d_Curve2d.hxx>
#define TheCurveTool Extrema_Curve2dTool
#define TheCurveTool_hxx <Extrema_Curve2dTool.hxx>
#define TheExtPElC Extrema_ExtPElC2d
#define TheExtPElC_hxx <Extrema_ExtPElC2d.hxx>
#define ThePoint gp_Pnt2d
#define ThePoint_hxx <gp_Pnt2d.hxx>
#define TheVector gp_Vec2d
#define TheVector_hxx <gp_Vec2d.hxx>
#define ThePOnC Extrema_POnCurv2d
#define ThePOnC_hxx <Extrema_POnCurv2d.hxx>
#define TheSequenceOfPOnC Extrema_SequenceOfPOnCurv2d
#define TheSequenceOfPOnC_hxx <Extrema_SequenceOfPOnCurv2d.hxx>
#define Extrema_EPC Extrema_EPCOfExtPC2d
#define Extrema_EPC_hxx <Extrema_EPCOfExtPC2d.hxx>
#define Extrema_PCFOfEPC Extrema_PCFOfEPCOfExtPC2d
#define Extrema_PCFOfEPC_hxx <Extrema_PCFOfEPCOfExtPC2d.hxx>
#define Extrema_PCFOfEPC Extrema_PCFOfEPCOfExtPC2d
#define Extrema_PCFOfEPC_hxx <Extrema_PCFOfEPCOfExtPC2d.hxx>
#define Extrema_GExtPC Extrema_ExtPC2d
#define Extrema_GExtPC_hxx <Extrema_ExtPC2d.hxx>
#include <Extrema_GExtPC.gxx>

