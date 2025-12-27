// Created on: 1995-07-27
// Created by: Modelistation
// Copyright (c) 1995-1999 Matra Datavision
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

#include <Prs3d_ToolCylinder.hxx>

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Triangle.hxx>
#include <NCollection_Array1.hxx>
#include <Prs3d_ToolQuadric.hxx>

//=================================================================================================

Prs3d_ToolCylinder::Prs3d_ToolCylinder(const double    theBottomRad,
                                       const double    theTopRad,
                                       const double    theHeight,
                                       const int theNbSlices,
                                       const int theNbStacks)
    : myBottomRadius(theBottomRad),
      myTopRadius(theTopRad),
      myHeight(theHeight)
{
  myStacksNb = theNbStacks;
  mySlicesNb = theNbSlices;
}

//=================================================================================================

gp_Pnt Prs3d_ToolCylinder::Vertex(const double theU, const double theV) const
{
  const double aU      = theU * M_PI * 2.0;
  const double aRadius = myBottomRadius + (myTopRadius - myBottomRadius) * theV;
  return gp_Pnt(std::cos(aU) * aRadius, std::sin(aU) * aRadius, theV * myHeight);
}

//=================================================================================================

gp_Dir Prs3d_ToolCylinder::Normal(const double theU, const double) const
{
  const double aU = theU * M_PI * 2.0;
  return gp_Dir(std::cos(aU) * myHeight, std::sin(aU) * myHeight, myBottomRadius - myTopRadius);
}

//=================================================================================================

occ::handle<Graphic3d_ArrayOfTriangles> Prs3d_ToolCylinder::Create(const double    theBottomRad,
                                                              const double    theTopRad,
                                                              const double    theHeight,
                                                              const int theNbSlices,
                                                              const int theNbStacks,
                                                              const gp_Trsf&         theTrsf)
{
  occ::handle<Graphic3d_ArrayOfTriangles> anArray;
  Prs3d_ToolCylinder aTool(theBottomRad, theTopRad, theHeight, theNbSlices, theNbStacks);
  aTool.FillArray(anArray, theTrsf);
  return anArray;
}
