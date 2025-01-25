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

#include <Extrema_LocateExtPC.hxx>

#include <Standard_DomainError.hxx>
#include <StdFail_NotDone.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Extrema_CurveTool.hxx>
#include <Extrema_ExtPElC.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_ELPCOfLocateExtPC.hxx>
#include <Extrema_EPCOfELPCOfLocateExtPC.hxx>
#include <Extrema_LocEPCOfLocateExtPC.hxx>
#include <Extrema_PCLocFOfLocEPCOfLocateExtPC.hxx>

#define TheCurve Adaptor3d_Curve
#define TheCurve_hxx <Adaptor3d_Curve.hxx>
#define TheCurveTool Extrema_CurveTool
#define TheCurveTool_hxx <Extrema_CurveTool.hxx>
#define TheExtPElC Extrema_ExtPElC
#define TheExtPElC_hxx <Extrema_ExtPElC.hxx>
#define ThePoint gp_Pnt
#define ThePoint_hxx <gp_Pnt.hxx>
#define TheVector gp_Vec
#define TheVector_hxx <gp_Vec.hxx>
#define ThePOnC Extrema_POnCurv
#define ThePOnC_hxx <Extrema_POnCurv.hxx>
#define TheSequenceOfPOnC Extrema_SequenceOfPOnCurv
#define TheSequenceOfPOnC_hxx <Extrema_SequenceOfPOnCurv.hxx>
#define Extrema_ELPC Extrema_ELPCOfLocateExtPC
#define Extrema_ELPC_hxx <Extrema_ELPCOfLocateExtPC.hxx>
#define Extrema_EPCOfELPC Extrema_EPCOfELPCOfLocateExtPC
#define Extrema_EPCOfELPC_hxx <Extrema_EPCOfELPCOfLocateExtPC.hxx>
#define Extrema_EPCOfELPC Extrema_EPCOfELPCOfLocateExtPC
#define Extrema_EPCOfELPC_hxx <Extrema_EPCOfELPCOfLocateExtPC.hxx>
#define Extrema_LocEPC Extrema_LocEPCOfLocateExtPC
#define Extrema_LocEPC_hxx <Extrema_LocEPCOfLocateExtPC.hxx>
#define Extrema_PCLocFOfLocEPC Extrema_PCLocFOfLocEPCOfLocateExtPC
#define Extrema_PCLocFOfLocEPC_hxx <Extrema_PCLocFOfLocEPCOfLocateExtPC.hxx>
#define Extrema_PCLocFOfLocEPC Extrema_PCLocFOfLocEPCOfLocateExtPC
#define Extrema_PCLocFOfLocEPC_hxx <Extrema_PCLocFOfLocEPCOfLocateExtPC.hxx>
#define Extrema_GLocateExtPC Extrema_LocateExtPC
#define Extrema_GLocateExtPC_hxx <Extrema_LocateExtPC.hxx>
#include "../Extrema/Extrema_GLocateExtPC.gxx"
