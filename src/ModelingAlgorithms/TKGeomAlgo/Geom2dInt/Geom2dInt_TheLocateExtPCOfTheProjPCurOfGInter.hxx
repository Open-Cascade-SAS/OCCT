// Created on: 1992-06-04
// Created by: Jacques GOUSSARD
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

#ifndef _Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter_HeaderFile
#define _Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter_HeaderFile

#include <Adaptor2d_Curve2d.hxx>
#include <Extrema_GenLocateExtPC.hxx>
#include <Extrema_POnCurv2d.hxx>
#include <Geom2dInt_Geom2dCurveTool.hxx>
#include <Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter.hxx>
#include <gp_Pnt2d.hxx>

//! Type alias for Geom2dInt local extremum search.
using Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter =
  Extrema_GenLocateExtPC<Adaptor2d_Curve2d,
                         Geom2dInt_Geom2dCurveTool,
                         Extrema_POnCurv2d,
                         gp_Pnt2d,
                         Geom2dInt_PCLocFOfTheLocateExtPCOfTheProjPCurOfGInter>;

#endif // _Geom2dInt_TheLocateExtPCOfTheProjPCurOfGInter_HeaderFile
