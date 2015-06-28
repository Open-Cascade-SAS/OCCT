// Created on: 1999-11-17
// Created by: Andrey BETENEV
// Copyright (c) 1999 Matra Datavision
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

#ifndef STEPConstruct_DataMapOfPointTransient_HeaderFile
#define STEPConstruct_DataMapOfPointTransient_HeaderFile

#include <gp_Pnt.hxx>
#include <Standard_Transient.hxx>
#include <STEPConstruct_PointHasher.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<gp_Pnt,Handle(Standard_Transient),STEPConstruct_PointHasher> STEPConstruct_DataMapOfPointTransient;
typedef NCollection_DataMap<gp_Pnt,Handle(Standard_Transient),STEPConstruct_PointHasher>::Iterator STEPConstruct_DataMapIteratorOfDataMapOfPointTransient;


#endif
