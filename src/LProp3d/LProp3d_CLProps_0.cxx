// Created on: 2002-08-02
// Created by: Alexander KARTOMIN  (akm)
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#include <LProp3d_CLProps.hxx>

#include <Adaptor3d_HCurve.hxx>
#include <LProp_BadContinuity.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <LProp_NotDefined.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <LProp3d_CurveTool.hxx>
 

#define Curve Handle(Adaptor3d_HCurve)
#define Curve_hxx <Adaptor3d_HCurve.hxx>
#define Vec gp_Vec
#define Vec_hxx <gp_Vec.hxx>
#define Pnt gp_Pnt
#define Pnt_hxx <gp_Pnt.hxx>
#define Dir gp_Dir
#define Dir_hxx <gp_Dir.hxx>
#define Tool LProp3d_CurveTool
#define Tool_hxx <LProp3d_CurveTool.hxx>
#define LProp_CLProps LProp3d_CLProps
#define LProp_CLProps_hxx <LProp3d_CLProps.hxx>
#include <LProp_CLProps.gxx>

