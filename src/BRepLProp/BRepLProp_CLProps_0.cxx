// Created on: 1994-02-24
// Created by: Laurent BOURESCHE
// Copyright (c) 1994-1999 Matra Datavision
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

#include <BRepLProp_CLProps.hxx>

#include <LProp_BadContinuity.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <LProp_NotDefined.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <gp_Dir.hxx>
#include <BRepLProp_CurveTool.hxx>
 

#define Curve BRepAdaptor_Curve
#define Curve_hxx <BRepAdaptor_Curve.hxx>
#define Vec gp_Vec
#define Vec_hxx <gp_Vec.hxx>
#define Pnt gp_Pnt
#define Pnt_hxx <gp_Pnt.hxx>
#define Dir gp_Dir
#define Dir_hxx <gp_Dir.hxx>
#define Tool BRepLProp_CurveTool
#define Tool_hxx <BRepLProp_CurveTool.hxx>
#define LProp_CLProps BRepLProp_CLProps
#define LProp_CLProps_hxx <BRepLProp_CLProps.hxx>
#include <LProp_CLProps.gxx>

