// Created on: 1997-12-05
// Created by: Robert COUBLANC
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

#include <DsgPrs_ShadedPlanePresentation.ixx>
#include <Prs3d_Root.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_ShadingAspect.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfPolygons.hxx>


//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void DsgPrs_ShadedPlanePresentation::Add(const Handle(Prs3d_Presentation)& aPresentation,
					 const Handle(Prs3d_Drawer)& aDrawer,	     
					 const gp_Pnt& aPt1, 
					 const gp_Pnt& aPt2,
					 const gp_Pnt& aPt3)
{
  Handle(Graphic3d_Group) TheGroup = Prs3d_Root::CurrentGroup(aPresentation);
  TheGroup->SetPrimitivesAspect(aDrawer->PlaneAspect()->EdgesAspect()->Aspect());
  TheGroup->SetPrimitivesAspect(aDrawer->ShadingAspect()->Aspect());

  Handle(Graphic3d_ArrayOfPolygons) aPrims = new Graphic3d_ArrayOfPolygons(4);
  aPrims->AddVertex(aPt1);
  aPrims->AddVertex(aPt2);
  aPrims->AddVertex(aPt3);
  aPrims->AddVertex(aPt1);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);
}
