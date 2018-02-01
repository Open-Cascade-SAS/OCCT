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

#include <Extrema_ECC2d.hxx>

#include <StdFail_InfiniteSolutions.hxx>
#include <StdFail_NotDone.hxx>
#include <Standard_OutOfRange.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Extrema_Curve2dTool.hxx>
#include <Extrema_ExtPC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
 

#define Curve1 Adaptor2d_Curve2d
#define Curve1_hxx <Adaptor2d_Curve2d.hxx>
#define Tool1 Extrema_Curve2dTool
#define Tool1_hxx <Extrema_Curve2dTool.hxx>
#define Curve2 Adaptor2d_Curve2d
#define Curve2_hxx <Adaptor2d_Curve2d.hxx>
#define Tool2 Extrema_Curve2dTool
#define Tool2_hxx <Extrema_Curve2dTool.hxx>
#define Handle_ArrayOfPnt Handle(TColgp_HArray1OfPnt2d)
#define ArrayOfPnt TColgp_HArray1OfPnt2d
#define ArrayOfPnt_hxx <TColgp_HArray1OfPnt2d.hxx>
#define POnC Extrema_POnCurv2d
#define POnC_hxx <Extrema_POnCurv2d.hxx>
#define Pnt gp_Pnt2d
#define Pnt_hxx <gp_Pnt2d.hxx>
#define Vec gp_Vec2d
#define Vec_hxx <gp_Vec2d.hxx>
#define Extrema_GExtPC Extrema_ExtPC2d
#define Extrema_GenExtCC Extrema_ECC2d
#define Extrema_GenExtCC_hxx <Extrema_ECC2d.hxx>
#include <Extrema_GenExtCC.gxx>

