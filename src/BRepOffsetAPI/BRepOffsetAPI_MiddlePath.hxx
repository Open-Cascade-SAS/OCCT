// Created on: 2012-08-06
// Created by: jgv@ROLEX
// Copyright (c) 2012-2014 OPEN CASCADE SAS
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

#ifndef _BRepOffsetAPI_MiddlePath_HeaderFile
#define _BRepOffsetAPI_MiddlePath_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Wire.hxx>
#include <TopTools_MapOfShape.hxx>
#include <BRepOffsetAPI_SequenceOfSequenceOfShape.hxx>
#include <BRepBuilderAPI_MakeShape.hxx>


//! Describes functions to build a middle path of a
//! pipe-like shape
class BRepOffsetAPI_MiddlePath  : public BRepBuilderAPI_MakeShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! General constructor.
  //! StartShape and EndShape may be
  //! a wire or a face
  Standard_EXPORT BRepOffsetAPI_MiddlePath(const TopoDS_Shape& aShape, const TopoDS_Shape& StartShape, const TopoDS_Shape& EndShape);
  
  Standard_EXPORT virtual void Build(const Message_ProgressRange& theRange = Message_ProgressRange()) Standard_OVERRIDE;




protected:





private:



  TopoDS_Shape myInitialShape;
  TopoDS_Wire myStartWire;
  TopoDS_Wire myEndWire;
  Standard_Boolean myClosedSection;
  Standard_Boolean myClosedRing;
  TopTools_MapOfShape myStartWireEdges;
  TopTools_MapOfShape myEndWireEdges;
  BRepOffsetAPI_SequenceOfSequenceOfShape myPaths;


};







#endif // _BRepOffsetAPI_MiddlePath_HeaderFile
