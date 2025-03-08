// Created on: 2016-02-04
// Created by: Anastasia BORISOVA
// Copyright (c) 2016 OPEN CASCADE SAS
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

#include <Prs3d_ToolDisk.hxx>

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Prs3d_ToolQuadric.hxx>

//=================================================================================================

Prs3d_ToolDisk::Prs3d_ToolDisk(const Standard_Real    theInnerRadius,
                               const Standard_Real    theOuterRadius,
                               const Standard_Integer theNbSlices,
                               const Standard_Integer theNbStacks)
    : myInnerRadius(theInnerRadius),
      myOuterRadius(theOuterRadius),
      myStartAngle(0.0),
      myEndAngle(M_PI * 2.0)
{
  mySlicesNb = theNbSlices;
  myStacksNb = theNbStacks;
}

//=================================================================================================

gp_Pnt Prs3d_ToolDisk::Vertex(const Standard_Real theU, const Standard_Real theV) const
{
  const Standard_Real aU      = myStartAngle + theU * (myEndAngle - myStartAngle);
  const Standard_Real aRadius = myInnerRadius + (myOuterRadius - myInnerRadius) * theV;
  return gp_Pnt(Cos(aU) * aRadius, Sin(aU) * aRadius, 0.0);
}

//=================================================================================================

Handle(Graphic3d_ArrayOfTriangles) Prs3d_ToolDisk::Create(const Standard_Real    theInnerRadius,
                                                          const Standard_Real    theOuterRadius,
                                                          const Standard_Integer theNbSlices,
                                                          const Standard_Integer theNbStacks,
                                                          const gp_Trsf&         theTrsf)
{
  Handle(Graphic3d_ArrayOfTriangles) anArray;
  Prs3d_ToolDisk aTool(theInnerRadius, theOuterRadius, theNbSlices, theNbStacks);
  aTool.FillArray(anArray, theTrsf);
  return anArray;
}
