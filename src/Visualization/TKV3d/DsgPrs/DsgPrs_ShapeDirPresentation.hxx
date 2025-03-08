// Created on: 1995-10-06
// Created by: Jing Cheng MEI
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _DsgPrs_ShapeDirPresentation_HeaderFile
#define _DsgPrs_ShapeDirPresentation_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Prs3d_Drawer.hxx>
#include <Prs3d_Presentation.hxx>

class TopoDS_Shape;

//! A framework to define display of the normal to the
//! surface of a shape.
class DsgPrs_ShapeDirPresentation
{
public:
  DEFINE_STANDARD_ALLOC

  //! Adds the shape shape and the mode mode to the
  //! presentation object prs.
  //! The display attributes of the normal are defined by the
  //! attribute manager aDrawer.
  //! mode determines whether the first or the last point of
  //! the normal is given to the presentation object. If the
  //! first point: 0; if the last point, 1.
  Standard_EXPORT static void Add(const Handle(Prs3d_Presentation)& prs,
                                  const Handle(Prs3d_Drawer)&       aDrawer,
                                  const TopoDS_Shape&               shape,
                                  const Standard_Integer            mode);

protected:
private:
};

#endif // _DsgPrs_ShapeDirPresentation_HeaderFile
