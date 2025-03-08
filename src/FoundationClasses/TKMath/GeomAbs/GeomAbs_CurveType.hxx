// Created on: 1993-02-22
// Created by: Modelistation
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _GeomAbs_CurveType_HeaderFile
#define _GeomAbs_CurveType_HeaderFile

//! Identifies the type of a curve.
enum GeomAbs_CurveType
{
  GeomAbs_Line,
  GeomAbs_Circle,
  GeomAbs_Ellipse,
  GeomAbs_Hyperbola,
  GeomAbs_Parabola,
  GeomAbs_BezierCurve,
  GeomAbs_BSplineCurve,
  GeomAbs_OffsetCurve,
  GeomAbs_OtherCurve
};

#endif // _GeomAbs_CurveType_HeaderFile
