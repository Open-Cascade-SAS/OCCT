// Copyright: 	Matra-Datavision 1996
// File:	DsgPrs_OffsetPresentation.cxx
// Created:	Wed Sep 18 17:29:20 1996
// Author:	Jacques MINOT
//		<jmi>

#include <DsgPrs_OffsetPresentation.ixx>

#include <gp_Lin.hxx>
#include <gp_Dir.hxx>

#include <ElCLib.hxx>

#include <gce_MakeLin.hxx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>

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
				     const gp_Pnt& OffsetPoint) {


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

  Graphic3d_Array1OfVertex V(1,2);

  Quantity_Length X,Y,Z;

  PointMin.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);

  PointMax.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  // trait de cote : 1er groupe
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);


  if (DimNulle) {

    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Arrow::Draw(aPresentation,offp,L4.Direction(),
		      LA->Arrow1Aspect()->Angle(),
		      LA->Arrow1Aspect()->Length());

    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    Prs3d_Arrow::Draw(aPresentation,offp,L4.Direction().Reversed(),
		      LA->Arrow1Aspect()->Angle(),
		      LA->Arrow1Aspect()->Length());
  } else {
    Prs3d_Root::NewGroup(aPresentation);
    Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
    
    if (dist < (LA->Arrow1Aspect()->Length()+LA->Arrow2Aspect()->Length())) {
      outside = Standard_True;
    }
    gp_Dir arrdir = L3.Direction().Reversed();
    
    if (outside) {
      arrdir.Reverse();
    }
    // fleche 1 : 2eme groupe
    Prs3d_Arrow::Draw(aPresentation,Proj1,arrdir,
		      LA->Arrow1Aspect()->Angle(),
		      LA->Arrow1Aspect()->Length());
    
    
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

  AttachmentPoint1.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  Proj1.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  // trait de rappel 1 : 5eme groupe
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  AttachmentPoint2.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  Proj2.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  // trait de rappel 2 : 6eme groupe
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
}

void DsgPrs_OffsetPresentation::AddAxes (const Handle(Prs3d_Presentation)& aPresentation,
					 const Handle(Prs3d_Drawer)& aDrawer,
					 const TCollection_ExtendedString& aText,
					 const gp_Pnt& AttachmentPoint1,
					 const gp_Pnt& AttachmentPoint2,
					 const gp_Dir& aDirection,
					 const gp_Dir& aDirection2,
					 const gp_Pnt& OffsetPoint) {



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

  Graphic3d_Array1OfVertex V(1,2);
  Quantity_Length X,Y,Z;

  AttachmentPoint1.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);

  Proj1.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  // trait d'axe : 1er groupe
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  Prs3d_Root::NewGroup(aPresentation);
  Handle(Graphic3d_AspectLine3d) Axe2Asp = new Graphic3d_AspectLine3d (acolor, atype, awidth);
  Axe2Asp->SetType  ( Aspect_TOL_DOTDASH);
  Axe2Asp->SetWidth ( 4.);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(Axe2Asp);

  AttachmentPoint2.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);

  Proj2.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  // trait d'axe: 2eme groupe
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

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
