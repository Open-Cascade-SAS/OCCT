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

#include <StdPrs_BndBox.hxx>

#include <Aspect_TypeOfLine.hxx>
#include <Graphic3d_AspectLine3d.hxx>
#include <Graphic3d_ArrayOfPolylines.hxx>
#include <Prs3d_LineAspect.hxx>
#include <Quantity_Color.hxx>

namespace
{
  static const Standard_Integer THE_INDICES[][3] =
  { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 },
    { 0, 1, 1 }, { 1, 1, 1 }, { 1, 1, 0 }, { 0, 1, 0 },
    { 0, 0, 0 }, { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 },
    { 0, 1, 1 }, { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 } };
}

//=======================================================================
//function : Add
//purpose  :
//=======================================================================
void StdPrs_BndBox::Add (const Handle(Prs3d_Presentation)& thePresentation,
                         const Bnd_Box&                    theBndBox,
                         const Handle(Prs3d_Drawer)&       theDrawer)
{
  if (theBndBox.IsVoid())
  {
    return;
  }

  Standard_Real X[2], Y[2], Z[2];
  theBndBox.Get (X[0], Y[0], Z[0], X[1], Y[1], Z[1]);

  Handle(Graphic3d_Group) aGroup = Prs3d_Root::CurrentGroup (thePresentation);
  Quantity_Color aColor = theDrawer->LineAspect()->Aspect()->Color();
  Standard_Real  aWidth = theDrawer->LineAspect()->Aspect()->Width();
  aGroup->SetGroupPrimitivesAspect (new Graphic3d_AspectLine3d (aColor, Aspect_TOL_DOTDASH, aWidth));

  Handle(Graphic3d_ArrayOfPolylines) aPolyline = new Graphic3d_ArrayOfPolylines(16);
  for(Standard_Integer aVertIter = 0; aVertIter < 16; ++aVertIter)
  {
    aPolyline->AddVertex (X[THE_INDICES[aVertIter][0]],
                          Y[THE_INDICES[aVertIter][1]],
                          Z[THE_INDICES[aVertIter][2]]);
  }
  aGroup->AddPrimitiveArray (aPolyline);
}
