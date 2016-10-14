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

#include <Prs3d_ToolSphere.hxx>

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <Prs3d_ToolQuadric.hxx>

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
Prs3d_ToolSphere::Prs3d_ToolSphere (const Standard_Real    theRadius,
                                    const Standard_Integer theNbSlices,
                                    const Standard_Integer theNbStacks)
: myRadius (theRadius)
{
  mySlicesNb = theNbSlices;
  myStacksNb = theNbStacks;
}

//=======================================================================
//function : Vertex
//purpose  :
//=======================================================================
gp_Pnt Prs3d_ToolSphere::Vertex (const Standard_Real theU, const Standard_Real theV)
{
  const Standard_Real aU = theU * M_PI * 2.0;
  const Standard_Real aV = theV * M_PI;
  return gp_Pnt (myRadius * Cos (aU) * Sin (aV),
                -myRadius * Sin (aU) * Sin (aV),
                 myRadius * Cos (aV));
}

//=======================================================================
//function : Add
//purpose  :
//=======================================================================
gp_Dir Prs3d_ToolSphere::Normal (const Standard_Real theU, const Standard_Real theV)
{
  const Standard_Real aU = theU * M_PI * 2.0;
  const Standard_Real aV = theV * M_PI;
  return gp_Dir (Cos (aU) * Sin (aV),
                -Sin (aU) * Sin (aV),
                 Cos (aV));
}

//=======================================================================
//function : Perform
//purpose  :
//=======================================================================
Handle(Graphic3d_ArrayOfTriangles) Prs3d_ToolSphere::Create (const Standard_Real    theRadius,
                                                             const Standard_Integer theNbSlices,
                                                             const Standard_Integer theNbStacks,
                                                             const gp_Trsf&         theTrsf)
{
  Handle(Graphic3d_ArrayOfTriangles) anArray;
  Prs3d_ToolSphere aTool (theRadius, theNbSlices, theNbStacks);
  aTool.FillArray (anArray, theTrsf);
  return anArray;
}
