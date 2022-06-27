// Created on: 1993-06-02
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

#ifndef _Sweep_NumShapeTool_HeaderFile
#define _Sweep_NumShapeTool_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <Sweep_NumShape.hxx>
#include <Standard_Integer.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopAbs_Orientation.hxx>


//! This class provides  the indexation and  type analysis
//! services required by  the NumShape Directing Shapes of
//! Swept Primitives.
class Sweep_NumShapeTool 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Create a new NumShapeTool with <aShape>.  The Tool
  //! must prepare an indexation  for  all the subshapes
  //! of this shape.
  Standard_EXPORT Sweep_NumShapeTool(const Sweep_NumShape& aShape);
  
  //! Returns the number of subshapes in the shape.
  Standard_EXPORT Standard_Integer NbShapes() const;
  
  //! Returns the index of <aShape>.
  Standard_EXPORT Standard_Integer Index (const Sweep_NumShape& aShape) const;
  
  //! Returns the Shape at index anIndex
  Standard_EXPORT Sweep_NumShape Shape (const Standard_Integer anIndex) const;
  
  //! Returns the type of <aShape>.
  Standard_EXPORT TopAbs_ShapeEnum Type (const Sweep_NumShape& aShape) const;
  
  //! Returns the orientation of <aShape>.
  Standard_EXPORT TopAbs_Orientation Orientation (const Sweep_NumShape& aShape) const;
  
  //! Returns true if there is a First Vertex in the Shape.
  Standard_EXPORT Standard_Boolean HasFirstVertex() const;
  
  //! Returns true if there is a Last Vertex in the Shape.
  Standard_EXPORT Standard_Boolean HasLastVertex() const;
  
  //! Returns the first vertex.
  Standard_EXPORT Sweep_NumShape FirstVertex() const;
  
  //! Returns the last vertex.
  Standard_EXPORT Sweep_NumShape LastVertex() const;




protected:





private:



  Sweep_NumShape myNumShape;


};







#endif // _Sweep_NumShapeTool_HeaderFile
