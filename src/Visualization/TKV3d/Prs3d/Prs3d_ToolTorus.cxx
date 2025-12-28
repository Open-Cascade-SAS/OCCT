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

void Prs3d_ToolTorus::init(const double    theMajorRad,
                           const double    theMinorRad,
                           const double    theAngle1,
                           const double    theAngle2,
                           const double    theAngle,
                           const int theNbSlices,
                           const int theNbStacks)
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

gp_Pnt Prs3d_ToolTorus::Vertex(const double theU, const double theV) const
{
  const double aU = theU * myAngle;
  const double aV = myVMin + theV * (myVMax - myVMin);
  return gp_Pnt((myMajorRadius + myMinorRadius * std::cos(aV)) * std::cos(aU),
                (myMajorRadius + myMinorRadius * std::cos(aV)) * std::sin(aU),
                myMinorRadius * std::sin(aV));
}

//=================================================================================================

gp_Dir Prs3d_ToolTorus::Normal(const double theU, const double theV) const
{
  const double aU = theU * myAngle;
  const double aV = myVMin + theV * (myVMax - myVMin);
  return gp_Dir(std::cos(aU) * std::cos(aV), std::sin(aU) * std::cos(aV), std::sin(aV));
}

//=================================================================================================

occ::handle<Graphic3d_ArrayOfTriangles> Prs3d_ToolTorus::Create(const double    theMajorRad,
                                                           const double    theMinorRad,
                                                           const double    theAngle1,
                                                           const double    theAngle2,
                                                           const double    theAngle,
                                                           const int theNbSlices,
                                                           const int theNbStacks,
                                                           const gp_Trsf&         theTrsf)
{
  occ::handle<Graphic3d_ArrayOfTriangles> anArray;
  Prs3d_ToolTorus
    aTool(theMajorRad, theMinorRad, theAngle1, theAngle2, theAngle, theNbSlices, theNbStacks);
  aTool.FillArray(anArray, theTrsf);
  return anArray;
}
