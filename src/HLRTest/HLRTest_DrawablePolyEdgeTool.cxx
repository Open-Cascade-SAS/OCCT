// Created on: 1992-10-14
// Created by: Christophe MARION
// Copyright (c) 1992-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.

// Modified by cma, Mon Oct 23 16:11:46 1995

#include <HLRTest_DrawablePolyEdgeTool.ixx>
#include <HLRBRep_BiPoint.hxx>
#include <HLRBRep_ListIteratorOfListOfBPoint.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRAlgo_EdgeIterator.hxx>
#include <Draw_Color.hxx>
#include <OSD_Chronometer.hxx>

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
    myAlgo->Hide(Coordinates,status,S,reg1,regn,outl,intl);
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
	myAlgo->Show(Coordinates,S,reg1,regn,outl,intl);
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

