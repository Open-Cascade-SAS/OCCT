// Created on: 1993-10-11
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

// Modified by cma, Fri Nov 10 17:36:13 1995

#include <BRep_Builder.hxx>
#include <BRepLib_MakeEdge2d.hxx>
#include <HLRAlgo_EdgeIterator.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRBRep_BiPnt2D.hxx>
#include <HLRBRep_ListIteratorOfListOfBPnt2D.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <HLRBRep_PolyHLRToShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : HLRBRep_PolyHLRToShape
//purpose  : 
//=======================================================================

HLRBRep_PolyHLRToShape::HLRBRep_PolyHLRToShape ()
{}

//=======================================================================
//function : Update
//purpose  : 
//=======================================================================

void HLRBRep_PolyHLRToShape::Update (const Handle(HLRBRep_PolyAlgo)& A)
{
  myAlgo = A;
  myHideMode = Standard_True;
  Standard_Real sta,end;
  Standard_ShortReal tolsta,tolend;
  HLRAlgo_EdgeIterator It;
  myBiPntVis.Clear();
  myBiPntHid.Clear();
  TopoDS_Shape S;
  Standard_Boolean reg1,regn,outl,intl;
  const gp_Trsf& T = myAlgo->Projector().Transformation();
  HLRAlgo_EdgeStatus status;

  for (myAlgo->InitHide(); myAlgo->MoreHide(); myAlgo->NextHide()) {
    HLRAlgo_BiPoint::PointsT& aPoints = myAlgo->Hide(status,S,reg1,regn,outl,intl);
    gp_XYZ aSta = aPoints.Pnt1;
    gp_XYZ aEnd = aPoints.Pnt2;
    T.Transforms(aSta);
    T.Transforms(aEnd);
    const gp_XY aSta2D(aSta.X(), aSta.Y());
    const gp_XY aEnd2D(aEnd.X(), aEnd.Y());
    const gp_XY aD = aEnd2D - aSta2D;
    if (aD.Modulus() > 1.e-10) {
    
      for (It.InitVisible(status);
	   It.MoreVisible();
	   It.NextVisible()) {
	It.Visible(sta,tolsta,end,tolend);
	myBiPntVis.Append
	  (HLRBRep_BiPnt2D
	   (aSta2D + sta * aD, aSta2D + end * aD,
	    S,reg1,regn,outl,intl));
      }
      
      for (It.InitHidden(status);
	   It.MoreHidden();
	   It.NextHidden()) {
	It.Hidden(sta,tolsta,end,tolend);
	myBiPntHid.Append
	  (HLRBRep_BiPnt2D
	   (aSta2D + sta * aD, aSta2D + end * aD,
	    S,reg1,regn,outl,intl));
      }
    }
  }
}

//=======================================================================
//function : InternalCompound
//purpose  : 
//=======================================================================

TopoDS_Shape 
HLRBRep_PolyHLRToShape::InternalCompound (const Standard_Integer typ,
					  const Standard_Boolean visible,
					  const TopoDS_Shape& S)
{
  TopTools_MapOfShape Map;
  if (!S.IsNull()) {
    TopExp_Explorer ex;
    for (ex.Init(S,TopAbs_EDGE); ex.More(); ex.Next())
      Map.Add(ex.Current());
    for (ex.Init(S,TopAbs_FACE); ex.More(); ex.Next())
      Map.Add(ex.Current());
  }
  Standard_Boolean todraw,reg1,regn,outl,intl;
  Standard_Boolean added = Standard_False;
  TopoDS_Shape Result;
  BRep_Builder B;
  B.MakeCompound(TopoDS::Compound(Result));

  if (myHideMode) {
    HLRBRep_ListIteratorOfListOfBPnt2D It;
    if (visible) It.Initialize(myBiPntVis);
    else         It.Initialize(myBiPntHid);
    
    for (; It.More(); It.Next()) {
      const HLRBRep_BiPnt2D& BP = It.Value();
      reg1 = BP.Rg1Line();
      regn = BP.RgNLine();
      outl = BP.OutLine();
      intl = BP.IntLine();
      if      (typ == 1) todraw =  intl;
      else if (typ == 2) todraw =  reg1 && !regn && !outl;
      else if (typ == 3) todraw =  regn && !outl;
      else               todraw = !(intl || (reg1 && !outl));
      if (todraw)
	if (!S.IsNull()) todraw = Map.Contains(BP.Shape());
      if (todraw) {
        const gp_Pnt2d& FirstP2d = BP.P1();
        const gp_Pnt2d& LastP2d  = BP.P2();
        if (FirstP2d.SquareDistance(LastP2d) > 1.e-20)
        {
          B.Add(Result,BRepLib_MakeEdge2d(BP.P1(),BP.P2()));
          added = Standard_True;
        }
      }
    }
  }
  else {
    const gp_Trsf& T = myAlgo->Projector().Transformation();
    TopoDS_Shape SBP;

    for (myAlgo->InitShow(); myAlgo->MoreShow(); myAlgo->NextShow()) {
      HLRAlgo_BiPoint::PointsT& aPoints = myAlgo->Show(SBP,reg1,regn,outl,intl);
      if      (typ == 1) todraw =  intl;
      else if (typ == 2) todraw =  reg1 && !regn && !outl;
      else if (typ == 3) todraw =  regn && !outl;
      else               todraw = !(intl || (reg1 && !outl));
      if (todraw)
	if (!S.IsNull()) todraw = Map.Contains(SBP);
      if (todraw) {
  gp_XYZ aSta = aPoints.Pnt1, aEnd = aPoints.Pnt2;
	T.Transforms(aSta);
	T.Transforms(aEnd);
  const gp_XY aSta2D(aSta.X(), aSta.Y());
  const gp_XY aEnd2D(aEnd.X(), aEnd.Y());
  const gp_XY aD = aEnd2D - aSta2D;
	if (aD.SquareModulus() > 1.e-20) {
	  B.Add(Result,BRepLib_MakeEdge2d(aSta2D, aEnd2D));
	  added = Standard_True;
	}
      }
    }
  }
  if (!added) Result = TopoDS_Shape();
  return Result;
}
