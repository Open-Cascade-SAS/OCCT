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


#include <StdPrs_ToolCylinder.hxx>

#include <Graphic3d_ArrayOfTriangles.hxx>
#include <Poly_Array1OfTriangle.hxx>
#include <StdPrs_ToolQuadric.hxx>

//=======================================================================
//function : Constructor
//purpose  :
//=======================================================================
StdPrs_ToolCylinder::StdPrs_ToolCylinder (const Standard_ShortReal theBottomRad,
                                          const Standard_ShortReal theTopRad,
                                          const Standard_ShortReal theHeight,
                                          const Standard_Integer theSlicesNb,
                                          const Standard_Integer theStacksNb)
: myBottomRadius (theBottomRad),
  myTopRadius (theTopRad),
  myHeight (theHeight)
{
  myStacksNb = theStacksNb;
  mySlicesNb = theSlicesNb;
}

//=======================================================================
//function : Vertex
//purpose  :
//=======================================================================
gp_Pnt StdPrs_ToolCylinder::Vertex (const Standard_Real theU, const Standard_Real theV)
{
  const Standard_ShortReal aU = static_cast<Standard_ShortReal> (theU * M_PI * 2.0);
  const Standard_ShortReal aRadius = myBottomRadius + (myTopRadius - myBottomRadius) * (Standard_ShortReal)theV;
  return gp_Pnt (cosf(aU) * aRadius,
                 sinf(aU) * aRadius,
                 theV * myHeight);
}

//=======================================================================
//function : Add
//purpose  :
//=======================================================================
gp_Dir StdPrs_ToolCylinder::Normal (const Standard_Real theU, const Standard_Real /*theV*/)
{
  const Standard_ShortReal aU = static_cast<Standard_ShortReal> (theU * M_PI * 2.0);
  return gp_Dir (gp_Vec(cosf(aU) * myHeight,
                 sinf(aU) * myHeight,
                 myBottomRadius - myTopRadius).Normalized().XYZ());
}
