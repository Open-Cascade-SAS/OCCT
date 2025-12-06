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

#ifndef _Extrema_PCFOfEPCOfExtPC_HeaderFile
#define _Extrema_PCFOfEPCOfExtPC_HeaderFile

#include <Adaptor3d_Curve.hxx>
#include <Extrema_CurveTool.hxx>
#include <Extrema_GFuncExtPC.hxx>
#include <Extrema_POnCurv.hxx>
#include <Extrema_SequenceOfPOnCurv.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//! Type alias for 3D point-curve extremum function.
using Extrema_PCFOfEPCOfExtPC = Extrema_GFuncExtPC<Adaptor3d_Curve,
                                                   Extrema_CurveTool,
                                                   Extrema_POnCurv,
                                                   gp_Pnt,
                                                   gp_Vec,
                                                   Extrema_SequenceOfPOnCurv>;

#endif // _Extrema_PCFOfEPCOfExtPC_HeaderFile
