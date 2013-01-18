// Created on: 1996-01-16
// Created by: Jean-Pierre COMBE
// Copyright (c) 1996-1999 Matra Datavision
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

#include <DsgPrs_TangentPresentation.ixx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Prs3d_PointAspect.hxx>
#include <Geom_CartesianPoint.hxx>
#include <StdPrs_Point.hxx>
#include <gp_Vec.hxx>
#include <Prs3d_Arrow.hxx>

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
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  LA->LineAspect()->SetTypeOfLine(Aspect_TOL_SOLID);
  Handle(Prs3d_ArrowAspect) ARR1 = LA->Arrow1Aspect();
  Handle(Prs3d_ArrowAspect) ARR2 = LA->Arrow2Aspect();
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
  Prs3d_Arrow::Draw(aPresentation,p1,aDirection,LA->Arrow1Aspect()->Angle(),LA->Arrow1Aspect()->Length());

  // fleche 2
  Prs3d_Root::NewGroup(aPresentation);
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  Prs3d_Arrow::Draw(aPresentation,p2,aDirection.Reversed(),LA->Arrow2Aspect()->Angle(),LA->Arrow2Aspect()->Length());
}
