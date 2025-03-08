// Created on: 2002-11-13
// Created by: Galina KULIKOVA
// Copyright (c) 2002-2014 OPEN CASCADE SAS
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

#ifndef _ShapeUpgrade_RemoveLocations_HeaderFile
#define _ShapeUpgrade_RemoveLocations_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <Standard_Transient.hxx>

class ShapeUpgrade_RemoveLocations;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_RemoveLocations, Standard_Transient)

//! Removes all locations sub-shapes of specified shape
class ShapeUpgrade_RemoveLocations : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT ShapeUpgrade_RemoveLocations();

  //! Removes all location correspondingly to RemoveLevel.
  Standard_EXPORT Standard_Boolean Remove(const TopoDS_Shape& theShape);

  //! Returns shape with removed locations.
  TopoDS_Shape GetResult() const;

  //! sets level starting with that location will be removed,
  //! by default TopAbs_SHAPE. In this case locations will be kept for specified shape
  //! and if specified shape is TopAbs_COMPOUND for sub-shapes of first level.
  void SetRemoveLevel(const TopAbs_ShapeEnum theLevel);

  //! sets level starting with that location will be removed.Value of level can be set to
  //! TopAbs_SHAPE,TopAbs_COMPOUND,TopAbs_SOLID,TopAbs_SHELL,TopAbs_FACE.By default TopAbs_SHAPE.
  //! In this case location will be removed for all shape types for exception of compound.
  TopAbs_ShapeEnum RemoveLevel() const;

  //! Returns modified shape obtained from initial shape.
  TopoDS_Shape ModifiedShape(const TopoDS_Shape& theInitShape) const;

  //! Returns map of modified shapes.
  const TopTools_DataMapOfShapeShape& GetModifiedShapesMap() const { return myMapNewShapes; }

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_RemoveLocations, Standard_Transient)

protected:
private:
  Standard_EXPORT Standard_Boolean MakeNewShape(const TopoDS_Shape&    theShape,
                                                const TopoDS_Shape&    theAncShape,
                                                TopoDS_Shape&          theNewShape,
                                                const Standard_Boolean theRemoveLoc);

  TopAbs_ShapeEnum             myLevelRemoving;
  TopoDS_Shape                 myShape;
  TopTools_DataMapOfShapeShape myMapNewShapes;
};

#include <ShapeUpgrade_RemoveLocations.lxx>

#endif // _ShapeUpgrade_RemoveLocations_HeaderFile
