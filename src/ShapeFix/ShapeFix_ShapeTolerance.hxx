// Created on: 1998-07-22
// Created by: data exchange team
// Copyright (c) 1998-1999 Matra Datavision
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

#ifndef _ShapeFix_ShapeTolerance_HeaderFile
#define _ShapeFix_ShapeTolerance_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_Boolean.hxx>
#include <Standard_Real.hxx>
#include <TopAbs_ShapeEnum.hxx>
class TopoDS_Shape;


//! Modifies tolerances of sub-shapes (vertices, edges, faces)
class ShapeFix_ShapeTolerance 
{
public:

  DEFINE_STANDARD_ALLOC

  
  Standard_EXPORT ShapeFix_ShapeTolerance();
  
  //! Limits tolerances in a shape as follows :
  //! tmin = tmax -> as SetTolerance (forces)
  //! tmin = 0   -> maximum tolerance will be <tmax>
  //! tmax = 0 or not given (more generally, tmax < tmin) ->
  //! <tmax> ignored, minimum will be <tmin>
  //! else, maximum will be <max> and minimum will be <min>
  //! styp = VERTEX : only vertices are set
  //! styp = EDGE   : only edges are set
  //! styp = FACE   : only faces are set
  //! styp = WIRE   : to have edges and their vertices set
  //! styp = other value : all (vertices,edges,faces) are set
  //! Returns True if at least one tolerance of the sub-shape has
  //! been modified
  Standard_EXPORT Standard_Boolean LimitTolerance (const TopoDS_Shape& shape, const Standard_Real tmin, const Standard_Real tmax = 0.0, const TopAbs_ShapeEnum styp = TopAbs_SHAPE) const;
  
  //! Sets (enforces) tolerances in a shape to the given value
  //! styp = VERTEX : only vertices are set
  //! styp = EDGE   : only edges are set
  //! styp = FACE   : only faces are set
  //! styp = WIRE   : to have edges and their vertices set
  //! styp = other value : all (vertices,edges,faces) are set
  Standard_EXPORT void SetTolerance (const TopoDS_Shape& shape, const Standard_Real preci, const TopAbs_ShapeEnum styp = TopAbs_SHAPE) const;




protected:





private:





};







#endif // _ShapeFix_ShapeTolerance_HeaderFile
