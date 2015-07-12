// Created on: 1995-01-27
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

#include <GeomInt_TheMultiLineOfWLApprox.hxx>

#include <IntPatch_WLine.hxx>
#include <ApproxInt_SvSurfaces.hxx>
 

#define Handle_TheLine Handle(IntPatch_WLine)
#define TheLine IntPatch_WLine
#define TheLine_hxx <IntPatch_WLine.hxx>
#define TheSvSurfaces ApproxInt_SvSurfaces
#define TheSvSurfaces_hxx <ApproxInt_SvSurfaces.hxx>
#define ApproxInt_MultiLine GeomInt_TheMultiLineOfWLApprox
#define ApproxInt_MultiLine_hxx <GeomInt_TheMultiLineOfWLApprox.hxx>
#include <ApproxInt_MultiLine.gxx>

