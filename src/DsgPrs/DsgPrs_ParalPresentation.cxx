// Created on: 1995-11-28
// Created by: Jean-Pierre COMBE
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

#include <DsgPrs_ParalPresentation.ixx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <ElCLib.hxx>
#include <gce_MakeLin.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <TCollection_AsciiString.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Text.hxx>
#include <DsgPrs.hxx>

void DsgPrs_ParalPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				    const Handle(Prs3d_Drawer)& aDrawer,
				    const TCollection_ExtendedString& aText,
				    const gp_Pnt& AttachmentPoint1,
				    const gp_Pnt& AttachmentPoint2,
				    const gp_Dir& aDirection,
				    const gp_Pnt& OffsetPoint)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  gp_Lin L1 (AttachmentPoint1,aDirection);
  gp_Lin L2 (AttachmentPoint2,aDirection);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L1,OffsetPoint),L1);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L2,OffsetPoint),L2);
  gp_Lin L3 = gce_MakeLin(Proj1,Proj2);
  Standard_Real parmin,parmax,parcur;
  parmin = ElCLib::Parameter(L3,Proj1);
  parmax = parmin;
  parcur = ElCLib::Parameter(L3,Proj2);
  Standard_Real dist = Abs(parmin-parcur);
  if (parcur < parmin) parmin = parcur;
  if (parcur > parmax) parmax = parcur;
  parcur = ElCLib::Parameter(L3,OffsetPoint);
  gp_Pnt offp = ElCLib::Value(parcur,L3);

  Standard_Boolean outside = Standard_False;
  if (parcur < parmin) {
    parmin = parcur;
    outside = Standard_True;
  }
  if (parcur > parmax) {
    parmax = parcur;
    outside = Standard_True;
  }

  gp_Pnt PointMin = ElCLib::Value(parmin,L3);
  gp_Pnt PointMax = ElCLib::Value(parmax,L3);

  // processing of side : 1st group
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(6);
  aPrims->AddVertex(PointMin);
  aPrims->AddVertex(PointMax);

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  if (dist < (LA->Arrow1Aspect()->Length()+LA->Arrow2Aspect()->Length()))
    outside = Standard_True;
  gp_Dir arrdir = L3.Direction().Reversed();
  if (outside)
    arrdir.Reverse();

  // arrow 1 : 2nd group
  Prs3d_Arrow::Draw(aPresentation,Proj1,arrdir,LA->Arrow1Aspect()->Angle(),LA->Arrow1Aspect()->Length());

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  // arrow 2 : 3rd group
  Prs3d_Arrow::Draw(aPresentation,Proj2,arrdir.Reversed(),LA->Arrow2Aspect()->Angle(),LA->Arrow2Aspect()->Length());

  Prs3d_Root::NewGroup(aPresentation);
  
  // text : 4th group
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,offp);

  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  // processing of call 1 : 5th group
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(Proj1);
  
  // processing of call 2 : 6th group
  aPrims->AddVertex(AttachmentPoint2);
  aPrims->AddVertex(Proj2);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
}


//==========================================================================
// function : DsgPrs_ParalPresentation::Add
// purpose  : it is possible to choose the symbol of extremities of the face (arrow, point...)
//==========================================================================
void DsgPrs_ParalPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				    const Handle(Prs3d_Drawer)& aDrawer,
				    const TCollection_ExtendedString& aText,
				    const gp_Pnt& AttachmentPoint1,
				    const gp_Pnt& AttachmentPoint2,
				    const gp_Dir& aDirection,
				    const gp_Pnt& OffsetPoint,
				    const DsgPrs_ArrowSide ArrowPrs)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Lin L1 (AttachmentPoint1,aDirection);
  gp_Lin L2 (AttachmentPoint2,aDirection);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L1,OffsetPoint),L1);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L2,OffsetPoint),L2);
  gp_Lin L3 = gce_MakeLin(Proj1,Proj2);
  Standard_Real parmin,parmax,parcur;
  parmin = ElCLib::Parameter(L3,Proj1);
  parmax = parmin;
  parcur = ElCLib::Parameter(L3,Proj2);
  Standard_Real dist = Abs(parmin-parcur);
  if (parcur < parmin) parmin = parcur;
  if (parcur > parmax) parmax = parcur;
  parcur = ElCLib::Parameter(L3,OffsetPoint);
  gp_Pnt offp = ElCLib::Value(parcur,L3);

  Standard_Boolean outside = Standard_False;
  if (parcur < parmin) {
    parmin = parcur;
    outside = Standard_True;
  }
  if (parcur > parmax) {
    parmax = parcur;
    outside = Standard_True;
  }

  gp_Pnt PointMin = ElCLib::Value(parmin,L3);
  gp_Pnt PointMax = ElCLib::Value(parmax,L3);

  // processing of face 
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(6);
  aPrims->AddVertex(PointMin);
  aPrims->AddVertex(PointMax);
  
  if (dist < (LA->Arrow1Aspect()->Length()+LA->Arrow2Aspect()->Length()))
    outside = Standard_True;
  gp_Dir arrdir = L3.Direction().Reversed();
  if (outside)
    arrdir.Reverse();

  // processing of call 1 
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(Proj1);
  
  // processing of call 2 
  aPrims->AddVertex(AttachmentPoint2);
  aPrims->AddVertex(Proj2);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // text 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,offp);
  
  //arrows
  DsgPrs::ComputeSymbol(aPresentation,LA,Proj1,Proj2,arrdir,arrdir.Reversed(),ArrowPrs);
}
