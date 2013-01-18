// Created on: 1998-01-27
// Created by: Julia GERASIMOVA
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

#include <DsgPrs_EqualDistancePresentation.ixx>

#include <DsgPrs.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Prs3d_Root.hxx>
#include <gp_Dir.hxx>
#include <gce_MakeDir.hxx>
#include <gp_Vec.hxx>
#include <gp_Pln.hxx>
#include <Precision.hxx>
#include <ElCLib.hxx>
#include <gp_Circ.hxx>
#include <TCollection_ExtendedString.hxx>
#include <Prs3d_Text.hxx>

//=================================================================================
//function  : Add
//=================================================================================
void DsgPrs_EqualDistancePresentation::Add( const Handle( Prs3d_Presentation )& aPresentation,
					    const Handle( Prs3d_Drawer )& aDrawer,
					    const gp_Pnt& Point1,
					    const gp_Pnt& Point2,
					    const gp_Pnt& Point3,
					    const gp_Pnt& Point4,
					    const Handle( Geom_Plane )& Plane )
{
  Handle( Prs3d_LengthAspect ) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup( aPresentation )->SetPrimitivesAspect( LA->LineAspect()->Aspect() );

  // Line between two middles
  gp_Pnt Middle12( (Point1.XYZ() + Point2.XYZ()) * 0.5 ), Middle34( (Point3.XYZ() + Point4.XYZ()) * 0.5 );

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(Middle12);
  aPrims->AddVertex(Middle34);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // Add presentation of arrows (points)
  gp_Dir aDir( 0, 0, 1 );
  DsgPrs::ComputeSymbol(aPresentation, LA, Middle12, Middle34, aDir, aDir, DsgPrs_AS_BOTHPT );
// ota -- begin --  
  // Two small lines in the middle of this line
  gp_Pnt Middle( (Middle12.XYZ() + Middle34.XYZ()) * 0.5 ), aTextPos;
  Standard_Real Dist = Middle12.Distance( Middle34 );
  Standard_Real SmallDist;
  gp_Dir LineDir, OrtDir;
  gp_Vec LineVec, OrtVec;

  if (Dist > Precision::Confusion())
  {
    SmallDist = Dist * 0.05; // 1/20.0 part
    if (SmallDist <= Precision::Confusion())
      SmallDist = Dist;
    LineDir = gce_MakeDir( Middle12, Middle34 );
    OrtDir  = Plane->Pln().Axis().Direction() ^ LineDir;
    LineVec = gp_Vec( LineDir ) * SmallDist;
    OrtVec  = gp_Vec( OrtDir ) * SmallDist;

    aTextPos = Middle.Translated( OrtVec );
  }
  else
  {
    gp_Vec Vec1( Middle, Point1 );

    if (Vec1.SquareMagnitude() > Precision::Confusion()*Precision::Confusion())
	{
	  Standard_Real Angle = gp_Vec( Middle, Point1 ).Angle( gp_Vec( Middle, Point3 ) );
	  gp_Pnt MidPnt = Point1.Rotated( Plane->Pln().Axis(), Angle*0.5 );
	  OrtDir  = gce_MakeDir( Middle, MidPnt );
	  LineDir = OrtDir ^ Plane->Pln().Axis().Direction();
	  
	  Standard_Real Distance = Point1.Distance( Point2 );
	  SmallDist = Distance * 0.05; // 1/20.0
	  if (SmallDist <= Precision::Confusion())
	    SmallDist = Distance;
	  
	  OrtVec = gp_Vec( OrtDir ) * SmallDist;
	  LineVec = gp_Vec( LineDir ) * SmallDist;
	}
    else
	{
	  SmallDist = 5.0;
	  OrtVec = gp_Vec( Plane->Pln().XAxis().Direction() ) * SmallDist;
	  LineVec = gp_Vec( Plane->Pln().YAxis().Direction() ) * SmallDist;
	}
    aTextPos =  Middle.Translated (OrtVec);
  }

  TCollection_ExtendedString aText("==");

  //Draw the text
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(), aText, aTextPos);
}


//==================================================================================
//function  : AddInterval
//purpose   : is used for presentation of interval between two lines or two points, 
//            or between one line and one point.
//==================================================================================
 void DsgPrs_EqualDistancePresentation::AddInterval(const Handle(Prs3d_Presentation)& aPresentation,
						    const Handle(Prs3d_Drawer)& aDrawer,
						    const gp_Pnt& aPoint1,
						    const gp_Pnt& aPoint2,
						    const gp_Dir& aDirection,
						    const gp_Pnt& aPosition,
						    const DsgPrs_ArrowSide anArrowSide,
						    gp_Pnt& aProj1,
						    gp_Pnt& aProj2) 
{
  const Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  gp_Lin L1 (aPoint1,aDirection);
  gp_Lin L2 (aPoint2,aDirection);
  aProj1 = ElCLib::Value(ElCLib::Parameter(L1, aPosition),L1);
  aProj2 = ElCLib::Value(ElCLib::Parameter(L2, aPosition),L2);

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(4);
  aPrims->AddVertex(aPoint1);
  aPrims->AddVertex(aProj1);
  aPrims->AddVertex(aProj2);
  aPrims->AddVertex(aPoint2);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  //add arrows presentation
  gp_Dir aDir(aProj2.XYZ() - aProj1.XYZ());
  
  DsgPrs::ComputeSymbol(aPresentation, LA, aProj1, aProj2, aDir.Reversed(), aDir, anArrowSide);
}


//========================================================================
// function : AddIntervalBetweenTwoArcs 
// purpose  : is used for presentation of interval between two arcs. One
//            of the arcs can have a zero radius (being a point really) 
//========================================================================
 void 
  DsgPrs_EqualDistancePresentation::AddIntervalBetweenTwoArcs(const Handle(Prs3d_Presentation)& aPresentation,
							      const Handle(Prs3d_Drawer)& aDrawer,
							      const gp_Circ& aCirc1,
							      const gp_Circ& aCirc2,
							      const gp_Pnt& aPoint1,
							      const gp_Pnt& aPoint2,
							      const gp_Pnt& aPoint3,
							      const gp_Pnt& aPoint4,
							      const DsgPrs_ArrowSide anArrowSide) 
{
  const Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Standard_Real aPar11, aPar12, aPar21, aPar22;
  if(aCirc1.Radius() > Precision::Confusion()){
    aPar11 = ElCLib::Parameter (aCirc1, aPoint1);
    aPar12 = ElCLib::Parameter(aCirc1, aPoint2);
  }
  else {
    aPar11 = M_PI;
    aPar12 = M_PI;
  }
  if (aCirc2.Radius() > Precision::Confusion()){
    aPar21 = ElCLib::Parameter(aCirc2, aPoint3 );
    aPar22 = ElCLib::Parameter(aCirc2, aPoint4);
  }
  else {
    aPar21 = M_PI;
    aPar22 = M_PI;
  }

  Handle(Graphic3d_ArrayOfPrimitives) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aPoint2);
  aPrims->AddVertex(aPoint4);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  Standard_Integer i, aNodeNb;
  Standard_Real aDelta, aCurPar;
  if(aPar12 < aPar11 ) aPar12 += 2.*M_PI;
  if (Abs(aPar12 - aPar11) > Precision::Confusion())
  {
    aNodeNb = Standard_Integer(Max(Abs(aPar12 - aPar11)*50./M_PI + 0.5, 4.));
    aDelta = (aPar12 - aPar11)/aNodeNb;
    aCurPar= aPar11;

    aPrims = new Graphic3d_ArrayOfPolylines(aNodeNb+1);
    for (i = 1; i<= aNodeNb; aCurPar += aDelta, i++)
      aPrims->AddVertex(ElCLib::Value( aCurPar, aCirc1));
    aPrims->AddVertex(aPoint2);
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  }
  if (aPar22 < aPar21) aPar22 += 2.*M_PI;
  if ( Abs(aPar22 - aPar21) > Precision::Confusion())
  {
    aNodeNb = Standard_Integer(Max(Abs(aPar22 - aPar21)*50./M_PI + 0.5, 4.));
    aDelta = (aPar22 - aPar21)/aNodeNb;
    aCurPar= aPar21;

    aPrims = new Graphic3d_ArrayOfPolylines(aNodeNb+1);
    for (i = 1; i<= aNodeNb; aCurPar += aDelta, i++)
      aPrims->AddVertex(ElCLib::Value( aCurPar, aCirc2));
    aPrims->AddVertex(aPoint4);
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
  }

  //get the direction of interval
  gp_Dir DirOfArrow;
  if(aPoint4.Distance(aPoint2) > Precision::Confusion())
  {
    DirOfArrow.SetXYZ(aPoint4.XYZ() - aPoint2.XYZ());
  }
  else
  {
    //Let's take the radius direction
    gp_Pnt aCenter = aCirc1.Location();
    if(aPoint4.Distance(aCenter) < Precision::Confusion())
      return;
    DirOfArrow.SetXYZ(aPoint4.XYZ() - aCenter.XYZ());
  }

  // Add presentation of arrows
  DsgPrs::ComputeSymbol( aPresentation, LA, aPoint2, aPoint4, DirOfArrow.Reversed(), DirOfArrow, anArrowSide );
}
//-- ota -- end
