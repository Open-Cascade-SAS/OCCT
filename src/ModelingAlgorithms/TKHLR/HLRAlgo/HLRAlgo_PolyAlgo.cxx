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
#include <NCollection_List.hxx>
#include <HLRAlgo_PolyShellData.hxx>
#include <HLRAlgo_PolyMask.hxx>
#include <Precision.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRAlgo_PolyAlgo, Standard_Transient)

//=================================================================================================

HLRAlgo_PolyAlgo::HLRAlgo_PolyAlgo()
    : myNbrShell(0),
      myCurShell(0),
      myFound(false)
{
  myTriangle.TolParam = 0.00000001;
  myTriangle.TolAng   = 0.0001;
}

//=================================================================================================

void HLRAlgo_PolyAlgo::Init(const int theNbShells)
{
  myHShell.Resize(1, theNbShells, false);
}

//=================================================================================================

void HLRAlgo_PolyAlgo::Clear()
{
  NCollection_Array1<occ::handle<HLRAlgo_PolyShellData>> anEmpty;
  myHShell.Move(anEmpty);
  myNbrShell = 0;
}

//=================================================================================================

void HLRAlgo_PolyAlgo::Update()
{
  int              j;
  int              nxMin, nyMin, nzMin, nxMax, nyMax, nzMax;
  double           xShellMin, yShellMin, zShellMin;
  double           xShellMax, yShellMax, zShellMax;
  double           xPolyTMin, yPolyTMin, zPolyTMin;
  double           xPolyTMax, yPolyTMax, zPolyTMax;
  double           xTrianMin, yTrianMin, zTrianMin;
  double           xTrianMax, yTrianMax, zTrianMax;
  double           xSegmnMin, ySegmnMin, zSegmnMin;
  double           xSegmnMax, ySegmnMax, zSegmnMax;
  constexpr double Big = Precision::Infinite();
  Bnd_Box          aBox;

  myNbrShell = myHShell.Size();
  for (int aShellIter = myHShell.Lower(); aShellIter <= myHShell.Upper(); ++aShellIter)
  {
    const occ::handle<HLRAlgo_PolyShellData>& aPsd = myHShell.ChangeValue(aShellIter);
    aPsd->UpdateGlobalMinMax(aBox);
  }

  const auto [aXMin, aXMax, aYMin, aYMax, aZMin, aZMax] = aBox.Get();
  double dx                                             = aXMax - aXMin;
  double dy                                             = aYMax - aYMin;
  double dz                                             = aZMax - aZMin;
  double precad                                         = dx;
  if (precad < dy)
    precad = dy;
  if (precad < dz)
    precad = dz;
  myTriangle.Tolerance = precad * myTriangle.TolParam;
  precad               = precad * 0.01;
  double SurDX         = 1020 / (dx + precad);
  double SurDY         = 1020 / (dy + precad);
  double SurDZ         = 508 / (dz + precad);
  precad               = precad * 0.5;
  double DecaX         = -aXMin + precad;
  double DecaY         = -aYMin + precad;
  double DecaZ         = -aZMin + precad;

  for (int aShellIter = myHShell.Lower(); aShellIter <= myHShell.Upper(); ++aShellIter)
  {
    const occ::handle<HLRAlgo_PolyShellData>& aPsd          = myHShell.ChangeValue(aShellIter);
    HLRAlgo_PolyShellData::ShellIndices&      aShellIndices = aPsd->Indices();
    xShellMin                                               = Big;
    yShellMin                                               = Big;
    zShellMin                                               = Big;
    xShellMax                                               = -Big;
    yShellMax                                               = -Big;
    zShellMax                                               = -Big;

    for (mySegListIt.Initialize(aPsd->Edges()); mySegListIt.More(); mySegListIt.Next())
    {
      HLRAlgo_BiPoint&           BP         = mySegListIt.ChangeValue();
      HLRAlgo_BiPoint::PointsT&  aPoints    = BP.Points();
      HLRAlgo_BiPoint::IndicesT& theIndices = BP.Indices();
      if (aPoints.PntP1.X() < aPoints.PntP2.X())
      {
        xSegmnMin = aPoints.PntP1.X();
        xSegmnMax = aPoints.PntP2.X();
      }
      else
      {
        xSegmnMin = aPoints.PntP2.X();
        xSegmnMax = aPoints.PntP1.X();
      }
      if (aPoints.PntP1.Y() < aPoints.PntP2.Y())
      {
        ySegmnMin = aPoints.PntP1.Y();
        ySegmnMax = aPoints.PntP2.Y();
      }
      else
      {
        ySegmnMin = aPoints.PntP2.Y();
        ySegmnMax = aPoints.PntP1.Y();
      }
      if (aPoints.PntP1.Z() < aPoints.PntP2.Z())
      {
        zSegmnMin = aPoints.PntP1.Z();
        zSegmnMax = aPoints.PntP2.Z();
      }
      else
      {
        zSegmnMin = aPoints.PntP2.Z();
        zSegmnMax = aPoints.PntP1.Z();
      }
      nxMin             = (int)((DecaX + xSegmnMin) * SurDX);
      nyMin             = (int)((DecaY + ySegmnMin) * SurDY);
      nzMin             = (int)((DecaZ + zSegmnMin) * SurDZ);
      nxMax             = (int)((DecaX + xSegmnMax) * SurDX);
      nyMax             = (int)((DecaY + ySegmnMax) * SurDY);
      nzMax             = (int)((DecaZ + zSegmnMax) * SurDZ);
      theIndices.MinSeg = nyMin + (nxMin << 11);
      theIndices.MinSeg <<= 10;
      theIndices.MinSeg += nzMin;
      theIndices.MaxSeg = nyMax + (nxMax << 11);
      theIndices.MaxSeg <<= 10;
      theIndices.MaxSeg += nzMax + 0x00000200;
      if (xShellMin > xSegmnMin)
        xShellMin = xSegmnMin;
      if (xShellMax < xSegmnMax)
        xShellMax = xSegmnMax;
      if (yShellMin > ySegmnMin)
        yShellMin = ySegmnMin;
      if (yShellMax < ySegmnMax)
        yShellMax = ySegmnMax;
      if (zShellMin > zSegmnMin)
        zShellMin = zSegmnMin;
      if (zShellMax < zSegmnMax)
        zShellMax = zSegmnMax;
    }
    NCollection_Array1<occ::handle<HLRAlgo_PolyData>>& aPolyg = aPsd->PolyData();
    const int                                          nbFace = aPolyg.Upper();
    int                                                nbFaHi = 0;
    for (j = 1; j <= nbFace; j++)
    {
      const occ::handle<HLRAlgo_PolyData>& aPd = aPolyg.ChangeValue(j);
      if (aPd->Hiding())
      {
        nbFaHi++;
        xPolyTMin = Big;
        yPolyTMin = Big;
        zPolyTMin = Big;
        xPolyTMax = -Big;
        yPolyTMax = -Big;
        zPolyTMax = -Big;
        int                            otheri, nbHide = 0; // min,max;
        double                         X1, X2, X3, Y1, Y2, Y3, Z1, Z2, Z3;
        double                         dn, dnx, dny, dnz, dx1, dy1, dz1, dx2, dy2, dz2, dx3, dy3;
        double                         adx1, ady1, adx2, ady2, adx3, ady3;
        double                         a = 0., b = 0., c = 0., d = 0.;
        HLRAlgo_PolyData::FaceIndices& PolyTIndices       = aPd->Indices();
        NCollection_Array1<gp_XYZ>&    Nodes              = aPd->Nodes();
        NCollection_Array1<HLRAlgo_TriangleData>&   TData = aPd->TData();
        NCollection_Array1<HLRAlgo_PolyHidingData>& PHDat = aPd->PHDat();
        int                                         nbT   = TData.Upper();

        for (otheri = 1; otheri <= nbT; otheri++)
        {
          HLRAlgo_TriangleData& aTD = TData.ChangeValue(otheri);
          if (aTD.Flags & HLRAlgo_PolyMask_FMskHiding)
          {
            const gp_XYZ& P1 = Nodes.Value(aTD.Node1);
            const gp_XYZ& P2 = Nodes.Value(aTD.Node2);
            const gp_XYZ& P3 = Nodes.Value(aTD.Node3);
            X1               = P1.X();
            Y1               = P1.Y();
            Z1               = P1.Z();
            X2               = P2.X();
            Y2               = P2.Y();
            Z2               = P2.Z();
            X3               = P3.X();
            Y3               = P3.Y();
            Z3               = P3.Z();
            xTrianMax = xTrianMin = X1;
            yTrianMax = yTrianMin = Y1;
            zTrianMax = zTrianMin = Z1;
            if (xTrianMin > X2)
              xTrianMin = X2;
            else if (xTrianMax < X2)
              xTrianMax = X2;
            if (yTrianMin > Y2)
              yTrianMin = Y2;
            else if (yTrianMax < Y2)
              yTrianMax = Y2;
            if (zTrianMin > Z2)
              zTrianMin = Z2;
            else if (zTrianMax < Z2)
              zTrianMax = Z2;
            if (xTrianMin > X3)
              xTrianMin = X3;
            else if (xTrianMax < X3)
              xTrianMax = X3;
            if (yTrianMin > Y3)
              yTrianMin = Y3;
            else if (yTrianMax < Y3)
              yTrianMax = Y3;
            if (zTrianMin > Z3)
              zTrianMin = Z3;
            else if (zTrianMax < Z3)
              zTrianMax = Z3;
            nxMin = (int)((DecaX + xTrianMin) * SurDX);
            nyMin = (int)((DecaY + yTrianMin) * SurDY);
            nzMin = (int)((DecaZ + zTrianMin) * SurDZ);
            nxMax = (int)((DecaX + xTrianMax) * SurDX);
            nyMax = (int)((DecaY + yTrianMax) * SurDY);
            nzMax = (int)((DecaZ + zTrianMax) * SurDZ);
            int MinTrian, MaxTrian;
            MinTrian = nyMin + (nxMin << 11);
            MinTrian <<= 10;
            MinTrian += nzMin - 0x00000200;
            MaxTrian = nyMax + (nxMax << 11);
            MaxTrian <<= 10;
            MaxTrian += nzMax;
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
            dn  = sqrt(dnx * dnx + dny * dny + dnz * dnz);
            if (dn > 0)
            {
              a = dnx / dn;
              b = dny / dn;
              c = dnz / dn;
            }
            d = a * X1 + b * Y1 + c * Z1;
            nbHide++;
            PHDat(nbHide).Set(otheri, MinTrian, MaxTrian, a, b, c, d);
            adx1 = dx1;
            ady1 = dy1;
            if (dx1 < 0)
              adx1 = -dx1;
            if (dy1 < 0)
              ady1 = -dy1;
            adx2 = dx2;
            ady2 = dy2;
            if (dx2 < 0)
              adx2 = -dx2;
            if (dy2 < 0)
              ady2 = -dy2;
            adx3 = dx3;
            ady3 = dy3;
            if (dx3 < 0)
              adx3 = -dx3;
            if (dy3 < 0)
              ady3 = -dy3;
            if (adx1 > ady1)
              aTD.Flags |= HLRAlgo_PolyMask_EMskGrALin1;
            else
              aTD.Flags &= ~HLRAlgo_PolyMask_EMskGrALin1;
            if (adx2 > ady2)
              aTD.Flags |= HLRAlgo_PolyMask_EMskGrALin2;
            else
              aTD.Flags &= ~HLRAlgo_PolyMask_EMskGrALin2;
            if (adx3 > ady3)
              aTD.Flags |= HLRAlgo_PolyMask_EMskGrALin3;
            else
              aTD.Flags &= ~HLRAlgo_PolyMask_EMskGrALin3;
            if (xPolyTMin > xTrianMin)
              xPolyTMin = xTrianMin;
            if (xPolyTMax < xTrianMax)
              xPolyTMax = xTrianMax;
            if (yPolyTMin > yTrianMin)
              yPolyTMin = yTrianMin;
            if (yPolyTMax < yTrianMax)
              yPolyTMax = yTrianMax;
            if (zPolyTMin > zTrianMin)
              zPolyTMin = zTrianMin;
            if (zPolyTMax < zTrianMax)
              zPolyTMax = zTrianMax;
          }
        }
        nxMin            = (int)((DecaX + xPolyTMin) * SurDX);
        nyMin            = (int)((DecaY + yPolyTMin) * SurDY);
        nzMin            = (int)((DecaZ + zPolyTMin) * SurDZ);
        nxMax            = (int)((DecaX + xPolyTMax) * SurDX);
        nyMax            = (int)((DecaY + yPolyTMax) * SurDY);
        nzMax            = (int)((DecaZ + zPolyTMax) * SurDZ);
        PolyTIndices.Min = nyMin + (nxMin << 11);
        PolyTIndices.Min <<= 10;
        PolyTIndices.Min += nzMin - 0x00000200;
        PolyTIndices.Max = nyMax + (nxMax << 11);
        PolyTIndices.Max <<= 10;
        PolyTIndices.Max += nzMax;
        if (xShellMin > xPolyTMin)
          xShellMin = xPolyTMin;
        if (xShellMax < xPolyTMax)
          xShellMax = xPolyTMax;
        if (yShellMin > yPolyTMin)
          yShellMin = yPolyTMin;
        if (yShellMax < yPolyTMax)
          yShellMax = yPolyTMax;
        if (zShellMin > zPolyTMin)
          zShellMin = zPolyTMin;
        if (zShellMax < zPolyTMax)
          zShellMax = zPolyTMax;
      }
    }
    if (nbFaHi > 0)
    {
      nxMin             = (int)((DecaX + xShellMin) * SurDX);
      nyMin             = (int)((DecaY + yShellMin) * SurDY);
      nzMin             = (int)((DecaZ + zShellMin) * SurDZ);
      nxMax             = (int)((DecaX + xShellMax) * SurDX);
      nyMax             = (int)((DecaY + yShellMax) * SurDY);
      nzMax             = (int)((DecaZ + zShellMax) * SurDZ);
      aShellIndices.Min = nyMin + (nxMin << 11);
      aShellIndices.Min <<= 10;
      aShellIndices.Min += nzMin - 0x00000200;
      aShellIndices.Max = nyMax + (nxMax << 11);
      aShellIndices.Max <<= 10;
      aShellIndices.Max += nzMax;
      aPsd->UpdateHiding(nbFaHi);
      int aHiddenIndex = 1;
      for (j = 1; j <= nbFace; j++)
      {
        const occ::handle<HLRAlgo_PolyData>& aPd = aPolyg.ChangeValue(j);
        if (aPd->Hiding())
        {
          aPsd->HidingPolyData().SetValue(aHiddenIndex++, aPd);
        }
      }
    }
    else
    {
      aPsd->UpdateHiding(0);
      aShellIndices.Min = 0;
      aShellIndices.Max = 0;
    }
  }
}

//=================================================================================================

void HLRAlgo_PolyAlgo::NextHide()
{
  myFound = false;
  if (myCurShell != 0)
  {
    mySegListIt.Next();
    if (mySegListIt.More())
      myFound = true;
  }

  if (!myFound)
  {
    myCurShell++;

    while (myCurShell <= myNbrShell && !myFound)
    {
      const occ::handle<HLRAlgo_PolyShellData>& aData = myHShell.ChangeValue(myCurShell);
      mySegListIt.Initialize(aData->Edges());
      if (mySegListIt.More())
      {
        myFound = true;
      }
      else
      {
        myCurShell++;
      }
    }
  }
}

//=================================================================================================

HLRAlgo_BiPoint::PointsT& HLRAlgo_PolyAlgo::Hide(HLRAlgo_EdgeStatus& theStatus,
                                                 int&                theIndex,
                                                 bool&               theReg1,
                                                 bool&               theRegn,
                                                 bool&               theOutl,
                                                 bool&               theIntl)
{
  HLRAlgo_BiPoint&           aBP       = mySegListIt.ChangeValue();
  HLRAlgo_BiPoint::PointsT&  aPoints   = aBP.Points();
  HLRAlgo_BiPoint::IndicesT& anIndices = aBP.Indices();
  theStatus = HLRAlgo_EdgeStatus(0.0, (float)myTriangle.TolParam, 1.0, (float)myTriangle.TolParam);
  theIndex  = anIndices.ShapeIndex;
  theReg1   = aBP.Rg1Line();
  theRegn   = aBP.RgNLine();
  theOutl   = aBP.OutLine();
  theIntl   = aBP.IntLine();
  if (aBP.Hidden())
  {
    theStatus.HideAll();
    return aPoints;
  }

  for (int s = 1; s <= myNbrShell; s++)
  {
    const occ::handle<HLRAlgo_PolyShellData>& aPsd = myHShell.ChangeValue(s);
    if (!aPsd->Hiding())
    {
      continue;
    }

    HLRAlgo_PolyShellData::ShellIndices& aShellIndices = aPsd->Indices();
    if (((aShellIndices.Max - anIndices.MinSeg) & 0x80100200) == 0
        && ((anIndices.MaxSeg - aShellIndices.Min) & 0x80100000) == 0)
    {
      const bool                                         isHidingShell = (s == myCurShell);
      NCollection_Array1<occ::handle<HLRAlgo_PolyData>>& aFace         = aPsd->HidingPolyData();
      const int                                          nbFace        = aFace.Upper();
      for (int f = 1; f <= nbFace; f++)
      {
        const occ::handle<HLRAlgo_PolyData>& aPd = aFace.ChangeValue(f);
        aPd->HideByPolyData(aPoints, myTriangle, anIndices, isHidingShell, theStatus);
      }
    }
  }
  return aPoints;
}

//=================================================================================================

void HLRAlgo_PolyAlgo::NextShow()
{
  myFound = false;
  if (myCurShell != 0)
  {
    mySegListIt.Next();
    if (mySegListIt.More())
      myFound = true;
  }
  if (!myFound)
  {
    myCurShell++;

    while (myCurShell <= myNbrShell && !myFound)
    {
      mySegListIt.Initialize(myHShell.ChangeValue(myCurShell)->Edges());
      if (mySegListIt.More())
      {
        myFound = true;
      }
      else
      {
        myCurShell++;
      }
    }
  }
}

//=================================================================================================

HLRAlgo_BiPoint::PointsT& HLRAlgo_PolyAlgo::Show(int&  Index,
                                                 bool& reg1,
                                                 bool& regn,
                                                 bool& outl,
                                                 bool& intl)
{
  HLRAlgo_BiPoint&           BP         = mySegListIt.ChangeValue();
  HLRAlgo_BiPoint::IndicesT& theIndices = BP.Indices();
  HLRAlgo_BiPoint::PointsT&  aPoints    = BP.Points();
  Index                                 = theIndices.ShapeIndex;
  reg1                                  = BP.Rg1Line();
  regn                                  = BP.RgNLine();
  outl                                  = BP.OutLine();
  intl                                  = BP.IntLine();
  return aPoints;
}
