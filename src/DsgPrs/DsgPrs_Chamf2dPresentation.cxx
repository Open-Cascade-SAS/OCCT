// Created on: 1996-03-19
// Created by: Flore Lantheaume
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



#include <DsgPrs_Chamf2dPresentation.ixx>

#include <Graphic3d_Group.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_LengthAspect.hxx>
#include <Prs3d_Text.hxx>

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <DsgPrs.hxx>

void DsgPrs_Chamf2dPresentation::Add(
			   const Handle(Prs3d_Presentation)& aPresentation,
			   const Handle(Prs3d_Drawer)& aDrawer,
			   const gp_Pnt& aPntAttach,
			   const gp_Pnt& aPntEnd,
			   const TCollection_ExtendedString& aText)
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aPntAttach);
  aPrims->AddVertex(aPntEnd);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  gp_Dir ArrowDir(aPntAttach.XYZ()-aPntEnd.XYZ());
  Prs3d_Arrow::Draw(aPresentation,aPntAttach,ArrowDir,LA->Arrow1Aspect()->Angle(),LA->Arrow1Aspect()->Length());
		     
  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntEnd);
}


//==========================================================================
// function : DsgPrs_Chamf2dPresentation::Add
// purpose  : it is possible to choose the symbol of extremities of the face (arrow, point ...)
//==========================================================================

void DsgPrs_Chamf2dPresentation::Add(
			   const Handle(Prs3d_Presentation)& aPresentation,
			   const Handle(Prs3d_Drawer)& aDrawer,
			   const gp_Pnt& aPntAttach,
			   const gp_Pnt& aPntEnd,
			   const TCollection_ExtendedString& aText,
			   const DsgPrs_ArrowSide ArrowPrs) 
{
  Handle(Prs3d_LengthAspect) LA = aDrawer->LengthAspect();
  
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aPntAttach);
  aPrims->AddVertex(aPntEnd);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  Prs3d_Text::Draw(aPresentation,LA->TextAspect(),aText,aPntEnd);

  gp_Dir ArrowDir(aPntAttach.XYZ()-aPntEnd.XYZ());
  gp_Dir ArrowDir1 = ArrowDir;
  ArrowDir1.Reverse();

  DsgPrs::ComputeSymbol(aPresentation,LA,aPntEnd,aPntAttach,ArrowDir1,ArrowDir,ArrowPrs);
}
