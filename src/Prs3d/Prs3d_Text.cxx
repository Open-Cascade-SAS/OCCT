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

#include <Prs3d_Text.hxx>

#include <gp_Pnt.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_Vertex.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_TextAspect.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

// =======================================================================
// function : Draw
// purpose  :
// =======================================================================
void Prs3d_Text::Draw (const Handle(Graphic3d_Group)& theGroup,
                       const Handle(Prs3d_TextAspect)& theAspect,
                       const TCollection_ExtendedString& theText,
                       const gp_Pnt& theAttachmentPoint)
{
  Standard_Real x, y, z;
  theAttachmentPoint.Coord(x,y,z);

  theGroup->SetPrimitivesAspect (theAspect->Aspect());
  theGroup->Text (theText,
                  Graphic3d_Vertex(x,y,z),
                  theAspect->Height(),
                  theAspect->Angle(),
                  theAspect->Orientation(),
                  theAspect->HorizontalJustification(),
                  theAspect->VerticalJustification());
}

// =======================================================================
// function : Draw
// purpose  :
// =======================================================================
void Prs3d_Text::Draw (const Handle(Graphic3d_Group)&    theGroup,
                       const Handle(Prs3d_TextAspect)&   theAspect,
                       const TCollection_ExtendedString& theText,
                       const gp_Ax2&                     theOrientation,
                       const Standard_Boolean            theHasOwnAnchor)
{
  theGroup->SetPrimitivesAspect (theAspect->Aspect());
  theGroup->Text (theText,
                  theOrientation,
                  theAspect->Height(),
                  theAspect->Angle(),
                  theAspect->Orientation(),
                  theAspect->HorizontalJustification(),
                  theAspect->VerticalJustification(),
                  Standard_True,
                  theHasOwnAnchor);
}
