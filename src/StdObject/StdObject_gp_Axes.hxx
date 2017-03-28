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


#ifndef _StdObject_gp_Axes_HeaderFile
#define _StdObject_gp_Axes_HeaderFile


#include <StdObject_gp_Vectors.hxx>

#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>


inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Ax2d& theAx)
{
  gp_Pnt2d aLoc;
  gp_Dir2d aDir;
  theReadData >> aLoc >> aDir;
  theAx = gp_Ax2d (aLoc, aDir);
  return theReadData;
}

inline StdObjMgt_WriteData& 
  write (StdObjMgt_WriteData::Object theWriteData, const gp_Ax2d& theAx)
{
  const gp_Pnt2d& aLoc = theAx.Location();
  const gp_Dir2d& aDir = theAx.Direction();
  theWriteData << aLoc << aDir;
  return theWriteData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_Ax2d& theAx)
{
  const gp_Pnt2d& aLoc = theAx.Location();
  const gp_Dir2d& aDir = theAx.Direction();
  theWriteData << aLoc << aDir;
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Ax22d& theAx)
{
  gp_Pnt2d aLoc;
  gp_Dir2d aYDir, aXDir;
  theReadData >> aLoc >> aYDir >> aXDir;
  theAx = gp_Ax22d (aLoc, aXDir, aYDir);
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_Ax22d& theAx)
{
  const gp_Pnt2d& aLoc = theAx.Location();
  const gp_Dir2d& aYDir = theAx.YDirection();
  const gp_Dir2d& aXDir = theAx.XDirection();
  theWriteData << aLoc << aYDir << aXDir;
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Ax1& theAx)
{
  gp_Pnt aLoc;
  gp_Dir aDir;
  theReadData >> aLoc >> aDir;
  theAx = gp_Ax1 (aLoc, aDir);
  return theReadData;
}

inline StdObjMgt_WriteData&
  write(StdObjMgt_WriteData::Object theWriteData, const gp_Ax1& theAx)
{
  const gp_Pnt& aLoc = theAx.Location();
  const gp_Dir& aDir = theAx.Direction();
  theWriteData << aLoc << aDir;
  return theWriteData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_Ax1& theAx)
{
  const gp_Pnt& aLoc = theAx.Location();
  const gp_Dir& aDir = theAx.Direction();
  theWriteData << aLoc << aDir;
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Ax2& theAx)
{
  gp_Ax1 anAx;
  gp_Dir aYDir, aXDir;
  theReadData >> anAx >> aYDir >> aXDir;
  theAx = gp_Ax2 (anAx.Location(), anAx.Direction(), aXDir);
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData::Object theWriteData, const gp_Ax2& theAx)
{
  const gp_Ax1& anAx = theAx.Axis();
  const gp_Dir& aYDir = theAx.YDirection();
  const gp_Dir& aXDir = theAx.XDirection();
  theWriteData << anAx << aYDir << aXDir;
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData::Object theReadData, gp_Ax3& theAx)
{
  gp_Ax1 anAx;
  gp_Dir aYDir, aXDir;
  theReadData >> anAx >> aYDir >> aXDir;
  theAx = gp_Ax3 (anAx.Location(), anAx.Direction(), aXDir);
  if (aYDir * theAx.YDirection() < 0.)
    theAx.YReverse();
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
(StdObjMgt_WriteData::Object theWriteData, const gp_Ax3& theAx)
{
  const gp_Ax1& anAx = theAx.Axis();
  const gp_Dir& aYDir = theAx.YDirection();
  const gp_Dir& aXDir = theAx.XDirection();
  theWriteData << anAx << aYDir << aXDir;
  return theWriteData;
}


#endif
