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

#ifndef No_Exception
#define No_Exception
#endif


#include <HLRAlgo_BiPoint.hxx>

#define PntX1  myCoordinates[ 0]
#define PntY1  myCoordinates[ 1]
#define PntZ1  myCoordinates[ 2]
#define PntX2  myCoordinates[ 3]
#define PntY2  myCoordinates[ 4]
#define PntZ2  myCoordinates[ 5]
#define PntXP1 myCoordinates[ 6]
#define PntYP1 myCoordinates[ 7]
#define PntZP1 myCoordinates[ 8]
#define PntXP2 myCoordinates[ 9]
#define PntYP2 myCoordinates[10]
#define PntZP2 myCoordinates[11]

#define ShapeIndex  myIndices[0]
#define FaceConex1  myIndices[1]
#define Face1Pt1    myIndices[2]
#define Face1Pt2    myIndices[3]
#define FaceConex2  myIndices[4]
#define Face2Pt1    myIndices[5]
#define Face2Pt2    myIndices[6]
#define MinSeg      myIndices[7]
#define MaxSeg      myIndices[8]
#define SegFlags    myIndices[9]

//=======================================================================
//function : HLRAlgo_BiPoint
//purpose  : 
//=======================================================================

HLRAlgo_BiPoint::HLRAlgo_BiPoint (const Standard_Real X1,
				  const Standard_Real Y1,
				  const Standard_Real Z1,
				  const Standard_Real X2,
				  const Standard_Real Y2,
				  const Standard_Real Z2,
				  const Standard_Real XT1,
				  const Standard_Real YT1,
				  const Standard_Real ZT1,
				  const Standard_Real XT2,
				  const Standard_Real YT2,
				  const Standard_Real ZT2,
				  const Standard_Integer Index,
				  const Standard_Boolean reg1,
				  const Standard_Boolean regn,
				  const Standard_Boolean outl,
				  const Standard_Boolean intl)
{
  PntX1 = X1;
  PntY1 = Y1;
  PntZ1 = Z1;
  PntX2 = X2;
  PntY2 = Y2;
  PntZ2 = Z2;
  PntXP1 = XT1;
  PntYP1 = YT1;
  PntZP1 = ZT1;
  PntXP2 = XT2;
  PntYP2 = YT2;
  PntZP2 = ZT2;
  ShapeIndex = Index;
  FaceConex1 = Face1Pt1   = Face1Pt2   = 
    FaceConex2 = Face2Pt1   = Face2Pt2   = 0;
  SegFlags   = 0;
  Rg1Line(reg1);
  RgNLine(regn);
  OutLine(outl);
  IntLine(intl);
  Hidden(Standard_False);
}

//=======================================================================
//function : HLRAlgo_BiPoint
//purpose  : 
//=======================================================================

HLRAlgo_BiPoint::HLRAlgo_BiPoint (const Standard_Real X1,
				  const Standard_Real Y1,
				  const Standard_Real Z1,
				  const Standard_Real X2,
				  const Standard_Real Y2,
				  const Standard_Real Z2,
				  const Standard_Real XT1,
				  const Standard_Real YT1,
				  const Standard_Real ZT1,
				  const Standard_Real XT2,
				  const Standard_Real YT2,
				  const Standard_Real ZT2,
				  const Standard_Integer Index,
				  const Standard_Boolean flag)
{
  PntX1 = X1;
  PntY1 = Y1;
  PntZ1 = Z1;
  PntX2 = X2;
  PntY2 = Y2;
  PntZ2 = Z2;
  PntXP1 = XT1;
  PntYP1 = YT1;
  PntZP1 = ZT1;
  PntXP2 = XT2;
  PntYP2 = YT2;
  PntZP2 = ZT2;
  ShapeIndex = Index;
  FaceConex1 = Face1Pt1   = Face1Pt2   = 
    FaceConex2 = Face2Pt1   = Face2Pt2   = 0;
  SegFlags   = flag;
  Hidden(Standard_False);
}

//=======================================================================
//function : HLRAlgo_BiPoint
//purpose  : 
//=======================================================================

HLRAlgo_BiPoint::HLRAlgo_BiPoint (const Standard_Real X1,
				  const Standard_Real Y1,
				  const Standard_Real Z1,
				  const Standard_Real X2,
				  const Standard_Real Y2,
				  const Standard_Real Z2,
				  const Standard_Real XT1,
				  const Standard_Real YT1,
				  const Standard_Real ZT1,
				  const Standard_Real XT2,
				  const Standard_Real YT2,
				  const Standard_Real ZT2,
				  const Standard_Integer Index,
				  const Standard_Integer i1,
				  const Standard_Integer i1p1,
				  const Standard_Integer i1p2,
				  const Standard_Boolean reg1,
				  const Standard_Boolean regn,
				  const Standard_Boolean outl,
				  const Standard_Boolean intl)
{
  PntX1 = X1;
  PntY1 = Y1;
  PntZ1 = Z1;
  PntX2 = X2;
  PntY2 = Y2;
  PntZ2 = Z2;
  PntXP1 = XT1;
  PntYP1 = YT1;
  PntZP1 = ZT1;
  PntXP2 = XT2;
  PntYP2 = YT2;
  PntZP2 = ZT2;
  ShapeIndex = Index;
  FaceConex1 = i1;
  Face1Pt1   = i1p1;
  Face1Pt2   = i1p2;
  FaceConex2 = Face2Pt1   = Face2Pt2   = 0;
  SegFlags   = 0;
  Rg1Line(reg1);
  RgNLine(regn);
  OutLine(outl);
  IntLine(intl);
  Hidden(Standard_False);
}

//=======================================================================
//function : HLRAlgo_BiPoint
//purpose  : 
//=======================================================================

HLRAlgo_BiPoint::HLRAlgo_BiPoint (const Standard_Real X1,
				  const Standard_Real Y1,
				  const Standard_Real Z1,
				  const Standard_Real X2,
				  const Standard_Real Y2,
				  const Standard_Real Z2,
				  const Standard_Real XT1,
				  const Standard_Real YT1,
				  const Standard_Real ZT1,
				  const Standard_Real XT2,
				  const Standard_Real YT2,
				  const Standard_Real ZT2,
				  const Standard_Integer Index,
				  const Standard_Integer i1,
				  const Standard_Integer i1p1,
				  const Standard_Integer i1p2,
				  const Standard_Boolean flag)
{
  PntX1 = X1;
  PntY1 = Y1;
  PntZ1 = Z1;
  PntX2 = X2;
  PntY2 = Y2;
  PntZ2 = Z2;
  PntXP1 = XT1;
  PntYP1 = YT1;
  PntZP1 = ZT1;
  PntXP2 = XT2;
  PntYP2 = YT2;
  PntZP2 = ZT2;
  ShapeIndex = Index;
  FaceConex1 = i1;
  Face1Pt1   = i1p1;
  Face1Pt2   = i1p2;
  FaceConex2 = Face2Pt1   = Face2Pt2   = 0;
  SegFlags   = flag;
  Hidden(Standard_False);
}

//=======================================================================
//function : HLRAlgo_BiPoint
//purpose  : 
//=======================================================================

HLRAlgo_BiPoint::HLRAlgo_BiPoint (const Standard_Real X1,
				  const Standard_Real Y1,
				  const Standard_Real Z1,
				  const Standard_Real X2,
				  const Standard_Real Y2,
				  const Standard_Real Z2,
				  const Standard_Real XT1,
				  const Standard_Real YT1,
				  const Standard_Real ZT1,
				  const Standard_Real XT2,
				  const Standard_Real YT2,
				  const Standard_Real ZT2,
				  const Standard_Integer Index,
				  const Standard_Integer i1,
				  const Standard_Integer i1p1,
				  const Standard_Integer i1p2,
				  const Standard_Integer i2,
				  const Standard_Integer i2p1,
				  const Standard_Integer i2p2,
				  const Standard_Boolean reg1,
				  const Standard_Boolean regn,
				  const Standard_Boolean outl,
				  const Standard_Boolean intl)
{
  PntX1 = X1;
  PntY1 = Y1;
  PntZ1 = Z1;
  PntX2 = X2;
  PntY2 = Y2;
  PntZ2 = Z2;
  PntXP1 = XT1;
  PntYP1 = YT1;
  PntZP1 = ZT1;
  PntXP2 = XT2;
  PntYP2 = YT2;
  PntZP2 = ZT2;
  ShapeIndex = Index;
  FaceConex1 = i1;
  Face1Pt1   = i1p1;
  Face1Pt2   = i1p2;
  FaceConex2 = i2;
  Face2Pt1   = i2p1;
  Face2Pt2   = i2p2;
  SegFlags   = 0;
  Rg1Line(reg1);
  RgNLine(regn);
  OutLine(outl);
  IntLine(intl);
  Hidden(Standard_False);
}

//=======================================================================
//function : HLRAlgo_BiPoint
//purpose  : 
//=======================================================================

HLRAlgo_BiPoint::HLRAlgo_BiPoint (const Standard_Real X1,
				  const Standard_Real Y1,
				  const Standard_Real Z1,
				  const Standard_Real X2,
				  const Standard_Real Y2,
				  const Standard_Real Z2,
				  const Standard_Real XT1,
				  const Standard_Real YT1,
				  const Standard_Real ZT1,
				  const Standard_Real XT2,
				  const Standard_Real YT2,
				  const Standard_Real ZT2,
				  const Standard_Integer Index,
				  const Standard_Integer i1,
				  const Standard_Integer i1p1,
				  const Standard_Integer i1p2,
				  const Standard_Integer i2,
				  const Standard_Integer i2p1,
				  const Standard_Integer i2p2,
				  const Standard_Boolean flag)
{
  PntX1 = X1;
  PntY1 = Y1;
  PntZ1 = Z1;
  PntX2 = X2;
  PntY2 = Y2;
  PntZ2 = Z2;
  PntXP1 = XT1;
  PntYP1 = YT1;
  PntZP1 = ZT1;
  PntXP2 = XT2;
  PntYP2 = YT2;
  PntZP2 = ZT2;
  ShapeIndex = Index;
  FaceConex1 = i1;
  Face1Pt1   = i1p1;
  Face1Pt2   = i1p2;
  FaceConex2 = i2;
  Face2Pt1   = i2p1;
  Face2Pt2   = i2p2;
  SegFlags   = flag;
  Hidden(Standard_False);
}
