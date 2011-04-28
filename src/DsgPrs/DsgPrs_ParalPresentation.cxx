// File:	DsgPrs_ParalPresentation.cxx
// Created:	Tue Nov 28 10:14:40 1995
// Author:	Jean-Pierre COMBE
//		<jpi>


#include <DsgPrs_ParalPresentation.ixx>
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
#include <Prs3d_Text.hxx>
#include <DsgPrs.hxx>

void DsgPrs_ParalPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				    const Handle(Prs3d_Drawer)& aDrawer,
				    const TCollection_ExtendedString& aText,
				    const gp_Pnt& AttachmentPoint1,
				    const gp_Pnt& AttachmentPoint2,
				    const gp_Dir& aDirection,
				    const gp_Pnt& OffsetPoint) {


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

  Graphic3d_Array1OfVertex V(1,2);

  Quantity_Length X,Y,Z;

  PointMin.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);

  PointMax.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);
  // processing of side : 1st group
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  if (dist < (LA->Arrow1Aspect()->Length()+LA->Arrow2Aspect()->Length())) {
    outside = Standard_True;
  }
  gp_Dir arrdir = L3.Direction().Reversed();

  if (outside) {
    arrdir.Reverse();
  }
  // arrow 1 : 2nd group
  Prs3d_Arrow::Draw(aPresentation,Proj1,arrdir,
		    LA->Arrow1Aspect()->Angle(),
		    LA->Arrow1Aspect()->Length());

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  // arrow 2 : 3rd group
  Prs3d_Arrow::Draw(aPresentation,Proj2,arrdir.Reversed(),
		    LA->Arrow2Aspect()->Angle(),
		    LA->Arrow2Aspect()->Length());

  Prs3d_Root::NewGroup(aPresentation);
  
  // text : 4th group
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,offp);
  
  AttachmentPoint1.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  Proj1.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  // processing of call 1 : 5th group
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  AttachmentPoint2.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  Proj2.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);

  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  // processing of call 2 : 6th group
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
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
				    const DsgPrs_ArrowSide ArrowPrs){


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

  Graphic3d_Array1OfVertex V(1,2);

  Quantity_Length X,Y,Z;

  PointMin.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);

  PointMax.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);

  // processing of face 
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  if (dist < (LA->Arrow1Aspect()->Length()+LA->Arrow2Aspect()->Length())) {
    outside = Standard_True;
  }
  gp_Dir arrdir = L3.Direction().Reversed();

  if (outside) {
    arrdir.Reverse();
  }


  AttachmentPoint1.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  Proj1.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);

  // processing of call 1 
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  AttachmentPoint2.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  Proj2.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);

   // processing of call 2 
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  
  // text 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,offp);
  
  //arrows
  DsgPrs::ComputeSymbol(aPresentation,LA,Proj1,Proj2,arrdir,arrdir.Reversed(),ArrowPrs);


}


