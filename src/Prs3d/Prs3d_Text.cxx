// Created on: 1993-09-14
// Created by: Jean-Louis FRENKEL
// Copyright (c) 1993-1999 Matra Datavision
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


#include <gp_Pnt.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Text.hxx>
#include <Prs3d_TextAspect.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

void Prs3d_Text::Draw (
		       const Handle(Prs3d_Presentation)& aPresentation,
		       const Handle(Prs3d_TextAspect)& anAspect,
		       const TCollection_ExtendedString& aText,
		       const gp_Pnt& AttachmentPoint) {


  Prs3d_Root::CurrentGroup(aPresentation)->SetPrimitivesAspect(anAspect->Aspect());
  Standard_Real x,y,z;
  AttachmentPoint.Coord(x,y,z);

  
// POP Graphic3d_Grup accepte de l'extended
  Prs3d_Root::CurrentGroup(aPresentation)->Text(
//                     TCollection_AsciiString(aText).ToCString(),
		      aText,
	             Graphic3d_Vertex(x,y,z),
                     anAspect->Height(),
                     anAspect->Angle(),
                     anAspect->Orientation(),
                     anAspect->HorizontalJustification(),
                     anAspect->VerticalJustification());
}


void Prs3d_Text::Draw (
		       const Handle(Prs3d_Presentation)& aPresentation,
		       const Handle(Prs3d_Drawer)& aDrawer,
		       const TCollection_ExtendedString& aText,
		       const gp_Pnt& AttachmentPoint) {

  
  Prs3d_Text::Draw(aPresentation,aDrawer->TextAspect(),aText,AttachmentPoint);
  }

// =======================================================================
// function : Draw
// purpose  :
// =======================================================================
void Prs3d_Text::Draw (const Handle(Prs3d_Presentation)& thePresentation,
                       const Handle(Prs3d_TextAspect)&   theAspect,
                       const TCollection_ExtendedString& theText,
                       const gp_Ax2&                     theOrientation)
{
  Prs3d_Root::CurrentGroup (thePresentation)->SetPrimitivesAspect (theAspect->Aspect());
  Prs3d_Root::CurrentGroup (thePresentation)->Text (theText,
                                                    theOrientation,
                                                    theAspect->Height(),
                                                    theAspect->Angle(),
                                                    theAspect->Orientation(),
                                                    theAspect->HorizontalJustification(),
                                                    theAspect->VerticalJustification());
}
