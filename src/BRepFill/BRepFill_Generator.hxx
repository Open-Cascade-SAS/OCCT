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
#include <Standard_Handle.hxx>

#include <TopTools_SequenceOfShape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopTools_DataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
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
  
  Standard_EXPORT void AddWire (const TopoDS_Wire& Wire);
  
  //! Compute the  shell.
  Standard_EXPORT void Perform();
  
    const TopoDS_Shell& Shell() const;
  
  //! Returns  all   the shapes created
  Standard_EXPORT const TopTools_DataMapOfShapeListOfShape& Generated() const;
  
  //! Returns   the  shapes  created  from   a  subshape
  //! <SSection>  of a  section.
  Standard_EXPORT const TopTools_ListOfShape& GeneratedShapes (const TopoDS_Shape& SSection) const;




protected:





private:



  TopTools_SequenceOfShape myWires;
  TopoDS_Shell myShell;
  TopTools_DataMapOfShapeListOfShape myMap;


};


#include <BRepFill_Generator.lxx>





#endif // _BRepFill_Generator_HeaderFile
