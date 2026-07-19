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
#include <Poly_Triangle.hxx>
#include <NCollection_Array1.hxx>
#include <Prs3d_ToolQuadric.hxx>

//=================================================================================================

Prs3d_ToolDisk::Prs3d_ToolDisk(const double theInnerRadius,
                               const double theOuterRadius,
                               const int    theNbSlices,
                               const int    theNbStacks)
    : myInnerRadius(theInnerRadius),
      myOuterRadius(theOuterRadius),
      myStartAngle(0.0),
      myEndAngle(M_PI * 2.0)
{
  mySlicesNb = theNbSlices;
  myStacksNb = theNbStacks;
}

//=================================================================================================

gp_Pnt Prs3d_ToolDisk::Vertex(const double theU, const double theV) const
{
  const double aU      = myStartAngle + theU * (myEndAngle - myStartAngle);
  const double aRadius = myInnerRadius + (myOuterRadius - myInnerRadius) * theV;
  return gp_Pnt(std::cos(aU) * aRadius, std::sin(aU) * aRadius, 0.0);
}

//=================================================================================================

occ::handle<Graphic3d_ArrayOfTriangles> Prs3d_ToolDisk::Create(const double   theInnerRadius,
                                                               const double   theOuterRadius,
                                                               const int      theNbSlices,
                                                               const int      theNbStacks,
                                                               const gp_Trsf& theTrsf)
{
  occ::handle<Graphic3d_ArrayOfTriangles> anArray;
  Prs3d_ToolDisk aTool(theInnerRadius, theOuterRadius, theNbSlices, theNbStacks);
  aTool.FillArray(anArray, theTrsf);
  return anArray;
}
