// Created on: 1995-12-06
// Created by: Jacques GOUSSARD
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

#ifndef _BRepCheck_Status_HeaderFile
#define _BRepCheck_Status_HeaderFile


enum BRepCheck_Status
{
BRepCheck_NoError,
BRepCheck_InvalidPointOnCurve,
BRepCheck_InvalidPointOnCurveOnSurface,
BRepCheck_InvalidPointOnSurface,
BRepCheck_No3DCurve,
BRepCheck_Multiple3DCurve,
BRepCheck_Invalid3DCurve,
BRepCheck_NoCurveOnSurface,
BRepCheck_InvalidCurveOnSurface,
BRepCheck_InvalidCurveOnClosedSurface,
BRepCheck_InvalidSameRangeFlag,
BRepCheck_InvalidSameParameterFlag,
BRepCheck_InvalidDegeneratedFlag,
BRepCheck_FreeEdge,
BRepCheck_InvalidMultiConnexity,
BRepCheck_InvalidRange,
BRepCheck_EmptyWire,
BRepCheck_RedundantEdge,
BRepCheck_SelfIntersectingWire,
BRepCheck_NoSurface,
BRepCheck_InvalidWire,
BRepCheck_RedundantWire,
BRepCheck_IntersectingWires,
BRepCheck_InvalidImbricationOfWires,
BRepCheck_EmptyShell,
BRepCheck_RedundantFace,
BRepCheck_InvalidImbricationOfShells,
BRepCheck_UnorientableShape,
BRepCheck_NotClosed,
BRepCheck_NotConnected,
BRepCheck_SubshapeNotInShape,
BRepCheck_BadOrientation,
BRepCheck_BadOrientationOfSubshape,
BRepCheck_InvalidPolygonOnTriangulation,
BRepCheck_InvalidToleranceValue,
BRepCheck_EnclosedRegion,
BRepCheck_CheckFail
};

#endif // _BRepCheck_Status_HeaderFile
