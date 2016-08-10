// Created on: 1995-12-08
// Created by: Jean-Pierre COMBE
// Copyright (c) 1995-1999 Matra Datavision
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


#include <Aspect_TypeOfMarker.hxx>
#include <DsgPrs_SymbPresentation.hxx>
#include <Geom_CartesianPoint.hxx>
#include <gp_Pnt.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <StdPrs_Point.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

void DsgPrs_SymbPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				   const Handle(Prs3d_Drawer)& aDrawer,
				   const TCollection_ExtendedString& aText,
				   const gp_Pnt& OffsetPoint) 
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  Handle(Prs3d_TextAspect) TA = LA->TextAspect();
  TA->SetColor(Quantity_NOC_GREEN);
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), TA, aText, OffsetPoint);
  
  // 2eme groupe : marker
  Handle(Geom_CartesianPoint) theP = new Geom_CartesianPoint(OffsetPoint);
  Handle(Prs3d_PointAspect) PA = aDrawer->PointAspect();
  PA->SetTypeOfMarker(Aspect_TOM_RING2);
  StdPrs_Point::Add(aPresentation,theP,aDrawer);
}


