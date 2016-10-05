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

#include <HLRBRep_CLProps.hxx>

#include <LProp_BadContinuity.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <LProp_NotDefined.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <HLRBRep_CLPropsATool.hxx>
 

#define Curve HLRBRep_Curve*
#define Curve_hxx <HLRBRep_Curve.hxx>
#define Vec gp_Vec2d
#define Vec_hxx <gp_Vec2d.hxx>
#define Pnt gp_Pnt2d
#define Pnt_hxx <gp_Pnt2d.hxx>
#define Dir gp_Dir2d
#define Dir_hxx <gp_Dir2d.hxx>
#define Tool HLRBRep_CLPropsATool
#define Tool_hxx <HLRBRep_CLPropsATool.hxx>
#define LProp_CLProps HLRBRep_CLProps
#define LProp_CLProps_hxx <HLRBRep_CLProps.hxx>
#include <LProp_CLProps.gxx>

