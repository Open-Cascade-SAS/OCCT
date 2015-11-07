// Created on: 1992-05-06
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

#include <IntPatch_TheSOnBounds.hxx>

#include <StdFail_NotDone.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ConstructionError.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <IntPatch_HCurve2dTool.hxx>
#include <IntPatch_HInterTool.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <IntPatch_ArcFunction.hxx>
#include <IntPatch_ThePathPointOfTheSOnBounds.hxx>
#include <IntPatch_SequenceOfPathPointOfTheSOnBounds.hxx>
#include <IntPatch_TheSegmentOfTheSOnBounds.hxx>
#include <IntPatch_SequenceOfSegmentOfTheSOnBounds.hxx> 

#define TheVertex Handle(Adaptor3d_HVertex)
#define TheVertex_hxx <Adaptor3d_HVertex.hxx>
#define TheArc Handle(Adaptor2d_HCurve2d)
#define TheArc_hxx <Adaptor2d_HCurve2d.hxx>
#define TheArcTool IntPatch_HCurve2dTool
#define TheArcTool_hxx <IntPatch_HCurve2dTool.hxx>
#define TheSOBTool IntPatch_HInterTool
#define TheSOBTool_hxx <IntPatch_HInterTool.hxx>
#define Handle_TheTopolTool Handle(Adaptor3d_TopolTool)
#define TheTopolTool Adaptor3d_TopolTool
#define TheTopolTool_hxx <Adaptor3d_TopolTool.hxx>
#define TheFunction IntPatch_ArcFunction
#define TheFunction_hxx <IntPatch_ArcFunction.hxx>
#define IntStart_ThePathPoint IntPatch_ThePathPointOfTheSOnBounds
#define IntStart_ThePathPoint_hxx <IntPatch_ThePathPointOfTheSOnBounds.hxx>
#define IntStart_SequenceOfPathPoint IntPatch_SequenceOfPathPointOfTheSOnBounds
#define IntStart_SequenceOfPathPoint_hxx <IntPatch_SequenceOfPathPointOfTheSOnBounds.hxx>
#define IntStart_TheSegment IntPatch_TheSegmentOfTheSOnBounds
#define IntStart_TheSegment_hxx <IntPatch_TheSegmentOfTheSOnBounds.hxx>
#define IntStart_SequenceOfSegment IntPatch_SequenceOfSegmentOfTheSOnBounds
#define IntStart_SequenceOfSegment_hxx <IntPatch_SequenceOfSegmentOfTheSOnBounds.hxx>
#define IntStart_SearchOnBoundaries IntPatch_TheSOnBounds
#define IntStart_SearchOnBoundaries_hxx <IntPatch_TheSOnBounds.hxx>
#include <IntStart_SearchOnBoundaries.gxx>

