// Created on: 1997-04-17
// Created by: Christophe MARION
// Copyright (c) 1997-1999 Matra Datavision
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

//=================================================================================================

inline bool HLRBRep_EdgeData::Selected() const
{
  return (myFlags & EMaskSelected) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::Selected(const bool B)
{
  if (B)
    myFlags |= EMaskSelected;
  else
    myFlags &= ~EMaskSelected;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::Rg1Line() const
{
  return (myFlags & EMaskRg1Line) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::Rg1Line(const bool B)
{
  if (B)
    myFlags |= EMaskRg1Line;
  else
    myFlags &= ~EMaskRg1Line;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::RgNLine() const
{
  return (myFlags & EMaskRgNLine) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::RgNLine(const bool B)
{
  if (B)
    myFlags |= EMaskRgNLine;
  else
    myFlags &= ~EMaskRgNLine;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::Vertical() const
{
  return (myFlags & EMaskVertical) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::Vertical(const bool B)
{
  if (B)
    myFlags |= EMaskVertical;
  else
    myFlags &= ~EMaskVertical;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::Simple() const
{
  return (myFlags & EMaskSimple) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::Simple(const bool B)
{
  if (B)
    myFlags |= EMaskSimple;
  else
    myFlags &= ~EMaskSimple;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::OutLVSta() const
{
  return (myFlags & EMaskOutLVSta) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::OutLVSta(const bool B)
{
  if (B)
    myFlags |= EMaskOutLVSta;
  else
    myFlags &= ~EMaskOutLVSta;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::OutLVEnd() const
{
  return (myFlags & EMaskOutLVEnd) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::OutLVEnd(const bool B)
{
  if (B)
    myFlags |= EMaskOutLVEnd;
  else
    myFlags &= ~EMaskOutLVEnd;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::CutAtSta() const
{
  return (myFlags & EMaskCutAtSta) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::CutAtSta(const bool B)
{
  if (B)
    myFlags |= EMaskCutAtSta;
  else
    myFlags &= ~EMaskCutAtSta;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::CutAtEnd() const
{
  return (myFlags & EMaskCutAtEnd) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::CutAtEnd(const bool B)
{
  if (B)
    myFlags |= EMaskCutAtEnd;
  else
    myFlags &= ~EMaskCutAtEnd;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::VerAtSta() const
{
  return (myFlags & EMaskVerAtSta) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::VerAtSta(const bool B)
{
  if (B)
    myFlags |= EMaskVerAtSta;
  else
    myFlags &= ~EMaskVerAtSta;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::VerAtEnd() const
{
  return (myFlags & EMaskVerAtEnd) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::VerAtEnd(const bool B)
{
  if (B)
    myFlags |= EMaskVerAtEnd;
  else
    myFlags &= ~EMaskVerAtEnd;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::AutoIntersectionDone() const
{
  return (myFlags & EMaskIntDone) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::AutoIntersectionDone(const bool B)
{
  if (B)
    myFlags |= EMaskIntDone;
  else
    myFlags &= ~EMaskIntDone;
}

//=================================================================================================

inline bool HLRBRep_EdgeData::Used() const
{
  return (myFlags & EMaskUsed) != 0;
}

//=================================================================================================

inline void HLRBRep_EdgeData::Used(const bool B)
{
  if (B)
    myFlags |= EMaskUsed;
  else
    myFlags &= ~EMaskUsed;
}

//=================================================================================================

inline int HLRBRep_EdgeData::HideCount() const
{
  return myHideCount;
}

//=================================================================================================

inline void HLRBRep_EdgeData::HideCount(const int I)
{
  myHideCount = I;
}

//=================================================================================================

inline int HLRBRep_EdgeData::VSta() const
{
  return myVSta;
}

//=================================================================================================

inline void HLRBRep_EdgeData::VSta(const int I)
{
  myVSta = I;
}

//=================================================================================================

inline int HLRBRep_EdgeData::VEnd() const
{
  return myVEnd;
}

//=================================================================================================

inline void HLRBRep_EdgeData::VEnd(const int I)
{
  myVEnd = I;
}

//=================================================================================================

inline HLRAlgo_EdgeStatus& HLRBRep_EdgeData::Status()
{
  return myStatus;
}

//=================================================================================================

inline HLRBRep_Curve& HLRBRep_EdgeData::ChangeGeometry()
{
  return myGeometry;
}

//=================================================================================================

inline const HLRBRep_Curve& HLRBRep_EdgeData::Geometry() const
{
  return myGeometry;
}

//=================================================================================================

inline float HLRBRep_EdgeData::Tolerance() const
{
  return myTolerance;
}
