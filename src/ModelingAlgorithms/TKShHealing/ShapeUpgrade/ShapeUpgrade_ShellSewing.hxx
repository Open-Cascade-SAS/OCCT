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

#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_OrderedMap.hxx>

#include <cstddef>
class ShapeBuild_ReShape;
class TopoDS_Shape;

//! Applies sewing independently to every shell of a shape.
//! Sewed shells are substituted through ShapeBuild_ReShape, and rebuilt solids are oriented so
//! that the infinite point lies outside.
class ShapeUpgrade_ShellSewing
{
public:
  DEFINE_STANDARD_ALLOC

  //! Constructs an empty shell-sewing tool.
  Standard_EXPORT ShapeUpgrade_ShellSewing();

  //! Sews every shell in the shape and substitutes the sewing results into a rebuilt shape.
  //! A non-positive tolerance is replaced by the mean tolerance of the input shape.
  //! @param[in] theShape shape whose shells are to be sewed
  //! @param[in] theTolerance sewing tolerance, or a non-positive value to compute it automatically
  //! @return rebuilt shape, or a null shape when no shell can be sewed
  Standard_EXPORT TopoDS_Shape ApplySewing(const TopoDS_Shape& theShape,
                                           const double        theTolerance = 0.0);

private:
  Standard_EXPORT void Init(const TopoDS_Shape& theShape);

  Standard_EXPORT size_t Prepare(const double theTolerance);

  Standard_EXPORT TopoDS_Shape Apply(const TopoDS_Shape& theShape, const double theTolerance);

  NCollection_OrderedMap<TopoDS_Shape, TopTools_ShapeMapHasher> myShells;
  occ::handle<ShapeBuild_ReShape>                               myReShape;
};

#endif // _ShapeUpgrade_ShellSewing_HeaderFile
