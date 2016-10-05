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
#include <HLRBRep_ListIteratorOfListOfBPoint.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <TopoDS_Shape.hxx>

#define PntX1 ((Standard_Real*)Coordinates)[0]
#define PntY1 ((Standard_Real*)Coordinates)[1]
#define PntZ1 ((Standard_Real*)Coordinates)[2]
#define PntX2 ((Standard_Real*)Coordinates)[3]
#define PntY2 ((Standard_Real*)Coordinates)[4]
#define PntZ2 ((Standard_Real*)Coordinates)[5]

//=======================================================================
//function : DBRep_HideData
//purpose  : 
//=======================================================================

DBRep_HideData::DBRep_HideData()
{}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void DBRep_HideData::Set(const Standard_Integer viewID,
			 const gp_Trsf& TProj,
			 const Standard_Real focal,
			 const TopoDS_Shape& S,
                         const Standard_Real ang)
{
  myView = viewID;
  myTrsf = TProj;
  myFocal = focal;
  myAngle = ang;

  Handle(HLRBRep_PolyAlgo) hider = new HLRBRep_PolyAlgo(S);
  hider->Angle(ang);
  hider->Projector(HLRAlgo_Projector(myTrsf,myFocal > 0.,myFocal));
  hider->Update();

  Standard_Real sta,end,dx,dy,dz;
  Standard_ShortReal tolsta,tolend;
  HLRAlgo_EdgeIterator It;
  myBiPntVis.Clear();
  myBiPntHid.Clear();
  TopoDS_Shape Sori;
  Standard_Boolean reg1,regn,outl,intl;
  Standard_Address Coordinates;
  HLRAlgo_EdgeStatus status;

  for (hider->InitHide(); hider->MoreHide(); hider->NextHide()) {
    Coordinates = &hider->Hide(status,Sori,reg1,regn,outl,intl);
    dx = PntX2 - PntX1;
    dy = PntY2 - PntY1;
    dz = PntZ2 - PntZ1;
    
    for (It.InitVisible(status);
	 It.MoreVisible();
	 It.NextVisible()) {
      It.Visible(sta,tolsta,end,tolend);
      myBiPntVis.Append
	(HLRBRep_BiPoint
	 (PntX1 + sta * dx,PntY1 + sta * dy,PntZ1 + sta * dz,
	  PntX1 + end * dx,PntY1 + end * dy,PntZ1 + end * dz,
	  Sori,reg1,regn,outl,intl));
    }
    
    for (It.InitHidden(status);
	 It.MoreHidden();
	 It.NextHidden()) {
      It.Hidden(sta,tolsta,end,tolend);
      myBiPntHid.Append
	(HLRBRep_BiPoint
	 (PntX1 + sta * dx,PntY1 + sta * dy,PntZ1 + sta * dz,
	  PntX1 + end * dx,PntY1 + end * dy,PntZ1 + end * dz,
	  Sori,reg1,regn,outl,intl));
    }
  }
}

//=======================================================================
//function : IsSame
//purpose  : 
//=======================================================================

Standard_Boolean DBRep_HideData::IsSame(const gp_Trsf& TProj,
					const Standard_Real focal) const
{
  if (focal > 0) {
    if (myFocal <= 0) return Standard_False;
    if (myFocal != focal) return Standard_False;
    const gp_XYZ& T1 = TProj .TranslationPart();
    const gp_XYZ& T2 = myTrsf.TranslationPart();

    for (Standard_Integer i = 1; i <= 3; i++) {
      if (T1.Coord(i) != T2.Coord(i))
	return Standard_False;
    }
  }
  const gp_Mat& M1 = TProj .HVectorialPart();
  const gp_Mat& M2 = myTrsf.HVectorialPart();

  for (Standard_Integer i = 1; i <= 3; i++) {

    for (Standard_Integer j = 1; j <= 3; j++) {
      if (M1.Value(i,j) != M2.Value(i,j))
	return Standard_False;
    }
  }
  return Standard_True;
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void DBRep_HideData::DrawOn(Draw_Display& D,
			    const Standard_Boolean withRg1,
			    const Standard_Boolean withRgN,
			    const Standard_Boolean withHid,
			    const Draw_Color& VisCol,
			    const Draw_Color& HidCol)
{
  Standard_Boolean firstPick = Standard_True;
  HLRBRep_ListIteratorOfListOfBPoint It;
//  Standard_Boolean reg1,regn,outl;

  if (withHid) {
    D.SetColor(HidCol);
      
    for (It.Initialize(myBiPntHid);
	 It.More();
	 It.Next()) {
      const HLRBRep_BiPoint& BP = It.Value();
      Standard_Boolean todraw = Standard_True;
      if ((!withRg1 && BP.Rg1Line() && !BP.OutLine()) ||
	  (!withRgN && BP.RgNLine() && !BP.OutLine()))
	todraw = Standard_False;
      if (todraw) {
	D.MoveTo(BP.P1());
	D.DrawTo(BP.P2());
	if (firstPick && D.HasPicked()) {
	  firstPick = Standard_False;
	  myPickShap = BP.Shape();
	}
      }
    }
  }
  D.SetColor(VisCol);

  for (It.Initialize(myBiPntVis);
       It.More();
       It.Next()) {
    const HLRBRep_BiPoint& BP = It.Value();
    Standard_Boolean todraw = Standard_True;
    if ((!withRg1 && BP.Rg1Line() && !BP.OutLine()) ||
	(!withRgN && BP.RgNLine() && !BP.OutLine()))
      todraw = Standard_False;
    if (todraw) {
      D.MoveTo(BP.P1());
      D.DrawTo(BP.P2());
      if (firstPick && D.HasPicked()) {
	firstPick = Standard_False;
	myPickShap = BP.Shape();
      }
    }
  }
}
			    
//=======================================================================
//function : LastPick
//purpose  : 
//=======================================================================

const TopoDS_Shape &  DBRep_HideData::LastPick () const
{ return myPickShap; }
