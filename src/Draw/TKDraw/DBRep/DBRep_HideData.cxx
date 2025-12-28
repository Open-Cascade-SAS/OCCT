// Created on: 1995-09-22
// Created by: Remi LEQUETTE
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

#include <DBRep_HideData.hxx>
#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <gp_Trsf.hxx>
#include <HLRAlgo_EdgeIterator.hxx>
#include <HLRBRep_BiPoint.hxx>
#include <NCollection_List.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <TopoDS_Shape.hxx>

#define PntX1 ((double*)Coordinates)[0]
#define PntY1 ((double*)Coordinates)[1]
#define PntZ1 ((double*)Coordinates)[2]
#define PntX2 ((double*)Coordinates)[3]
#define PntY2 ((double*)Coordinates)[4]
#define PntZ2 ((double*)Coordinates)[5]

//=================================================================================================

DBRep_HideData::DBRep_HideData()
    : myView(-1),
      myFocal(0.0),
      myAngle(0.0)
{
}

//=================================================================================================

void DBRep_HideData::Set(const int           viewID,
                         const gp_Trsf&      TProj,
                         const double        focal,
                         const TopoDS_Shape& S,
                         const double        ang)
{
  myView  = viewID;
  myTrsf  = TProj;
  myFocal = focal;
  myAngle = ang;

  occ::handle<HLRBRep_PolyAlgo> hider = new HLRBRep_PolyAlgo(S);
  hider->Projector(HLRAlgo_Projector(myTrsf, myFocal > 0., myFocal));
  hider->Update();

  double               sta, end, dx, dy, dz;
  float                tolsta, tolend;
  HLRAlgo_EdgeIterator It;
  myBiPntVis.Clear();
  myBiPntHid.Clear();
  TopoDS_Shape       Sori;
  bool               reg1, regn, outl, intl;
  void*              Coordinates;
  HLRAlgo_EdgeStatus status;

  for (hider->InitHide(); hider->MoreHide(); hider->NextHide())
  {
    Coordinates = &hider->Hide(status, Sori, reg1, regn, outl, intl);
    dx          = PntX2 - PntX1;
    dy          = PntY2 - PntY1;
    dz          = PntZ2 - PntZ1;

    for (It.InitVisible(status); It.MoreVisible(); It.NextVisible())
    {
      It.Visible(sta, tolsta, end, tolend);
      myBiPntVis.Append(HLRBRep_BiPoint(PntX1 + sta * dx,
                                        PntY1 + sta * dy,
                                        PntZ1 + sta * dz,
                                        PntX1 + end * dx,
                                        PntY1 + end * dy,
                                        PntZ1 + end * dz,
                                        Sori,
                                        reg1,
                                        regn,
                                        outl,
                                        intl));
    }

    for (It.InitHidden(status); It.MoreHidden(); It.NextHidden())
    {
      It.Hidden(sta, tolsta, end, tolend);
      myBiPntHid.Append(HLRBRep_BiPoint(PntX1 + sta * dx,
                                        PntY1 + sta * dy,
                                        PntZ1 + sta * dz,
                                        PntX1 + end * dx,
                                        PntY1 + end * dy,
                                        PntZ1 + end * dz,
                                        Sori,
                                        reg1,
                                        regn,
                                        outl,
                                        intl));
    }
  }
}

//=================================================================================================

bool DBRep_HideData::IsSame(const gp_Trsf& TProj, const double focal) const
{
  if (focal > 0)
  {
    if (myFocal <= 0)
      return false;
    if (myFocal != focal)
      return false;
    const gp_XYZ& T1 = TProj.TranslationPart();
    const gp_XYZ& T2 = myTrsf.TranslationPart();

    for (int i = 1; i <= 3; i++)
    {
      if (T1.Coord(i) != T2.Coord(i))
        return false;
    }
  }
  const gp_Mat& M1 = TProj.HVectorialPart();
  const gp_Mat& M2 = myTrsf.HVectorialPart();

  for (int i = 1; i <= 3; i++)
  {

    for (int j = 1; j <= 3; j++)
    {
      if (M1.Value(i, j) != M2.Value(i, j))
        return false;
    }
  }
  return true;
}

//=================================================================================================

void DBRep_HideData::DrawOn(Draw_Display&     D,
                            const bool        withRg1,
                            const bool        withRgN,
                            const bool        withHid,
                            const Draw_Color& VisCol,
                            const Draw_Color& HidCol)
{
  bool                                        firstPick = true;
  NCollection_List<HLRBRep_BiPoint>::Iterator It;
  //  bool reg1,regn,outl;

  if (withHid)
  {
    D.SetColor(HidCol);

    for (It.Initialize(myBiPntHid); It.More(); It.Next())
    {
      const HLRBRep_BiPoint& BP     = It.Value();
      bool                   todraw = true;
      if ((!withRg1 && BP.Rg1Line() && !BP.OutLine())
          || (!withRgN && BP.RgNLine() && !BP.OutLine()))
        todraw = false;
      if (todraw)
      {
        D.MoveTo(BP.P1());
        D.DrawTo(BP.P2());
        if (firstPick && D.HasPicked())
        {
          firstPick  = false;
          myPickShap = BP.Shape();
        }
      }
    }
  }
  D.SetColor(VisCol);

  for (It.Initialize(myBiPntVis); It.More(); It.Next())
  {
    const HLRBRep_BiPoint& BP     = It.Value();
    bool                   todraw = true;
    if ((!withRg1 && BP.Rg1Line() && !BP.OutLine()) || (!withRgN && BP.RgNLine() && !BP.OutLine()))
      todraw = false;
    if (todraw)
    {
      D.MoveTo(BP.P1());
      D.DrawTo(BP.P2());
      if (firstPick && D.HasPicked())
      {
        firstPick  = false;
        myPickShap = BP.Shape();
      }
    }
  }
}

//=================================================================================================

const TopoDS_Shape& DBRep_HideData::LastPick() const
{
  return myPickShap;
}
