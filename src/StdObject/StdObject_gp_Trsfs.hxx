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


#ifndef _StdObject_gp_Trsfs_HeaderFile
#define _StdObject_gp_Trsfs_HeaderFile


#include <StdObject_gp_Vectors.hxx>

#include <gp_Mat2d.hxx>
#include <gp_Mat.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Trsf.hxx>


inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData& theReadData, gp_Mat2d& theMat)
{
  StdObjMgt_ReadData::ObjectSentry aSentry (theReadData);
  theReadData
    >> theMat(1, 1) >> theMat(1, 2)
    >> theMat(2, 1) >> theMat(2, 2);
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData& theWriteData, const gp_Mat2d& theMat)
{
  StdObjMgt_WriteData::ObjectSentry aSentry (theWriteData);

  theWriteData
    << theMat(1, 1) << theMat(1, 2)
    << theMat(2, 1) << theMat(2, 2);
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData& theReadData, gp_Mat& theMat)
{
  StdObjMgt_ReadData::ObjectSentry aSentry (theReadData);
  theReadData
    >> theMat(1, 1) >> theMat(1, 2) >> theMat(1, 3)
    >> theMat(2, 1) >> theMat(2, 2) >> theMat(2, 3)
    >> theMat(3, 1) >> theMat(3, 2) >> theMat(3, 3);
  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData& theWriteData, const gp_Mat& theMat)
{
  StdObjMgt_WriteData::ObjectSentry aSentry (theWriteData);

  theWriteData
    << theMat(1, 1) << theMat(1, 2) << theMat(1, 3)
    << theMat(2, 1) << theMat(2, 2) << theMat(2, 3)
    << theMat(3, 1) << theMat(3, 2) << theMat(3, 3);
  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData& theReadData, gp_Trsf2d& theTrsf)
{
  StdObjMgt_ReadData::ObjectSentry aSentry (theReadData);

  Standard_Real    aScale;
  Standard_Integer aForm;
  gp_Mat2d         aMat;
  gp_XY            aLoc;

  theReadData >> aScale >> aForm >> aMat >> aLoc;

  theTrsf.SetValues (aScale * aMat(1, 1), aScale * aMat(1, 2), aLoc.X(),
                     aScale * aMat(2, 1), aScale * aMat(2, 2), aLoc.Y());

  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData& theWriteData, const gp_Trsf2d& theTrsf)
{
  StdObjMgt_WriteData::ObjectSentry aSentry (theWriteData);

  Standard_Real    aScale = theTrsf.ScaleFactor();
  Standard_Integer aForm  = theTrsf.Form();
  const gp_Mat2d&  aMat   = theTrsf.HVectorialPart();
  const gp_XY&     aLoc   = theTrsf.TranslationPart();

  theWriteData << aScale << aForm << aMat << aLoc;

  return theWriteData;
}

inline StdObjMgt_ReadData& operator >>
  (StdObjMgt_ReadData& theReadData, gp_Trsf& theTrsf)
{
  StdObjMgt_ReadData::ObjectSentry aSentry (theReadData);

  Standard_Real    aScale;
  Standard_Integer aForm;
  gp_Mat           aMat;
  gp_XYZ           aLoc;

  theReadData >> aScale >> aForm >> aMat >> aLoc;

  theTrsf.SetValues (
    aScale * aMat(1, 1), aScale * aMat(1, 2), aScale * aMat(1, 3), aLoc.X(),
    aScale * aMat(2, 1), aScale * aMat(2, 2), aScale * aMat(2, 3), aLoc.Y(),
    aScale * aMat(3, 1), aScale * aMat(3, 2), aScale * aMat(3, 3), aLoc.Z());

  return theReadData;
}

inline StdObjMgt_WriteData& operator <<
  (StdObjMgt_WriteData& theWriteData, const gp_Trsf& theTrsf)
{
  StdObjMgt_WriteData::ObjectSentry aSentry (theWriteData);

  Standard_Real    aScale = theTrsf.ScaleFactor();
  Standard_Integer aForm  = theTrsf.Form();
  const gp_Mat&    aMat   = theTrsf.HVectorialPart();
  const gp_XYZ&    aLoc   = theTrsf.TranslationPart();

  theWriteData << aScale << aForm << aMat << aLoc;

  return theWriteData;
}

#endif
