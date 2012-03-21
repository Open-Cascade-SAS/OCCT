// Created on: 1995-09-19
// Created by: Laurent PAINNOT
// Copyright (c) 1995-1999 Matra Datavision
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



#include <StdPrs_HLRPolyShape.ixx>

#include <StdPrs_WFShape.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Graphic3d_ArrayOfPrimitives.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Prs3d_LineAspect.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <HLRBRep_ListOfBPoint.hxx>
#include <HLRBRep_BiPoint.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRAlgo_EdgeIterator.hxx>
#include <HLRBRep_ListIteratorOfListOfBPoint.hxx>
#include <TColgp_SequenceOfPnt.hxx>

#define PntX1 ((Standard_Real*)Coordinates)[0]
#define PntY1 ((Standard_Real*)Coordinates)[1]
#define PntZ1 ((Standard_Real*)Coordinates)[2]
#define PntX2 ((Standard_Real*)Coordinates)[3]
#define PntY2 ((Standard_Real*)Coordinates)[4]
#define PntZ2 ((Standard_Real*)Coordinates)[5]

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void StdPrs_HLRPolyShape::Add(const Handle (Prs3d_Presentation)& aPresentation,
			      const TopoDS_Shape&                 aShape,
			      const Handle (Prs3d_Drawer)&        aDrawer,
			      const Handle (Prs3d_Projector)&     aProjector)
{
  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup(aPresentation);

  TopExp_Explorer ex;

  Standard_Boolean isPrimArrayEnabled = Graphic3d_ArrayOfPrimitives::IsEnable();

  // find vertices not under ancestors.
  TopAbs_ShapeEnum E = aShape.ShapeType();
  if (E == TopAbs_COMPOUND) {
    // it is necessary to present isolated vertexes instead of hiding them.
    for (ex.Init(aShape, TopAbs_VERTEX, TopAbs_EDGE); ex.More(); ex.Next()) {
      StdPrs_WFShape::Add(aPresentation, ex.Current(), aDrawer);
    }
  }

  Graphic3d_Array1OfVertex Vertex(1,2);
  TColgp_SequenceOfPnt HiddenPnts;
  TColgp_SequenceOfPnt SeenPnts;

  Standard_Boolean rel = aDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE;
  Standard_Real def;
  if (rel) def = aDrawer->HLRDeviationCoefficient();    
  else     def = aDrawer->MaximalChordialDeviation();
  BRepMesh_IncrementalMesh mesh(aShape, def, rel, aDrawer->HLRAngle());
  
  Handle(HLRBRep_PolyAlgo) hider = new HLRBRep_PolyAlgo(aShape);

  hider->Projector(aProjector->Projector());
  hider->Angle(aDrawer->HLRAngle());
  hider->Update();
  //Standard_Integer i;
  Standard_Real sta,end,dx,dy,dz;
  Standard_ShortReal tolsta, tolend;
  //gp_Pnt PSta, PEnd;
  HLRAlgo_EdgeStatus status;
  HLRAlgo_EdgeIterator It;
  Standard_Boolean reg1,regn,outl, intl;
  Standard_Address Coordinates;
  TopoDS_Shape S;

  HLRBRep_ListOfBPoint BiPntVis, BiPntHid;
  
  for (hider->InitHide(); hider->MoreHide(); hider->NextHide()) {
    hider->Hide(Coordinates, status, S, reg1, regn, outl, intl);
    
    dx = PntX2 - PntX1;
    dy = PntY2 - PntY1;
    dz = PntZ2 - PntZ1;
    
    for (It.InitVisible(status); It.MoreVisible(); It.NextVisible()) {
      It.Visible(sta,tolsta,end,tolend);
      BiPntVis.Append
	(HLRBRep_BiPoint
	 (PntX1 + sta * dx,PntY1 + sta * dy,PntZ1 + sta * dz,
	  PntX1 + end * dx,PntY1 + end * dy,PntZ1 + end * dz,
	  S,reg1,regn,outl,intl));
    }
    
    for (It.InitHidden(status); It.MoreHidden(); It.NextHidden()) {
      It.Hidden(sta,tolsta,end,tolend);
      BiPntHid.Append
	(HLRBRep_BiPoint
	 (PntX1 + sta * dx,PntY1 + sta * dy,PntZ1 + sta * dz,
	  PntX1 + end * dx,PntY1 + end * dy,PntZ1 + end * dz,
	  S,reg1,regn,outl,intl));	
    }
  }
  
  
  // storage in the group:
  
  HLRBRep_ListIteratorOfListOfBPoint ItB;

  if (aDrawer->DrawHiddenLine()) {
    if(!isPrimArrayEnabled) {
      aGroup->SetPrimitivesAspect(aDrawer->HiddenLineAspect()->Aspect());
      aGroup->BeginPrimitives();
    }
    for (ItB.Initialize(BiPntHid); ItB.More(); ItB.Next()) {
      const HLRBRep_BiPoint& BP = ItB.Value();
      if (!BP.RgNLine() || BP.OutLine()) {
	const gp_Pnt& P1 = BP.P1();
	const gp_Pnt& P2 = BP.P2();
	HiddenPnts.Append(P1);
	HiddenPnts.Append(P2);
	Vertex(1).SetCoord(P1.X(), P1.Y(), P1.Z());
	Vertex(2).SetCoord(P2.X(), P2.Y(), P2.Z());
	if(!isPrimArrayEnabled)
	  aGroup->Polyline(Vertex);
      }
    }
    if(!isPrimArrayEnabled)
      aGroup->EndPrimitives();
  }

  if(!isPrimArrayEnabled) {
    aGroup->SetPrimitivesAspect(aDrawer->SeenLineAspect()->Aspect());
    aGroup->BeginPrimitives();
  }
  for (ItB.Initialize(BiPntVis); ItB.More(); ItB.Next()) {
    const HLRBRep_BiPoint& BP = ItB.Value();
    if (!BP.RgNLine() || BP.OutLine()) {
      const gp_Pnt& P1 = BP.P1();
      const gp_Pnt& P2 = BP.P2();
      SeenPnts.Append(P1);
      SeenPnts.Append(P2);
      Vertex(1).SetCoord(P1.X(), P1.Y(), P1.Z());
      Vertex(2).SetCoord(P2.X(), P2.Y(), P2.Z());
      if(!isPrimArrayEnabled)
	aGroup->Polyline(Vertex);
    }
  }
  if(!isPrimArrayEnabled)
    aGroup->EndPrimitives();
  if(isPrimArrayEnabled) {
    Standard_Integer nbVertices = HiddenPnts.Length();
    if(nbVertices > 0) {
      Handle(Graphic3d_ArrayOfPolylines) HiddenArray = new Graphic3d_ArrayOfPolylines(nbVertices, (Standard_Integer)nbVertices/2);
      for(int i=1; i<=nbVertices; i+=2) {
	HiddenArray->AddBound(2);
	HiddenArray->AddVertex(HiddenPnts.Value(i));
	HiddenArray->AddVertex(HiddenPnts.Value(i+1));
      }
      aGroup->SetPrimitivesAspect(aDrawer->HiddenLineAspect()->Aspect());
      aGroup->BeginPrimitives();
      aGroup->AddPrimitiveArray(HiddenArray);
      aGroup->EndPrimitives();
    }
    nbVertices = SeenPnts.Length();
    if(nbVertices > 0) {
      Handle(Graphic3d_ArrayOfPolylines) SeenArray = new Graphic3d_ArrayOfPolylines(nbVertices, (Standard_Integer)nbVertices/2);
      for(int i=1; i<=nbVertices; i+=2) {
	SeenArray->AddBound(2);
	SeenArray->AddVertex(SeenPnts.Value(i));
	SeenArray->AddVertex(SeenPnts.Value(i+1));
      }
      aGroup->SetPrimitivesAspect(aDrawer->SeenLineAspect()->Aspect());
      aGroup->BeginPrimitives();
      aGroup->AddPrimitiveArray(SeenArray);
      aGroup->EndPrimitives();
    }
  }
}   

