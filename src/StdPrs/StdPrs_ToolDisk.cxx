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


#include <StdPrs_ToolDisk.hxx>

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <StdPrs_ToolQuadric.hxx>

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
StdPrs_ToolDisk::StdPrs_ToolDisk (const Standard_ShortReal theInnerRadius,
                                  const Standard_ShortReal theOuterRadius,
                                  const Standard_Integer theSlicesNb,
                                  const Standard_Integer theStacksNb)
: myInnerRadius (theInnerRadius),
  myOuterRadius (theOuterRadius)
{
  mySlicesNb = theSlicesNb;
  myStacksNb = theStacksNb;
}

//=======================================================================
//function : Vertex
//purpose  :
//=======================================================================
gp_Pnt StdPrs_ToolDisk::Vertex (const Standard_Real theU, const Standard_Real theV)
{
  const Standard_ShortReal aU = static_cast<Standard_ShortReal> (theU * M_PI * 2.0);
  const Standard_ShortReal aRadius = myInnerRadius + (myOuterRadius - myInnerRadius) * (Standard_ShortReal)theV;
  return gp_Pnt (cosf(aU) * aRadius,
                 sinf(aU) * aRadius,
                 0.0f);
}

//=======================================================================
//function : Add
//purpose  :
//=======================================================================
gp_Dir StdPrs_ToolDisk::Normal (const Standard_Real /*theU*/, const Standard_Real /*theV*/)
{
  return gp_Dir(0.0f, 0.0f, -1.0f);
}
