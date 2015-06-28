// Created on: 1993-06-17
// Created by: Jean Yves LEBEY
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

#ifndef TopOpeBRepDS_MapOfPoint_HeaderFile
#define TopOpeBRepDS_MapOfPoint_HeaderFile

#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_PointData.hxx>
#include <TColStd_MapIntegerHasher.hxx>
#include <NCollection_DataMap.hxx>

typedef NCollection_DataMap<Standard_Integer,TopOpeBRepDS_PointData,TColStd_MapIntegerHasher> TopOpeBRepDS_MapOfPoint;
typedef NCollection_DataMap<Standard_Integer,TopOpeBRepDS_PointData,TColStd_MapIntegerHasher>::Iterator TopOpeBRepDS_DataMapIteratorOfMapOfPoint;


#endif
