// Created on: 2014-03-12
// Created by: Kirill GAVRILOV
// Copyright (c) 2014 OPEN CASCADE SAS
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

#include <Prs3d_PresentationShadow.hxx>


IMPLEMENT_STANDARD_RTTIEXT(Prs3d_PresentationShadow,Prs3d_Presentation)

//=======================================================================
//function : Prs3d_PresentationShadow
//purpose  :
//=======================================================================
Prs3d_PresentationShadow::Prs3d_PresentationShadow (const Handle(Graphic3d_StructureManager)& theViewer,
                                                    const Handle(Prs3d_Presentation)&         thePrs)
: Prs3d_Presentation (theViewer, thePrs),
  myParentStructId (thePrs->Identification()),
  myParentAffinity (thePrs->CStructure()->ViewAffinity)
{
  //
}

//=======================================================================
//function : CalculateBoundBox
//purpose  :
//=======================================================================
void Prs3d_PresentationShadow::CalculateBoundBox()
{
  //
}
