// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BRepMesh_DegreeOfFreedom_HeaderFile
#define _BRepMesh_DegreeOfFreedom_HeaderFile

enum BRepMesh_DegreeOfFreedom
{
  BRepMesh_Free,
  BRepMesh_InVolume,
  BRepMesh_OnSurface,
  BRepMesh_OnCurve,
  BRepMesh_Fixed,
  BRepMesh_Frontier,
  BRepMesh_Deleted
};

#endif
