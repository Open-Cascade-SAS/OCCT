// Created on: 1995-05-05
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
//#define No_Exception
#endif


#include <HLRAlgo_BiPoint.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRAlgo_ListOfBPoint.hxx>
#include <HLRAlgo_PolyAlgo.hxx>
#include <HLRAlgo_PolyData.hxx>
#include <HLRAlgo_PolyShellData.hxx>
#include <Precision.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRAlgo_PolyAlgo,MMgt_TShared)

#define EMskGrALin1  ((Standard_Boolean)   8)
#define EMskGrALin2  ((Standard_Boolean)  16)
#define EMskGrALin3  ((Standard_Boolean)  32)
#define FMskHiding   ((Standard_Boolean) 256)

#define TriNode1 ((Standard_Integer*)TriIndices)[0]
#define TriNode2 ((Standard_Integer*)TriIndices)[1]
#define TriNode3 ((Standard_Integer*)TriIndices)[2]
#define TriFlags ((Standard_Boolean*)TriIndices)[3]

#define XV1          myRealPtr[0]
#define XV2          myRealPtr[1]
#define XV3          myRealPtr[2]
#define YV1          myRealPtr[3]
#define YV2          myRealPtr[4]
#define YV3          myRealPtr[5]
#define Param        myRealPtr[6]
#define myTolParam   myRealPtr[7]
#define myTolAngular myRealPtr[8]
#define myTolerance  myRealPtr[9]

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

#define TotXMin    ((Standard_Real*)TotMinMax)[0]
#define TotYMin    ((Standard_Real*)TotMinMax)[1]
#define TotZMin    ((Standard_Real*)TotMinMax)[2]
#define TotXMax    ((Standard_Real*)TotMinMax)[3]
#define TotYMax    ((Standard_Real*)TotMinMax)[4]
#define TotZMax    ((Standard_Real*)TotMinMax)[5]

#define ShapeIndex ((Standard_Integer*)IndexPtr)[0]
#define FaceConex1 ((Standard_Integer*)IndexPtr)[1]
#define Face1Pt1   ((Standard_Integer*)IndexPtr)[2]
#define Face1Pt2   ((Standard_Integer*)IndexPtr)[3]
#define FaceConex2 ((Standard_Integer*)IndexPtr)[4]
#define Face2Pt1   ((Standard_Integer*)IndexPtr)[5]
#define Face2Pt2   ((Standard_Integer*)IndexPtr)[6]
#define MinSeg     ((Standard_Integer*)IndexPtr)[7]
#define MaxSeg     ((Standard_Integer*)IndexPtr)[8]
#define SegFlags   ((Standard_Integer*)IndexPtr)[9]

#define MinShell ((Standard_Integer*)ShellIndices)[0]
#define MaxShell ((Standard_Integer*)ShellIndices)[1]

#define FIndex   ((Standard_Integer*)PolyTIndices)[0]
#define MinPolyT ((Standard_Integer*)PolyTIndices)[1]
#define MaxPolyT ((Standard_Integer*)PolyTIndices)[2]

//static Standard_Integer ERROR = Standard_False;

//=======================================================================
//function : HLRAlgo_PolyAlgo
//purpose  : 
//=======================================================================

HLRAlgo_PolyAlgo::HLRAlgo_PolyAlgo ()
{
  myTolParam   = 0.00000001;
  myTolAngular = 0.0001;
}

//=======================================================================
//function : Init
//purpose  : 
//=======================================================================

void
HLRAlgo_PolyAlgo::Init (const Handle(TColStd_HArray1OfTransient)& HShell)
{ myHShell = HShell; }

//=======================================================================
//function : Clear
//purpose  : 
//=======================================================================

void HLRAlgo_PolyAlgo::Clear ()
{
  if (!myHShell.IsNull()) {
    myHShell.Nullify();
  }
  myNbrShell = 0;
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void HLRAlgo_PolyAlgo::Update ()
{
  Standard_Integer i,j;
  Standard_Integer nxMin,nyMin,nzMin,nxMax,nyMax,nzMax;
  Standard_Real xShellMin,yShellMin,zShellMin;
  Standard_Real xShellMax,yShellMax,zShellMax;
  Standard_Real xPolyTMin,yPolyTMin,zPolyTMin;
  Standard_Real xPolyTMax,yPolyTMax,zPolyTMax;
  Standard_Real xTrianMin,yTrianMin,zTrianMin;
  Standard_Real xTrianMax,yTrianMax,zTrianMax;
  Standard_Real xSegmnMin,ySegmnMin,zSegmnMin;
  Standard_Real xSegmnMax,ySegmnMax,zSegmnMax;
  Standard_Real TotMinMax[6];
  Standard_Real Big = Precision::Infinite();
  TotMinMax[0] =  Big;
  TotMinMax[1] =  Big;
  TotMinMax[2] =  Big;
  TotMinMax[3] = -Big;
  TotMinMax[4] = -Big;
  TotMinMax[5] = -Big;
  TColStd_Array1OfTransient& Shell = myHShell->ChangeArray1();
  myNbrShell = Shell.Upper();
  Handle(HLRAlgo_PolyShellData)* psd1 = 
    (Handle(HLRAlgo_PolyShellData)*)&(Shell.ChangeValue(1));

  for (i = 1; i <= myNbrShell; i++) {
    (*psd1)->UpdateGlobalMinMax(&TotMinMax);
    psd1++;
  }

  Standard_Real dx = TotMinMax[3] - TotMinMax[0];
  Standard_Real dy = TotMinMax[4] - TotMinMax[1];
  Standard_Real dz = TotMinMax[5] - TotMinMax[2];
  Standard_Real    precad = dx;
  if (precad < dy) precad = dy;
  if (precad < dz) precad = dz;
  myTolerance = precad * myTolParam;
  precad = precad * 0.01;
  Standard_Real SurDX = 1020 / (dx + precad);
  Standard_Real SurDY = 1020 / (dy + precad);
  Standard_Real SurDZ =  508 / (dz + precad);
  precad = precad * 0.5;
  Standard_Real DecaX = - TotMinMax[0] + precad;
  Standard_Real DecaY = - TotMinMax[1] + precad;
  Standard_Real DecaZ = - TotMinMax[2] + precad;

  Handle(HLRAlgo_PolyShellData)* psd2 = 
    (Handle(HLRAlgo_PolyShellData)*)&(Shell.ChangeValue(1));

  for (i = 1; i <= myNbrShell; i++) {
    const Standard_Address ShellIndices = (*psd2)->Indices();
    xShellMin =  Big;
    yShellMin =  Big;
    zShellMin =  Big;
    xShellMax = -Big;
    yShellMax = -Big;
    zShellMax = -Big;

    for (mySegListIt.Initialize((*psd2)->Edges());
	 mySegListIt.More();
	 mySegListIt.Next()) {      
      HLRAlgo_BiPoint& BP = mySegListIt.Value();
      const Standard_Address Coordinates = BP.Coordinates();
      const Standard_Address IndexPtr    = BP.Indices();
      if (PntXP1 < PntXP2) { xSegmnMin = PntXP1; xSegmnMax = PntXP2; }
      else                 { xSegmnMin = PntXP2; xSegmnMax = PntXP1; }
      if (PntYP1 < PntYP2) { ySegmnMin = PntYP1; ySegmnMax = PntYP2; }
      else                 { ySegmnMin = PntYP2; ySegmnMax = PntYP1; }
      if (PntZP1 < PntZP2) { zSegmnMin = PntZP1; zSegmnMax = PntZP2; }
      else                 { zSegmnMin = PntZP2; zSegmnMax = PntZP1; }
      nxMin = (Standard_Integer)((DecaX + xSegmnMin) * SurDX);
      nyMin = (Standard_Integer)((DecaY + ySegmnMin) * SurDY);
      nzMin = (Standard_Integer)((DecaZ + zSegmnMin) * SurDZ);
      nxMax = (Standard_Integer)((DecaX + xSegmnMax) * SurDX);
      nyMax = (Standard_Integer)((DecaY + ySegmnMax) * SurDY);
      nzMax = (Standard_Integer)((DecaZ + zSegmnMax) * SurDZ);
      MinSeg = nyMin + (nxMin << 11);
      MinSeg <<= 10;
      MinSeg += nzMin;
      MaxSeg = nyMax + (nxMax << 11);
      MaxSeg <<= 10;
      MaxSeg += nzMax + 0x00000200;
      if (xShellMin > xSegmnMin) xShellMin = xSegmnMin;
      if (xShellMax < xSegmnMax) xShellMax = xSegmnMax;
      if (yShellMin > ySegmnMin) yShellMin = ySegmnMin;
      if (yShellMax < ySegmnMax) yShellMax = ySegmnMax;
      if (zShellMin > zSegmnMin) zShellMin = zSegmnMin;
      if (zShellMax < zSegmnMax) zShellMax = zSegmnMax;
    }
    TColStd_Array1OfTransient& Polyg = (*psd2)->PolyData();
    Standard_Integer nbFace = Polyg.Upper();
    Standard_Integer nbFaHi = 0;
    Handle(HLRAlgo_PolyData)* pd = NULL;
    if(nbFace > 0) pd = (Handle(HLRAlgo_PolyData)*)&(Polyg.ChangeValue(1));
    
    for (j = 1; j <= nbFace; j++) {
      if ((*pd)->Hiding()) {
	nbFaHi++;
	xPolyTMin =  Big;
	yPolyTMin =  Big;
	zPolyTMin =  Big;
	xPolyTMax = -Big;
	yPolyTMax = -Big;
	zPolyTMax = -Big;
	Standard_Integer otheri,nbHide = 0;//min,max;
	Standard_Real X1,X2,X3,Y1,Y2,Y3,Z1,Z2,Z3;
	Standard_Real dn,dnx,dny,dnz,dx1,dy1,dz1,dx2,dy2,dz2,dx3,dy3;
	Standard_Real adx1,ady1,adx2,ady2,adx3,ady3;
	Standard_Real a =0.,b =0.,c =0.,d =0.;
	const Standard_Address PolyTIndices = (*pd)->Indices();
	TColgp_Array1OfXYZ   & Nodes        = (*pd)->Nodes();
	HLRAlgo_Array1OfTData& TData        = (*pd)->TData();
	HLRAlgo_Array1OfPHDat& PHDat        = (*pd)->PHDat();
	Standard_Integer nbT = TData.Upper();
	HLRAlgo_TriangleData* TD = &(TData.ChangeValue(1));
	
	for (otheri = 1; otheri <= nbT; otheri++) {
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
	    xTrianMax = xTrianMin = X1;
	    yTrianMax = yTrianMin = Y1;
	    zTrianMax = zTrianMin = Z1;
	    if      (xTrianMin > X2) xTrianMin = X2;
	    else if (xTrianMax < X2) xTrianMax = X2;
	    if      (yTrianMin > Y2) yTrianMin = Y2;
	    else if (yTrianMax < Y2) yTrianMax = Y2;
	    if      (zTrianMin > Z2) zTrianMin = Z2;
	    else if (zTrianMax < Z2) zTrianMax = Z2;
	    if      (xTrianMin > X3) xTrianMin = X3;
	    else if (xTrianMax < X3) xTrianMax = X3;
	    if      (yTrianMin > Y3) yTrianMin = Y3;
	    else if (yTrianMax < Y3) yTrianMax = Y3;
	    if      (zTrianMin > Z3) zTrianMin = Z3;
	    else if (zTrianMax < Z3) zTrianMax = Z3;
	    nxMin = (Standard_Integer)((DecaX + xTrianMin) * SurDX);
	    nyMin = (Standard_Integer)((DecaY + yTrianMin) * SurDY);
	    nzMin = (Standard_Integer)((DecaZ + zTrianMin) * SurDZ);
	    nxMax = (Standard_Integer)((DecaX + xTrianMax) * SurDX);
	    nyMax = (Standard_Integer)((DecaY + yTrianMax) * SurDY);
	    nzMax = (Standard_Integer)((DecaZ + zTrianMax) * SurDZ);
	    Standard_Integer MinTrian,MaxTrian;
	    MinTrian   = nyMin + (nxMin << 11);
	    MinTrian <<= 10;
	    MinTrian  += nzMin - 0x00000200;
	    MaxTrian   = nyMax + (nxMax << 11);
	    MaxTrian <<= 10;
	    MaxTrian  += nzMax;
	    dx1 = X2 - X1;
	    dy1 = Y2 - Y1;
	    dz1 = Z2 - Z1;
	    dx2 = X3 - X2;
	    dy2 = Y3 - Y2;
	    dz2 = Z3 - Z2;
	    dx3 = X1 - X3;
	    dy3 = Y1 - Y3;
	    dnx = dy1 * dz2 - dy2 * dz1;
	    dny = dz1 * dx2 - dz2 * dx1;
	    dnz = dx1 * dy2 - dx2 * dy1;
	    dn = sqrt(dnx * dnx + dny * dny + dnz * dnz);
	    if (dn > 0) {
	      a = dnx / dn;
	      b = dny / dn;
	      c = dnz / dn;
	    }
	    d = a * X1 + b * Y1 + c * Z1;
	    nbHide++;
	    PHDat(nbHide).Set(otheri,MinTrian,MaxTrian,a,b,c,d);
	    adx1 = dx1;
	    ady1 = dy1;
	    if (dx1 < 0) adx1 = -dx1;
	    if (dy1 < 0) ady1 = -dy1;
	    adx2 = dx2;
	    ady2 = dy2;
	    if (dx2 < 0) adx2 = -dx2;
	    if (dy2 < 0) ady2 = -dy2;
	    adx3 = dx3;
	    ady3 = dy3;
	    if (dx3 < 0) adx3 = -dx3;
	    if (dy3 < 0) ady3 = -dy3;
	    if (adx1 > ady1) TriFlags |=  EMskGrALin1;
	    else             TriFlags &= ~EMskGrALin1;
	    if (adx2 > ady2) TriFlags |=  EMskGrALin2;
	    else             TriFlags &= ~EMskGrALin2;
	    if (adx3 > ady3) TriFlags |=  EMskGrALin3;
	    else             TriFlags &= ~EMskGrALin3;
	    if (xPolyTMin > xTrianMin) xPolyTMin = xTrianMin;
	    if (xPolyTMax < xTrianMax) xPolyTMax = xTrianMax;
	    if (yPolyTMin > yTrianMin) yPolyTMin = yTrianMin;
	    if (yPolyTMax < yTrianMax) yPolyTMax = yTrianMax;
	    if (zPolyTMin > zTrianMin) zPolyTMin = zTrianMin;
	    if (zPolyTMax < zTrianMax) zPolyTMax = zTrianMax;
	  }
	  TD++;
	}
	nxMin = (Standard_Integer)((DecaX + xPolyTMin) * SurDX);
	nyMin = (Standard_Integer)((DecaY + yPolyTMin) * SurDY);
	nzMin = (Standard_Integer)((DecaZ + zPolyTMin) * SurDZ);
	nxMax = (Standard_Integer)((DecaX + xPolyTMax) * SurDX);
	nyMax = (Standard_Integer)((DecaY + yPolyTMax) * SurDY);
	nzMax = (Standard_Integer)((DecaZ + zPolyTMax) * SurDZ);
	MinPolyT   = nyMin + (nxMin << 11);
	MinPolyT <<= 10;
	MinPolyT  += nzMin - 0x00000200;
	MaxPolyT   = nyMax + (nxMax << 11);
	MaxPolyT <<= 10;
	MaxPolyT  += nzMax;
	if (xShellMin > xPolyTMin) xShellMin = xPolyTMin;
	if (xShellMax < xPolyTMax) xShellMax = xPolyTMax;
	if (yShellMin > yPolyTMin) yShellMin = yPolyTMin;
	if (yShellMax < yPolyTMax) yShellMax = yPolyTMax;
	if (zShellMin > zPolyTMin) zShellMin = zPolyTMin;
	if (zShellMax < zPolyTMax) zShellMax = zPolyTMax;
      }
      pd++;
    }
    if (nbFaHi > 0) {
      nxMin = (Standard_Integer)((DecaX + xShellMin) * SurDX);
      nyMin = (Standard_Integer)((DecaY + yShellMin) * SurDY);
      nzMin = (Standard_Integer)((DecaZ + zShellMin) * SurDZ);
      nxMax = (Standard_Integer)((DecaX + xShellMax) * SurDX);
      nyMax = (Standard_Integer)((DecaY + yShellMax) * SurDY);
      nzMax = (Standard_Integer)((DecaZ + zShellMax) * SurDZ);
      MinShell   = nyMin + (nxMin << 11);
      MinShell <<= 10;
      MinShell  += nzMin - 0x00000200;
      MaxShell   = nyMax + (nxMax << 11);
      MaxShell <<= 10;
      MaxShell  += nzMax;
      (*psd2)->UpdateHiding(nbFaHi);
      TColStd_Array1OfTransient& PolHi = (*psd2)->HidingPolyData();
      pd = (Handle(HLRAlgo_PolyData)*)&(Polyg.ChangeValue(1));
      Handle(HLRAlgo_PolyData)* phd = 
	(Handle(HLRAlgo_PolyData)*)&(PolHi.ChangeValue(1));
      
      for (j = 1; j <= nbFace; j++) {
	if ((*pd)->Hiding()) {
	  *phd = *pd;
	  phd++;
	}
	pd++;
      }
    }
    else {
      (*psd2)->UpdateHiding(0);
      MinShell = 0;
      MaxShell = 0;
    }
    psd2++;
  }
}

//=======================================================================
//function : NextHide
//purpose  : 
//=======================================================================

void HLRAlgo_PolyAlgo::NextHide ()
{
  myFound = Standard_False;
  if (myCurShell != 0) {
    mySegListIt.Next();
    if (mySegListIt.More()) myFound = Standard_True;
  }
  if (!myFound) {
    myCurShell++;

//POP ADN pour que cela marche sur WNT
//    while (myCurShell <= myNbrShell && !myFound) {
//      mySegListIt.Initialize((*(Handle(HLRAlgo_PolyShellData)*)&
//			      (myHShell->ChangeValue(myCurShell)))
//			     ->Edges());
    while (myCurShell <= myNbrShell && !myFound) {
	  Handle(HLRAlgo_PolyShellData) data = 
		  Handle(HLRAlgo_PolyShellData)::DownCast(myHShell->Value(myCurShell));
      mySegListIt.Initialize(data->Edges());
      if (mySegListIt.More()) myFound = Standard_True;
      else                    myCurShell++;
    }
  }
}

//=======================================================================
//function : Hide
//purpose  : 
//=======================================================================

void HLRAlgo_PolyAlgo::Hide (Standard_Address& Coordinates,
			     HLRAlgo_EdgeStatus& status,
			     Standard_Integer& Index,
			     Standard_Boolean& reg1,
			     Standard_Boolean& regn,
			     Standard_Boolean& outl,
			     Standard_Boolean& intl)
{
  HLRAlgo_BiPoint& BP             = mySegListIt.Value();
  Coordinates                     = BP.Coordinates();
  const Standard_Address IndexPtr = BP.Indices();
  status = HLRAlgo_EdgeStatus(0.,(Standard_ShortReal)myTolParam,1.,(Standard_ShortReal)myTolParam);
  Index = ShapeIndex;
  reg1       = BP.Rg1Line();
  regn       = BP.RgNLine();
  outl       = BP.OutLine();
  intl       = BP.IntLine();
  if (BP.Hidden())
    status.HideAll();
  else {
    Standard_Boolean HidingShell;
    TColStd_Array1OfTransient& Shell = myHShell->ChangeArray1();
    Handle(HLRAlgo_PolyShellData)* psd = 
      (Handle(HLRAlgo_PolyShellData)*)&(Shell.ChangeValue(1));
    
    for (Standard_Integer s = 1; s <= myNbrShell; s++) {
      if ((*psd)->Hiding()) {
	const Standard_Address ShellIndices = (*psd)->Indices();
	if (((MaxShell - MinSeg) & 0x80100200) == 0 &&
	    ((MaxSeg - MinShell) & 0x80100000) == 0) {
	  HidingShell = (s == myCurShell);
	  TColStd_Array1OfTransient& Face = (*psd)->HidingPolyData();
	  Standard_Integer nbFace = Face.Upper();
	  Handle(HLRAlgo_PolyData)* pd = 
	    (Handle(HLRAlgo_PolyData)*)&(Face.ChangeValue(1));
	  
	  for (Standard_Integer f = 1; f <= nbFace; f++) {
	    (*pd)->HideByPolyData(Coordinates,
				  (Standard_Address)myRealPtr,
				  IndexPtr,
				  HidingShell,
				  status);
	    pd++;
	  }
	}
      }
      psd++;
    }
  }
}

//=======================================================================
//function : NextShow
//purpose  : 
//=======================================================================

void HLRAlgo_PolyAlgo::NextShow ()
{
  myFound = Standard_False;
  if (myCurShell != 0) {
    mySegListIt.Next();
    if (mySegListIt.More()) myFound = Standard_True;
  }
  if (!myFound) {
    myCurShell++;

    while (myCurShell <= myNbrShell && !myFound) {
      mySegListIt.Initialize((*(Handle(HLRAlgo_PolyShellData)*)&
			      (myHShell->ChangeValue(myCurShell)))
			     ->Edges());
      if (mySegListIt.More()) myFound = Standard_True;
      else                    myCurShell++;
    }
  }
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

void HLRAlgo_PolyAlgo::Show (Standard_Address& Coordinates,
			     Standard_Integer& Index,
			     Standard_Boolean& reg1,
			     Standard_Boolean& regn,
			     Standard_Boolean& outl,
			     Standard_Boolean& intl)
{
  HLRAlgo_BiPoint& BP = mySegListIt.Value();
  Standard_Address IndexPtr = BP.Indices();
  Coordinates = BP.Coordinates();
  Index = ShapeIndex;
  reg1  = BP.Rg1Line();
  regn  = BP.RgNLine();
  outl  = BP.OutLine();
  intl  = BP.IntLine();
}

