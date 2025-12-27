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

#ifndef _HLRBRep_EdgeData_HeaderFile
#define _HLRBRep_EdgeData_HeaderFile

#include <HLRAlgo_WiresBlock.hxx>

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Standard_Integer.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRBRep_Curve.hxx>
class TopoDS_Edge;

// resolve name collisions with X11 headers
#ifdef Status
  #undef Status
#endif

class HLRBRep_EdgeData
{
public:
  DEFINE_STANDARD_ALLOC

  HLRBRep_EdgeData()
      : myFlags(0),
        myHideCount(0)
  {
    Selected(true);
  }

  Standard_EXPORT void Set(const bool   Reg1,
                           const bool   RegN,
                           const TopoDS_Edge&       EG,
                           const int   V1,
                           const int   V2,
                           const bool   Out1,
                           const bool   Out2,
                           const bool   Cut1,
                           const bool   Cut2,
                           const double      Start,
                           const float TolStart,
                           const double      End,
                           const float TolEnd);

  bool Selected() const;

  void Selected(const bool B);

  bool Rg1Line() const;

  void Rg1Line(const bool B);

  bool RgNLine() const;

  void RgNLine(const bool B);

  bool Vertical() const;

  void Vertical(const bool B);

  bool Simple() const;

  void Simple(const bool B);

  bool OutLVSta() const;

  void OutLVSta(const bool B);

  bool OutLVEnd() const;

  void OutLVEnd(const bool B);

  bool CutAtSta() const;

  void CutAtSta(const bool B);

  bool CutAtEnd() const;

  void CutAtEnd(const bool B);

  bool VerAtSta() const;

  void VerAtSta(const bool B);

  bool VerAtEnd() const;

  void VerAtEnd(const bool B);

  bool AutoIntersectionDone() const;

  void AutoIntersectionDone(const bool B);

  bool Used() const;

  void Used(const bool B);

  int HideCount() const;

  void HideCount(const int I);

  int VSta() const;

  void VSta(const int I);

  int VEnd() const;

  void VEnd(const int I);

  void UpdateMinMax(const HLRAlgo_EdgesBlock::MinMaxIndices& theTotMinMax)
  {
    myMinMax = theTotMinMax;
  }

  HLRAlgo_EdgesBlock::MinMaxIndices& MinMax() { return myMinMax; }

  HLRAlgo_EdgeStatus& Status();

  HLRBRep_Curve& ChangeGeometry();

  const HLRBRep_Curve& Geometry() const;

  HLRBRep_Curve* Curve() { return &myGeometry; }

  float Tolerance() const;

protected:
  enum EMaskFlags
  {
    EMaskSelected = 1,
    EMaskUsed     = 2,
    EMaskRg1Line  = 4,
    EMaskVertical = 8,
    EMaskSimple   = 16,
    EMaskOutLVSta = 32,
    EMaskOutLVEnd = 64,
    EMaskIntDone  = 128,
    EMaskCutAtSta = 256,
    EMaskCutAtEnd = 512,
    EMaskVerAtSta = 1024,
    EMaskVerAtEnd = 2048,
    EMaskRgNLine  = 4096
  };

private:
  int                  myFlags;
  int                  myHideCount;
  int                  myVSta;
  int                  myVEnd;
  HLRAlgo_EdgesBlock::MinMaxIndices myMinMax;
  HLRAlgo_EdgeStatus                myStatus;
  HLRBRep_Curve                     myGeometry;
  float                myTolerance;
};

#include <HLRBRep_EdgeData.lxx>

#endif // _HLRBRep_EdgeData_HeaderFile
