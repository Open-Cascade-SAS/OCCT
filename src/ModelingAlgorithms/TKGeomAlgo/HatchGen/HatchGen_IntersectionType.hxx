// Created on: 1993-10-25
// Created by: Jean Marc LACHAUME
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

#ifndef _HatchGen_IntersectionType_HeaderFile
#define _HatchGen_IntersectionType_HeaderFile

//! Intersection type between the hatching and the
//! element.
enum HatchGen_IntersectionType
{
  HatchGen_TRUE,
  HatchGen_TOUCH,
  HatchGen_TANGENT,
  HatchGen_UNDETERMINED
};

#endif // _HatchGen_IntersectionType_HeaderFile
