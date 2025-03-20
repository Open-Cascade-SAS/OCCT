// Created on: 2014-10-14
// Created by: Anton POLETAEV
// Copyright (c) 2013-2014 OPEN CASCADE SAS
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

#include <Prs3d_BndBox.hxx>

#include <Prs3d_LineAspect.hxx>

//=================================================================================================

void Prs3d_BndBox::Add(const Handle(Prs3d_Presentation)& thePresentation,
                       const Bnd_Box&                    theBndBox,
                       const Handle(Prs3d_Drawer)&       theDrawer)
{
  if (!theBndBox.IsVoid())
  {
    Handle(Graphic3d_Group) aGroup = thePresentation->CurrentGroup();
    aGroup->SetGroupPrimitivesAspect(
      new Graphic3d_AspectLine3d(theDrawer->LineAspect()->Aspect()->Color(),
                                 Aspect_TOL_DOTDASH,
                                 theDrawer->LineAspect()->Aspect()->Width()));
    aGroup->AddPrimitiveArray(FillSegments(theBndBox));
  }
}

//=================================================================================================

void Prs3d_BndBox::Add(const Handle(Prs3d_Presentation)& thePresentation,
                       const Bnd_OBB&                    theBndBox,
                       const Handle(Prs3d_Drawer)&       theDrawer)
{
  if (!theBndBox.IsVoid())
  {
    Handle(Graphic3d_Group) aGroup = thePresentation->CurrentGroup();
    aGroup->SetGroupPrimitivesAspect(
      new Graphic3d_AspectLine3d(theDrawer->LineAspect()->Aspect()->Color(),
                                 Aspect_TOL_DOTDASH,
                                 theDrawer->LineAspect()->Aspect()->Width()));
    aGroup->AddPrimitiveArray(FillSegments(theBndBox));
  }
}
