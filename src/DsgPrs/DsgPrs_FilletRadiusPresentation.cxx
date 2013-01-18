// Created on: 1997-12-08
// Created by: Serguei ZARITCHNY
// Copyright (c) 1997-1999 Matra Datavision
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

#include <DsgPrs_FilletRadiusPresentation.ixx>

#include <gp_Lin.hxx>
#include <gp_Dir.hxx>
#include <ElCLib.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Text.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <ElCLib.hxx>

#include <Graphic3d_Vertex.hxx>
#include <Graphic3d_AspectMarker3d.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Aspect_TypeOfLine.hxx>
#include <Aspect_TypeOfMarker.hxx>
#include <Aspect_AspectMarker.hxx>
#include <Quantity_Color.hxx>
#include <DsgPrs.hxx>
#include <Precision.hxx>

#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>

//pop pour NT
//#if WNT
#include <stdio.h>
//#endif
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void DsgPrs_FilletRadiusPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
					   const Handle(Prs3d_Drawer)& aDrawer,
					   const Standard_Real theval,
					   const TCollection_ExtendedString & aText,
					   const gp_Pnt & aPosition,
					   const gp_Dir & aNormalDir,
					   const gp_Pnt & aBasePnt,
					   const gp_Pnt & aFirstPoint,
					   const gp_Pnt & aSecondPoint,
					   const gp_Pnt & aCenter,
					   const DsgPrs_ArrowSide ArrowPrs,
					   const Standard_Boolean drawRevers,
					         gp_Pnt & DrawPosition,
					         gp_Pnt & EndOfArrow,
					         Handle(Geom_TrimmedCurve)& TrimCurve,
					         Standard_Boolean & HasCircle )
{
  char valcar[80];
  sprintf(valcar,"%5.2f",theval);

  Standard_Real FirstParCirc, LastParCirc;
  Standard_Boolean SpecCase;
  gp_Dir DirOfArrow;
  gp_Circ FilletCirc;
  //  gp_Pnt NewPosition, EndOfArrow;
  Handle( Prs3d_LengthAspect ) LA = aDrawer->LengthAspect();
  Prs3d_Root::CurrentGroup( aPresentation )->SetPrimitivesAspect( LA->LineAspect()->Aspect() );

  Standard_Real ArrowLength = LA->Arrow1Aspect()->Length();
  DsgPrs::ComputeFilletRadiusPresentation( ArrowLength,
					   theval,
					   aPosition,
					   aNormalDir,
					   aFirstPoint,
					   aSecondPoint,
					   aCenter,
					   aBasePnt,
					   drawRevers,
					   SpecCase,
					   FilletCirc,
					   FirstParCirc,
					   LastParCirc,
					   EndOfArrow,
					   DirOfArrow,
					   DrawPosition //NewPosition
					   );
  // Creating the fillet's arc 				      
  if( !SpecCase )
  {
    const Standard_Real Alpha = Abs(LastParCirc - FirstParCirc);
    const Standard_Integer NodeNumber = Max (4 , Standard_Integer (50. * Alpha / M_PI));
    const Standard_Real delta = Alpha / ( NodeNumber - 1 );

    Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(NodeNumber);
    for (Standard_Integer i = 0 ; i < NodeNumber; i++, FirstParCirc += delta)
	  aPrims->AddVertex(ElCLib::Value( FirstParCirc, FilletCirc ));
    Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

    HasCircle = Standard_True;
    Handle(Geom_Circle) Circle = new Geom_Circle( FilletCirc );
    TrimCurve = new Geom_TrimmedCurve( Circle,  FirstParCirc, LastParCirc );
  }
  else // null or PI anle or Radius = 0
  {
    HasCircle = Standard_False;
  }
  
  // Line from position to intersection point on fillet's circle (EndOfArrow)
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(DrawPosition);
  aPrims->AddVertex(EndOfArrow);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // Drawing the text
  Prs3d_Text::Draw(aPresentation, LA->TextAspect(), aText, DrawPosition);
   
  // Add presentation of arrows
  DsgPrs::ComputeSymbol( aPresentation, LA, EndOfArrow, EndOfArrow, DirOfArrow, DirOfArrow, ArrowPrs );
}
