// Created on: 1995-09-19
// Created by: Laurent PAINNOT
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

#include <StdPrs_HLRPolyShape.hxx>

#include <BRepMesh_IncrementalMesh.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <HLRAlgo_EdgeIterator.hxx>
#include <HLRAlgo_EdgeStatus.hxx>
#include <HLRBRep_BiPoint.hxx>
#include <HLRBRep_ListIteratorOfListOfBPoint.hxx>
#include <HLRBRep_ListOfBPoint.hxx>
#include <HLRBRep_PolyAlgo.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Projector.hxx>
#include <StdPrs_WFShape.hxx>
#include <TColgp_SequenceOfPnt.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS_Shape.hxx>

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

  // find vertices not under ancestors.
  TopAbs_ShapeEnum E = aShape.ShapeType();
  if (E == TopAbs_COMPOUND) {
    // it is necessary to present isolated vertexes instead of hiding them.
    for (ex.Init(aShape, TopAbs_VERTEX, TopAbs_EDGE); ex.More(); ex.Next()) {
      StdPrs_WFShape::Add(aPresentation, ex.Current(), aDrawer);
    }
  }

  if (aDrawer->IsAutoTriangulation())
  {
    const Standard_Boolean aRel = aDrawer->TypeOfDeflection() == Aspect_TOD_RELATIVE;
    Standard_Real aDef = aRel ? aDrawer->HLRDeviationCoefficient() : aDrawer->MaximalChordialDeviation();
    BRepMesh_FastDiscret::Parameters aMeshParams;
    aMeshParams.Relative = aRel;
    aMeshParams.Angle = aDrawer->HLRAngle();
    aMeshParams.Deflection = aDef;
    BRepMesh_IncrementalMesh mesh(aShape, aMeshParams);
  }
  
  Handle(HLRBRep_PolyAlgo) hider = new HLRBRep_PolyAlgo(aShape);

  hider->Projector(aProjector->Projector());
  hider->Angle(aDrawer->HLRAngle());
  hider->Update();
  Standard_Real sta,end,dx,dy,dz;
  Standard_ShortReal tolsta, tolend;
  HLRAlgo_EdgeStatus status;
  HLRAlgo_EdgeIterator It;
  Standard_Boolean reg1,regn,outl, intl;
  Standard_Address Coordinates;
  TopoDS_Shape S;

  HLRBRep_ListOfBPoint BiPntVis, BiPntHid;
  
  for (hider->InitHide(); hider->MoreHide(); hider->NextHide())
  {
    Coordinates = &hider->Hide(status, S, reg1, regn, outl, intl);
    
    dx = PntX2 - PntX1;
    dy = PntY2 - PntY1;
    dz = PntZ2 - PntZ1;
    
    for (It.InitVisible(status); It.MoreVisible(); It.NextVisible())
    {
      It.Visible(sta,tolsta,end,tolend);
      BiPntVis.Append
        (HLRBRep_BiPoint
           (PntX1 + sta * dx,PntY1 + sta * dy,PntZ1 + sta * dz,
            PntX1 + end * dx,PntY1 + end * dy,PntZ1 + end * dz,
            S,reg1,regn,outl,intl));
    }
    
    for (It.InitHidden(status); It.MoreHidden(); It.NextHidden())
    {
      It.Hidden(sta,tolsta,end,tolend);
      BiPntHid.Append
        (HLRBRep_BiPoint
           (PntX1 + sta * dx,PntY1 + sta * dy,PntZ1 + sta * dz,
            PntX1 + end * dx,PntY1 + end * dy,PntZ1 + end * dz,
            S,reg1,regn,outl,intl));	
    }
  }

  // storage in the group
  if (aDrawer->DrawHiddenLine())
  {
    Standard_Integer aNbHiddenSegments = 0;
    for (HLRBRep_ListIteratorOfListOfBPoint aBPntHidIter (BiPntHid); aBPntHidIter.More(); aBPntHidIter.Next())
    {
      const HLRBRep_BiPoint& aBPnt = aBPntHidIter.Value();
      if (!aBPnt.RgNLine()
        || aBPnt.OutLine())
      {
        ++aNbHiddenSegments;
      }
    }
    if (aNbHiddenSegments > 0)
    {
      Handle(Graphic3d_ArrayOfSegments) aHiddenArray = new Graphic3d_ArrayOfSegments (aNbHiddenSegments * 2);
      for (HLRBRep_ListIteratorOfListOfBPoint aBPntHidIter (BiPntHid); aBPntHidIter.More(); aBPntHidIter.Next())
      {
        const HLRBRep_BiPoint& aBPnt = aBPntHidIter.Value();
        if (!aBPnt.RgNLine()
          || aBPnt.OutLine())
        {
          aHiddenArray->AddVertex (aBPnt.P1());
          aHiddenArray->AddVertex (aBPnt.P2());
        }
      }

      aGroup->SetPrimitivesAspect (aDrawer->HiddenLineAspect()->Aspect());
      aGroup->AddPrimitiveArray (aHiddenArray);
    }
  }
  {
    Standard_Integer aNbSeenSegments = 0;
    for (HLRBRep_ListIteratorOfListOfBPoint aBPntVisIter (BiPntVis); aBPntVisIter.More(); aBPntVisIter.Next())
    {
      const HLRBRep_BiPoint& aBPnt = aBPntVisIter.Value();
      if (!aBPnt.RgNLine()
        || aBPnt.OutLine())
      {
        ++aNbSeenSegments;
      }
    }
    if (aNbSeenSegments > 0)
    {
      Handle(Graphic3d_ArrayOfSegments) aSeenArray = new Graphic3d_ArrayOfSegments (aNbSeenSegments * 2);
      for (HLRBRep_ListIteratorOfListOfBPoint aBPntVisIter (BiPntVis); aBPntVisIter.More(); aBPntVisIter.Next())
      {
        const HLRBRep_BiPoint& aBPnt = aBPntVisIter.Value();
        if (!aBPnt.RgNLine()
          || aBPnt.OutLine())
        {
          aSeenArray->AddVertex (aBPnt.P1());
          aSeenArray->AddVertex (aBPnt.P2());
        }
      }
      aGroup->SetPrimitivesAspect (aDrawer->SeenLineAspect()->Aspect());
      aGroup->AddPrimitiveArray (aSeenArray);
    }
  }
}   
