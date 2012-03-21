// Created on: 1995-12-08
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



#include <DsgPrs_SymbPresentation.ixx>

#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_TextAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Geom_CartesianPoint.hxx>
#include <StdPrs_Point.hxx>
#include <TCollection_AsciiString.hxx>
#include <Prs3d_Text.hxx>
#include <Aspect_TypeOfMarker.hxx>

void DsgPrs_SymbPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				   const Handle(Prs3d_Drawer)& aDrawer,
				   const TCollection_ExtendedString& aText,
				   const gp_Pnt& OffsetPoint) 
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  Handle(Prs3d_TextAspect) TA = LA->TextAspect();
  TA->SetColor(Quantity_NOC_GREEN);
  Prs3d_Text::Draw(aPresentation,TA,aText, OffsetPoint);
  
  // 2eme groupe : marker
  Handle(Geom_CartesianPoint) theP = new Geom_CartesianPoint(OffsetPoint);
  Handle(Prs3d_PointAspect) PA = aDrawer->PointAspect();
  PA->SetTypeOfMarker(Aspect_TOM_RING2);
  StdPrs_Point::Add(aPresentation,theP,aDrawer);
}


