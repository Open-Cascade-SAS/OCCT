// Created on: 1994-12-09
// Created by: Jacques GOUSSARD
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

#ifndef _BRepBuilderAPI_NurbsConvert_HeaderFile
#define _BRepBuilderAPI_NurbsConvert_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <BRepBuilderAPI_ModifyShape.hxx>
#include <Standard_Boolean.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <BRepTools_ReShape.hxx>

class TopoDS_Shape;


//! Conversion of the complete geometry of a shape
//! (all 3D analytical representation of surfaces and curves)
//! into NURBS geometry (execpt for Planes). For example,
//! all curves supporting edges of the basis shape are converted
//! into BSpline curves, and all surfaces supporting its faces are
//! converted into BSpline surfaces.
class BRepBuilderAPI_NurbsConvert  : public BRepBuilderAPI_ModifyShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a framework for converting the geometry of a
  //! shape into NURBS geometry. Use the function Perform
  //! to define the shape to convert.
  Standard_EXPORT BRepBuilderAPI_NurbsConvert();
  
  //! Builds a new shape by converting the geometry of the
  //! shape S into NURBS geometry. Specifically, all curves
  //! supporting edges of S are converted into BSpline
  //! curves, and all surfaces supporting its faces are
  //! converted into BSpline surfaces.
  //! Use the function Shape to access the new shape.
  //! Note: the constructed framework can be reused to
  //! convert other shapes. You specify these with the
  //! function Perform.
  Standard_EXPORT BRepBuilderAPI_NurbsConvert(const TopoDS_Shape& S, const Standard_Boolean Copy = Standard_False);
  
  //! Builds a new shape by converting the geometry of the
  //! shape S into NURBS geometry.
  //! Specifically, all curves supporting edges of S are
  //! converted into BSpline curves, and all surfaces
  //! supporting its faces are converted into BSpline surfaces.
  //! Use the function Shape to access the new shape.
  //! Note: this framework can be reused to convert other
  //! shapes: you specify them by calling the function Perform again.
  Standard_EXPORT void Perform (const TopoDS_Shape& S, const Standard_Boolean Copy = Standard_False);


  //! Returns the list  of shapes modified from the shape
  //! <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& S);
  
  //! Returns the modified shape corresponding to <S>.
  //! S can correspond to the entire initial shape or to its subshape.
  //! Exceptions
  //! Standard_NoSuchObject if S is not the initial shape or
  //! a subshape of the initial shape to which the
  //! transformation has been applied. 
  Standard_EXPORT virtual TopoDS_Shape ModifiedShape (const TopoDS_Shape& S) const;


protected:





private:

  Standard_EXPORT void CorrectVertexTol();

  TopTools_DataMapOfShapeShape myVtxToReplace;
  BRepTools_ReShape mySubs;

};







#endif // _BRepBuilderAPI_NurbsConvert_HeaderFile
