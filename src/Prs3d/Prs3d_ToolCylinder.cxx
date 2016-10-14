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
#include <Poly_Array1OfTriangle.hxx>
#include <Prs3d_ToolQuadric.hxx>

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
Prs3d_ToolCylinder::Prs3d_ToolCylinder (const Standard_Real    theBottomRad,
                                        const Standard_Real    theTopRad,
                                        const Standard_Real    theHeight,
                                        const Standard_Integer theNbSlices,
                                        const Standard_Integer theNbStacks)
: myBottomRadius (theBottomRad),
  myTopRadius (theTopRad),
  myHeight (theHeight)
{
  myStacksNb = theNbStacks;
  mySlicesNb = theNbSlices;
}

//=======================================================================
//function : Vertex
//purpose  :
//=======================================================================
gp_Pnt Prs3d_ToolCylinder::Vertex (const Standard_Real theU, const Standard_Real theV)
{
  const Standard_Real aU      = theU * M_PI * 2.0;
  const Standard_Real aRadius = myBottomRadius + (myTopRadius - myBottomRadius) * theV;
  return gp_Pnt (Cos (aU) * aRadius,
                 Sin (aU) * aRadius,
                 theV * myHeight);
}

//=======================================================================
//function : Add
//purpose  :
//=======================================================================
gp_Dir Prs3d_ToolCylinder::Normal (const Standard_Real theU, const Standard_Real /*theV*/)
{
  const Standard_Real aU = theU * M_PI * 2.0;
  return gp_Dir (Cos (aU) * myHeight,
                 Sin (aU) * myHeight,
                 myBottomRadius - myTopRadius);
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
Handle(Graphic3d_ArrayOfTriangles) Prs3d_ToolCylinder::Create (const Standard_Real    theBottomRad,
                                                               const Standard_Real    theTopRad,
                                                               const Standard_Real    theHeight,
                                                               const Standard_Integer theNbSlices,
                                                               const Standard_Integer theNbStacks,
                                                               const gp_Trsf&         theTrsf)
{
  Handle(Graphic3d_ArrayOfTriangles) anArray;
  Prs3d_ToolCylinder aTool (theBottomRad, theTopRad, theHeight, theNbSlices, theNbStacks);
  aTool.FillArray (anArray, theTrsf);
  return anArray;
}
