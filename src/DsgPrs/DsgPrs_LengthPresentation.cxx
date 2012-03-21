// Copyright (c) 1998-1999 Matra Datavision
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

#include <DsgPrs_LengthPresentation.ixx>
#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <ElCLib.hxx>
#include <gce_MakeLin.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Array1OfVertex.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <TCollection_AsciiString.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_Text.hxx>
#include <Precision.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Aspect_AspectMarker.hxx>
#include <Quantity_Color.hxx>
#include <DsgPrs.hxx>

#include <Geom_Curve.hxx>


void DsgPrs_LengthPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
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
  gp_Lin L3;
  if (!Proj1.IsEqual(Proj2,Precision::Confusion())) {
    L3 = gce_MakeLin(Proj1,Proj2);
  }
  else {
    L3 = gp_Lin(Proj1,aDirection);
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
  // face processing : 1st group
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

//==================================================================================
//function : Add
//purpose  : Adds presentation of length dimension between two planar faces
//==================================================================================

void DsgPrs_LengthPresentation::Add( const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const gp_Pnt& AttachmentPoint1,
				     const gp_Pnt& AttachmentPoint2,
				     const gp_Pln& PlaneOfFaces,
				     const gp_Dir& aDirection,
				     const gp_Pnt& OffsetPoint,
				     const DsgPrs_ArrowSide ArrowPrs ) 
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Pnt EndOfArrow1, EndOfArrow2;
  gp_Dir DirOfArrow1;
  
  DsgPrs::ComputePlanarFacesLengthPresentation( LA->Arrow1Aspect()->Length(),
					        LA->Arrow2Aspect()->Length(),
					        AttachmentPoint1,
					        AttachmentPoint2,
					        aDirection,
					        OffsetPoint,
					        PlaneOfFaces,
					        EndOfArrow1,
					        EndOfArrow2,
					        DirOfArrow1 );
  Graphic3d_Array1OfVertex VertexArray( 1, 2 );

  // Parameters for length's line
  gp_Lin LengthLine( OffsetPoint, DirOfArrow1 );
  Standard_Real Par1 = ElCLib::Parameter( LengthLine, EndOfArrow1 );
  Standard_Real Par2 = ElCLib::Parameter( LengthLine, EndOfArrow2 );
  gp_Pnt FirstPoint, LastPoint;
  if (Par1 > 0.0 && Par2 > 0.0 || Par1 < 0.0 && Par2 < 0.0)
    {
      FirstPoint = OffsetPoint;
      LastPoint  = (Abs( Par1 ) > Abs( Par2 ))? EndOfArrow1 : EndOfArrow2;
    }
  else
    {
      FirstPoint = EndOfArrow1;
      LastPoint  = EndOfArrow2;
    }

  // Creating the length's line
  VertexArray(1).SetCoord( FirstPoint.X(), FirstPoint.Y(), FirstPoint.Z() );
  VertexArray(2).SetCoord( LastPoint.X(), LastPoint.Y(), LastPoint.Z() );
  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( VertexArray );

  // Add presentation of arrows
  DsgPrs::ComputeSymbol( aPresentation, LA,
			 EndOfArrow1, EndOfArrow2, //EndOfArrow1,
			 DirOfArrow1, DirOfArrow1.Reversed(), ArrowPrs );
 
  // Drawing the text
  Prs3d_Text::Draw( aPresentation, LA->TextAspect(), aText, OffsetPoint );

  // Line from AttachmentPoint1 to end of Arrow1
  VertexArray(1).SetCoord(AttachmentPoint1.X(),
			  AttachmentPoint1.Y(),
			  AttachmentPoint1.Z());
  VertexArray(2).SetCoord( EndOfArrow1.X(), EndOfArrow1.Y(), EndOfArrow1.Z() );
  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( VertexArray );

  // Line from AttachmentPoint2 to end of Arrow2
  VertexArray(1).SetCoord(AttachmentPoint2.X(),
			  AttachmentPoint2.Y(),
			  AttachmentPoint2.Z());
  VertexArray(2).SetCoord( EndOfArrow2.X(), EndOfArrow2.Y(), EndOfArrow2.Z() );
  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( VertexArray );
}


//=========================================================================================
//function : Add
//purpose  : adds presentation of length between two edges, vertex and edge or two vertices
//=========================================================================================

void DsgPrs_LengthPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
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
  gp_Lin L3;
  if (!Proj1.IsEqual(Proj2,Precision::Confusion())) {
    L3 = gce_MakeLin(Proj1,Proj2);
  }
  else {
    L3 = gp_Lin(Proj1,aDirection);
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

  // processing of call  1 
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);
  
  AttachmentPoint2.Coord(X,Y,Z);
  V(1).SetCoord(X,Y,Z);
  Proj2.Coord(X,Y,Z);
  V(2).SetCoord(X,Y,Z);

  // processing of call 2 
  Prs3d_Root::CurrentGroup(aPresentation)->Polyline(V);

 
  // text 
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,offp);

  // symbols at the extremities of the face
  DsgPrs::ComputeSymbol(aPresentation,LA,Proj1,Proj2,arrdir,arrdir.Reversed(),ArrowPrs);
}



//==================================================================================
//function : Add
//purpose  : Adds presentation of length dimension between two curvilinear faces
//==================================================================================

void DsgPrs_LengthPresentation::Add( const Handle(Prs3d_Presentation)& aPresentation,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const TCollection_ExtendedString& aText,
				     const Handle( Geom_Surface )& SecondSurf,
				     const gp_Pnt& AttachmentPoint1,
				     const gp_Pnt& AttachmentPoint2,
				     const gp_Dir& aDirection,
				     const gp_Pnt& OffsetPoint,
				     const DsgPrs_ArrowSide ArrowPrs ) 
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  gp_Pnt EndOfArrow2;
  gp_Dir DirOfArrow1;
  Handle( Geom_Curve ) VCurve, UCurve;
  Standard_Real FirstU, deltaU = 0.0e0, FirstV, deltaV = 0.0e0;
  
  DsgPrs::ComputeCurvilinearFacesLengthPresentation( LA->Arrow1Aspect()->Length(),
						     LA->Arrow2Aspect()->Length(),
						     SecondSurf,
						     AttachmentPoint1,
						     AttachmentPoint2,
						     aDirection,
						     EndOfArrow2,
						     DirOfArrow1,
						     VCurve,
						     UCurve,
						     FirstU, deltaU,
						     FirstV, deltaV );
						       
  Graphic3d_Array1OfVertex VertexArray( 1, 2 );

  gp_Lin LengthLine( OffsetPoint, DirOfArrow1 );
  Standard_Real Par1 = ElCLib::Parameter( LengthLine, AttachmentPoint1 );
  Standard_Real Par2 = ElCLib::Parameter( LengthLine, EndOfArrow2 );
  gp_Pnt FirstPoint, LastPoint;
  if (Par1 > 0.0 && Par2 > 0.0 || Par1 < 0.0 && Par2 < 0.0)
    {
      FirstPoint = OffsetPoint;
      LastPoint  = (Abs( Par1 ) > Abs( Par2 ))? AttachmentPoint1 : EndOfArrow2;
    }
  else
    {
      FirstPoint = AttachmentPoint1;
      LastPoint  = EndOfArrow2;
    }

  // Creating the length's line
  VertexArray(1).SetCoord( FirstPoint.X(), FirstPoint.Y(), FirstPoint.Z() );
  VertexArray(2).SetCoord( LastPoint.X(), LastPoint.Y(), LastPoint.Z() );
  Prs3d_Root::CurrentGroup( aPresentation )->Polyline( VertexArray );

  // Add presentation of arrows
  DsgPrs::ComputeSymbol( aPresentation, LA, 
			 AttachmentPoint1, EndOfArrow2,
			 DirOfArrow1, DirOfArrow1.Reversed(), ArrowPrs );
 
  // Drawing the text
  Prs3d_Text::Draw( aPresentation, LA->TextAspect(), aText, OffsetPoint );
 
  // Two curves from end of Arrow2 to AttachmentPoint2
  Standard_Real Alpha, delta;
  Standard_Integer NodeNumber;
  
  Alpha  = Abs( deltaU );
  if (Alpha > Precision::Angular() && Alpha<Precision::Infinite())
    {
      NodeNumber = Max( 4 , Standard_Integer (50. * Alpha / M_PI) );
      Graphic3d_Array1OfVertex ApproxCurve( 1, NodeNumber );
      delta = deltaU / (Standard_Real)( NodeNumber - 1 );
      gp_Pnt CurPnt;
      for (Standard_Integer i = 1; i <= NodeNumber; i++)
	{
	  CurPnt =  VCurve->Value( FirstU );
	  ApproxCurve(i).SetCoord( CurPnt.X(), CurPnt.Y(), CurPnt.Z() );
	  FirstU += delta;
	}
      Prs3d_Root::CurrentGroup( aPresentation )->Polyline( ApproxCurve );
    }
  Alpha  = Abs( deltaV );
  if (Alpha > Precision::Angular() && Alpha<Precision::Infinite())
    {
      NodeNumber = Max( 4 , Standard_Integer (50. * Alpha / M_PI) );
      Graphic3d_Array1OfVertex ApproxCurve( 1, NodeNumber );
      delta = deltaV / (Standard_Real)( NodeNumber - 1 );
      gp_Pnt CurPnt;
      for (Standard_Integer i = 1; i <= NodeNumber; i++)
	{
	  CurPnt =  UCurve->Value( FirstV );
	  ApproxCurve(i).SetCoord( CurPnt.X(), CurPnt.Y(), CurPnt.Z() );
	  FirstV += delta;
	}
      Prs3d_Root::CurrentGroup( aPresentation )->Polyline( ApproxCurve );
    }
}






//================================
// Function:
// Purpose: Rob 26-mar-96
//=================================

void DsgPrs_LengthPresentation::Add (const Handle(Prs3d_Presentation)& aPrs,
				     const Handle(Prs3d_Drawer)& aDrawer,
				     const gp_Pnt& Pt1,
				     const gp_Pnt& Pt2,
				     const DsgPrs_ArrowSide ArrowPrs) 
{
  Prs3d_Root::CurrentGroup(aPrs)->BeginPrimitives();
  Graphic3d_Array1OfVertex Vx(1,2);
  Vx(1).SetCoord(Pt1.X(),Pt1.Y(),Pt1.Z());
  Vx(2).SetCoord(Pt2.X(),Pt2.Y(),Pt2.Z());
  Prs3d_Root::CurrentGroup(aPrs)->Polyline(Vx);
  
  gp_Vec V ;
  switch(ArrowPrs){
  case DsgPrs_AS_NONE:
    break;
  case DsgPrs_AS_LASTAR:
    Prs3d_Arrow::Draw(aPrs,Pt2,gp_Dir(gp_Vec(Pt1,Pt2)), 
		      aDrawer->LengthAspect()->Arrow1Aspect()->Angle(),
		      aDrawer->LengthAspect()->Arrow1Aspect()->Length());
    break;
  case DsgPrs_AS_FIRSTAR:
    Prs3d_Arrow::Draw(aPrs,Pt1,gp_Dir(gp_Vec(Pt2,Pt1)), 
		      aDrawer->LengthAspect()->Arrow1Aspect()->Angle(),
		      aDrawer->LengthAspect()->Arrow1Aspect()->Length());
    break;
  case DsgPrs_AS_BOTHAR:
    V = gp_Vec(Pt1,Pt2);
    Prs3d_Arrow::Draw(aPrs,Pt2,gp_Dir(V), 
		      aDrawer->LengthAspect()->Arrow1Aspect()->Angle(),
		      aDrawer->LengthAspect()->Arrow1Aspect()->Length());
    Prs3d_Arrow::Draw(aPrs,Pt1,gp_Dir(V.Reversed()), 
		      aDrawer->LengthAspect()->Arrow1Aspect()->Angle(),
		      aDrawer->LengthAspect()->Arrow1Aspect()->Length());
    break;
#ifndef DEB
  default:
    break;
#endif
  }
  Prs3d_Root::CurrentGroup(aPrs)->EndPrimitives();
  
}
