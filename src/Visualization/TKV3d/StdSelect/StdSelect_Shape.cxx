// Created on: 1998-03-27
// Created by: Robert COUBLANC
// Copyright (c) 1998-1999 Matra Datavision
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

#include <StdSelect_Shape.hxx>

#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>
#include <Standard_Type.hxx>
#include <StdPrs_WFShape.hxx>
#include <StdPrs_ShadedShape.hxx>
#include <TopoDS_Shape.hxx>

IMPLEMENT_STANDARD_RTTIEXT(StdSelect_Shape, PrsMgr_PresentableObject)

StdSelect_Shape::StdSelect_Shape(const TopoDS_Shape&         theShape,
                                 const Handle(Prs3d_Drawer)& theDrawer)
    : mysh(theShape)
{
  if (!theDrawer.IsNull())
  {
    myDrawer->SetLink(theDrawer);
  }
}

void StdSelect_Shape::Compute(const Handle(PrsMgr_PresentationManager)&,
                              const Handle(Prs3d_Presentation)& thePrs,
                              const Standard_Integer            theMode)
{
  if (mysh.IsNull())
  {
    return;
  }

  Standard_Boolean canShade = (mysh.ShapeType() < 5 || mysh.ShapeType() == 8);
  if (theMode == 1)
  {
    if (canShade)
    {
      StdPrs_ShadedShape::Add(thePrs, mysh, myDrawer);
    }
    else
    {
      StdPrs_WFShape::Add(thePrs, mysh, myDrawer);
    }
  }
  else if (theMode == 0)
  {
    StdPrs_WFShape::Add(thePrs, mysh, myDrawer);
  }
}

void StdSelect_Shape::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, PrsMgr_PresentableObject)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &mysh)
}
