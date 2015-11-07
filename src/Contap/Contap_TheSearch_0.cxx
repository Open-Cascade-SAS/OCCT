// Created on: 1993-02-05
// Created by: Jacques GOUSSARD
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

#include <Contap_TheSearch.hxx>

#include <StdFail_NotDone.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_ConstructionError.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <Adaptor2d_HCurve2d.hxx>
#include <Contap_HCurve2dTool.hxx>
#include <Contap_HContTool.hxx>
#include <Adaptor3d_TopolTool.hxx>
#include <Contap_ArcFunction.hxx>
#include <Contap_ThePathPointOfTheSearch.hxx>
#include <Contap_SequenceOfPathPointOfTheSearch.hxx>
#include <Contap_TheSegmentOfTheSearch.hxx>
#include <Contap_SequenceOfSegmentOfTheSearch.hxx>

#define TheVertex Handle(Adaptor3d_HVertex)
#define TheVertex_hxx <Adaptor3d_HVertex.hxx>
#define TheArc Handle(Adaptor2d_HCurve2d)
#define TheArc_hxx <Adaptor2d_HCurve2d.hxx>
#define TheArcTool Contap_HCurve2dTool
#define TheArcTool_hxx <Contap_HCurve2dTool.hxx>
#define TheSOBTool Contap_HContTool
#define TheSOBTool_hxx <Contap_HContTool.hxx>
#define Handle_TheTopolTool Handle(Adaptor3d_TopolTool)
#define TheTopolTool Adaptor3d_TopolTool
#define TheTopolTool_hxx <Adaptor3d_TopolTool.hxx>
#define TheFunction Contap_ArcFunction
#define TheFunction_hxx <Contap_ArcFunction.hxx>
#define IntStart_ThePathPoint Contap_ThePathPointOfTheSearch
#define IntStart_ThePathPoint_hxx <Contap_ThePathPointOfTheSearch.hxx>
#define IntStart_SequenceOfPathPoint Contap_SequenceOfPathPointOfTheSearch
#define IntStart_SequenceOfPathPoint_hxx <Contap_SequenceOfPathPointOfTheSearch.hxx>
#define IntStart_TheSegment Contap_TheSegmentOfTheSearch
#define IntStart_TheSegment_hxx <Contap_TheSegmentOfTheSearch.hxx>
#define IntStart_SequenceOfSegment Contap_SequenceOfSegmentOfTheSearch
#define IntStart_SequenceOfSegment_hxx <Contap_SequenceOfSegmentOfTheSearch.hxx>
#define IntStart_SearchOnBoundaries Contap_TheSearch
#define IntStart_SearchOnBoundaries_hxx <Contap_TheSearch.hxx>
#include <IntStart_SearchOnBoundaries.gxx>

