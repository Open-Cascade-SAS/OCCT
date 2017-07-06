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

#include <HLRAlgo_PolyData.hxx>

#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRAlgo_PolyMask.hxx>

#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRAlgo_PolyData,Standard_Transient)

#ifdef OCCT_DEBUG
static Standard_Integer HLRAlgo_PolyData_ERROR = Standard_False;
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
HLRAlgo_PolyData::UpdateGlobalMinMax (Box& theBox)
{
  Standard_Integer i;
  Standard_Real X1,X2,X3,Y1,Y2,Y3,Z1,Z2,Z3;
  const TColgp_Array1OfXYZ& Nodes = myHNodes->Array1();
  HLRAlgo_Array1OfTData&    TData = myHTData->ChangeArray1();
  Standard_Integer          nbT   = TData.Upper();
  HLRAlgo_TriangleData*     TD    = &(TData.ChangeValue(1));
  
  for (i = 1; i <= nbT; i++) {
    if (TD->Flags & HLRAlgo_PolyMask_FMskHiding) {
      const gp_XYZ& P1 = Nodes(TD->Node1);
      const gp_XYZ& P2 = Nodes(TD->Node2);
      const gp_XYZ& P3 = Nodes(TD->Node3);
      X1 = P1.X();
      Y1 = P1.Y();
      Z1 = P1.Z();
      X2 = P2.X();
      Y2 = P2.Y();
      Z2 = P2.Z();
      X3 = P3.X();
      Y3 = P3.Y();
      Z3 = P3.Z();
      if      (theBox.XMin > X1) theBox.XMin = X1;
      else if (theBox.XMax < X1) theBox.XMax = X1;
      if      (theBox.YMin > Y1) theBox.YMin = Y1;
      else if (theBox.YMax < Y1) theBox.YMax = Y1;
      if      (theBox.ZMin > Z1) theBox.ZMin = Z1;
      else if (theBox.ZMax < Z1) theBox.ZMax = Z1;
      if      (theBox.XMin > X2) theBox.XMin = X2;
      else if (theBox.XMax < X2) theBox.XMax = X2;
      if      (theBox.YMin > Y2) theBox.YMin = Y2;
      else if (theBox.YMax < Y2) theBox.YMax = Y2;
      if      (theBox.ZMin > Z2) theBox.ZMin = Z2;
      else if (theBox.ZMax < Z2) theBox.ZMax = Z2;
      if      (theBox.XMin > X3) theBox.XMin = X3;
      else if (theBox.XMax < X3) theBox.XMax = X3;
      if      (theBox.YMin > Y3) theBox.YMin = Y3;
      else if (theBox.YMax < Y3) theBox.YMax = Y3;
      if      (theBox.ZMin > Z3) theBox.ZMin = Z3;
      else if (theBox.ZMax < Z3) theBox.ZMax = Z3;
    }
    TD++;
  }
}

//=======================================================================
//function : HideByPolyData
//purpose  : 
//=======================================================================

void HLRAlgo_PolyData::HideByPolyData (const HLRAlgo_BiPoint::PointsT& thePoints,
				       Triangle& theTriangle,
				       HLRAlgo_BiPoint::IndicesT& theIndices,
				       const Standard_Boolean HidingShell,
				       HLRAlgo_EdgeStatus& status)
{
  if (((myFaceIndices.Max - theIndices.MinSeg) & 0x80100200) == 0 &&
      ((theIndices.MaxSeg - myFaceIndices.Min) & 0x80100000) == 0) {
    HLRAlgo_Array1OfPHDat& PHDat = myHPHDat->ChangeArray1();
    const HLRAlgo_Array1OfTData& TData = myHTData->Array1();
    Standard_Real d1,d2;
    Standard_Boolean NotConnex    = Standard_False;
    Standard_Boolean isCrossing   = Standard_False;
    Standard_Boolean toHideBefore = Standard_False;
    Standard_Integer TFlag = 0;
    Standard_Integer h,h2 = PHDat.Upper();
    HLRAlgo_PolyHidingData* PH = &(PHDat(1));
    
    for (h = 1; h <= h2; h++) {
      HLRAlgo_PolyHidingData::TriangleIndices& aTriangleIndices = PH->Indices();
      if (((aTriangleIndices.Max - theIndices.MinSeg) & 0x80100200) == 0 &&
	  ((theIndices.MaxSeg - aTriangleIndices.Min) & 0x80100000) == 0) {
	const HLRAlgo_TriangleData& aTriangle = TData(aTriangleIndices.Index);
	NotConnex = Standard_True;
	if (HidingShell) {
	  if      (myFaceIndices.Index == theIndices.FaceConex1) {
	    if      (theIndices.Face1Pt1 == aTriangle.Node1)
	      NotConnex = theIndices.Face1Pt2 != aTriangle.Node2 && theIndices.Face1Pt2 != aTriangle.Node3;
	    else if (theIndices.Face1Pt1 == aTriangle.Node2)
	      NotConnex = theIndices.Face1Pt2 != aTriangle.Node3 && theIndices.Face1Pt2 != aTriangle.Node1;
	    else if (theIndices.Face1Pt1 == aTriangle.Node3)
	      NotConnex = theIndices.Face1Pt2 != aTriangle.Node1 && theIndices.Face1Pt2 != aTriangle.Node2;
	  }
	  else if (myFaceIndices.Index == theIndices.FaceConex2) {
	    if      (theIndices.Face2Pt1 == aTriangle.Node1)
	      NotConnex = theIndices.Face2Pt2 != aTriangle.Node2 && theIndices.Face2Pt2 != aTriangle.Node3;
	    else if (theIndices.Face2Pt1 == aTriangle.Node2)
	      NotConnex = theIndices.Face2Pt2 != aTriangle.Node3 && theIndices.Face2Pt2 != aTriangle.Node1;
	    else if (theIndices.Face2Pt1 == aTriangle.Node3)
	      NotConnex = theIndices.Face2Pt2 != aTriangle.Node1 && theIndices.Face2Pt2 != aTriangle.Node2;
	  }
	}
	if (NotConnex) {
	  HLRAlgo_PolyHidingData::PlaneT& aPlane = PH->Plane();
	  d1 = aPlane.Normal * thePoints.PntP1 - aPlane.D;
	  d2 = aPlane.Normal * thePoints.PntP2 - aPlane.D;
	  if      (d1 > theTriangle.Tolerance) {
	    if    (d2 < -theTriangle.Tolerance) {
	      theTriangle.Param = d1 / ( d1 - d2 );
	      toHideBefore = Standard_False;
	      isCrossing   = Standard_True;
	      TFlag = aTriangle.Flags;
	      const TColgp_Array1OfXYZ& Nodes = myHNodes->Array1();
	      const gp_XYZ            & P1    = Nodes(aTriangle.Node1);
	      const gp_XYZ            & P2    = Nodes(aTriangle.Node2);
	      const gp_XYZ            & P3    = Nodes(aTriangle.Node3);
        theTriangle.V1 = gp_XY(P1.X(), P1.Y());
        theTriangle.V2 = gp_XY(P2.X(), P2.Y());
        theTriangle.V3 = gp_XY(P3.X(), P3.Y());
	      hideByOneTriangle (thePoints, theTriangle, isCrossing, toHideBefore, TFlag, status);
	    }
	  }
	  else if (d1 < -theTriangle.Tolerance) {
	    if    (d2 > theTriangle.Tolerance) {
	      theTriangle.Param = d1 / ( d1 - d2 );
	      toHideBefore = Standard_True;
	      isCrossing   = Standard_True;
	      TFlag = aTriangle.Flags;
	      const TColgp_Array1OfXYZ& Nodes = myHNodes->Array1();
	      const gp_XYZ            & P1    = Nodes(aTriangle.Node1);
	      const gp_XYZ            & P2    = Nodes(aTriangle.Node2);
	      const gp_XYZ            & P3    = Nodes(aTriangle.Node3);
	      theTriangle.V1 = gp_XY(P1.X(), P1.Y());
	      theTriangle.V2 = gp_XY(P2.X(), P2.Y());
	      theTriangle.V3 = gp_XY(P3.X(), P3.Y());
	      hideByOneTriangle (thePoints, theTriangle, isCrossing, toHideBefore, TFlag, status);
	    }
	    else {
	      isCrossing = Standard_False;
	      TFlag = aTriangle.Flags;
	      const TColgp_Array1OfXYZ& Nodes = myHNodes->Array1();
	      const gp_XYZ            & P1    = Nodes(aTriangle.Node1);
	      const gp_XYZ            & P2    = Nodes(aTriangle.Node2);
	      const gp_XYZ            & P3    = Nodes(aTriangle.Node3);
	      theTriangle.V1 = gp_XY(P1.X(), P1.Y());
	      theTriangle.V2 = gp_XY(P2.X(), P2.Y());
	      theTriangle.V3 = gp_XY(P3.X(), P3.Y());
	      hideByOneTriangle (thePoints, theTriangle, isCrossing, toHideBefore, TFlag, status);
	    }
	  }
	  else if (d2 < -theTriangle.Tolerance) {
	    isCrossing = Standard_False;
	    TFlag = aTriangle.Flags;
	    const TColgp_Array1OfXYZ& Nodes = myHNodes->Array1();
	    const gp_XYZ            & P1    = Nodes(aTriangle.Node1);
	    const gp_XYZ            & P2    = Nodes(aTriangle.Node2);
	    const gp_XYZ            & P3    = Nodes(aTriangle.Node3);
	    theTriangle.V1 = gp_XY(P1.X(), P1.Y());
	    theTriangle.V2 = gp_XY(P2.X(), P2.Y());
	    theTriangle.V3 = gp_XY(P3.X(), P3.Y());
	    hideByOneTriangle(thePoints, theTriangle, isCrossing, toHideBefore, TFlag, status);
	  }
	}
      }
      PH++;
    }
  }
}

//=======================================================================
//function : hideByOneTriangle
//purpose  :
//=======================================================================

void HLRAlgo_PolyData::hideByOneTriangle (const HLRAlgo_BiPoint::PointsT& thePoints,
                                          Triangle& theTriangle,
                                          const Standard_Boolean Crossing,
                                          const Standard_Boolean HideBefore,
                                          const Standard_Integer TrFlags,
                                          HLRAlgo_EdgeStatus& status)
{
  Standard_Boolean CrosSeg = Standard_False;
  Standard_Integer n1 = 0;
  Standard_Real pd1 = 0., pd2 = 0.;
  Standard_Integer nn1 = 0, nn2 = 0;
  Standard_Real pend = 1., psta = 0., pp = 0., pdp = 0.;
  Standard_Integer npi = -1;
  Standard_Boolean o[] = {Standard_False, Standard_False};
  Standard_Boolean  m[] = {Standard_False, Standard_False};
  Standard_Real p[] = {0., 0.};
  Standard_Integer npiRej = 0;

  {
  const gp_XY aD = theTriangle.V2 - theTriangle.V1;
  const gp_XY aA = (1 / aD.Modulus()) * gp_XY(-aD.Y(), aD.X());
  const Standard_Real aDot = aA * theTriangle.V1;
  const Standard_Real d1 = aA * thePoints.PntP12D() - aDot;
  const Standard_Real d2 = aA * thePoints.PntP22D() - aDot;
  if      (d1 > theTriangle.Tolerance) {
    if (d2 < -theTriangle.Tolerance) {
      n1 =  2;
      CrosSeg = Standard_True;
    }
    else
      CrosSeg = Standard_False;
  }
  else if (d1 < -theTriangle.Tolerance) {
    if (d2 > theTriangle.Tolerance) {
      n1 = -1;
      CrosSeg = Standard_True;
    }
    else return;
  }
  else {
    if      (d2 > theTriangle.Tolerance)
      CrosSeg = Standard_False;
    else if (d2 < -theTriangle.Tolerance) return;
    else {
      CrosSeg = Standard_False;
      if (TrFlags & HLRAlgo_PolyMask_EMskGrALin1) {
	pd1 = (thePoints.PntP1.X() - theTriangle.V1.X()) / aD.X();
	pd2 = (thePoints.PntP2.X() - theTriangle.V1.X()) / aD.X();
      }
      else {
	pd1 = (thePoints.PntP1.Y() - theTriangle.V1.Y()) / aD.Y();
	pd2 = (thePoints.PntP2.Y() - theTriangle.V1.Y()) / aD.Y();
      }
      if      (pd1      < -theTriangle.TolParam) nn1 = 1;
      else if (pd1      < theTriangle.TolParam) nn1 = 2;
      else if (pd1 - 1. < -theTriangle.TolParam) nn1 = 3;
      else if (pd1 - 1. < theTriangle.TolParam) nn1 = 4;
      else                           nn1 = 5;
      if      (pd2      < -theTriangle.TolParam) nn2 = 1;
      else if (pd2      < theTriangle.TolParam) nn2 = 2;
      else if (pd2 - 1. < -theTriangle.TolParam) nn2 = 3;
      else if (pd2 - 1. < theTriangle.TolParam) nn2 = 4;
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
    if (TrFlags & HLRAlgo_PolyMask_EMskGrALin1)
      pdp = (thePoints.PntP1.X() + (thePoints.PntP2.X() - thePoints.PntP1.X()) * pp - theTriangle.V1.X()) / aD.X();
    else
      pdp = (thePoints.PntP1.Y() + (thePoints.PntP2.Y() - thePoints.PntP1.Y()) * pp - theTriangle.V1.Y()) / aD.Y();
    Standard_Boolean OutSideP = Standard_False;
    Standard_Boolean Multiple = Standard_False;
    if      (pdp      < -theTriangle.TolParam) OutSideP = Standard_True;
    else if (pdp      < theTriangle.TolParam) {
      Multiple = Standard_True;

      for (Standard_Integer l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;

	  if (o[l] != (n1 == -1)) {
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
    else if (pdp - 1. < -theTriangle.TolParam) {}
    else if (pdp - 1. < theTriangle.TolParam) {
      Multiple = Standard_True;

      for (Standard_Integer l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;
	  if (o[l] != (n1 == -1)) {
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
      else if (HLRAlgo_PolyData_ERROR) {
	cout << " error : HLRAlgo_PolyData::HideByOneTriangle " << endl;
	cout << " ( more than 2 points )." << endl;
      }
#endif
    }
  }
  }

  {
  const gp_XY aD = theTriangle.V3 - theTriangle.V2;
  const gp_XY aA = (1 / aD.Modulus()) * gp_XY(-aD.Y(), aD.X());
  const Standard_Real aDot = aA * theTriangle.V2;
  const Standard_Real d1 = aA * thePoints.PntP12D() - aDot;
  const Standard_Real d2 = aA * thePoints.PntP22D() - aDot;
  if      (d1 > theTriangle.Tolerance) {
    if (d2 < -theTriangle.Tolerance) {
      n1 =  2;
      CrosSeg = Standard_True;
    }
    else
      CrosSeg = Standard_False;
  }
  else if (d1 < -theTriangle.Tolerance) {
    if (d2 > theTriangle.Tolerance) {
      n1 = -1;
      CrosSeg = Standard_True;
    }
    else return;
  }
  else {
    if      (d2 > theTriangle.Tolerance)
      CrosSeg = Standard_False;
    else if (d2 < -theTriangle.Tolerance) return;
    else {
      CrosSeg = Standard_False;
      if (TrFlags & HLRAlgo_PolyMask_EMskGrALin2) {
	pd1 = (thePoints.PntP1.X() - theTriangle.V2.X()) / aD.X();
	pd2 = (thePoints.PntP2.X() - theTriangle.V2.X()) / aD.X();
      }
      else {
	pd1 = (thePoints.PntP1.Y() - theTriangle.V2.Y()) / aD.Y();
	pd2 = (thePoints.PntP2.Y() - theTriangle.V2.Y()) / aD.Y();
      }
      if      (pd1      < -theTriangle.TolParam) nn1 = 1;
      else if (pd1      < theTriangle.TolParam) nn1 = 2;
      else if (pd1 - 1. < -theTriangle.TolParam) nn1 = 3;
      else if (pd1 - 1. < theTriangle.TolParam) nn1 = 4;
      else                           nn1 = 5;
      if      (pd2      < -theTriangle.TolParam) nn2 = 1;
      else if (pd2      < theTriangle.TolParam) nn2 = 2;
      else if (pd2 - 1. < -theTriangle.TolParam) nn2 = 3;
      else if (pd2 - 1. < theTriangle.TolParam) nn2 = 4;
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
    if (TrFlags & HLRAlgo_PolyMask_EMskGrALin2)
      pdp = (thePoints.PntP1.X() + (thePoints.Pnt2.X() - thePoints.PntP1.X()) * pp - theTriangle.V2.X()) / aD.X();
    else
      pdp = (thePoints.PntP1.Y() + (thePoints.PntP2.Y() - thePoints.PntP1.Y()) * pp - theTriangle.V2.Y()) / aD.Y();
    Standard_Boolean OutSideP = Standard_False;
    Standard_Boolean Multiple = Standard_False;
    if      (pdp      < -theTriangle.TolParam) OutSideP = Standard_True;
    else if (pdp      < theTriangle.TolParam) {
      Multiple = Standard_True;

      for (Standard_Integer l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;
	  if (o[l] != (n1 == -1)) {
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
    else if (pdp - 1. < -theTriangle.TolParam) {}
    else if (pdp - 1. < theTriangle.TolParam) {
      Multiple = Standard_True;

      for (Standard_Integer l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;
	  if (o[l] != (n1 == -1)) {
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
      else if (HLRAlgo_PolyData_ERROR) {
	cout << " error : HLRAlgo_PolyData::HideByOneTriangle " << endl;
	cout << " ( more than 2 points )." << endl;
      }
#endif
    }
  }
  }

  {
  const gp_XY aD = theTriangle.V1 - theTriangle.V3;
  const gp_XY aA = (1 / aD.Modulus()) * gp_XY(-aD.Y(), aD.X());
  const Standard_Real aDot = aA * theTriangle.V3;
  const Standard_Real d1 = aA * thePoints.PntP12D() - aDot;
  const Standard_Real d2 = aA * thePoints.PntP22D() - aDot;
  if      (d1 > theTriangle.Tolerance) {
    if (d2 < -theTriangle.Tolerance) {
      n1 =  2;
      CrosSeg = Standard_True;
    }
    else
      CrosSeg = Standard_False;
  }
  else if (d1 < -theTriangle.Tolerance) {
    if (d2 > theTriangle.Tolerance) {
      n1 = -1;
      CrosSeg = Standard_True;
    }
    else return;
  }
  else {
    if      (d2 > theTriangle.Tolerance)
      CrosSeg = Standard_False;
    else if (d2 < -theTriangle.Tolerance) return;
    else {
      CrosSeg = Standard_False;
      if (TrFlags & HLRAlgo_PolyMask_EMskGrALin3) {
	pd1 = (thePoints.PntP1.X() - theTriangle.V3.X()) / aD.X();
	pd2 = (thePoints.PntP2.X() - theTriangle.V3.X()) / aD.X();
      }
      else {
	pd1 = (thePoints.PntP1.Y() - theTriangle.V3.Y()) / aD.Y();
	pd2 = (thePoints.PntP2.Y() - theTriangle.V3.Y()) / aD.Y();
      }
      if      (pd1      < -theTriangle.TolParam) nn1 = 1;
      else if (pd1      < theTriangle.TolParam) nn1 = 2;
      else if (pd1 - 1. < -theTriangle.TolParam) nn1 = 3;
      else if (pd1 - 1. < theTriangle.TolParam) nn1 = 4;
      else                           nn1 = 5;
      if      (pd2      < -theTriangle.TolParam) nn2 = 1;
      else if (pd2      < theTriangle.TolParam) nn2 = 2;
      else if (pd2 - 1. < -theTriangle.TolParam) nn2 = 3;
      else if (pd2 - 1. < theTriangle.TolParam) nn2 = 4;
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
    if (TrFlags & HLRAlgo_PolyMask_EMskGrALin3)
      pdp = (thePoints.PntP1.X() + (thePoints.PntP2.X() - thePoints.PntP1.X()) * pp - theTriangle.V3.X()) / aD.X();
    else
      pdp = (thePoints.PntP1.Y() + (thePoints.PntP2.Y() - thePoints.PntP1.Y()) * pp - theTriangle.V3.Y()) / aD.Y();
    Standard_Boolean OutSideP = Standard_False;
    Standard_Boolean Multiple = Standard_False;
    if      (pdp      < -theTriangle.TolParam) OutSideP = Standard_True;
    else if (pdp      < theTriangle.TolParam) {
      Multiple = Standard_True;

      for (Standard_Integer l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;
	  if (o[l] != (n1 == -1)) {
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
    else if (pdp - 1. < -theTriangle.TolParam) {}
    else if (pdp - 1. < theTriangle.TolParam) {
      Multiple = Standard_True;

      for (Standard_Integer l = 0; l <= npi; l++) {
	if (m[l]) {
	  OutSideP = Standard_True;
	  if (o[l] != (n1 == -1)) {
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
      else if (HLRAlgo_PolyData_ERROR) {
	cout << " error : HLRAlgo_PolyData::HideByOneTriangle " << endl;
	cout << " ( more than 2 points )." << endl;
      }
#endif
    }
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
      if      (theTriangle.Param - psta < theTriangle.TolParam) return;
      else if (theTriangle.Param < pend)          pend   = theTriangle.Param;
    }
    else {
      if      (pend - theTriangle.Param < theTriangle.TolParam) return;
      else if (psta < theTriangle.Param)          psta   = theTriangle.Param;
    }
  }

  Standard_Boolean total;
  if (psta > 0) total = psta < theTriangle.TolParam;
  else          total = psta > -theTriangle.TolParam;
  if (total) {
    Standard_Real pfin = pend - 1.;
    if (pfin > 0) total = pfin < theTriangle.TolParam;
    else          total = pfin > -theTriangle.TolParam;
  }
  if (total) status.HideAll();
  else       status.Hide(psta,(Standard_ShortReal)theTriangle.TolParam,pend,(Standard_ShortReal)theTriangle.TolParam,
			 Standard_False,Standard_False);
}
