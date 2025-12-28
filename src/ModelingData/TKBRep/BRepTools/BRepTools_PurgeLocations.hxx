// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _BRepTools_PurgeLocations_HeaderFile
#define _BRepTools_PurgeLocations_HeaderFile

#include <Standard.hxx>
#include <Standard_Transient.hxx>
#include <BRepTools_ReShape.hxx>
#include <TopTools_LocationSet.hxx>

//! Removes location datums, which satisfy conditions:
//! aTrsf.IsNegative() || (std::abs(std::abs(aTrsf.ScaleFactor()) - 1.) >
//! TopLoc_Location::ScalePrec()) from all locations of shape and its subshapes
class BRepTools_PurgeLocations
{

public:
  Standard_EXPORT BRepTools_PurgeLocations();

  //! Removes all locations correspondingly to criterium from theShape.
  Standard_EXPORT bool Perform(const TopoDS_Shape& theShape);

  //! Returns shape with removed locations.
  Standard_EXPORT const TopoDS_Shape& GetResult() const;

  Standard_EXPORT bool IsDone() const;

  //! Returns modified shape obtained from initial shape.
  TopoDS_Shape ModifiedShape(const TopoDS_Shape& theInitShape) const;

private:
  void AddShape(const TopoDS_Shape& theS);
  bool PurgeLocation(const TopoDS_Shape& theS, TopoDS_Shape& theRes);

  bool                                                                     myDone;
  TopoDS_Shape                                                             myShape;
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher>            myMapShapes;
  TopTools_LocationSet                                                     myLocations;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> myMapNewShapes;
  occ::handle<BRepTools_ReShape>                                           myReShape;
};

#endif // _BRepTools_PurgeLocations_HeaderFile
