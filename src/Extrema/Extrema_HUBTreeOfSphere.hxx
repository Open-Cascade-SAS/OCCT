// Created on: 2005-03-17
// Created by: OPEN CASCADE Support
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _Extrema_HUBTreeOfSphere_HeaderFile
#define _Extrema_HUBTreeOfSphere_HeaderFile

#include <NCollection_UBTreeFiller.hxx>
#include <NCollection_Handle.hxx>
#include <Bnd_Sphere.hxx>

typedef NCollection_UBTree<Standard_Integer,Bnd_Sphere> Extrema_UBTreeOfSphere;
typedef NCollection_UBTreeFiller<Standard_Integer,Bnd_Sphere> Extrema_UBTreeFillerOfSphere;
typedef NCollection_Handle<Extrema_UBTreeOfSphere> Extrema_HUBTreeOfSphere;

#endif //_Extrema_HUBTreeOfSphere_HeaderFile
