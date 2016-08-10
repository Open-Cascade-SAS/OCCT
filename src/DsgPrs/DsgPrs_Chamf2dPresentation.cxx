// Created on: 1996-03-19
// Created by: Flore Lantheaume
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


#include <DsgPrs.hxx>
#include <DsgPrs_Chamf2dPresentation.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <Graphic3d_ArrayOfSegments.hxx>
#include <Graphic3d_Group.hxx>
#include <Prs3d_Arrow.hxx>
#include <Prs3d_ArrowAspect.hxx>
#include <Prs3d_DimensionAspect.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <TCollection_ExtendedString.hxx>

void DsgPrs_Chamf2dPresentation::Add(
			   const Handle(Prs3d_Presentation)& aPresentation,
			   const Handle(Prs3d_Drawer)& aDrawer,
			   const gp_Pnt& aPntAttach,
			   const gp_Pnt& aPntEnd,
			   const TCollection_ExtendedString& aText)
{
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aPntAttach);
  aPrims->AddVertex(aPntEnd);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  gp_Dir ArrowDir(aPntAttach.XYZ()-aPntEnd.XYZ());
  Prs3d_Arrow::Draw (Prs3d_Root::CurrentGroup (aPresentation), aPntAttach, ArrowDir, LA->ArrowAspect()->Angle(), LA->ArrowAspect()->Length());
		     
  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, aPntEnd);
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
  Handle(Prs3d_DimensionAspect) LA = aDrawer->DimensionAspect();
  
  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(LA->LineAspect()->Aspect());
  
  Handle(Graphic3d_ArrayOfSegments) aPrims = new Graphic3d_ArrayOfSegments(2);
  aPrims->AddVertex(aPntAttach);
  aPrims->AddVertex(aPntEnd);
  Prs3d_Root::CurrentGroup(aPresentation)->AddPrimitiveArray(aPrims);

  Prs3d_Text::Draw (Prs3d_Root::CurrentGroup (aPresentation), LA->TextAspect(), aText, aPntEnd);

  gp_Dir ArrowDir(aPntAttach.XYZ()-aPntEnd.XYZ());
  gp_Dir ArrowDir1 = ArrowDir;
  ArrowDir1.Reverse();

  DsgPrs::ComputeSymbol(aPresentation,LA,aPntEnd,aPntAttach,ArrowDir1,ArrowDir,ArrowPrs);
}
