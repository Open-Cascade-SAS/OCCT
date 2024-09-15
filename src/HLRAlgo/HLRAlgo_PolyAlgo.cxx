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

#include <HLRAlgo_PolyAlgo.hxx>

#include <HLRAlgo_BiPoint.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRAlgo_ListOfBPoint.hxx>
#include <HLRAlgo_PolyShellData.hxx>
#include <HLRAlgo_PolyMask.hxx>
#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRAlgo_PolyAlgo,Standard_Transient)

//=======================================================================
//function : HLRAlgo_PolyAlgo
//purpose  : 
//=======================================================================

HLRAlgo_PolyAlgo::HLRAlgo_PolyAlgo ()
: myNbrShell(0),
  myCurShell(0),
  myFound(Standard_False)
{
  myTriangle.TolParam   = 0.00000001;
  myTriangle.TolAng = 0.0001;
}

//=======================================================================
//function : Init
//purpose  :
//=======================================================================
void HLRAlgo_PolyAlgo::Init (const Standard_Integer theNbShells)
{
  myHShell.Resize (1, theNbShells, false);
}

//=======================================================================
//function : Clear
//purpose  :
//=======================================================================
void HLRAlgo_PolyAlgo::Clear()
{
  NCollection_Array1<Handle(HLRAlgo_PolyShellData)> anEmpty;
  myHShell.Move (anEmpty);
  myNbrShell = 0;
}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void HLRAlgo_PolyAlgo::Update ()
{
  Standard_Integer j;
  Standard_Integer nxMin,nyMin,nzMin,nxMax,nyMax,nzMax;
  Standard_Real xShellMin,yShellMin,zShellMin;
  Standard_Real xShellMax,yShellMax,zShellMax;
  Standard_Real xPolyTMin,yPolyTMin,zPolyTMin;
  Standard_Real xPolyTMax,yPolyTMax,zPolyTMax;
  Standard_Real xTrianMin,yTrianMin,zTrianMin;
  Standard_Real xTrianMax,yTrianMax,zTrianMax;
  Standard_Real xSegmnMin,ySegmnMin,zSegmnMin;
  Standard_Real xSegmnMax,ySegmnMax,zSegmnMax;
  constexpr Standard_Real Big = Precision::Infinite();
  HLRAlgo_PolyData::Box aBox(Big, Big, Big, -Big, -Big, -Big);

  myNbrShell = myHShell.Size();
  for (Standard_Integer aShellIter = myHShell.Lower(); aShellIter <= myHShell.Upper(); ++aShellIter)
  {
    const Handle(HLRAlgo_PolyShellData)& aPsd = myHShell.ChangeValue (aShellIter);
    aPsd->UpdateGlobalMinMax (aBox);
  }

  Standard_Real dx = aBox.XMax - aBox.XMin;
  Standard_Real dy = aBox.YMax - aBox.YMin;
  Standard_Real dz = aBox.ZMax - aBox.ZMin;
  Standard_Real    precad = dx;
  if (precad < dy) precad = dy;
  if (precad < dz) precad = dz;
  myTriangle.Tolerance = precad * myTriangle.TolParam;
  precad = precad * 0.01;
  Standard_Real SurDX = 1020 / (dx + precad);
  Standard_Real SurDY = 1020 / (dy + precad);
  Standard_Real SurDZ =  508 / (dz + precad);
  precad = precad * 0.5;
  Standard_Real DecaX = - aBox.XMin + precad;
  Standard_Real DecaY = - aBox.YMin + precad;
  Standard_Real DecaZ = - aBox.ZMin + precad;

  for (Standard_Integer aShellIter = myHShell.Lower(); aShellIter <= myHShell.Upper(); ++aShellIter)
  {
    const Handle(HLRAlgo_PolyShellData)& aPsd = myHShell.ChangeValue (aShellIter);
    HLRAlgo_PolyShellData::ShellIndices& aShellIndices = aPsd->Indices();
    xShellMin =  Big;
    yShellMin =  Big;
    zShellMin =  Big;
    xShellMax = -Big;
    yShellMax = -Big;
    zShellMax = -Big;

    for (mySegListIt.Initialize (aPsd->Edges()); mySegListIt.More(); mySegListIt.Next())
    {
      HLRAlgo_BiPoint& BP = mySegListIt.ChangeValue();
      HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
      HLRAlgo_BiPoint::IndicesT& theIndices = BP.Indices();
      if (aPoints.PntP1.X() < aPoints.PntP2.X()) { xSegmnMin = aPoints.PntP1.X(); xSegmnMax = aPoints.PntP2.X(); }
      else                 { xSegmnMin = aPoints.PntP2.X(); xSegmnMax = aPoints.PntP1.X(); }
      if (aPoints.PntP1.Y() < aPoints.PntP2.Y()) { ySegmnMin = aPoints.PntP1.Y(); ySegmnMax = aPoints.PntP2.Y(); }
      else                 { ySegmnMin = aPoints.PntP2.Y(); ySegmnMax = aPoints.PntP1.Y(); }
      if (aPoints.PntP1.Z() < aPoints.PntP2.Z()) { zSegmnMin = aPoints.PntP1.Z(); zSegmnMax = aPoints.PntP2.Z(); }
      else                 { zSegmnMin = aPoints.PntP2.Z(); zSegmnMax = aPoints.PntP1.Z(); }
      nxMin = (Standard_Integer)((DecaX + xSegmnMin) * SurDX);
      nyMin = (Standard_Integer)((DecaY + ySegmnMin) * SurDY);
      nzMin = (Standard_Integer)((DecaZ + zSegmnMin) * SurDZ);
      nxMax = (Standard_Integer)((DecaX + xSegmnMax) * SurDX);
      nyMax = (Standard_Integer)((DecaY + ySegmnMax) * SurDY);
      nzMax = (Standard_Integer)((DecaZ + zSegmnMax) * SurDZ);
      theIndices.MinSeg = nyMin + (nxMin << 11);
      theIndices.MinSeg <<= 10;
      theIndices.MinSeg += nzMin;
      theIndices.MaxSeg = nyMax + (nxMax << 11);
      theIndices.MaxSeg <<= 10;
      theIndices.MaxSeg += nzMax + 0x00000200;
      if (xShellMin > xSegmnMin) xShellMin = xSegmnMin;
      if (xShellMax < xSegmnMax) xShellMax = xSegmnMax;
      if (yShellMin > ySegmnMin) yShellMin = ySegmnMin;
      if (yShellMax < ySegmnMax) yShellMax = ySegmnMax;
      if (zShellMin > zSegmnMin) zShellMin = zSegmnMin;
      if (zShellMax < zSegmnMax) zShellMax = zSegmnMax;
    }
    NCollection_Array1<Handle(HLRAlgo_PolyData)>& aPolyg = aPsd->PolyData();
    const Standard_Integer nbFace = aPolyg.Upper();
    Standard_Integer nbFaHi = 0;
    for (j = 1; j <= nbFace; j++)
    {
      const Handle(HLRAlgo_PolyData)& aPd = aPolyg.ChangeValue (j);
      if (aPd->Hiding())
      {
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
	HLRAlgo_PolyData::FaceIndices& PolyTIndices = aPd->Indices();
	TColgp_Array1OfXYZ   & Nodes        = aPd->Nodes();
	HLRAlgo_Array1OfTData& TData        = aPd->TData();
	HLRAlgo_Array1OfPHDat& PHDat        = aPd->PHDat();
	Standard_Integer nbT = TData.Upper();
	
	for (otheri = 1; otheri <= nbT; otheri++) {
	  HLRAlgo_TriangleData& aTD = TData.ChangeValue (otheri);
	  if (aTD.Flags & HLRAlgo_PolyMask_FMskHiding) {
	    const gp_XYZ& P1 = Nodes.Value (aTD.Node1);
	    const gp_XYZ& P2 = Nodes.Value (aTD.Node2);
	    const gp_XYZ& P3 = Nodes.Value (aTD.Node3);
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
	    if (adx1 > ady1) aTD.Flags |=  HLRAlgo_PolyMask_EMskGrALin1;
	    else             aTD.Flags &= ~HLRAlgo_PolyMask_EMskGrALin1;
	    if (adx2 > ady2) aTD.Flags |=  HLRAlgo_PolyMask_EMskGrALin2;
	    else             aTD.Flags &= ~HLRAlgo_PolyMask_EMskGrALin2;
	    if (adx3 > ady3) aTD.Flags |=  HLRAlgo_PolyMask_EMskGrALin3;
	    else             aTD.Flags &= ~HLRAlgo_PolyMask_EMskGrALin3;
	    if (xPolyTMin > xTrianMin) xPolyTMin = xTrianMin;
	    if (xPolyTMax < xTrianMax) xPolyTMax = xTrianMax;
	    if (yPolyTMin > yTrianMin) yPolyTMin = yTrianMin;
	    if (yPolyTMax < yTrianMax) yPolyTMax = yTrianMax;
	    if (zPolyTMin > zTrianMin) zPolyTMin = zTrianMin;
	    if (zPolyTMax < zTrianMax) zPolyTMax = zTrianMax;
	  }
	}
	nxMin = (Standard_Integer)((DecaX + xPolyTMin) * SurDX);
	nyMin = (Standard_Integer)((DecaY + yPolyTMin) * SurDY);
	nzMin = (Standard_Integer)((DecaZ + zPolyTMin) * SurDZ);
	nxMax = (Standard_Integer)((DecaX + xPolyTMax) * SurDX);
	nyMax = (Standard_Integer)((DecaY + yPolyTMax) * SurDY);
	nzMax = (Standard_Integer)((DecaZ + zPolyTMax) * SurDZ);
	PolyTIndices.Min = nyMin + (nxMin << 11);
	PolyTIndices.Min <<= 10;
	PolyTIndices.Min  += nzMin - 0x00000200;
	PolyTIndices.Max   = nyMax + (nxMax << 11);
	PolyTIndices.Max <<= 10;
	PolyTIndices.Max  += nzMax;
	if (xShellMin > xPolyTMin) xShellMin = xPolyTMin;
	if (xShellMax < xPolyTMax) xShellMax = xPolyTMax;
	if (yShellMin > yPolyTMin) yShellMin = yPolyTMin;
	if (yShellMax < yPolyTMax) yShellMax = yPolyTMax;
	if (zShellMin > zPolyTMin) zShellMin = zPolyTMin;
	if (zShellMax < zPolyTMax) zShellMax = zPolyTMax;
      }
    }
    if (nbFaHi > 0) {
      nxMin = (Standard_Integer)((DecaX + xShellMin) * SurDX);
      nyMin = (Standard_Integer)((DecaY + yShellMin) * SurDY);
      nzMin = (Standard_Integer)((DecaZ + zShellMin) * SurDZ);
      nxMax = (Standard_Integer)((DecaX + xShellMax) * SurDX);
      nyMax = (Standard_Integer)((DecaY + yShellMax) * SurDY);
      nzMax = (Standard_Integer)((DecaZ + zShellMax) * SurDZ);
      aShellIndices.Min = nyMin + (nxMin << 11);
      aShellIndices.Min <<= 10;
      aShellIndices.Min += nzMin - 0x00000200;
      aShellIndices.Max = nyMax + (nxMax << 11);
      aShellIndices.Max <<= 10;
      aShellIndices.Max += nzMax;
      aPsd->UpdateHiding(nbFaHi);
      Standard_Integer aHiddenIndex = 1;
      for (j = 1; j <= nbFace; j++)
      {
        const Handle(HLRAlgo_PolyData)& aPd = aPolyg.ChangeValue (j);
        if (aPd->Hiding())
        {
          aPsd->HidingPolyData().SetValue (aHiddenIndex++, aPd);
        }
      }
    }
    else
    {
      aPsd->UpdateHiding (0);
      aShellIndices.Min = 0;
      aShellIndices.Max = 0;
    }
  }
}

//=======================================================================
//function : NextHide
//purpose  : 
//=======================================================================
void HLRAlgo_PolyAlgo::NextHide()
{
  myFound = Standard_False;
  if (myCurShell != 0)
  {
    mySegListIt.Next();
    if (mySegListIt.More()) myFound = Standard_True;
  }

  if (!myFound)
  {
    myCurShell++;

    while (myCurShell <= myNbrShell && !myFound)
    {
      const Handle(HLRAlgo_PolyShellData)& aData = myHShell.ChangeValue (myCurShell);
      mySegListIt.Initialize (aData->Edges());
      if (mySegListIt.More()) { myFound = Standard_True; }
      else                    { myCurShell++; }
    }
  }
}

//=======================================================================
//function : Hide
//purpose  :
//=======================================================================
HLRAlgo_BiPoint::PointsT& HLRAlgo_PolyAlgo::Hide (HLRAlgo_EdgeStatus& theStatus,
                                                  Standard_Integer& theIndex,
                                                  Standard_Boolean& theReg1,
                                                  Standard_Boolean& theRegn,
                                                  Standard_Boolean& theOutl,
                                                  Standard_Boolean& theIntl)
{
  HLRAlgo_BiPoint& aBP = mySegListIt.ChangeValue();
  HLRAlgo_BiPoint::PointsT&  aPoints   = aBP.Points();
  HLRAlgo_BiPoint::IndicesT& anIndices = aBP.Indices();
  theStatus = HLRAlgo_EdgeStatus (0.0, (Standard_ShortReal)myTriangle.TolParam,
                                  1.0, (Standard_ShortReal)myTriangle.TolParam);
  theIndex = anIndices.ShapeIndex;
  theReg1  = aBP.Rg1Line();
  theRegn  = aBP.RgNLine();
  theOutl  = aBP.OutLine();
  theIntl  = aBP.IntLine();
  if (aBP.Hidden())
  {
    theStatus.HideAll();
    return aPoints;
  }

  for (Standard_Integer s = 1; s <= myNbrShell; s++)
  {
    const Handle(HLRAlgo_PolyShellData)& aPsd = myHShell.ChangeValue (s);
    if (!aPsd->Hiding())
    {
      continue;
    }

    HLRAlgo_PolyShellData::ShellIndices& aShellIndices = aPsd->Indices();
    if (((aShellIndices.Max - anIndices.MinSeg) & 0x80100200) == 0 &&
        ((anIndices.MaxSeg - aShellIndices.Min) & 0x80100000) == 0)
    {
      const Standard_Boolean isHidingShell = (s == myCurShell);
      NCollection_Array1<Handle(HLRAlgo_PolyData)>& aFace = aPsd->HidingPolyData();
      const Standard_Integer nbFace = aFace.Upper();
      for (Standard_Integer f = 1; f <= nbFace; f++)
      {
        const Handle(HLRAlgo_PolyData)& aPd = aFace.ChangeValue (f);
        aPd->HideByPolyData (aPoints,
                              myTriangle,
                              anIndices,
                              isHidingShell,
                              theStatus);
      }
    }
  }
  return aPoints;
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
  if (!myFound)
  {
    myCurShell++;

    while (myCurShell <= myNbrShell && !myFound)
    {
      mySegListIt.Initialize (myHShell.ChangeValue (myCurShell)->Edges());
      if (mySegListIt.More()) { myFound = Standard_True; }
      else                    { myCurShell++; }
    }
  }
}

//=======================================================================
//function : Show
//purpose  : 
//=======================================================================

HLRAlgo_BiPoint::PointsT& HLRAlgo_PolyAlgo::Show (
			     Standard_Integer& Index,
			     Standard_Boolean& reg1,
			     Standard_Boolean& regn,
			     Standard_Boolean& outl,
			     Standard_Boolean& intl)
{
  HLRAlgo_BiPoint& BP = mySegListIt.ChangeValue();
  HLRAlgo_BiPoint::IndicesT& theIndices = BP.Indices();
  HLRAlgo_BiPoint::PointsT& aPoints = BP.Points();
  Index = theIndices.ShapeIndex;
  reg1  = BP.Rg1Line();
  regn  = BP.RgNLine();
  outl  = BP.OutLine();
  intl  = BP.IntLine();
  return aPoints;
}
