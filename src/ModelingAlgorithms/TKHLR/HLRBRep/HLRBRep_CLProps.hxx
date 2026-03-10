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

#ifndef _HLRBRep_CLProps_HeaderFile
#define _HLRBRep_CLProps_HeaderFile

#include <GeomLProp_CLProps.hxx>
#include <HLRBRep_CLPropsATool.hxx>
#include <HLRBRep_Curve.hxx>

//! Alias for 2D curve local properties using HLRBRep_Curve with ToolAccess.
using HLRBRep_CLProps = GeomLProp_CLPropsBase<gp_Pnt2d,
                                             gp_Vec2d,
                                             gp_Dir2d,
                                             const HLRBRep_Curve*,
                                             LProp_CurveUtils::ToolAccess<HLRBRep_CLPropsATool>>;

#endif // _HLRBRep_CLProps_HeaderFile
