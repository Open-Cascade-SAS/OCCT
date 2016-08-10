// Created on: 1996-09-18
// Created by: Jacques MINOT
// Copyright (c) 1996-1999 Matra Datavision
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

#include <DsgPrs_OffsetPresentation.hxx>

#include <ElCLib.hxx>
#include <gce_MakeLin.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPoints.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

void DsgPrs_OffsetPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const gp_Pnt& AttachmentPoint1,
				     const gp_Pnt& AttachmentPoint2,
				     const gp_Dir& aDirection,
				     const gp_Dir& aDirection2,
				     const gp_Pnt& OffsetPoint)
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
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
    Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), offp, L4.Direction(), LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());
    Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), offp, L4.Direction().Reversed(), LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());
  }
  else
  {
    if (dist < (LA->ArrowAspect()->Length()+LA->ArrowAspect()->Length()))
      outside = Standard_True;
    gp_Dir arrdir = L3.Direction().Reversed();
    if (outside)
      arrdir.Reverse();

    // fleche 1 : 2eme groupe
    Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), Proj1, arrdir, LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());

    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    
    // ball 1 : 3eme groupe
    Quantity_Color aColor = LA->LineAspect()->Aspect()->Color();
    Handle(Graphic3d_AspectMarker3d) aMarkerAsp = new Graphic3d_AspectMarker3d (Aspect_TOM_O, aColor, 1.0);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect (aMarkerAsp);
    Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
    anArrayOfPoints->AddVertex (Proj2.X(), Proj2.Y(), Proj2.Z());
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints);

    Prs3d_Root::NewGroup(aPresentation);

    // texte : 4eme groupe
    Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, offp);
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
					 const TCollection_ExtendedString& /*aText*/,
					 const gp_Pnt& AttachmentPoint1,
					 const gp_Pnt& AttachmentPoint2,
					 const gp_Dir& aDirection,
					 const gp_Dir& /*aDirection2*/,
					 const gp_Pnt& OffsetPoint)
{
  gp_Lin L1 (AttachmentPoint1,aDirection);
  gp_Pnt Proj1 = ElCLib::Value(ElCLib::Parameter(L1,OffsetPoint),L1);

  gp_Lin L2 (AttachmentPoint2,aDirection);
  gp_Pnt Proj2 = ElCLib::Value(ElCLib::Parameter(L2,OffsetPoint),L2);

  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Quantity_Color    acolor = LA->LineAspect()->Aspect()->Color();
  Aspect_TypeOfLine atype  = LA->LineAspect()->Aspect()->Type();
  Standard_Real     awidth = LA->LineAspect()->Aspect()->Width();

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
  Handle(Graphic3d_ArrayOfPoints) anArrayOfPoints = new Graphic3d_ArrayOfPoints (1);
  anArrayOfPoints->AddVertex (Proj2.X(), Proj2.Y(), Proj2.Z());

  Prs3d_Root::NewGroup(aPresentation);
  Handle(Graphic3d_AspectMarker3d) MarkerAsp = new Graphic3d_AspectMarker3d();
  MarkerAsp->SetType(Aspect_TOM_O);
  MarkerAsp->SetScale(4.);
  //MarkerAsp->SetColor(Quantity_Color(Quantity_NOC_RED));
  MarkerAsp->SetColor(acolor);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(MarkerAsp);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints);

  Prs3d_Root::NewGroup(aPresentation);
  Handle(Graphic3d_AspectMarker3d) Marker2Asp = new Graphic3d_AspectMarker3d();
  Marker2Asp->SetType(Aspect_TOM_O);
  Marker2Asp->SetScale(2.);
  //Marker2Asp->SetColor(Quantity_Color(Quantity_NOC_GREEN));
  Marker2Asp->SetColor(acolor);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(Marker2Asp);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray (anArrayOfPoints);
}
