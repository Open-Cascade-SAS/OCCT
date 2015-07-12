// Created on: 1993-01-11
// Created by: Christophe MARION
// Copyright (c) 1993-1999 Matra Datavision
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
//#define No_Exception
#endif


#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRAlgo_PolyData.hxx>
#include <Standard_Type.hxx>

#define EMskGrALin1  ((Standard_Boolean)   8)
#define EMskGrALin2  ((Standard_Boolean)  16)
#define EMskGrALin3  ((Standard_Boolean)  32)
#define FMskHiding   ((Standard_Boolean) 256)

#define FIndex  myIndices[0]
#define MinFac  myIndices[1]
#define MaxFac  myIndices[2]

#define TriNode1   ((Standard_Integer*)TriIndices)[0]
#define TriNode2   ((Standard_Integer*)TriIndices)[1]
#define TriNode3   ((Standard_Integer*)TriIndices)[2]
#define TriFlags   ((Standard_Boolean*)TriIndices)[3]

#define Crossing   ((Standard_Boolean*)BooleanPtr)[0]
#define HideBefore ((Standard_Boolean*)BooleanPtr)[1]
#define TrFlags    ((Standard_Boolean*)BooleanPtr)[2]

#define Crosi      BooleanPtr[0]
#define HdBef      BooleanPtr[1]
#define TFlag      BooleanPtr[2]

#define PntX1  ((Standard_Real*)Coordinates)[ 0]
#define PntY1  ((Standard_Real*)Coordinates)[ 1]
#define PntZ1  ((Standard_Real*)Coordinates)[ 2]
#define PntX2  ((Standard_Real*)Coordinates)[ 3]
#define PntY2  ((Standard_Real*)Coordinates)[ 4]
#define PntZ2  ((Standard_Real*)Coordinates)[ 5]
#define PntXP1 ((Standard_Real*)Coordinates)[ 6]
#define PntYP1 ((Standard_Real*)Coordinates)[ 7]
#define PntZP1 ((Standard_Real*)Coordinates)[ 8]
#define PntXP2 ((Standard_Real*)Coordinates)[ 9]
#define PntYP2 ((Standard_Real*)Coordinates)[10]
#define PntZP2 ((Standard_Real*)Coordinates)[11]

#define XV1        ((Standard_Real*)RealPtr)[0]
#define XV2        ((Standard_Real*)RealPtr)[1]
#define XV3        ((Standard_Real*)RealPtr)[2]
#define YV1        ((Standard_Real*)RealPtr)[3]
#define YV2        ((Standard_Real*)RealPtr)[4]
#define YV3        ((Standard_Real*)RealPtr)[5]
#define Param      ((Standard_Real*)RealPtr)[6]
#define TolParam   ((Standard_Real*)RealPtr)[7]
#define TolAng     ((Standard_Real*)RealPtr)[8]
#define Tolerance  ((Standard_Real*)RealPtr)[9]

#define TotXMin    ((Standard_Real*)TotMinMax)[0]
#define TotYMin    ((Standard_Real*)TotMinMax)[1]
#define TotZMin    ((Standard_Real*)TotMinMax)[2]
#define TotXMax    ((Standard_Real*)TotMinMax)[3]
#define TotYMax    ((Standard_Real*)TotMinMax)[4]
#define TotZMax    ((Standard_Real*)TotMinMax)[5]

#define IndexTri   ((Standard_Integer*)MinMaxPtr)[0]
#define MinTri     ((Standard_Integer*)MinMaxPtr)[1]
#define MaxTri     ((Standard_Integer*)MinMaxPtr)[2]

#define APlan      ((Standard_Real*)PlanPtr)[0]
#define BPlan      ((Standard_Real*)PlanPtr)[1]
#define CPlan      ((Standard_Real*)PlanPtr)[2]
#define DPlan      ((Standard_Real*)PlanPtr)[3]

#define ShapeIndex  ((Standard_Integer*)Indices)[0]
#define FaceConex1  ((Standard_Integer*)Indices)[1]
#define Face1Pt1    ((Standard_Integer*)Indices)[2]
#define Face1Pt2    ((Standard_Integer*)Indices)[3]
#define FaceConex2  ((Standard_Integer*)Indices)[4]
#define Face2Pt1    ((Standard_Integer*)Indices)[5]
#define Face2Pt2    ((Standard_Integer*)Indices)[6]
#define MinSeg      ((Standard_Integer*)Indices)[7]
#define MaxSeg      ((Standard_Integer*)Indices)[8]
#define SegFlags    ((Standard_Integer*)Indices)[9]
#ifdef OCCT_DEBUG
static Standard_Integer ERROR = Standard_False;
#endif
//=======================================================================
//function : PolyData
//purpose  : 
//=======================================================================

HLRAlgo_PolyData::HLRAlgo_PolyData ()
{}

//=======================================================================
//function : HNodes
//purpose  : 
//=======================================================================

void HLRAlgo_PolyData::HNodes(const Handle(TColgp_HArray1OfXYZ)& HNodes)
{ myHNodes = HNodes; }

//=======================================================================
//function : HTData
//purpose  : 
//=======================================================================

void HLRAlgo_PolyData::HTData(const Handle(HLRAlgo_HArray1OfTData)& HTData)
{ myHTData = HTData; }

//=======================================================================
//function : HPHDat
//purpose  : 
//=======================================================================

void HLRAlgo_PolyData::HPHDat(const Handle(HLRAlgo_HArray1OfPHDat)& HPHDat)
{ myHPHDat = HPHDat; }

//=======================================================================
//function : UpdateGlobalMinMax
//purpose  : 
//=======================================================================

void
HLRAlgo_PolyData::UpdateGlobalMinMax (const Standard_Address TotMinMax)
{
  Standard_Integer i;
  Standard_Real X1,X2,X3,Y1,Y2,Y3,Z1,Z2,Z3;
  const TColgp_Array1OfXYZ& Nodes = myHNodes->Array1();
  HLRAlgo_Array1OfTData&    TData = myHTData->ChangeArray1();
  Standard_Integer          nbT   = TData.Upper();
  HLRAlgo_TriangleData*     TD    = &(TData.ChangeValue(1));
  
  for (i = 1; i <= nbT; i++) {
    const Standard_Address TriIndices = TD->Indices();
    if (TriFlags & FMskHiding) {
      const gp_XYZ& P1 = Nodes(TriNode1);
      const gp_XYZ& P2 = Nodes(TriNode2);
      const gp_XYZ& P3 = Nodes(TriNode3);
      X1 = P1.X();
      Y1 = P1.Y();
      Z1 = P1.Z();
      X2 = P2.X();
      Y2 = P2.Y();
      Z2 = P2.Z();
      X3 = P3.X();
      Y3 = P3.Y();
      Z3 = P3.Z();
      if      (TotXMin > X1) TotXMin = X1;
      else if (TotXMax < X1) TotXMax = X1;
      if      (TotYMin > Y1) TotYMin = Y1;
      else if (TotYMax < Y1) TotYMax = Y1;
      if      (TotZMin > Z1) TotZMin = Z1;
      else if (TotZMax < Z1) TotZMax = Z1;
      if      (TotXMin > X2) TotXMin = X2;
      else if (TotXMax < X2) TotXMax = X2;
      if      (TotYMin > Y2) TotYMin = Y2;
      else if (TotYMax < Y2) TotYMax = Y2;
      if      (TotZMin > Z2) TotZMin = Z2;
      else if (TotZMax < Z2) TotZMax = Z2;
      if      (TotXMin > X3) TotXMin = X3;
      else if (TotXMax < X3) TotXMax = X3;
      if      (TotYMin > Y3) TotYMin = Y3;
      else if (TotYMax < Y3) TotYMax = Y3;
      if      (TotZMin > Z3) TotZMin = Z3;
      else if (TotZMax < Z3) TotZMax = Z3;
    }
    TD++;
  }
}

//=======================================================================
//function : HideByPolyData
//purpose  : 
//=======================================================================

void HLRAlgo_PolyData::HideByPolyData (const Standard_Address Coordinates,
				       const Standard_Address RealPtr,
				       const Standard_Address Indices,
				       const Standard_Boolean HidingShell,
				       HLRAlgo_EdgeStatus& status)
{
  if (((MaxFac - MinSeg) & 0x80100200) == 0 &&
      ((MaxSeg - MinFac) & 0x80100000) == 0) {
    HLRAlgo_Array1OfPHDat& PHDat = myHPHDat->ChangeArray1();
    const HLRAlgo_Array1OfTData& TData = myHTData->Array1();
    Standard_Real d1,d2;
    Standard_Boolean NotConnex,BooleanPtr[3];
    Standard_Address PlanPtr,MinMaxPtr,TriIndices;
    Standard_Integer h,h2 = PHDat.Upper();
    HLRAlgo_PolyHidingData* PH = &(PHDat(1));
    
    for (h = 1; h <= h2; h++) {
      MinMaxPtr = PH->IndexAndMinMax();
      if (((MaxTri - MinSeg) & 0x80100200) == 0 &&
	  ((MaxSeg - MinTri) & 0x80100000) == 0) {
	TriIndices = TData(IndexTri).Indices();
	NotConnex = Standard_True;
	if (HidingShell) {
	  if      (FIndex == FaceConex1) {
	    if      (Face1Pt1 == TriNode1)
	      NotConnex = Face1Pt2 != TriNode2 && Face1Pt2 != TriNode3;
	    else if (Face1Pt1 == TriNode2)
	      NotConnex = Face1Pt2 != TriNode3 && Face1Pt2 != TriNode1;
	    else if (Face1Pt1 == TriNode3)
	      NotConnex = Face1Pt2 != TriNode1 && Face1Pt2 != TriNode2;
	  }
	  else if (FIndex == FaceConex2) {
	    if      (Face2Pt1 == TriNode1)
	      NotConnex = Face2Pt2 != TriNode2 && Face2Pt2 != TriNode3;
	    else if (Face2Pt1 == TriNode2)
	      NotConnex = Face2Pt2 != TriNode3 && Face2Pt2 != TriNode1;
	    else if (Face2Pt1 == TriNode3)
	      NotConnex = Face2Pt2 != TriNode1 && Face2Pt2 != TriNode2;
	  }
	}
	if (NotConnex) {
	  PlanPtr = PH->Plan();
	  d1 = APlan * PntXP1 + BPlan * PntYP1 + CPlan * PntZP1 - DPlan;
	  d2 = APlan * PntXP2 + BPlan * PntYP2 + CPlan * PntZP2 - DPlan;
	  if      (d1 >  Tolerance) {
	    if    (d2 < -Tolerance) {
	      Param = d1 / ( d1 - d2 );
	      HdBef = Standard_False;
	      Crosi = Standard_True;
	      TFlag = TriFlags;
	      const TColgp_Array1OfXYZ& Nodes = myHNodes->Array1();
	      const gp_XYZ            & P1    = Nodes(TriNode1);
	      const gp_XYZ            & P2    = Nodes(TriNode2);
	      const gp_XYZ            & P3    = Nodes(TriNode3);
	      XV1 = P1.X();
	      YV1 = P1.Y();
	      XV2 = P2.X();
	      YV2 = P2.Y();
	      XV3 = P3.X();
	      YV3 = P3.Y();
	      HideByOneTriangle(Coordinates,
				RealPtr,
				&BooleanPtr,
				PlanPtr,
				status);
	    }
	  }
	  else if (d1 < -Tolerance) {
	    if    (d2 >  Tolerance) {
	      Param = d1 / ( d1 - d2 );
	      HdBef = Standard_True;
	      Crosi = Standard_True;
	      TFlag = TriFlags;
	      const TColgp_Array1OfXYZ& Nodes = myHNodes->Array1();
	      const gp_XYZ            & P1    = Nodes(TriNode1);
	      const gp_XYZ            & P2    = Nodes(TriNode2);
	      const gp_XYZ            & P3    = Nodes(TriNode3);
	      XV1 = P1.X();
	      YV1 = P1.Y();
	      XV2 = P2.X();
	      YV2 = P2.Y();
	      XV3 = P3.X();
	      YV3 = P3.Y();
	      HideByOneTriangle(Coordinates,
				RealPtr,
				&BooleanPtr,
				PlanPtr,
				status);
	    }
	    else {
	      Crosi = Standard_False;
	      TFlag = TriFlags;
	      const TColgp_Array1OfXYZ& Nodes = myHNodes->Array1();
	      const gp_XYZ            & P1    = Nodes(TriNode1);
	      const gp_XYZ            & P2    = Nodes(TriNode2);
	      const gp_XYZ            & P3    = Nodes(TriNode3);
	      XV1 = P1.X();
	      YV1 = P1.Y();
	      XV2 = P2.X();
	      YV2 = P2.Y();
	      XV3 = P3.X();
	      YV3 = P3.Y();
	      HideByOneTriangle(Coordinates,
				RealPtr,
				&BooleanPtr,
				PlanPtr,
				status);
	    }
	  }
	  else if (d2 < -Tolerance) {
	    Crosi = Standard_False;
	    TFlag = TriFlags;
	    const TColgp_Array1OfXYZ& Nodes = myHNodes->Array1();
	    const gp_XYZ            & P1    = Nodes(TriNode1);
	    const gp_XYZ            & P2    = Nodes(TriNode2);
	    const gp_XYZ            & P3    = Nodes(TriNode3);
	    XV1 = P1.X();
	    YV1 = P1.Y();
	    XV2 = P2.X();
	    YV2 = P2.Y();
	    XV3 = P3.X();
	    YV3 = P3.Y();
	    HideByOneTriangle(Coordinates,
			      RealPtr,
			      &BooleanPtr,
			      PlanPtr,
			      status);
	  }
	}
      }
      PH++;
    }
  }
}

//=======================================================================
//function : HideByOneTriangle
//purpose  : 
//=======================================================================

void HLRAlgo_PolyData::
HideByOneTriangle (const Standard_Address Coordinates,
		   const Standard_Address RealPtr,
		   const Standard_Address BooleanPtr,
		   const Standard_Address ,
		   HLRAlgo_EdgeStatus& status)
{
  Standard_Boolean o[2],m[2];
  Standard_Integer l,n1=0,nn1,nn2,npi=-1,npiRej=0;
  Standard_Real a,b,c,da,db,d1,d2,p[2]={0.,0.},pd1,pd2,pdp,pp,psta=0.,pend=1.;
  Standard_Boolean CrosSeg;

  da = XV2 - XV1;
  db = YV2 - YV1;
  a = -db;
  b =  da;
  c = sqrt( a * a + b * b);
  a /= c;
  b /= c;
  c  = a * XV1  + b * YV1;
  d1 = a * PntXP1 + b * PntYP1 - c;
  d2 = a * PntXP2 + b * PntYP2 - c;
  if      (d1 >  Tolerance) {
    if (d2 < -Tolerance) {
      n1 =  2;
      CrosSeg = Standard_True;
    }
    else
      CrosSeg = Standard_False;
  }
  else if (d1 < -Tolerance) {
    if (d2 >  Tolerance) {
      n1 = -1;
      CrosSeg = Standard_True;
    }
    else return;
  }
  else {
    if      (d2 >  Tolerance)
      CrosSeg = Standard_False;
    else if (d2 < -Tolerance) return;
    else {
      CrosSeg = Standard_False;
      if (TrFlags & EMskGrALin1) {
	pd1 = (PntXP1 - XV1) / da;
	pd2 = (PntXP2 - XV1) / da;
      }
      else {
	pd1 = (PntYP1 - YV1) / db;
	pd2 = (PntYP2 - YV1) / db;
      }
      if      (pd1      < -TolParam) nn1 = 1;
      else if (pd1      <  TolParam) nn1 = 2;
      else if (pd1 - 1. < -TolParam) nn1 = 3;
      else if (pd1 - 1. <  TolParam) nn1 = 4;
      else                           nn1 = 5;
      if      (pd2      < -TolParam) nn2 = 1;
      else if (pd2      <  TolParam) nn2 = 2;
      else if (pd2 - 1. < -TolParam) nn2 = 3;
      else if (pd2 - 1. <  TolParam) nn2 = 4;
      else                           nn2 = 5;
      if      (nn1 == 3) {
	if      (nn2 == 1) pend = pd1 / (pd1 - pd2);
	else if (nn2 == 5) pend = (1. - pd1) / (pd2 - pd1);
      }
      else if (nn1 == 1) {
	if (nn2 <= 2) return;
	else {
	  psta = - pd1 / (pd2 - pd1);
	  if (nn2 == 5) pend = (1. - pd1) / (pd2 - pd1);
	}
      }
      else if (nn1 == 5) {
	if (nn2 >= 4) return;
	else {
	  psta = (pd1 - 1.) / (pd1 - pd2);
	  if (nn2 == 1) pend = pd1 / (pd1 - pd2);
	}
      }
      else if (nn1 == 2) {
	if (nn2 == 1) return;
	else if (nn2 == 5) pend = (1. - pd1) / (pd2 - pd1);
      }
      else if (nn1 == 4) {
	if (nn2 == 5) return;
	else if (nn2 == 1) pend = pd1 / (pd1 - pd2);
      }
    }
  }
  if (CrosSeg) {
    Standard_Real ad1 = d1;
    if (d1 < 0) ad1 = -d1;
    Standard_Real ad2 = d2;
    if (d2 < 0) ad2 = -d2;
    pp = ad1 / ( ad1 + ad2 );
    if (TrFlags & EMskGrALin1)
      pdp = (PntXP1 + (PntXP2 - PntXP1) * pp - XV1) / da;
    else
      pdp = (PntYP1 + (PntYP2 - PntYP1) * pp - YV1) / db;
    Standard_Boolean OutSideP = Standard_False;
    Standard_Boolean Multiple = Standard_False;
    if      (pdp      < -TolParam) OutSideP = Standard_True;
    else if (pdp      <  TolParam) {
      Multiple = Standard_True;

      for (l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;

	  if (o[l] != (Standard_Boolean)(n1 == -1)) {
	    if (l == 0 && npi == 1) {
	      p[0] = p[1];
	      o[0] = o[1];
	      m[0] = m[1];
	    }
	    npi--;
	    npiRej++;
	  }
	}
      }
    }
    else if (pdp - 1. < -TolParam) {}
    else if (pdp - 1. <  TolParam) {
      Multiple = Standard_True;

      for (l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;
	  if (o[l] != (Standard_Boolean)(n1 == -1)) {
	    if (l == 0 && npi == 1) {
	      p[0] = p[1];
	      o[0] = o[1];
	      m[0] = m[1];
	    }
	    npi--;
	    npiRej++;
	  }
	}
      }
    }
    else                           OutSideP = Standard_True;
    if (OutSideP) npiRej++;
    else {
      npi++;
      if (npi < 2) {
	p[npi] = pp;
	o[npi] = n1 == -1;
	m[npi] = Multiple;
      }
#ifdef OCCT_DEBUG
      else if (ERROR) {
	cout << " error : HLRAlgo_PolyData::HideByOneTriangle " << endl;
	cout << " ( more than 2 points )." << endl;
      }
#endif
    }
  }

  da = XV3 - XV2;
  db = YV3 - YV2;
  a = -db;
  b =  da;
  c = sqrt(a * a + b * b);
  a /= c;
  b /= c;
  c  = a * XV2  + b * YV2;
  d1 = a * PntXP1 + b * PntYP1 - c;
  d2 = a * PntXP2 + b * PntYP2 - c;
  if      (d1 >  Tolerance) {
    if (d2 < -Tolerance) {
      n1 =  2;
      CrosSeg = Standard_True;
    }
    else
      CrosSeg = Standard_False;
  }
  else if (d1 < -Tolerance) {
    if (d2 >  Tolerance) {
      n1 = -1;
      CrosSeg = Standard_True;
    }
    else return;
  }
  else {
    if      (d2 >  Tolerance)
      CrosSeg = Standard_False;
    else if (d2 < -Tolerance) return;
    else {
      CrosSeg = Standard_False;
      if (TrFlags & EMskGrALin2) {
	pd1 = (PntXP1 - XV2) / da;
	pd2 = (PntXP2 - XV2) / da;
      }
      else {
	pd1 = (PntYP1 - YV2) / db;
	pd2 = (PntYP2 - YV2) / db;
      }
      if      (pd1      < -TolParam) nn1 = 1;
      else if (pd1      <  TolParam) nn1 = 2;
      else if (pd1 - 1. < -TolParam) nn1 = 3;
      else if (pd1 - 1. <  TolParam) nn1 = 4;
      else                           nn1 = 5;
      if      (pd2      < -TolParam) nn2 = 1;
      else if (pd2      <  TolParam) nn2 = 2;
      else if (pd2 - 1. < -TolParam) nn2 = 3;
      else if (pd2 - 1. <  TolParam) nn2 = 4;
      else                           nn2 = 5;
      if      (nn1 == 3) {
	if      (nn2 == 1) pend = pd1 / (pd1 - pd2);
	else if (nn2 == 5) pend = (1. - pd1) / (pd2 - pd1);
      }
      else if (nn1 == 1) {
	if (nn2 <= 2) return;
	else {
	  psta = - pd1 / (pd2 - pd1);
	  if (nn2 == 5) pend = (1. - pd1) / (pd2 - pd1);
	}
      }
      else if (nn1 == 5) {
	if (nn2 >= 4) return;
	else {
	  psta = (pd1 - 1.) / (pd1 - pd2);
	  if (nn2 == 1) pend = pd1 / (pd1 - pd2);
	}
      }
      else if (nn1 == 2) {
	if (nn2 == 1) return;
	else if (nn2 == 5) pend = (1. - pd1) / (pd2 - pd1);
      }
      else if (nn1 == 4) {
	if (nn2 == 5) return;
	else if (nn2 == 1) pend = pd1 / (pd1 - pd2);
      }
    }
  }
  if (CrosSeg) {
    Standard_Real ad1 = d1;
    if (d1 < 0) ad1 = -d1;
    Standard_Real ad2 = d2;
    if (d2 < 0) ad2 = -d2;
    pp = ad1 / ( ad1 + ad2 );
    if (TrFlags & EMskGrALin2)
      pdp = (PntXP1 + (PntXP2 - PntXP1) * pp - XV2) / da;
    else
      pdp = (PntYP1 + (PntYP2 - PntYP1) * pp - YV2) / db;
    Standard_Boolean OutSideP = Standard_False;
    Standard_Boolean Multiple = Standard_False;
    if      (pdp      < -TolParam) OutSideP = Standard_True;
    else if (pdp      <  TolParam) {
      Multiple = Standard_True;

      for (l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;
	  if (o[l] != (Standard_Boolean)(n1 == -1)) {
	    if (l == 0 && npi == 1) {
	      p[0] = p[1];
	      o[0] = o[1];
	      m[0] = m[1];
	    }
	    npi--;
	    npiRej++;
	  }
	}
      }
    }
    else if (pdp - 1. < -TolParam) {}
    else if (pdp - 1. <  TolParam) {
      Multiple = Standard_True;

      for (l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;
	  if (o[l] != (Standard_Boolean)(n1 == -1)) {
	    if (l == 0 && npi == 1) {
	      p[0] = p[1];
	      o[0] = o[1];
	      m[0] = m[1];
	    }
	    npi--;
	    npiRej++;
	  }
	}
      }
    }
    else                           OutSideP = Standard_True;
    if (OutSideP) npiRej++;
    else {
      npi++;
      if (npi < 2) {
	p[npi] = pp;
	o[npi] = n1 == -1;
	m[npi] = Multiple;
      }
#ifdef OCCT_DEBUG
      else if (ERROR) {
	cout << " error : HLRAlgo_PolyData::HideByOneTriangle " << endl;
	cout << " ( more than 2 points )." << endl;
      }
#endif
    }
  }

  da = XV1 - XV3;
  db = YV1 - YV3;
  a = -db;
  b =  da;
  c = sqrt(a * a + b * b);
  a /= c;
  b /= c;
  c  = a * XV3  + b * YV3;
  d1 = a * PntXP1 + b * PntYP1 - c;
  d2 = a * PntXP2 + b * PntYP2 - c;
  if      (d1 >  Tolerance) {
    if (d2 < -Tolerance) {
      n1 =  2;
      CrosSeg = Standard_True;
    }
    else
      CrosSeg = Standard_False;
  }
  else if (d1 < -Tolerance) {
    if (d2 >  Tolerance) {
      n1 = -1;
      CrosSeg = Standard_True;
    }
    else return;
  }
  else {
    if      (d2 >  Tolerance)
      CrosSeg = Standard_False;
    else if (d2 < -Tolerance) return;
    else {
      CrosSeg = Standard_False;
      if (TrFlags & EMskGrALin3) {
	pd1 = (PntXP1 - XV3) / da;
	pd2 = (PntXP2 - XV3) / da;
      }
      else {
	pd1 = (PntYP1 - YV3) / db;
	pd2 = (PntYP2 - YV3) / db;
      }
      if      (pd1      < -TolParam) nn1 = 1;
      else if (pd1      <  TolParam) nn1 = 2;
      else if (pd1 - 1. < -TolParam) nn1 = 3;
      else if (pd1 - 1. <  TolParam) nn1 = 4;
      else                           nn1 = 5;
      if      (pd2      < -TolParam) nn2 = 1;
      else if (pd2      <  TolParam) nn2 = 2;
      else if (pd2 - 1. < -TolParam) nn2 = 3;
      else if (pd2 - 1. <  TolParam) nn2 = 4;
      else                           nn2 = 5;
      if      (nn1 == 3) {
	if      (nn2 == 1) pend = pd1 / (pd1 - pd2);
	else if (nn2 == 5) pend = (1. - pd1) / (pd2 - pd1);
      }
      else if (nn1 == 1) {
	if (nn2 <= 2) return;
	else {
	  psta = - pd1 / (pd2 - pd1);
	  if (nn2 == 5) pend = (1. - pd1) / (pd2 - pd1);
	}
      }
      else if (nn1 == 5) {
	if (nn2 >= 4) return;
	else {
	  psta = (pd1 - 1.) / (pd1 - pd2);
	  if (nn2 == 1) pend = pd1 / (pd1 - pd2);
	}
      }
      else if (nn1 == 2) {
	if (nn2 == 1) return;
	else if (nn2 == 5) pend = (1. - pd1) / (pd2 - pd1);
      }
      else if (nn1 == 4) {
	if (nn2 == 5) return;
	else if (nn2 == 1) pend = pd1 / (pd1 - pd2);
      }
    }
  }
  if (CrosSeg) {
    Standard_Real ad1 = d1;
    if (d1 < 0) ad1 = -d1;
    Standard_Real ad2 = d2;
    if (d2 < 0) ad2 = -d2;
    pp = ad1 / ( ad1 + ad2 );
    if (TrFlags & EMskGrALin3)
      pdp = (PntXP1 + (PntXP2 - PntXP1) * pp - XV3) / da;
    else
      pdp = (PntYP1 + (PntYP2 - PntYP1) * pp - YV3) / db;
    Standard_Boolean OutSideP = Standard_False;
    Standard_Boolean Multiple = Standard_False;
    if      (pdp      < -TolParam) OutSideP = Standard_True;
    else if (pdp      <  TolParam) {
      Multiple = Standard_True;

      for (l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;
	  if (o[l] != (Standard_Boolean)(n1 == -1)) {
	    if (l == 0 && npi == 1) {
	      p[0] = p[1];
	      o[0] = o[1];
	      m[0] = m[1];
	    }
	    npi--;
	    npiRej++;
	  }
	}
      }
    }
    else if (pdp - 1. < -TolParam) {}
    else if (pdp - 1. <  TolParam) {
      Multiple = Standard_True;

      for (l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;
	  if (o[l] != (Standard_Boolean)(n1 == -1)) {
	    if (l == 0 && npi == 1) {
	      p[0] = p[1];
	      o[0] = o[1];
	      m[0] = m[1];
	    }
	    npi--;
	    npiRej++;
	  }
	}
      }
    }
    else                           OutSideP = Standard_True;
    if (OutSideP) npiRej++;
    else {
      npi++;
      if (npi < 2) {
	p[npi] = pp;
	o[npi] = n1 == -1;
	m[npi] = Multiple;
      }
#ifdef OCCT_DEBUG
      else if (ERROR) {
	cout << " error : HLRAlgo_PolyData::HideByOneTriangle " << endl;
	cout << " ( more than 2 points )." << endl;
      }
#endif
    }
  }
  
  if (npi == -1) {
    if (npiRej >= 2) return;
  }
  else if (npi == 0) {
    if (o[0]) {
      psta = p[0];
      pend = 1.;
    }
    else {
      psta = 0.;
      pend = p[0];
    }
  }
  else if (npi == 1) {
    if (p[0] > p[1]) {
      psta = p[1];
      pend = p[0];
    }
    else {
      psta = p[0];
      pend = p[1];
    }
  }

  if (Crossing) {
    if (HideBefore) {
      if      (Param-psta < TolParam) return;
      else if (Param < pend)          pend   = Param;
    }
    else {
      if      (pend-Param < TolParam) return;
      else if (psta < Param)          psta   = Param;
    }
  }

  Standard_Boolean total;
  if (psta > 0) total = psta <  TolParam;
  else          total = psta > -TolParam;
  if (total) {
    Standard_Real pfin = pend - 1.;
    if (pfin > 0) total = pfin <  TolParam;
    else          total = pfin > -TolParam;
  }
  if (total) status.HideAll();
  else       status.Hide(psta,(Standard_ShortReal)TolParam,pend,(Standard_ShortReal)TolParam,
			 Standard_False,Standard_False);
}
