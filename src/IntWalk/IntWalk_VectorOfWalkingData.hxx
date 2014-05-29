// Created on: 2013-0603
// Created by: Roman LYGIN
// Copyright (c) 2013-2013 OPEN CASCADE SAS
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

#ifndef IntWalk_VectorOfWalkingData_HeaderFile
#define IntWalk_VectorOfWalkingData_HeaderFile

#include <vector>
#include <NCollection_StdAllocator.hxx>

// Defines a dynamic vector of work data.

struct IntWalk_WalkingData
{
    Standard_Real    ustart;
    Standard_Real    vstart;
    Standard_Integer etat;
};

typedef std::vector<IntWalk_WalkingData, NCollection_StdAllocator<IntWalk_WalkingData> >
    IntWalk_VectorOfWalkingData;

#endif
