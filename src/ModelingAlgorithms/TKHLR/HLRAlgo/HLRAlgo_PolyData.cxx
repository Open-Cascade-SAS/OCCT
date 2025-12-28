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

IMPLEMENT_STANDARD_RTTIEXT(HLRAlgo_PolyData, Standard_Transient)

#ifdef OCCT_DEBUG
static int HLRAlgo_PolyData_ERROR = false;
#endif
//=================================================================================================

HLRAlgo_PolyData::HLRAlgo_PolyData() {}

//=================================================================================================

void HLRAlgo_PolyData::HNodes(const occ::handle<NCollection_HArray1<gp_XYZ>>& HNodes)
{
  myHNodes = HNodes;
}

//=================================================================================================

void HLRAlgo_PolyData::HTData(const occ::handle<NCollection_HArray1<HLRAlgo_TriangleData>>& HTData)
{
  myHTData = HTData;
}

//=================================================================================================

void HLRAlgo_PolyData::HPHDat(const occ::handle<NCollection_HArray1<HLRAlgo_PolyHidingData>>& HPHDat)
{
  myHPHDat = HPHDat;
}

//=================================================================================================

void HLRAlgo_PolyData::UpdateGlobalMinMax(Bnd_Box& theBox)
{
  const NCollection_Array1<gp_XYZ>&    Nodes = myHNodes->Array1();
  const NCollection_Array1<HLRAlgo_TriangleData>& TData = myHTData->Array1();
  const int       nbT   = TData.Upper();

  for (int i = 1; i <= nbT; i++)
  {
    const HLRAlgo_TriangleData& TD = TData.Value(i);
    if (TD.Flags & HLRAlgo_PolyMask_FMskHiding)
    {
      const gp_XYZ& P1 = Nodes(TD.Node1);
      const gp_XYZ& P2 = Nodes(TD.Node2);
      const gp_XYZ& P3 = Nodes(TD.Node3);
      theBox.Update(P1.X(), P1.Y(), P1.Z());
      theBox.Update(P2.X(), P2.Y(), P2.Z());
      theBox.Update(P3.X(), P3.Y(), P3.Z());
    }
  }
}

//=================================================================================================

void HLRAlgo_PolyData::HideByPolyData(const HLRAlgo_BiPoint::PointsT& thePoints,
                                      Triangle&                       theTriangle,
                                      HLRAlgo_BiPoint::IndicesT&      theIndices,
                                      const bool          HidingShell,
                                      HLRAlgo_EdgeStatus&             status)
{
  if (((myFaceIndices.Max - theIndices.MinSeg) & 0x80100200) == 0
      && ((theIndices.MaxSeg - myFaceIndices.Min) & 0x80100000) == 0)
  {
    NCollection_Array1<HLRAlgo_PolyHidingData>&       PHDat = myHPHDat->ChangeArray1();
    const NCollection_Array1<HLRAlgo_TriangleData>& TData = myHTData->Array1();
    double                d1, d2;
    bool             NotConnex    = false;
    bool             isCrossing   = false;
    bool             toHideBefore = false;
    int             TFlag        = 0;
    int             h, h2 = PHDat.Upper();
    HLRAlgo_PolyHidingData*      PH = &(PHDat(1));

    for (h = 1; h <= h2; h++)
    {
      HLRAlgo_PolyHidingData::TriangleIndices& aTriangleIndices = PH->Indices();
      if (((aTriangleIndices.Max - theIndices.MinSeg) & 0x80100200) == 0
          && ((theIndices.MaxSeg - aTriangleIndices.Min) & 0x80100000) == 0)
      {
        const HLRAlgo_TriangleData& aTriangle = TData(aTriangleIndices.Index);
        NotConnex                             = true;
        if (HidingShell)
        {
          if (myFaceIndices.Index == theIndices.FaceConex1)
          {
            if (theIndices.Face1Pt1 == aTriangle.Node1)
              NotConnex =
                theIndices.Face1Pt2 != aTriangle.Node2 && theIndices.Face1Pt2 != aTriangle.Node3;
            else if (theIndices.Face1Pt1 == aTriangle.Node2)
              NotConnex =
                theIndices.Face1Pt2 != aTriangle.Node3 && theIndices.Face1Pt2 != aTriangle.Node1;
            else if (theIndices.Face1Pt1 == aTriangle.Node3)
              NotConnex =
                theIndices.Face1Pt2 != aTriangle.Node1 && theIndices.Face1Pt2 != aTriangle.Node2;
          }
          else if (myFaceIndices.Index == theIndices.FaceConex2)
          {
            if (theIndices.Face2Pt1 == aTriangle.Node1)
              NotConnex =
                theIndices.Face2Pt2 != aTriangle.Node2 && theIndices.Face2Pt2 != aTriangle.Node3;
            else if (theIndices.Face2Pt1 == aTriangle.Node2)
              NotConnex =
                theIndices.Face2Pt2 != aTriangle.Node3 && theIndices.Face2Pt2 != aTriangle.Node1;
            else if (theIndices.Face2Pt1 == aTriangle.Node3)
              NotConnex =
                theIndices.Face2Pt2 != aTriangle.Node1 && theIndices.Face2Pt2 != aTriangle.Node2;
          }
        }
        if (NotConnex)
        {
          HLRAlgo_PolyHidingData::PlaneT& aPlane = PH->Plane();
          d1                                     = aPlane.Normal * thePoints.PntP1 - aPlane.D;
          d2                                     = aPlane.Normal * thePoints.PntP2 - aPlane.D;
          if (d1 > theTriangle.Tolerance)
          {
            if (d2 < -theTriangle.Tolerance)
            {
              theTriangle.Param               = d1 / (d1 - d2);
              toHideBefore                    = false;
              isCrossing                      = true;
              TFlag                           = aTriangle.Flags;
              const NCollection_Array1<gp_XYZ>& Nodes = myHNodes->Array1();
              const gp_XYZ&             P1    = Nodes(aTriangle.Node1);
              const gp_XYZ&             P2    = Nodes(aTriangle.Node2);
              const gp_XYZ&             P3    = Nodes(aTriangle.Node3);
              theTriangle.V1                  = gp_XY(P1.X(), P1.Y());
              theTriangle.V2                  = gp_XY(P2.X(), P2.Y());
              theTriangle.V3                  = gp_XY(P3.X(), P3.Y());
              hideByOneTriangle(thePoints, theTriangle, isCrossing, toHideBefore, TFlag, status);
            }
          }
          else if (d1 < -theTriangle.Tolerance)
          {
            if (d2 > theTriangle.Tolerance)
            {
              theTriangle.Param               = d1 / (d1 - d2);
              toHideBefore                    = true;
              isCrossing                      = true;
              TFlag                           = aTriangle.Flags;
              const NCollection_Array1<gp_XYZ>& Nodes = myHNodes->Array1();
              const gp_XYZ&             P1    = Nodes(aTriangle.Node1);
              const gp_XYZ&             P2    = Nodes(aTriangle.Node2);
              const gp_XYZ&             P3    = Nodes(aTriangle.Node3);
              theTriangle.V1                  = gp_XY(P1.X(), P1.Y());
              theTriangle.V2                  = gp_XY(P2.X(), P2.Y());
              theTriangle.V3                  = gp_XY(P3.X(), P3.Y());
              hideByOneTriangle(thePoints, theTriangle, isCrossing, toHideBefore, TFlag, status);
            }
            else
            {
              isCrossing                      = false;
              TFlag                           = aTriangle.Flags;
              const NCollection_Array1<gp_XYZ>& Nodes = myHNodes->Array1();
              const gp_XYZ&             P1    = Nodes(aTriangle.Node1);
              const gp_XYZ&             P2    = Nodes(aTriangle.Node2);
              const gp_XYZ&             P3    = Nodes(aTriangle.Node3);
              theTriangle.V1                  = gp_XY(P1.X(), P1.Y());
              theTriangle.V2                  = gp_XY(P2.X(), P2.Y());
              theTriangle.V3                  = gp_XY(P3.X(), P3.Y());
              hideByOneTriangle(thePoints, theTriangle, isCrossing, toHideBefore, TFlag, status);
            }
          }
          else if (d2 < -theTriangle.Tolerance)
          {
            isCrossing                      = false;
            TFlag                           = aTriangle.Flags;
            const NCollection_Array1<gp_XYZ>& Nodes = myHNodes->Array1();
            const gp_XYZ&             P1    = Nodes(aTriangle.Node1);
            const gp_XYZ&             P2    = Nodes(aTriangle.Node2);
            const gp_XYZ&             P3    = Nodes(aTriangle.Node3);
            theTriangle.V1                  = gp_XY(P1.X(), P1.Y());
            theTriangle.V2                  = gp_XY(P2.X(), P2.Y());
            theTriangle.V3                  = gp_XY(P3.X(), P3.Y());
            hideByOneTriangle(thePoints, theTriangle, isCrossing, toHideBefore, TFlag, status);
          }
        }
      }
      PH++;
    }
  }
}

//=================================================================================================

void HLRAlgo_PolyData::hideByOneTriangle(const HLRAlgo_BiPoint::PointsT& thePoints,
                                         Triangle&                       theTriangle,
                                         const bool          Crossing,
                                         const bool          HideBefore,
                                         const int          TrFlags,
                                         HLRAlgo_EdgeStatus&             status)
{
  bool CrosSeg = false;
  int n1      = 0;
  double    pd1 = 0., pd2 = 0.;
  int nn1 = 0, nn2 = 0;
  double    pend = 1., psta = 0., pp = 0., pdp = 0.;
  int npi    = -1;
  bool o[]    = {false, false};
  bool m[]    = {false, false};
  double    p[]    = {0., 0.};
  int npiRej = 0;

  {
    const gp_XY         aD   = theTriangle.V2 - theTriangle.V1;
    const gp_XY         aA   = (1 / aD.Modulus()) * gp_XY(-aD.Y(), aD.X());
    const double aDot = aA * theTriangle.V1;
    const double d1   = aA * thePoints.PntP12D() - aDot;
    const double d2   = aA * thePoints.PntP22D() - aDot;
    if (d1 > theTriangle.Tolerance)
    {
      if (d2 < -theTriangle.Tolerance)
      {
        n1      = 2;
        CrosSeg = true;
      }
      else
        CrosSeg = false;
    }
    else if (d1 < -theTriangle.Tolerance)
    {
      if (d2 > theTriangle.Tolerance)
      {
        n1      = -1;
        CrosSeg = true;
      }
      else
        return;
    }
    else
    {
      if (d2 > theTriangle.Tolerance)
        CrosSeg = false;
      else if (d2 < -theTriangle.Tolerance)
        return;
      else
      {
        CrosSeg = false;
        if (TrFlags & HLRAlgo_PolyMask_EMskGrALin1)
        {
          pd1 = (thePoints.PntP1.X() - theTriangle.V1.X()) / aD.X();
          pd2 = (thePoints.PntP2.X() - theTriangle.V1.X()) / aD.X();
        }
        else
        {
          pd1 = (thePoints.PntP1.Y() - theTriangle.V1.Y()) / aD.Y();
          pd2 = (thePoints.PntP2.Y() - theTriangle.V1.Y()) / aD.Y();
        }
        if (pd1 < -theTriangle.TolParam)
          nn1 = 1;
        else if (pd1 < theTriangle.TolParam)
          nn1 = 2;
        else if (pd1 - 1. < -theTriangle.TolParam)
          nn1 = 3;
        else if (pd1 - 1. < theTriangle.TolParam)
          nn1 = 4;
        else
          nn1 = 5;
        if (pd2 < -theTriangle.TolParam)
          nn2 = 1;
        else if (pd2 < theTriangle.TolParam)
          nn2 = 2;
        else if (pd2 - 1. < -theTriangle.TolParam)
          nn2 = 3;
        else if (pd2 - 1. < theTriangle.TolParam)
          nn2 = 4;
        else
          nn2 = 5;
        if (nn1 == 3)
        {
          if (nn2 == 1)
            pend = pd1 / (pd1 - pd2);
          else if (nn2 == 5)
            pend = (1. - pd1) / (pd2 - pd1);
        }
        else if (nn1 == 1)
        {
          if (nn2 <= 2)
            return;
          else
          {
            psta = -pd1 / (pd2 - pd1);
            if (nn2 == 5)
              pend = (1. - pd1) / (pd2 - pd1);
          }
        }
        else if (nn1 == 5)
        {
          if (nn2 >= 4)
            return;
          else
          {
            psta = (pd1 - 1.) / (pd1 - pd2);
            if (nn2 == 1)
              pend = pd1 / (pd1 - pd2);
          }
        }
        else if (nn1 == 2)
        {
          if (nn2 == 1)
            return;
          else if (nn2 == 5)
            pend = (1. - pd1) / (pd2 - pd1);
        }
        else if (nn1 == 4)
        {
          if (nn2 == 5)
            return;
          else if (nn2 == 1)
            pend = pd1 / (pd1 - pd2);
        }
      }
    }
    if (CrosSeg)
    {
      double ad1 = d1;
      if (d1 < 0)
        ad1 = -d1;
      double ad2 = d2;
      if (d2 < 0)
        ad2 = -d2;
      pp = ad1 / (ad1 + ad2);
      if (TrFlags & HLRAlgo_PolyMask_EMskGrALin1)
        pdp = (thePoints.PntP1.X() + (thePoints.PntP2.X() - thePoints.PntP1.X()) * pp
               - theTriangle.V1.X())
              / aD.X();
      else
        pdp = (thePoints.PntP1.Y() + (thePoints.PntP2.Y() - thePoints.PntP1.Y()) * pp
               - theTriangle.V1.Y())
              / aD.Y();
      bool OutSideP = false;
      bool Multiple = false;
      if (pdp < -theTriangle.TolParam)
        OutSideP = true;
      else if (pdp < theTriangle.TolParam)
      {
        Multiple = true;

        for (int l = 0; l <= npi; l++)
        {
          if (m[l])
          {
            OutSideP = true;

            if (o[l] != (n1 == -1))
            {
              if (l == 0 && npi == 1)
              {
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
      else if (pdp - 1. < -theTriangle.TolParam)
      {
      }
      else if (pdp - 1. < theTriangle.TolParam)
      {
        Multiple = true;

        for (int l = 0; l <= npi; l++)
        {
          if (m[l])
          {
            OutSideP = true;
            if (o[l] != (n1 == -1))
            {
              if (l == 0 && npi == 1)
              {
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
      else
        OutSideP = true;
      if (OutSideP)
        npiRej++;
      else
      {
        npi++;
        if (npi < 2)
        {
          p[npi] = pp;
          o[npi] = n1 == -1;
          m[npi] = Multiple;
        }
#ifdef OCCT_DEBUG
        else if (HLRAlgo_PolyData_ERROR)
        {
          std::cout << " error : HLRAlgo_PolyData::HideByOneTriangle " << std::endl;
          std::cout << " ( more than 2 points )." << std::endl;
        }
#endif
      }
    }
  }

  {
    const gp_XY         aD   = theTriangle.V3 - theTriangle.V2;
    const gp_XY         aA   = (1 / aD.Modulus()) * gp_XY(-aD.Y(), aD.X());
    const double aDot = aA * theTriangle.V2;
    const double d1   = aA * thePoints.PntP12D() - aDot;
    const double d2   = aA * thePoints.PntP22D() - aDot;
    if (d1 > theTriangle.Tolerance)
    {
      if (d2 < -theTriangle.Tolerance)
      {
        n1      = 2;
        CrosSeg = true;
      }
      else
        CrosSeg = false;
    }
    else if (d1 < -theTriangle.Tolerance)
    {
      if (d2 > theTriangle.Tolerance)
      {
        n1      = -1;
        CrosSeg = true;
      }
      else
        return;
    }
    else
    {
      if (d2 > theTriangle.Tolerance)
        CrosSeg = false;
      else if (d2 < -theTriangle.Tolerance)
        return;
      else
      {
        CrosSeg = false;
        if (TrFlags & HLRAlgo_PolyMask_EMskGrALin2)
        {
          pd1 = (thePoints.PntP1.X() - theTriangle.V2.X()) / aD.X();
          pd2 = (thePoints.PntP2.X() - theTriangle.V2.X()) / aD.X();
        }
        else
        {
          pd1 = (thePoints.PntP1.Y() - theTriangle.V2.Y()) / aD.Y();
          pd2 = (thePoints.PntP2.Y() - theTriangle.V2.Y()) / aD.Y();
        }
        if (pd1 < -theTriangle.TolParam)
          nn1 = 1;
        else if (pd1 < theTriangle.TolParam)
          nn1 = 2;
        else if (pd1 - 1. < -theTriangle.TolParam)
          nn1 = 3;
        else if (pd1 - 1. < theTriangle.TolParam)
          nn1 = 4;
        else
          nn1 = 5;
        if (pd2 < -theTriangle.TolParam)
          nn2 = 1;
        else if (pd2 < theTriangle.TolParam)
          nn2 = 2;
        else if (pd2 - 1. < -theTriangle.TolParam)
          nn2 = 3;
        else if (pd2 - 1. < theTriangle.TolParam)
          nn2 = 4;
        else
          nn2 = 5;
        if (nn1 == 3)
        {
          if (nn2 == 1)
            pend = pd1 / (pd1 - pd2);
          else if (nn2 == 5)
            pend = (1. - pd1) / (pd2 - pd1);
        }
        else if (nn1 == 1)
        {
          if (nn2 <= 2)
            return;
          else
          {
            psta = -pd1 / (pd2 - pd1);
            if (nn2 == 5)
              pend = (1. - pd1) / (pd2 - pd1);
          }
        }
        else if (nn1 == 5)
        {
          if (nn2 >= 4)
            return;
          else
          {
            psta = (pd1 - 1.) / (pd1 - pd2);
            if (nn2 == 1)
              pend = pd1 / (pd1 - pd2);
          }
        }
        else if (nn1 == 2)
        {
          if (nn2 == 1)
            return;
          else if (nn2 == 5)
            pend = (1. - pd1) / (pd2 - pd1);
        }
        else if (nn1 == 4)
        {
          if (nn2 == 5)
            return;
          else if (nn2 == 1)
            pend = pd1 / (pd1 - pd2);
        }
      }
    }
    if (CrosSeg)
    {
      double ad1 = d1;
      if (d1 < 0)
        ad1 = -d1;
      double ad2 = d2;
      if (d2 < 0)
        ad2 = -d2;
      pp = ad1 / (ad1 + ad2);
      if (TrFlags & HLRAlgo_PolyMask_EMskGrALin2)
        pdp = (thePoints.PntP1.X() + (thePoints.PntP2.X() - thePoints.PntP1.X()) * pp
               - theTriangle.V2.X())
              / aD.X();
      else
        pdp = (thePoints.PntP1.Y() + (thePoints.PntP2.Y() - thePoints.PntP1.Y()) * pp
               - theTriangle.V2.Y())
              / aD.Y();
      bool OutSideP = false;
      bool Multiple = false;
      if (pdp < -theTriangle.TolParam)
        OutSideP = true;
      else if (pdp < theTriangle.TolParam)
      {
        Multiple = true;

        for (int l = 0; l <= npi; l++)
        {
          if (m[l])
          {
            OutSideP = true;
            if (o[l] != (n1 == -1))
            {
              if (l == 0 && npi == 1)
              {
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
      else if (pdp - 1. < -theTriangle.TolParam)
      {
      }
      else if (pdp - 1. < theTriangle.TolParam)
      {
        Multiple = true;

        for (int l = 0; l <= npi; l++)
        {
          if (m[l])
          {
            OutSideP = true;
            if (o[l] != (n1 == -1))
            {
              if (l == 0 && npi == 1)
              {
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
      else
        OutSideP = true;
      if (OutSideP)
        npiRej++;
      else
      {
        npi++;
        if (npi < 2)
        {
          p[npi] = pp;
          o[npi] = n1 == -1;
          m[npi] = Multiple;
        }
#ifdef OCCT_DEBUG
        else if (HLRAlgo_PolyData_ERROR)
        {
          std::cout << " error : HLRAlgo_PolyData::HideByOneTriangle " << std::endl;
          std::cout << " ( more than 2 points )." << std::endl;
        }
#endif
      }
    }
  }

  {
    const gp_XY         aD   = theTriangle.V1 - theTriangle.V3;
    const gp_XY         aA   = (1 / aD.Modulus()) * gp_XY(-aD.Y(), aD.X());
    const double aDot = aA * theTriangle.V3;
    const double d1   = aA * thePoints.PntP12D() - aDot;
    const double d2   = aA * thePoints.PntP22D() - aDot;
    if (d1 > theTriangle.Tolerance)
    {
      if (d2 < -theTriangle.Tolerance)
      {
        n1      = 2;
        CrosSeg = true;
      }
      else
        CrosSeg = false;
    }
    else if (d1 < -theTriangle.Tolerance)
    {
      if (d2 > theTriangle.Tolerance)
      {
        n1      = -1;
        CrosSeg = true;
      }
      else
        return;
    }
    else
    {
      if (d2 > theTriangle.Tolerance)
        CrosSeg = false;
      else if (d2 < -theTriangle.Tolerance)
        return;
      else
      {
        CrosSeg = false;
        if (TrFlags & HLRAlgo_PolyMask_EMskGrALin3)
        {
          pd1 = (thePoints.PntP1.X() - theTriangle.V3.X()) / aD.X();
          pd2 = (thePoints.PntP2.X() - theTriangle.V3.X()) / aD.X();
        }
        else
        {
          pd1 = (thePoints.PntP1.Y() - theTriangle.V3.Y()) / aD.Y();
          pd2 = (thePoints.PntP2.Y() - theTriangle.V3.Y()) / aD.Y();
        }
        if (pd1 < -theTriangle.TolParam)
          nn1 = 1;
        else if (pd1 < theTriangle.TolParam)
          nn1 = 2;
        else if (pd1 - 1. < -theTriangle.TolParam)
          nn1 = 3;
        else if (pd1 - 1. < theTriangle.TolParam)
          nn1 = 4;
        else
          nn1 = 5;
        if (pd2 < -theTriangle.TolParam)
          nn2 = 1;
        else if (pd2 < theTriangle.TolParam)
          nn2 = 2;
        else if (pd2 - 1. < -theTriangle.TolParam)
          nn2 = 3;
        else if (pd2 - 1. < theTriangle.TolParam)
          nn2 = 4;
        else
          nn2 = 5;
        if (nn1 == 3)
        {
          if (nn2 == 1)
            pend = pd1 / (pd1 - pd2);
          else if (nn2 == 5)
            pend = (1. - pd1) / (pd2 - pd1);
        }
        else if (nn1 == 1)
        {
          if (nn2 <= 2)
            return;
          else
          {
            psta = -pd1 / (pd2 - pd1);
            if (nn2 == 5)
              pend = (1. - pd1) / (pd2 - pd1);
          }
        }
        else if (nn1 == 5)
        {
          if (nn2 >= 4)
            return;
          else
          {
            psta = (pd1 - 1.) / (pd1 - pd2);
            if (nn2 == 1)
              pend = pd1 / (pd1 - pd2);
          }
        }
        else if (nn1 == 2)
        {
          if (nn2 == 1)
            return;
          else if (nn2 == 5)
            pend = (1. - pd1) / (pd2 - pd1);
        }
        else if (nn1 == 4)
        {
          if (nn2 == 5)
            return;
          else if (nn2 == 1)
            pend = pd1 / (pd1 - pd2);
        }
      }
    }
    if (CrosSeg)
    {
      double ad1 = d1;
      if (d1 < 0)
        ad1 = -d1;
      double ad2 = d2;
      if (d2 < 0)
        ad2 = -d2;
      pp = ad1 / (ad1 + ad2);
      if (TrFlags & HLRAlgo_PolyMask_EMskGrALin3)
        pdp = (thePoints.PntP1.X() + (thePoints.PntP2.X() - thePoints.PntP1.X()) * pp
               - theTriangle.V3.X())
              / aD.X();
      else
        pdp = (thePoints.PntP1.Y() + (thePoints.PntP2.Y() - thePoints.PntP1.Y()) * pp
               - theTriangle.V3.Y())
              / aD.Y();
      bool OutSideP = false;
      bool Multiple = false;
      if (pdp < -theTriangle.TolParam)
        OutSideP = true;
      else if (pdp < theTriangle.TolParam)
      {
        Multiple = true;

        for (int l = 0; l <= npi; l++)
        {
          if (m[l])
          {
            OutSideP = true;
            if (o[l] != (n1 == -1))
            {
              if (l == 0 && npi == 1)
              {
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
      else if (pdp - 1. < -theTriangle.TolParam)
      {
      }
      else if (pdp - 1. < theTriangle.TolParam)
      {
        Multiple = true;

        for (int l = 0; l <= npi; l++)
        {
          if (m[l])
          {
            OutSideP = true;
            if (o[l] != (n1 == -1))
            {
              if (l == 0 && npi == 1)
              {
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
      else
        OutSideP = true;
      if (OutSideP)
        npiRej++;
      else
      {
        npi++;
        if (npi < 2)
        {
          p[npi] = pp;
          o[npi] = n1 == -1;
          m[npi] = Multiple;
        }
#ifdef OCCT_DEBUG
        else if (HLRAlgo_PolyData_ERROR)
        {
          std::cout << " error : HLRAlgo_PolyData::HideByOneTriangle " << std::endl;
          std::cout << " ( more than 2 points )." << std::endl;
        }
#endif
      }
    }
  }

  if (npi == -1)
  {
    if (npiRej >= 2)
      return;
  }
  else if (npi == 0)
  {
    if (o[0])
    {
      psta = p[0];
      pend = 1.;
    }
    else
    {
      psta = 0.;
      pend = p[0];
    }
  }
  else if (npi == 1)
  {
    if (p[0] > p[1])
    {
      psta = p[1];
      pend = p[0];
    }
    else
    {
      psta = p[0];
      pend = p[1];
    }
  }

  if (Crossing)
  {
    if (HideBefore)
    {
      if (theTriangle.Param - psta < theTriangle.TolParam)
        return;
      else if (theTriangle.Param < pend)
        pend = theTriangle.Param;
    }
    else
    {
      if (pend - theTriangle.Param < theTriangle.TolParam)
        return;
      else if (psta < theTriangle.Param)
        psta = theTriangle.Param;
    }
  }

  bool total;
  if (psta > 0)
    total = psta < theTriangle.TolParam;
  else
    total = psta > -theTriangle.TolParam;
  if (total)
  {
    double pfin = pend - 1.;
    if (pfin > 0)
      total = pfin < theTriangle.TolParam;
    else
      total = pfin > -theTriangle.TolParam;
  }
  if (total)
    status.HideAll();
  else
    status.Hide(psta,
                (float)theTriangle.TolParam,
                pend,
                (float)theTriangle.TolParam,
                false,
                false);
}
