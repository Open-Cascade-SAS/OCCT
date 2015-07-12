// Created on: 1995-03-08
// Created by: Mister rmi
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _StdSelect_HeaderFile
#define _StdSelect_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <SelectMgr_Selection.hxx>
#include <Prs3d_Drawer.hxx>
class StdSelect_BRepSelectionTool;
class StdSelect_BRepOwner;
class StdSelect_EdgeFilter;
class StdSelect_FaceFilter;
class StdSelect_ShapeTypeFilter;
class StdSelect_Prs;
class StdSelect_Shape;


//! The StdSelect package provides the following services
//! -   the definition of selection modes for topological shapes
//! -   the definition of several concrete filtertandard
//! Selection2d.ap classes
//! -   2D and 3D viewer selectors.
//! Note that each new Interactive Object must have all
//! its selection modes defined.
//! Standard Classes is useful to build
//! 3D Selectable Objects, and to process
//! 3D Selections:
//!
//! - Implementation of View Selector for dynamic selection
//! in Views from V3d.
//!
//! - Implementation of Tool class to decompose 3D BRep Objects
//! into sensitive Primitives for every desired mode of selection
//! (selection of vertex,edges,wires,faces,...)
//!
//! -  Implementation of dedicated Sensitives Entities:
//! Text for 2D Views (linked to Specific 2D projectors.)
class StdSelect 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! puts The same drawer in every BRepOwner Of SensitivePrimitive
  //! Used Only for hilight Of BRepOwner...
  Standard_EXPORT static void SetDrawerForBRepOwner (const Handle(SelectMgr_Selection)& aSelection, const Handle(Prs3d_Drawer)& aDrawer);




protected:





private:




friend class StdSelect_BRepSelectionTool;
friend class StdSelect_BRepOwner;
friend class StdSelect_EdgeFilter;
friend class StdSelect_FaceFilter;
friend class StdSelect_ShapeTypeFilter;
friend class StdSelect_Prs;
friend class StdSelect_Shape;

};







#endif // _StdSelect_HeaderFile
