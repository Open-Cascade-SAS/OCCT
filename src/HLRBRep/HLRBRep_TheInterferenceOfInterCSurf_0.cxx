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

#include <HLRBRep_TheInterferenceOfInterCSurf.hxx>

#include <HLRBRep_ThePolygonOfInterCSurf.hxx>
#include <HLRBRep_ThePolygonToolOfInterCSurf.hxx>
#include <HLRBRep_ThePolyhedronOfInterCSurf.hxx>
#include <HLRBRep_ThePolyhedronToolOfInterCSurf.hxx>
#include <gp_Lin.hxx>
#include <Bnd_BoundSortBox.hxx>
#include <gp_Pnt.hxx>
#include <gp_XYZ.hxx>
 

#define Polygon3d HLRBRep_ThePolygonOfInterCSurf
#define Polygon3d_hxx <HLRBRep_ThePolygonOfInterCSurf.hxx>
#define ToolPolygon3d HLRBRep_ThePolygonToolOfInterCSurf
#define ToolPolygon3d_hxx <HLRBRep_ThePolygonToolOfInterCSurf.hxx>
#define Polyhedron HLRBRep_ThePolyhedronOfInterCSurf
#define Polyhedron_hxx <HLRBRep_ThePolyhedronOfInterCSurf.hxx>
#define ToolPolyh HLRBRep_ThePolyhedronToolOfInterCSurf
#define ToolPolyh_hxx <HLRBRep_ThePolyhedronToolOfInterCSurf.hxx>
#define Intf_InterferencePolygonPolyhedron HLRBRep_TheInterferenceOfInterCSurf
#define Intf_InterferencePolygonPolyhedron_hxx <HLRBRep_TheInterferenceOfInterCSurf.hxx>
#include <Intf_InterferencePolygonPolyhedron.gxx>

