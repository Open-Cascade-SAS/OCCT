// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
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

// Modified by cma, Mon Oct 23 16:11:46 1995

#include <Draw_Color.hxx>
#include <Draw_Display.hxx>
#include <HLRAlgo_EdgeIterator.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRBRep_BiPoint.hxx>
#include <HLRBRep_ListIteratorOfListOfBPoint.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <HLRTest_DrawablePolyEdgeTool.hxx>
#include <OSD_Chronometer.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(HLRTest_DrawablePolyEdgeTool,Draw_Drawable3D)

#define PntX1 ((Standard_Real*)Coordinates)[0]
#define PntY1 ((Standard_Real*)Coordinates)[1]
#define PntZ1 ((Standard_Real*)Coordinates)[2]
#define PntX2 ((Standard_Real*)Coordinates)[3]
#define PntY2 ((Standard_Real*)Coordinates)[4]
#define PntZ2 ((Standard_Real*)Coordinates)[5]

//=======================================================================
//function : HLRTest_DrawablePolyEdgeTool
//purpose  : 
//=======================================================================

HLRTest_DrawablePolyEdgeTool::
HLRTest_DrawablePolyEdgeTool (const Handle(HLRBRep_PolyAlgo)& Alg,
			      const Standard_Integer ViewId,
			      const Standard_Boolean Debug) :
  myAlgo(Alg),
  myDispHid(Standard_False),
  myViewId(ViewId),
  myDebug(Debug),
  myHideMode(Standard_True)
{
  OSD_Chronometer ChronHide;
  if (myDebug) {
    ChronHide.Reset();
    ChronHide.Start();
  }
  Standard_Real sta,end,dx,dy,dz;
  Standard_ShortReal tolsta,tolend;
  HLRAlgo_EdgeIterator It;
  myBiPntVis.Clear();
  myBiPntHid.Clear();
  Standard_Address Coordinates;
  HLRAlgo_EdgeStatus status;
  TopoDS_Shape S;
  Standard_Boolean reg1,regn,outl,intl;

  for (myAlgo->InitHide(); myAlgo->MoreHide(); myAlgo->NextHide()) {
    Coordinates = &myAlgo->Hide(status,S,reg1,regn,outl,intl);
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
	  S,reg1,regn,outl,intl));
    }
    
    for (It.InitHidden(status);
	 It.MoreHidden();
	 It.NextHidden()) {
      It.Hidden(sta,tolsta,end,tolend);
      myBiPntHid.Append
	(HLRBRep_BiPoint
	 (PntX1 + sta * dx,PntY1 + sta * dy,PntZ1 + sta * dz,
	  PntX1 + end * dx,PntY1 + end * dy,PntZ1 + end * dz,
	  S,reg1,regn,outl,intl));
    }
  }
  if (myDebug) {
    ChronHide.Stop();
    cout << " Temps Hide   :";
    ChronHide.Show(); 
  }
}

//=======================================================================
//function : DrawOn
//purpose  : 
//=======================================================================

void HLRTest_DrawablePolyEdgeTool::DrawOn (Draw_Display& D) const
{
  if (myViewId == D.ViewId()) {
    if (myHideMode) {
      HLRBRep_ListIteratorOfListOfBPoint It;
      if (myDispHid) {
	D.SetColor(Draw_bleu);
	
	for (It.Initialize(myBiPntHid);
	     It.More();
	     It.Next()) {
	  const HLRBRep_BiPoint& BP = It.Value();
	  Standard_Boolean todraw = Standard_True;
	  if ((!myDispRg1 && BP.Rg1Line() && !BP.OutLine()) ||
	      (!myDispRgN && BP.RgNLine() && !BP.OutLine()))
	    todraw =  Standard_False;
	  if (todraw) {
	    D.MoveTo(BP.P1());
	    D.DrawTo(BP.P2());
	  }
	}
      }
      D.SetColor(Draw_vert);
      
      for (It.Initialize(myBiPntVis);
	   It.More();
	   It.Next()) {
	const HLRBRep_BiPoint& BP = It.Value();
	Standard_Boolean todraw = Standard_True;
	if ((!myDispRg1 && BP.Rg1Line() && !BP.OutLine()) ||
	    (!myDispRgN && BP.RgNLine() && !BP.OutLine()))
	  todraw =  Standard_False;
	if (todraw) {
	  D.MoveTo(BP.P1());
	  D.DrawTo(BP.P2());
	}
      }
    }
    else {
      Standard_Address Coordinates;
      TopoDS_Shape S;
      Standard_Boolean reg1,regn,outl,intl;
      D.SetColor(Draw_vert);

      for (myAlgo->InitShow(); myAlgo->MoreShow(); myAlgo->NextShow()) {
	Coordinates = &myAlgo->Show(S,reg1,regn,outl,intl);
	Standard_Boolean todraw = Standard_True;
	if ((!myDispRg1 && reg1 && !outl) ||
	    (!myDispRgN && regn && !outl))
	  todraw =  Standard_False;
	if (todraw) {
	  D.MoveTo(gp_Pnt(PntX1,PntY1,PntZ1));
	  D.DrawTo(gp_Pnt(PntX2,PntY2,PntZ2));
	}
      }
    }
  }
}

