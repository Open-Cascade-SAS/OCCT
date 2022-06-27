// Created on: 1993-08-12
// Created by: Jean Yves LEBEY
// Copyright (c) 1993-1999 Matra Datavision
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

#ifndef _TopOpeBRepTool_AncestorsTool_HeaderFile
#define _TopOpeBRepTool_AncestorsTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <TopAbs_ShapeEnum.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
class TopoDS_Shape;


//! Describes the ancestors tool needed by
//! the class DSFiller from TopOpeInter.
//!
//! This class has been created because it is not possible
//! to instantiate the argument TheAncestorsTool (of
//! DSFiller from TopOpeInter) with a  package (TopExp)
//! giving services as package methods.
class TopOpeBRepTool_AncestorsTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! same as package method TopExp::MapShapeListOfShapes()
  Standard_EXPORT static void MakeAncestors (const TopoDS_Shape& S, const TopAbs_ShapeEnum TS, const TopAbs_ShapeEnum TA, TopTools_IndexedDataMapOfShapeListOfShape& M);




protected:





private:





};







#endif // _TopOpeBRepTool_AncestorsTool_HeaderFile
