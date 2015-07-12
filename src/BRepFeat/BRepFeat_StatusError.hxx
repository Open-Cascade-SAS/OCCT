// Created on: 1995-06-13
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

#ifndef _BRepFeat_StatusError_HeaderFile
#define _BRepFeat_StatusError_HeaderFile

//! Discribes the error.
enum BRepFeat_StatusError
{
BRepFeat_OK,
BRepFeat_BadDirect,
BRepFeat_BadIntersect,
BRepFeat_EmptyBaryCurve,
BRepFeat_EmptyCutResult,
BRepFeat_FalseSide,
BRepFeat_IncDirection,
BRepFeat_IncSlidFace,
BRepFeat_IncParameter,
BRepFeat_IncTypes,
BRepFeat_IntervalOverlap,
BRepFeat_InvFirstShape,
BRepFeat_InvOption,
BRepFeat_InvShape,
BRepFeat_LocOpeNotDone,
BRepFeat_LocOpeInvNotDone,
BRepFeat_NoExtFace,
BRepFeat_NoFaceProf,
BRepFeat_NoGluer,
BRepFeat_NoIntersectF,
BRepFeat_NoIntersectU,
BRepFeat_NoParts,
BRepFeat_NoProjPt,
BRepFeat_NotInitialized,
BRepFeat_NotYetImplemented,
BRepFeat_NullRealTool,
BRepFeat_NullToolF,
BRepFeat_NullToolU
};

#endif // _BRepFeat_StatusError_HeaderFile
