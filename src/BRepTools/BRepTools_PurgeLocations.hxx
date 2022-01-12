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
//! aTrsf.IsNegative() || (Abs(Abs(aTrsf.ScaleFactor()) - 1.) > TopLoc_Location::ScalePrec())
//! from all locations of shape and its subshapes
class BRepTools_PurgeLocations
{

public:
  
  Standard_EXPORT BRepTools_PurgeLocations();
   
  //! Removes all locations correspodingly to criterium from theShape.
  Standard_EXPORT Standard_Boolean Perform(const TopoDS_Shape& theShape);

  //! Returns shape with removed locations.
  Standard_EXPORT const TopoDS_Shape& GetResult() const;

  Standard_EXPORT Standard_Boolean  IsDone() const;

  //! Returns modified shape obtained from initial shape.
  TopoDS_Shape ModifiedShape(const TopoDS_Shape& theInitShape) const;

private:

  void AddShape(const TopoDS_Shape& theS);
  Standard_Boolean PurgeLocation(const TopoDS_Shape& theS, TopoDS_Shape& theRes);

  Standard_Boolean myDone;
  TopoDS_Shape myShape;
  TopTools_IndexedMapOfShape myMapShapes;
  TopTools_LocationSet myLocations;
  TopTools_DataMapOfShapeShape myMapNewShapes;
  Handle(BRepTools_ReShape) myReShape;

};

#endif // _BRepTools_PurgeLocations_HeaderFile
