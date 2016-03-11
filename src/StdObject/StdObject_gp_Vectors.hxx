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


#ifndef _StdObject_gp_Vectors_HeaderFile
#define _StdObject_gp_Vectors_HeaderFile


#include <StdObjMgt_ReadData.hxx>

#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Dir.hxx>


inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_XY& theXY)
{
  Standard_Real aX, aY;
  theReadData >> aX >> aY;
  theXY.SetCoord (aX, aY);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Pnt2d& thePnt)
{
  Standard_Real aX, aY;
  theReadData >> aX >> aY;
  thePnt.SetCoord (aX, aY);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Vec2d& theVec)
{
  Standard_Real aX, aY;
  theReadData >> aX >> aY;
  theVec.SetCoord (aX, aY);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Dir2d& theDir)
{
  Standard_Real aX, aY;
  theReadData >> aX >> aY;
  theDir.SetCoord (aX, aY);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_XYZ& theXYZ)
{
  Standard_Real aX, aY, aZ;
  theReadData >> aX >> aY >> aZ;
  theXYZ.SetCoord (aX, aY, aZ);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Pnt& thePnt)
{
  Standard_Real aX, aY, aZ;
  theReadData >> aX >> aY >> aZ;
  thePnt.SetCoord (aX, aY, aZ);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Vec& theVec)
{
  Standard_Real aX, aY, aZ;
  theReadData >> aX >> aY >> aZ;
  theVec.SetCoord (aX, aY, aZ);
  return theReadData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Dir& theDir)
{
  Standard_Real aX, aY, aZ;
  theReadData >> aX >> aY >> aZ;
  theDir.SetCoord (aX, aY, aZ);
  return theReadData;
}


#endif
