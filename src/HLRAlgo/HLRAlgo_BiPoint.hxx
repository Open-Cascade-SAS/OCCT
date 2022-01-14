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
    Standard_Integer ShapeIndex;
    Standard_Integer FaceConex1;
    Standard_Integer Face1Pt1;
    Standard_Integer Face1Pt2;
    Standard_Integer FaceConex2;
    Standard_Integer Face2Pt1;
    Standard_Integer Face2Pt2;
    Standard_Integer MinSeg;
    Standard_Integer MaxSeg;
    Standard_Integer SegFlags;
  };

  struct PointsT
  {
    gp_XYZ Pnt1;
    gp_XYZ Pnt2;
    gp_XYZ PntP1;
    gp_XYZ PntP2;

    gp_XY PntP12D() const
    {
      return gp_XY(PntP1.X(), PntP1.Y());
    }

    gp_XY PntP22D() const
    {
      return gp_XY(PntP2.X(), PntP2.Y());
    }
  };

  DEFINE_STANDARD_ALLOC

  HLRAlgo_BiPoint() {}

  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Boolean reg1, const Standard_Boolean regn, const Standard_Boolean outl, const Standard_Boolean intl);
  
  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Integer flag);
  
  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Integer i1, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Boolean reg1, const Standard_Boolean regn, const Standard_Boolean outl, const Standard_Boolean intl);
  
  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Integer i1, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Integer flag);
  
  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Integer i1, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Integer i2, const Standard_Integer i2p1, const Standard_Integer i2p2, const Standard_Boolean reg1, const Standard_Boolean regn, const Standard_Boolean outl, const Standard_Boolean intl);
  
  Standard_EXPORT HLRAlgo_BiPoint(const Standard_Real X1, const Standard_Real Y1, const Standard_Real Z1, const Standard_Real X2, const Standard_Real Y2, const Standard_Real Z2, const Standard_Real XT1, const Standard_Real YT1, const Standard_Real ZT1, const Standard_Real XT2, const Standard_Real YT2, const Standard_Real ZT2, const Standard_Integer Index, const Standard_Integer i1, const Standard_Integer i1p1, const Standard_Integer i1p2, const Standard_Integer i2, const Standard_Integer i2p1, const Standard_Integer i2p2, const Standard_Integer flag);

  Standard_Boolean Rg1Line() const
  {
    return (myIndices.SegFlags & EMskRg1Line) != 0;
  }

  void Rg1Line (const Standard_Boolean B)
  {
    if (B) myIndices.SegFlags |=  EMskRg1Line;
    else   myIndices.SegFlags &= ~EMskRg1Line;
  }

  Standard_Boolean RgNLine() const
  {
    return (myIndices.SegFlags & EMskRgNLine) != 0;
  }

  void RgNLine (const Standard_Boolean B)
  {
    if (B) myIndices.SegFlags |=  EMskRgNLine;
    else   myIndices.SegFlags &= ~EMskRgNLine;
  }

  Standard_Boolean OutLine() const
  {
    return (myIndices.SegFlags & EMskOutLine) != 0;
  }

  void OutLine (const Standard_Boolean B)
  {
    if (B) myIndices.SegFlags |=  EMskOutLine;
    else   myIndices.SegFlags &= ~EMskOutLine;
  }

  Standard_Boolean IntLine() const
  {
    return (myIndices.SegFlags & EMskIntLine) != 0;
  }

  void IntLine (const Standard_Boolean B)
  {
    if (B) myIndices.SegFlags |=  EMskIntLine;
    else   myIndices.SegFlags &= ~EMskIntLine;
  }

  Standard_Boolean Hidden() const
  {
    return (myIndices.SegFlags & EMskHidden) != 0;
  }

  void Hidden (const Standard_Boolean B)
  {
    if (B) myIndices.SegFlags |=  EMskHidden;
    else   myIndices.SegFlags &= ~EMskHidden;
  }

  IndicesT& Indices()
  {
    return myIndices;
  }

  PointsT& Points()
  {
    return myPoints;
  }

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
  PointsT myPoints;

};

#endif // _HLRAlgo_BiPoint_HeaderFile
