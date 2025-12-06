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

#ifndef _Extrema_LocEPCOfLocateExtPC2d_HeaderFile
#define _Extrema_LocEPCOfLocateExtPC2d_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Extrema_Curve2dTool.hxx>
#include <Extrema_GenLocateExtPC.hxx>
#include <Extrema_PCLocFOfLocEPCOfLocateExtPC2d.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <gp_Pnt2d.hxx>

//! Type alias for 2D curve local extremum search.
using Extrema_LocEPCOfLocateExtPC2d = Extrema_GenLocateExtPC<Adaptor2d_Curve2d,
                                                             Extrema_Curve2dTool,
                                                             Extrema_POnCurv2d,
                                                             gp_Pnt2d,
                                                             Extrema_PCLocFOfLocEPCOfLocateExtPC2d>;

#endif // _Extrema_LocEPCOfLocateExtPC2d_HeaderFile
