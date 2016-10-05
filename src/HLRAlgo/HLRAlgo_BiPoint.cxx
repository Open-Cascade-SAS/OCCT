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
  myPoints.Pnt1.SetCoord(X1, Y1, Z1);
  myPoints.Pnt2.SetCoord(X2, Y2, Z2);
  myPoints.PntP1.SetCoord(XT1, YT1, ZT1);
  myPoints.PntP2.SetCoord(XT2, YT2, ZT2);
  myIndices.ShapeIndex = Index;
  myIndices.FaceConex1 = myIndices.Face1Pt1 = myIndices.Face1Pt2 =
    myIndices.FaceConex2 = myIndices.Face2Pt1 = myIndices.Face2Pt2 = 0;
  myIndices.SegFlags = 0;
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
				  const Standard_Integer flag)
{
  myPoints.Pnt1 = gp_XYZ(X1, Y1, Z1);
  myPoints.Pnt2 = gp_XYZ(X2, Y2, Z2);
  myPoints.PntP1 = gp_XYZ(XT1, YT1, ZT1);
  myPoints.PntP2 = gp_XYZ(XT2, YT2, ZT2);
  myIndices.ShapeIndex = Index;
  myIndices.FaceConex1 = myIndices.Face1Pt1 = myIndices.Face1Pt2 =
    myIndices.FaceConex2 = myIndices.Face2Pt1 = myIndices.Face2Pt2 = 0;
  myIndices.SegFlags = flag;
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
  myPoints.Pnt1.SetCoord(X1, Y1, Z1);
  myPoints.Pnt2.SetCoord(X2, Y2, Z2);
  myPoints.PntP1.SetCoord(XT1, YT1, ZT1);
  myPoints.PntP2.SetCoord(XT2, YT2, ZT2);
  myIndices.ShapeIndex = Index;
  myIndices.FaceConex1 = i1;
  myIndices.Face1Pt1 = i1p1;
  myIndices.Face1Pt2 = i1p2;
  myIndices.FaceConex2 = myIndices.Face2Pt1 = myIndices.Face2Pt2 = 0;
  myIndices.SegFlags = 0;
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
				  const Standard_Integer flag)
{
  myPoints.Pnt1.SetCoord(X1, Y1, Z1);
  myPoints.Pnt2.SetCoord(X2, Y2, Z2);
  myPoints.PntP1.SetCoord(XT1, YT1, ZT1);
  myPoints.PntP2.SetCoord(XT2, YT2, ZT2);
  myIndices.ShapeIndex = Index;
  myIndices.FaceConex1 = i1;
  myIndices.Face1Pt1 = i1p1;
  myIndices.Face1Pt2 = i1p2;
  myIndices.FaceConex2 = myIndices.Face2Pt1 = myIndices.Face2Pt2 = 0;
  myIndices.SegFlags = flag;
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
  myPoints.Pnt1.SetCoord(X1, Y1, Z1);
  myPoints.Pnt2.SetCoord(X2, Y2, Z2);
  myPoints.PntP1.SetCoord(XT1, YT1, ZT1);
  myPoints.PntP2.SetCoord(XT2, YT2, ZT2);
  myIndices.ShapeIndex = Index;
  myIndices.FaceConex1 = i1;
  myIndices.Face1Pt1 = i1p1;
  myIndices.Face1Pt2 = i1p2;
  myIndices.FaceConex2 = i2;
  myIndices.Face2Pt1 = i2p1;
  myIndices.Face2Pt2 = i2p2;
  myIndices.SegFlags = 0;
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
				  const Standard_Integer flag)
{
  myPoints.Pnt1.SetCoord(X1, Y1, Z1);
  myPoints.Pnt2.SetCoord(X2, Y2, Z2);
  myPoints.PntP1.SetCoord(XT1, YT1, ZT1);
  myPoints.PntP2.SetCoord(XT2, YT2, ZT2);
  myIndices.ShapeIndex = Index;
  myIndices.FaceConex1 = i1;
  myIndices.Face1Pt1 = i1p1;
  myIndices.Face1Pt2 = i1p2;
  myIndices.FaceConex2 = i2;
  myIndices.Face2Pt1 = i2p1;
  myIndices.Face2Pt2 = i2p2;
  myIndices.SegFlags = flag;
  Hidden(Standard_False);
}
