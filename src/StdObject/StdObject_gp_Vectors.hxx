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
#include <StdObjMgt_WriteData.hxx>

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

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_XY& theXY)
{
  Standard_Real aX = theXY.X(), aY = theXY.Y();
  theWriteData << aX << aY;
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Pnt2d& thePnt)
{
  gp_XY aXY;
  theReadData >> aXY;
  thePnt.SetXY (aXY);
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_Pnt2d& thePnt)
{
  theWriteData << thePnt.XY();
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Vec2d& theVec)
{
  gp_XY aXY;
  theReadData >> aXY;
  theVec.SetXY (aXY);
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_Vec2d& theVec)
{
  theWriteData << theVec.XY();
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Dir2d& theDir)
{
  gp_XY aXY;
  theReadData >> aXY;
  theDir.SetXY (aXY);
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_Dir2d& theDir)
{
  theWriteData << theDir.XY();
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_XYZ& theXYZ)
{
  Standard_Real aX, aY, aZ;
  theReadData >> aX >> aY >> aZ;
  theXYZ.SetCoord(aX, aY, aZ);
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_XYZ& theXYZ)
{
  Standard_Real aX = theXYZ.X(), aY = theXYZ.Y(), aZ = theXYZ.Z();
  theWriteData << aX << aY << aZ;
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Pnt& thePnt)
{
  gp_XYZ aXYZ;
  theReadData >> aXYZ;
  thePnt.SetXYZ (aXYZ);
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_Pnt& thePnt)
{
  theWriteData << thePnt.XYZ();
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Vec& theVec)
{
  gp_XYZ aXYZ;
  theReadData >> aXYZ;
  theVec.SetXYZ (aXYZ);
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_Vec& theVec)
{
  theWriteData << theVec.XYZ();
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Dir& theDir)
{
  gp_XYZ aXYZ;
  theReadData >> aXYZ;
  theDir.SetXYZ(aXYZ);
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_Dir& theDir)
{
  theWriteData << theDir.XYZ();
  return theWriteData;
}


#endif
