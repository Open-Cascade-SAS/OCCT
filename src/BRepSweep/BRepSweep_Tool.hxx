// Created on: 1993-06-08
// Created by: Laurent BOURESCHE
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

#ifndef _BRepSweep_Tool_HeaderFile
#define _BRepSweep_Tool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <TopTools_IndexedMapOfShape.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>
class TopoDS_Shape;


//! Provides  the  indexation and type  analysis  services
//! required by the TopoDS generating Shape of BRepSweep.
class BRepSweep_Tool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Initialize the tool  with <aShape>.  The IndexTool
  //! must prepare an indexation for  all  the subshapes
  //! of this shape.
  Standard_EXPORT BRepSweep_Tool(const TopoDS_Shape& aShape);
  
  //! Returns the number of subshapes in the shape.
  Standard_EXPORT Standard_Integer NbShapes() const;
  
  //! Returns the index of <aShape>.
  Standard_EXPORT Standard_Integer Index (const TopoDS_Shape& aShape) const;
  
  //! Returns the Shape at Index anIdex.
  Standard_EXPORT TopoDS_Shape Shape (const Standard_Integer anIndex) const;
  
  //! Returns the type of <aShape>.
  Standard_EXPORT TopAbs_ShapeEnum Type (const TopoDS_Shape& aShape) const;
  
  //! Returns the Orientation of <aShape>.
  Standard_EXPORT TopAbs_Orientation Orientation (const TopoDS_Shape& aShape) const;
  
  //! Set the Orientation of <aShape> with Or.
  Standard_EXPORT void SetOrientation (TopoDS_Shape& aShape, const TopAbs_Orientation Or) const;




protected:





private:



  TopTools_IndexedMapOfShape myMap;


};







#endif // _BRepSweep_Tool_HeaderFile
