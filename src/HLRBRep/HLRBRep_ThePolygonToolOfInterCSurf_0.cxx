// Created on: 1992-10-14
// Created by: Christophe MARION
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

#include <HLRBRep_ThePolygonToolOfInterCSurf.hxx>

#include <Standard_OutOfRange.hxx>
#include <gp_Pnt.hxx>
#include <HLRBRep_ThePolygonOfInterCSurf.hxx>
#include <Bnd_Box.hxx>
 

#define ThePoint gp_Pnt
#define ThePoint_hxx <gp_Pnt.hxx>
#define ThePolygon HLRBRep_ThePolygonOfInterCSurf
#define ThePolygon_hxx <HLRBRep_ThePolygonOfInterCSurf.hxx>
#define TheBoundingBox Bnd_Box
#define TheBoundingBox_hxx <Bnd_Box.hxx>
#define IntCurveSurface_PolygonTool HLRBRep_ThePolygonToolOfInterCSurf
#define IntCurveSurface_PolygonTool_hxx <HLRBRep_ThePolygonToolOfInterCSurf.hxx>
#include <IntCurveSurface_PolygonTool.gxx>

