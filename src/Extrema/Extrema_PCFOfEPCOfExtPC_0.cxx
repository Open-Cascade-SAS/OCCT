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

#include <Extrema_PCFOfEPCOfExtPC.hxx>

#include <Standard_OutOfRange.hxx>
#include <Standard_TypeMismatch.hxx>
#include <Adaptor3d_Curve.hxx>
#include <Extrema_CurveTool.hxx>
#include <Extrema_POnCurv.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Extrema_SequenceOfPOnCurv.hxx> 

#define Curve Adaptor3d_Curve
#define Curve_hxx <Adaptor3d_Curve.hxx>
#define Tool Extrema_CurveTool
#define Tool_hxx <Extrema_CurveTool.hxx>
#define POnC Extrema_POnCurv
#define POnC_hxx <Extrema_POnCurv.hxx>
#define Pnt gp_Pnt
#define Pnt_hxx <gp_Pnt.hxx>
#define Vec gp_Vec
#define Vec_hxx <gp_Vec.hxx>
#define Extrema_SeqPC Extrema_SequenceOfPOnCurv
#define Extrema_SeqPC_hxx <Extrema_SequenceOfPOnCurv.hxx>
#define Extrema_FuncExtPC Extrema_PCFOfEPCOfExtPC
#define Extrema_FuncExtPC_hxx <Extrema_PCFOfEPCOfExtPC.hxx>
#include <Extrema_FuncExtPC.gxx>

