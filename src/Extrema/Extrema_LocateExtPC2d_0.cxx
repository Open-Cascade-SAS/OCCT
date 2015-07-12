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

#include <Extrema_LocateExtPC2d.hxx>

#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Extrema_Curve2dTool.hxx>
#include <Extrema_ExtPElC2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <Extrema_ELPCOfLocateExtPC2d.hxx>
#include <Extrema_EPCOfELPCOfLocateExtPC2d.hxx>
#include <Extrema_LocEPCOfLocateExtPC2d.hxx>
#include <Extrema_PCLocFOfLocEPCOfLocateExtPC2d.hxx>
 

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
#define Extrema_ELPC Extrema_ELPCOfLocateExtPC2d
#define Extrema_ELPC_hxx <Extrema_ELPCOfLocateExtPC2d.hxx>
#define Extrema_EPCOfELPC Extrema_EPCOfELPCOfLocateExtPC2d
#define Extrema_EPCOfELPC_hxx <Extrema_EPCOfELPCOfLocateExtPC2d.hxx>
#define Extrema_EPCOfELPC Extrema_EPCOfELPCOfLocateExtPC2d
#define Extrema_EPCOfELPC_hxx <Extrema_EPCOfELPCOfLocateExtPC2d.hxx>
#define Extrema_LocEPC Extrema_LocEPCOfLocateExtPC2d
#define Extrema_LocEPC_hxx <Extrema_LocEPCOfLocateExtPC2d.hxx>
#define Extrema_PCLocFOfLocEPC Extrema_PCLocFOfLocEPCOfLocateExtPC2d
#define Extrema_PCLocFOfLocEPC_hxx <Extrema_PCLocFOfLocEPCOfLocateExtPC2d.hxx>
#define Extrema_PCLocFOfLocEPC Extrema_PCLocFOfLocEPCOfLocateExtPC2d
#define Extrema_PCLocFOfLocEPC_hxx <Extrema_PCLocFOfLocEPCOfLocateExtPC2d.hxx>
#define Extrema_GLocateExtPC Extrema_LocateExtPC2d
#define Extrema_GLocateExtPC_hxx <Extrema_LocateExtPC2d.hxx>
#include <Extrema_GLocateExtPC.gxx>

