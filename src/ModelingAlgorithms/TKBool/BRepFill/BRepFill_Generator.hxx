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
#include <TopTools_SequenceOfShape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <BRepTools_ReShape.hxx>
class TopoDS_Wire;
class TopoDS_Shape;

//! Compute a topological surface ( a  shell) using
//! generating wires. The face of the shell will be
//! ruled surfaces passing by the wires.
//! The wires must have the same number of edges.
class BRepFill_Generator
{
public:
  DEFINE_STANDARD_ALLOC

  Standard_EXPORT BRepFill_Generator();

  Standard_EXPORT void AddWire(const TopoDS_Wire& Wire);

  //! Compute the  shell.
  Standard_EXPORT void Perform();

  const TopoDS_Shell& Shell() const;

  //! Returns  all   the shapes created
  Standard_EXPORT const TopTools_DataMapOfShapeListOfShape& Generated() const;

  //! Returns   the  shapes  created  from   a  subshape
  //! <SSection>  of a  section.
  Standard_EXPORT const TopTools_ListOfShape& GeneratedShapes(const TopoDS_Shape& SSection) const;

  //! Returns a modified shape in the constructed shell,
  //! If shape is not changed (replaced) during operation => returns the same shape
  Standard_EXPORT TopoDS_Shape ResultShape(const TopoDS_Shape& theShape) const;

  //! Sets the mutable input state
  //! If true then the input profile can be modified
  //! inside the operation. Default value is true.
  Standard_EXPORT void SetMutableInput(const Standard_Boolean theIsMutableInput);

  //! Returns the current mutable input state
  Standard_EXPORT Standard_Boolean IsMutableInput() const;

  //! Returns status of the operation
  BRepFill_ThruSectionErrorStatus GetStatus() const { return myStatus; }

protected:
private:
  TopTools_SequenceOfShape           myWires;
  TopoDS_Shell                       myShell;
  TopTools_DataMapOfShapeListOfShape myMap;
  TopTools_DataMapOfShapeShape       myOldNewShapes;
  BRepTools_ReShape                  myReshaper;
  Standard_Boolean                   myMutableInput;
  BRepFill_ThruSectionErrorStatus    myStatus;
};

#include <BRepFill_Generator.lxx>

#endif // _BRepFill_Generator_HeaderFile
