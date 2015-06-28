// Created on: 1994-02-03
// Created by: Jean Marc LACHAUME
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef Geom2dHatch_MapOfElements_HeaderFile
#define Geom2dHatch_MapOfElements_HeaderFile

#include <Standard_Integer.hxx>
#include <Geom2dHatch_Element.hxx>
#include <TColStd_MapIntegerHasher.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<Standard_Integer,Geom2dHatch_Element,TColStd_MapIntegerHasher> Geom2dHatch_MapOfElements;
typedef NCollection_DataMap<Standard_Integer,Geom2dHatch_Element,TColStd_MapIntegerHasher>::Iterator Geom2dHatch_DataMapIteratorOfMapOfElements;


#endif
