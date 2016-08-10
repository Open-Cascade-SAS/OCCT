// Created on: 1996-01-16
// Created by: Jean-Pierre COMBE
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


#include <DsgPrs_TangentPresentation.hxx>
#include <Geom_CartesianPoint.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <StdPrs_Point.hxx>

void DsgPrs_TangentPresentation::Add (const Handle(Prs3d_Presentation)& aPresentation,
				      const Handle(Prs3d_Drawer)& aDrawer,
				      const gp_Pnt& OffsetPoint,
				      const gp_Dir& aDirection,
				      const Standard_Real length)
{
  gp_Vec vec(aDirection);
  gp_Vec vec1 = vec.Multiplied(length);
  gp_Vec vec2 = vec.Multiplied(-length);
  gp_Pnt p1 = OffsetPoint.Translated(vec1);
  gp_Pnt p2 = OffsetPoint.Translated(vec2);
  
  // Aspect
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  LA->LineAspect()->SetTypeOfLine(Aspect_TOL_SOLID);
  Handle(Prs3d_ArrowAspect) ARR1 = LA->ArrowAspect();
  Handle(Prs3d_ArrowAspect) ARR2 = LA->ArrowAspect();
  ARR1->SetLength(length/5);
  ARR2->SetLength(length/5);

  // Array1OfVertex
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());

  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(p1);
  aPrims->AddVertex(p2);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  // fleche 1 : 
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), p1, aDirection, LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());

  // fleche 2
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), p2, aDirection.Reversed(), LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());
}
