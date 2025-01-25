// Created on: 2019-02-25
// Created by: Artem NOVIKOV
// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <Prs3d_ToolSector.hxx>

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Prs3d_ToolQuadric.hxx>

//=================================================================================================

Prs3d_ToolSector::Prs3d_ToolSector(const Standard_Real    theRadius,
                                   const Standard_Integer theNbSlices,
                                   const Standard_Integer theNbStacks)
    : myRadius(theRadius)
{
  mySlicesNb = theNbSlices;
  myStacksNb = theNbStacks;
}

//=================================================================================================

gp_Pnt Prs3d_ToolSector::Vertex(const Standard_Real theU, const Standard_Real theV) const
{
  const Standard_Real aU      = theU * M_PI / 2.0;
  const Standard_Real aRadius = myRadius * theV;
  return gp_Pnt(Cos(aU) * aRadius, Sin(aU) * aRadius, 0.0);
}

//=================================================================================================

Handle(Graphic3d_ArrayOfTriangles) Prs3d_ToolSector::Create(const Standard_Real    theRadius,
                                                            const Standard_Integer theNbSlices,
                                                            const Standard_Integer theNbStacks,
                                                            const gp_Trsf&         theTrsf)
{
  Handle(Graphic3d_ArrayOfTriangles) anArray;
  Prs3d_ToolSector                   aTool(theRadius, theNbSlices, theNbStacks);
  aTool.FillArray(anArray, theTrsf);
  return anArray;
}
