// Created on: 1994-12-12
// Created by: Jacques GOUSSARD
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

#include <BRepBuilderAPI_Copy.hxx>
#include <BRepTools_CopyModification.hxx>

//=================================================================================================

BRepBuilderAPI_Copy::BRepBuilderAPI_Copy()
{
  myModification = new BRepTools_CopyModification(Standard_True, Standard_False);
}

//=================================================================================================

BRepBuilderAPI_Copy::BRepBuilderAPI_Copy(const TopoDS_Shape&    S,
                                         const Standard_Boolean copyGeom,
                                         const Standard_Boolean copyMesh)
{
  myModification = new BRepTools_CopyModification(copyGeom, copyMesh);
  DoModif(S);
}

//=================================================================================================

void BRepBuilderAPI_Copy::Perform(const TopoDS_Shape&    S,
                                  const Standard_Boolean copyGeom,
                                  const Standard_Boolean copyMesh)
{
  myModification = new BRepTools_CopyModification(copyGeom, copyMesh);
  NotDone(); // on force la copie si on vient deja d`en faire une
  DoModif(S);
}
