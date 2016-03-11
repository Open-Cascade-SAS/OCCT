// Copyright (c) 2015 OPEN CASCADE SAS
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


#ifndef _StdObject_gp_Surfaces_HeaderFile
#define _StdObject_gp_Surfaces_HeaderFile


#include <StdObject_gp_Axes.hxx>

#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>


inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Cone& theCone)
{
  gp_Ax3 anAx;
  Standard_Real aRadius, aSemiAngle;

  theReadData >> anAx >> aRadius >> aSemiAngle;

  theCone.SetPosition  (anAx);
  theCone.SetRadius    (aRadius);
  theCone.SetSemiAngle (aSemiAngle);

  return theReadData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Cylinder& theCyl)
{
  gp_Ax3 anAx;
  Standard_Real aRadius;

  theReadData >> anAx >> aRadius;

  theCyl.SetPosition (anAx);
  theCyl.SetRadius   (aRadius);

  return theReadData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Sphere& theSph)
{
  gp_Ax3 anAx;
  Standard_Real aRadius;

  theReadData >> anAx >> aRadius;

  theSph.SetPosition (anAx);
  theSph.SetRadius   (aRadius);

  return theReadData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Torus& theTorus)
{
  gp_Ax3 anAx;
  Standard_Real aMajorRadius, aMinorRadius;

  theReadData >> anAx >> aMajorRadius >> aMinorRadius;

  theTorus.SetPosition    (anAx);
  theTorus.SetMajorRadius (aMajorRadius);
  theTorus.SetMinorRadius (aMinorRadius);

  return theReadData;
}


#endif
