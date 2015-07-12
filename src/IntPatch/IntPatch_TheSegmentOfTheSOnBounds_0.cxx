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

#include <IntPatch_TheSegmentOfTheSOnBounds.hxx>

#include <Adaptor2d_HCurve2d.hxx>
#include <Standard_DomainError.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <IntPatch_ThePathPointOfTheSOnBounds.hxx>
 

#define TheVertex Handle(Adaptor3d_HVertex)
#define TheVertex_hxx <Adaptor3d_HVertex.hxx>
#define TheArc Handle(Adaptor2d_HCurve2d)
#define TheArc_hxx <Adaptor2d_HCurve2d.hxx>
#define ThePathPoint IntPatch_ThePathPointOfTheSOnBounds
#define ThePathPoint_hxx <IntPatch_ThePathPointOfTheSOnBounds.hxx>
#define IntStart_Segment IntPatch_TheSegmentOfTheSOnBounds
#define IntStart_Segment_hxx <IntPatch_TheSegmentOfTheSOnBounds.hxx>
#include <IntStart_Segment.gxx>

