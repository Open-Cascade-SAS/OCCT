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

#include <HLRBRep_ThePolyhedronToolOfInterCSurf.hxx>

#include <Standard_OutOfRange.hxx>
#include <HLRBRep_ThePolyhedronOfInterCSurf.hxx>
#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>

#define ThePolyhedron HLRBRep_ThePolyhedronOfInterCSurf
#define ThePolyhedron_hxx <HLRBRep_ThePolyhedronOfInterCSurf.hxx>
#define IntCurveSurface_PolyhedronTool HLRBRep_ThePolyhedronToolOfInterCSurf
#define IntCurveSurface_PolyhedronTool_hxx <HLRBRep_ThePolyhedronToolOfInterCSurf.hxx>
#include "../IntCurveSurface/IntCurveSurface_PolyhedronTool.gxx"
