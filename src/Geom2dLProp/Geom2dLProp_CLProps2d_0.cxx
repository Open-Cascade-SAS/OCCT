// Created on: 1992-03-26
// Created by: Herve LEGRAND
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

#include <Geom2dLProp_CLProps2d.hxx>

#include <Geom2d_Curve.hxx>
#include <LProp_BadContinuity.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <LProp_NotDefined.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <Geom2dLProp_Curve2dTool.hxx>

#define Curve Handle(Geom2d_Curve)
#define Curve_hxx <Geom2d_Curve.hxx>
#define Vec gp_Vec2d
#define Vec_hxx <gp_Vec2d.hxx>
#define Pnt gp_Pnt2d
#define Pnt_hxx <gp_Pnt2d.hxx>
#define Dir gp_Dir2d
#define Dir_hxx <gp_Dir2d.hxx>
#define Tool Geom2dLProp_Curve2dTool
#define Tool_hxx <Geom2dLProp_Curve2dTool.hxx>
#define LProp_CLProps Geom2dLProp_CLProps2d
#define LProp_CLProps_hxx <Geom2dLProp_CLProps2d.hxx>
#include "../LProp/LProp_CLProps.gxx"
