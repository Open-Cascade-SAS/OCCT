// Created on: 1998-06-03
// Created by: data exchange team
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

#ifndef _ShapeUpgrade_ShellSewing_HeaderFile
#define _ShapeUpgrade_ShellSewing_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <Standard_Integer.hxx>
class ShapeBuild_ReShape;
class TopoDS_Shape;

//! This class provides a tool for applying sewing algorithm from
//! BRepBuilderAPI: it takes a shape, calls sewing for each shell,
//! and then replaces sewed shells with use of ShapeBuild_ReShape
class ShapeUpgrade_ShellSewing
{
public:
  DEFINE_STANDARD_ALLOC

  //! Creates a ShellSewing, empty
  Standard_EXPORT ShapeUpgrade_ShellSewing();

  //! Builds a new shape from a former one, by calling Sewing from
  //! BRepBuilderAPI. Rebuilt solids are oriented to be "not infinite"
  //!
  //! If <tol> is not given (i.e. value 0. by default), it is
  //! computed as the mean tolerance recorded in <shape>
  //!
  //! If no shell has been sewed, this method returns the input
  //! shape
  Standard_EXPORT TopoDS_Shape ApplySewing(const TopoDS_Shape& shape,
                                           const Standard_Real tol = 0.0);

protected:
private:
  Standard_EXPORT void Init(const TopoDS_Shape& shape);

  Standard_EXPORT Standard_Integer Prepare(const Standard_Real tol);

  Standard_EXPORT TopoDS_Shape Apply(const TopoDS_Shape& shape, const Standard_Real tol);

  TopTools_IndexedMapOfShape myShells;
  Handle(ShapeBuild_ReShape) myReShape;
};

#endif // _ShapeUpgrade_ShellSewing_HeaderFile
