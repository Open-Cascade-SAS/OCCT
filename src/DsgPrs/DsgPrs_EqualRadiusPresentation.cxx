// Created on: 1998-01-20
// Created by: Julia GERASIMOVA
// Copyright (c) 1998-1999 Matra Datavision
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


#include <DsgPrs.hxx>
#include <DsgPrs_EqualRadiusPresentation.hxx>
#include <gce_MakeDir.hxx>
#include <Geom_Plane.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Graphic3d_Group.hxx>
#include <Precision.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <Prs3d_Text.hxx>
#include <TCollection_ExtendedString.hxx>

void DsgPrs_EqualRadiusPresentation::Add( const Handle( Prs3d_Presentation )& aPresentation,
					  const Handle( Prs3d_Drawer )& aDrawer,
					  const gp_Pnt& FirstCenter,
					  const gp_Pnt& SecondCenter,
					  const gp_Pnt& FirstPoint,
					  const gp_Pnt& SecondPoint,
					  const Handle( Geom_Plane )& Plane )
{
  Handle( Prs3d_DimensionAspect ) LA = aDrawer->DimensionAspect();
  Prs3d_Root::CurrentGroup( aPresentation )->SetPrimitivesAspect( LA->LineAspect()->Aspect() );

  Handle(Graphic3d_ArrayOfPolylines) aPrims = new Graphic3d_ArrayOfPolylines(4);
  aPrims->AddVertex(FirstPoint);
  aPrims->AddVertex(FirstCenter);
  aPrims->AddVertex(SecondCenter);
  aPrims->AddVertex(SecondPoint);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // Add presentation of arrows
  gp_Dir FirstDir = gce_MakeDir( FirstCenter, FirstPoint ), SecondDir = gce_MakeDir( SecondCenter, SecondPoint );
  DsgPrs::ComputeSymbol( aPresentation, LA, FirstCenter, FirstPoint, FirstDir.Reversed(), FirstDir, DsgPrs_AS_FIRSTPT_LASTAR );
  DsgPrs::ComputeSymbol( aPresentation, LA, SecondCenter, SecondPoint, SecondDir.Reversed(), SecondDir, DsgPrs_AS_FIRSTPT_LASTAR );

//ota === beging ===
  gp_Pnt Middle( (FirstCenter.XYZ() + SecondCenter.XYZ()) *0.5 ), aTextPos;
  Standard_Real SmallDist;
  //Mark of constraint
  TCollection_ExtendedString aText("==");
  
  Standard_Real Dist = FirstCenter.Distance( SecondCenter );
  if (Dist > Precision::Confusion())
  {
    SmallDist = Dist * 0.05; // take 1/20 part of length;
    if (SmallDist <= Precision::Confusion())
      SmallDist = Dist;
    gp_Dir LineDir = gce_MakeDir( FirstCenter, SecondCenter );
    gp_Dir OrtDir  = Plane->Pln().Axis().Direction() ^ LineDir;

    gp_Vec OrtVec  = gp_Vec( OrtDir ) * SmallDist;
      
    //Compute the text position
    aTextPos = Middle.Translated(OrtVec);
  }
  else
  {
    Standard_Real Rad = Max(FirstCenter.Distance( FirstPoint ), SecondCenter.Distance(SecondPoint));
      
    SmallDist = Rad *0.05; // take 1/20 part of length;
    if (SmallDist <= Precision::Confusion())
      SmallDist = Rad;

    gp_Vec aVec(SmallDist, SmallDist, SmallDist);
      
    //Compute the text position
    aTextPos = FirstCenter.Translated(aVec);
  }
  
  //Draw the text
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, aTextPos);
//ota === end ===     
}
