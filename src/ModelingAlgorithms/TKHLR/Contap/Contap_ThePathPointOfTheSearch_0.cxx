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

#include <Contap_ThePathPointOfTheSearch.hxx>

#include <Adaptor3d_HVertex.hxx>
#include <Adaptor2d_Curve2d.hxx>
#include <Standard_DomainError.hxx>
#include <gp_Pnt.hxx>

#define TheVertex Handle(Adaptor3d_HVertex)
#define TheVertex_hxx <Adaptor3d_HVertex.hxx>
#define TheArc Handle(Adaptor2d_Curve2d)
#define TheArc_hxx <Adaptor2d_Curve2d.hxx>
#define IntStart_PathPoint Contap_ThePathPointOfTheSearch
#define IntStart_PathPoint_hxx <Contap_ThePathPointOfTheSearch.hxx>
#include "../IntStart/IntStart_PathPoint.gxx"
