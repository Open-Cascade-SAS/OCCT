// Created on: 1997-12-08
// Created by: Serguei ZARITCHNY
// Copyright (c) 1997-1999 Matra Datavision
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

#include <DsgPrs_FilletRadiusPresentation.hxx>

#include <DsgPrs.hxx>
#include <ElCLib.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
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

#include <stdio.h>
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
  Handle( Prs3d_DimensionAspect ) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup( aPresentation )->SetPrimitivesAspect( LA->LineAspect()->Aspect() );

  Standard_Real ArrowLength = LA->ArrowAspect()->Length();
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
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, DrawPosition);
   
  // Add presentation of arrows
  DsgPrs::ComputeSymbol( aPresentation, LA, EndOfArrow, EndOfArrow, DirOfArrow, DirOfArrow, ArrowPrs );
}
