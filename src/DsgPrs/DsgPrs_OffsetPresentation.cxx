// Created on: 1996-09-18
// Created by: Jacques MINOT
// Copyright (c) 1996-1999 Matra Datavision
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

#include <DsgPrs_OffsetPresentation.ixx>

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
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Vertex.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Aspect_AspectMarker.hxx>

#include <Quantity_Color.hxx>

#include <Prs3d_Text.hxx>
#include <Precision.hxx>


void DsgPrs_OffsetPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const gp_Pnt& AttachmentPoint1,
				     const gp_Pnt& AttachmentPoint2,
				     const gp_Dir& aDirection,
				     const gp_Dir& aDirection2,
				     const gp_Pnt& OffsetPoint)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Lin L1 (AttachmentPoint1,aDirection);
  gp_Lin L2 (AttachmentPoint2,aDirection2);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L1,OffsetPoint),L1);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L2,OffsetPoint),L2);
  gp_Lin L3,L4;
  Standard_Boolean DimNulle = Standard_False;
  if (!Proj1.IsEqual(Proj2,Precision::Confusion()*100.)) {
    L3 = gce_MakeLin(Proj1,Proj2);
  }
  else {
    //cout<<"DsgPrs_OffsetPresentation Cote nulle"<<endl;
    DimNulle = Standard_True;
    L3 = gp_Lin(Proj1,aDirection); 
    gp_Vec v4 (Proj1,OffsetPoint);
    gp_Dir d4 (v4);
    L4 = gp_Lin(Proj1,d4); // normale
  }
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

  // trait de cote : 1er groupe
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(6);
  aPrims->AddVertex(PointMin);
  aPrims->AddVertex(PointMax);

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  if (DimNulle)
  {
    Prs3d_Arrow::Draw(aPresentation,offp,L4.Direction(),LA->Arrow1Aspect()->Angle(),LA->Arrow1Aspect()->Length());
    Prs3d_Arrow::Draw(aPresentation,offp,L4.Direction().Reversed(),LA->Arrow1Aspect()->Angle(),LA->Arrow1Aspect()->Length());
  }
  else
  {
    if (dist < (LA->Arrow1Aspect()->Length()+LA->Arrow2Aspect()->Length()))
      outside = Standard_True;
    gp_Dir arrdir = L3.Direction().Reversed();
    if (outside)
      arrdir.Reverse();

    // fleche 1 : 2eme groupe
    Prs3d_Arrow::Draw(aPresentation,Proj1,arrdir,LA->Arrow1Aspect()->Angle(),LA->Arrow1Aspect()->Length());

    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    
    // ball 1 : 3eme groupe
    Handle(Graphic3d_AspectMarker3d) MarkerAsp = new Graphic3d_AspectMarker3d();
    MarkerAsp->SetType(Aspect_TOM_BALL);
    MarkerAsp->SetScale(0.8);
    Quantity_Color acolor;
    Aspect_TypeOfLine atype;
    Standard_Real awidth;
    LA->LineAspect()->Aspect()->Values(acolor, atype, awidth);
    MarkerAsp->SetColor(acolor);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(MarkerAsp);
    Graphic3d_Vertex V3d(Proj2.X() ,Proj2.Y(), Proj2.Z());
    Prs3d_Root::CurrentGroup(aPresentation)->Marker(V3d);
    
    Prs3d_Root::NewGroup(aPresentation);
    
    // texte : 4eme groupe
    Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,offp);
  }

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  // trait de rappel 1 : 5eme groupe
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(Proj1);

  // trait de rappel 2 : 6eme groupe
  aPrims->AddVertex(AttachmentPoint2);
  aPrims->AddVertex(Proj2);

  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
}

void DsgPrs_OffsetPresentation::AddAxes (const Handle(Prs3d_Presentation)& aPresentation,
					 const Handle(Prs3d_Drawer)& aDrawer,
					 const TCollection_ExtendedString& aText,
					 const gp_Pnt& AttachmentPoint1,
					 const gp_Pnt& AttachmentPoint2,
					 const gp_Dir& aDirection,
					 const gp_Dir& aDirection2,
					 const gp_Pnt& OffsetPoint)
{
  gp_Lin L1 (AttachmentPoint1,aDirection);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L1,OffsetPoint),L1);

  gp_Lin L2 (AttachmentPoint2,aDirection);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L2,OffsetPoint),L2);

  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Quantity_Color acolor;
  Aspect_TypeOfLine atype;
  Standard_Real awidth;
  LA->LineAspect()->Aspect()->Values(acolor, atype, awidth);

  Handle(Graphic3d_AspectLine3d) AxeAsp = new Graphic3d_AspectLine3d (acolor, atype, awidth);
  AxeAsp->SetType( Aspect_TOL_DOTDASH);
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(AxeAsp);

  // trait d'axe : 1er groupe
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(AttachmentPoint1);
  aPrims->AddVertex(Proj1);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  Prs3d_Root::NewGroup(aPresentation);

  Handle(Graphic3d_AspectLine3d) Axe2Asp = new Graphic3d_AspectLine3d (acolor, atype, awidth);
  Axe2Asp->SetType  ( Aspect_TOL_DOTDASH);
  Axe2Asp->SetWidth ( 4.);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(Axe2Asp);

  // trait d'axe: 2eme groupe
  aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(AttachmentPoint2);
  aPrims->AddVertex(Proj2);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // anneau : 3eme et 4eme groupes
  Graphic3d_Vertex V3d(Proj2.X() ,Proj2.Y(), Proj2.Z());

  Prs3d_Root::NewGroup(aPresentation);
  Handle(Graphic3d_AspectMarker3d) MarkerAsp = new Graphic3d_AspectMarker3d();
  MarkerAsp->SetType(Aspect_TOM_O);
  MarkerAsp->SetScale(4.);
  //MarkerAsp->SetColor(Quantity_Color(Quantity_NOC_RED));
  MarkerAsp->SetColor(acolor);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(MarkerAsp);
  Prs3d_Root::CurrentGroup(aPresentation)->Marker(V3d);

  Prs3d_Root::NewGroup(aPresentation);
  Handle(Graphic3d_AspectMarker3d) Marker2Asp = new Graphic3d_AspectMarker3d();
  Marker2Asp->SetType(Aspect_TOM_O);
  Marker2Asp->SetScale(2.);
  //Marker2Asp->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  Marker2Asp->SetColor(acolor);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(Marker2Asp);
  Prs3d_Root::CurrentGroup(aPresentation)->Marker(V3d);
}
