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

#include <StdPrs_ToolSphere.hxx>

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <StdPrs_ToolQuadric.hxx>

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
StdPrs_ToolSphere::StdPrs_ToolSphere (const Standard_ShortReal theRadius,
                                      const Standard_Integer theSlicesNb,
                                      const Standard_Integer theStacksNb)
: myRadius (theRadius)
{
  mySlicesNb = theSlicesNb;
  myStacksNb = theStacksNb;
}

//=======================================================================
//function : Vertex
//purpose  :
//=======================================================================
gp_Pnt StdPrs_ToolSphere::Vertex (const Standard_Real theU, const Standard_Real theV)
{
  const Standard_ShortReal aU = static_cast<Standard_ShortReal> (theU * M_PI * 2.0);
  const Standard_ShortReal aV = static_cast<Standard_ShortReal> (theV * M_PI);
  return gp_Pnt (myRadius * cosf(aU) * sinf(aV),
                -myRadius * sinf(aU) * sinf(aV),
                 myRadius * cosf(aV));
}

//=======================================================================
//function : Add
//purpose  :
//=======================================================================
gp_Dir StdPrs_ToolSphere::Normal (const Standard_Real theU, const Standard_Real theV)
{
  const Standard_ShortReal aU = static_cast<Standard_ShortReal> (theU * M_PI * 2.0);
  const Standard_ShortReal aV = static_cast<Standard_ShortReal> (theV * M_PI);
  return gp_Dir (cosf(aU) * sinf(aV),
                -sinf(aU) * sinf(aV),
                 cosf(aV));
}
