// Created on: 2020-09-17
// Created by: Marina ZERNOVA
// Copyright (c) 2020 OPEN CASCADE SAS
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

#include <Prs3d_ToolTorus.hxx>

//=================================================================================================

void Prs3d_ToolTorus::init(const Standard_Real    theMajorRad,
                           const Standard_Real    theMinorRad,
                           const Standard_Real    theAngle1,
                           const Standard_Real    theAngle2,
                           const Standard_Real    theAngle,
                           const Standard_Integer theNbSlices,
                           const Standard_Integer theNbStacks)
{
  myMajorRadius = theMajorRad;
  myMinorRadius = theMinorRad;
  myVMin        = theAngle1;
  myVMax        = theAngle2;
  myAngle       = theAngle;
  mySlicesNb    = theNbSlices;
  myStacksNb    = theNbStacks;
}

//=================================================================================================

gp_Pnt Prs3d_ToolTorus::Vertex(const Standard_Real theU, const Standard_Real theV) const
{
  const Standard_Real aU = theU * myAngle;
  const Standard_Real aV = myVMin + theV * (myVMax - myVMin);
  return gp_Pnt((myMajorRadius + myMinorRadius * Cos(aV)) * Cos(aU),
                (myMajorRadius + myMinorRadius * Cos(aV)) * Sin(aU),
                myMinorRadius * Sin(aV));
}

//=================================================================================================

gp_Dir Prs3d_ToolTorus::Normal(const Standard_Real theU, const Standard_Real theV) const
{
  const Standard_Real aU = theU * myAngle;
  const Standard_Real aV = myVMin + theV * (myVMax - myVMin);
  return gp_Dir(Cos(aU) * Cos(aV), Sin(aU) * Cos(aV), Sin(aV));
}

//=================================================================================================

Handle(Graphic3d_ArrayOfTriangles) Prs3d_ToolTorus::Create(const Standard_Real    theMajorRad,
                                                           const Standard_Real    theMinorRad,
                                                           const Standard_Real    theAngle1,
                                                           const Standard_Real    theAngle2,
                                                           const Standard_Real    theAngle,
                                                           const Standard_Integer theNbSlices,
                                                           const Standard_Integer theNbStacks,
                                                           const gp_Trsf&         theTrsf)
{
  Handle(Graphic3d_ArrayOfTriangles) anArray;
  Prs3d_ToolTorus
    aTool(theMajorRad, theMinorRad, theAngle1, theAngle2, theAngle, theNbSlices, theNbStacks);
  aTool.FillArray(anArray, theTrsf);
  return anArray;
}
