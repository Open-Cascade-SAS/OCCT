// Created on: 1995-07-19
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Adaptor3d_Curve.hxx>
#include <Extrema_CurveTool.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

//=======================================================================
//function : IsPeriodic
//purpose  : 
//=======================================================================
Standard_Boolean Extrema_CurveTool::IsPeriodic(const Adaptor3d_Curve& C)
{
  GeomAbs_CurveType aType = GetType(C);
  if (aType == GeomAbs_Circle ||
      aType == GeomAbs_Ellipse)
    return Standard_True;
  else
    return C.IsPeriodic();
}


