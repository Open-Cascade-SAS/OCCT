// Created on: 1995-06-22
// Created by: Christophe MARION
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

#ifndef _HLRAlgo_BiPoint_HeaderFile
#define _HLRAlgo_BiPoint_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Real.hxx>
#include <Standard_Boolean.hxx>
#include <gp_XYZ.hxx>
#include <gp_XY.hxx>

class HLRAlgo_BiPoint
{
public:
  struct IndicesT
  {
    IndicesT()
        : ShapeIndex(-1),
          FaceConex1(0),
          Face1Pt1(0),
          Face1Pt2(0),
          FaceConex2(0),
          Face2Pt1(0),
          Face2Pt2(0),
          MinSeg(0),
          MaxSeg(0),
          SegFlags(0)
    {
    }

    int ShapeIndex;
    int FaceConex1;
    int Face1Pt1;
    int Face1Pt2;
    int FaceConex2;
    int Face2Pt1;
    int Face2Pt2;
    int MinSeg;
    int MaxSeg;
    int SegFlags;
  };

  struct PointsT
  {
    gp_XYZ Pnt1;
    gp_XYZ Pnt2;
    gp_XYZ PntP1;
    gp_XYZ PntP2;

    gp_XY PntP12D() const { return gp_XY(PntP1.X(), PntP1.Y()); }

    gp_XY PntP22D() const { return gp_XY(PntP2.X(), PntP2.Y()); }
  };

  DEFINE_STANDARD_ALLOC

  HLRAlgo_BiPoint() {}

  Standard_EXPORT HLRAlgo_BiPoint(const double    X1,
                                  const double    Y1,
                                  const double    Z1,
                                  const double    X2,
                                  const double    Y2,
                                  const double    Z2,
                                  const double    XT1,
                                  const double    YT1,
                                  const double    ZT1,
                                  const double    XT2,
                                  const double    YT2,
                                  const double    ZT2,
                                  const int Index,
                                  const bool reg1,
                                  const bool regn,
                                  const bool outl,
                                  const bool intl);

  Standard_EXPORT HLRAlgo_BiPoint(const double    X1,
                                  const double    Y1,
                                  const double    Z1,
                                  const double    X2,
                                  const double    Y2,
                                  const double    Z2,
                                  const double    XT1,
                                  const double    YT1,
                                  const double    ZT1,
                                  const double    XT2,
                                  const double    YT2,
                                  const double    ZT2,
                                  const int Index,
                                  const int flag);

  Standard_EXPORT HLRAlgo_BiPoint(const double    X1,
                                  const double    Y1,
                                  const double    Z1,
                                  const double    X2,
                                  const double    Y2,
                                  const double    Z2,
                                  const double    XT1,
                                  const double    YT1,
                                  const double    ZT1,
                                  const double    XT2,
                                  const double    YT2,
                                  const double    ZT2,
                                  const int Index,
                                  const int i1,
                                  const int i1p1,
                                  const int i1p2,
                                  const bool reg1,
                                  const bool regn,
                                  const bool outl,
                                  const bool intl);

  Standard_EXPORT HLRAlgo_BiPoint(const double    X1,
                                  const double    Y1,
                                  const double    Z1,
                                  const double    X2,
                                  const double    Y2,
                                  const double    Z2,
                                  const double    XT1,
                                  const double    YT1,
                                  const double    ZT1,
                                  const double    XT2,
                                  const double    YT2,
                                  const double    ZT2,
                                  const int Index,
                                  const int i1,
                                  const int i1p1,
                                  const int i1p2,
                                  const int flag);

  Standard_EXPORT HLRAlgo_BiPoint(const double    X1,
                                  const double    Y1,
                                  const double    Z1,
                                  const double    X2,
                                  const double    Y2,
                                  const double    Z2,
                                  const double    XT1,
                                  const double    YT1,
                                  const double    ZT1,
                                  const double    XT2,
                                  const double    YT2,
                                  const double    ZT2,
                                  const int Index,
                                  const int i1,
                                  const int i1p1,
                                  const int i1p2,
                                  const int i2,
                                  const int i2p1,
                                  const int i2p2,
                                  const bool reg1,
                                  const bool regn,
                                  const bool outl,
                                  const bool intl);

  Standard_EXPORT HLRAlgo_BiPoint(const double    X1,
                                  const double    Y1,
                                  const double    Z1,
                                  const double    X2,
                                  const double    Y2,
                                  const double    Z2,
                                  const double    XT1,
                                  const double    YT1,
                                  const double    ZT1,
                                  const double    XT2,
                                  const double    YT2,
                                  const double    ZT2,
                                  const int Index,
                                  const int i1,
                                  const int i1p1,
                                  const int i1p2,
                                  const int i2,
                                  const int i2p1,
                                  const int i2p2,
                                  const int flag);

  bool Rg1Line() const { return (myIndices.SegFlags & EMskRg1Line) != 0; }

  void Rg1Line(const bool B)
  {
    if (B)
      myIndices.SegFlags |= EMskRg1Line;
    else
      myIndices.SegFlags &= ~EMskRg1Line;
  }

  bool RgNLine() const { return (myIndices.SegFlags & EMskRgNLine) != 0; }

  void RgNLine(const bool B)
  {
    if (B)
      myIndices.SegFlags |= EMskRgNLine;
    else
      myIndices.SegFlags &= ~EMskRgNLine;
  }

  bool OutLine() const { return (myIndices.SegFlags & EMskOutLine) != 0; }

  void OutLine(const bool B)
  {
    if (B)
      myIndices.SegFlags |= EMskOutLine;
    else
      myIndices.SegFlags &= ~EMskOutLine;
  }

  bool IntLine() const { return (myIndices.SegFlags & EMskIntLine) != 0; }

  void IntLine(const bool B)
  {
    if (B)
      myIndices.SegFlags |= EMskIntLine;
    else
      myIndices.SegFlags &= ~EMskIntLine;
  }

  bool Hidden() const { return (myIndices.SegFlags & EMskHidden) != 0; }

  void Hidden(const bool B)
  {
    if (B)
      myIndices.SegFlags |= EMskHidden;
    else
      myIndices.SegFlags &= ~EMskHidden;
  }

  IndicesT& Indices() { return myIndices; }

  PointsT& Points() { return myPoints; }

protected:
  enum EMskFlags
  {
    EMskRg1Line = 1,
    EMskRgNLine = 2,
    EMskOutLine = 4,
    EMskIntLine = 8,
    EMskHidden  = 16
  };

private:
  IndicesT myIndices;
  PointsT  myPoints;
};

#endif // _HLRAlgo_BiPoint_HeaderFile
