// Created on: 1994-03-07
// Created by: Joelle CHAUVET
// Copyright (c) 1994-1999 Matra Datavision
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

#ifndef _BRepFill_Generator_HeaderFile
#define _BRepFill_Generator_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <BRepFill_ThruSectionErrorStatus.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_Sequence.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_DataMap.hxx>
#include <TopoDS_Shape.hxx>
#include <NCollection_List.hxx>
#include <BRepTools_ReShape.hxx>
class TopoDS_Wire;
class TopoDS_Shape;

//! Compute a topological surface (a shell) using
//! generating wires. The face of the shell will be
//! ruled surfaces passing by the wires.
//! The wires must have the same number of edges.
class BRepFill_Generator
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepFill_Generator();

  Standard_EXPORT void AddWire(const TopoDS_Wire& Wire);

  //! Compute the shell.
  Standard_EXPORT void Perform();

  const TopoDS_Shell& Shell() const;

  //! Returns all the shapes created
  Standard_EXPORT const NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>& Generated() const;

  //! Returns the shapes created from a subshape
  //! <SSection> of a section.
  Standard_EXPORT const NCollection_List<TopoDS_Shape>& GeneratedShapes(const TopoDS_Shape& SSection) const;

  //! Returns a modified shape in the constructed shell,
  //! If shape is not changed (replaced) during operation => returns the same shape
  Standard_EXPORT TopoDS_Shape ResultShape(const TopoDS_Shape& theShape) const;

  //! Sets the mutable input state
  //! If true then the input profile can be modified
  //! inside the operation. Default value is true.
  Standard_EXPORT void SetMutableInput(const bool theIsMutableInput);

  //! Returns the current mutable input state
  Standard_EXPORT bool IsMutableInput() const;

  //! Returns status of the operation
  BRepFill_ThruSectionErrorStatus GetStatus() const { return myStatus; }

private:
  NCollection_Sequence<TopoDS_Shape>           myWires;
  TopoDS_Shell                       myShell;
  NCollection_DataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher> myMap;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>       myOldNewShapes;
  BRepTools_ReShape                  myReshaper;
  bool                   myMutableInput;
  BRepFill_ThruSectionErrorStatus    myStatus;
};

#include <BRepFill_Generator.lxx>

#endif // _BRepFill_Generator_HeaderFile
