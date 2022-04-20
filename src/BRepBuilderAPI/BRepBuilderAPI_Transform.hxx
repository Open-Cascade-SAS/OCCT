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

#ifndef _BRepBuilderAPI_Transform_HeaderFile
#define _BRepBuilderAPI_Transform_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <gp_Trsf.hxx>
#include <BRepBuilderAPI_ModifyShape.hxx>
#include <TopTools_ListOfShape.hxx>
class TopoDS_Shape;


//! Geometric transformation on a shape.
//! The transformation to be applied is defined as a
//! gp_Trsf transformation, i.e. a transformation which does
//! not modify the underlying geometry of shapes.
//! The transformation is applied to:
//! -   all curves which support edges of a shape, and
//! -   all surfaces which support its faces.
//! A Transform object provides a framework for:
//! -   defining the geometric transformation to be applied,
//! -   implementing the transformation algorithm, and
//! -   consulting the results.
class BRepBuilderAPI_Transform  : public BRepBuilderAPI_ModifyShape
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Constructs a framework for applying the geometric
  //! transformation T to a shape. Use the function Perform
  //! to define the shape to transform.
  Standard_EXPORT BRepBuilderAPI_Transform(const gp_Trsf& T);
  
  //! Creates a transformation from the gp_Trsf <theTrsf>, and
  //! applies it to the shape <theShape>. If the transformation
  //! is  direct   and isometric (determinant  =  1) and
  //! <theCopyGeom> =  Standard_False,  the resulting shape  is
  //! <theShape> on   which  a  new  location has    been  set.
  //! Otherwise,  the   transformation is applied   on a
  //! duplication of <theShape>.
  //! If <theCopyMesh> is true, the triangulation will be copied,
  //! and the copy will be assigned to the result shape.
  Standard_EXPORT BRepBuilderAPI_Transform(const TopoDS_Shape&    theShape,
                                           const gp_Trsf&         theTrsf,
                                           const Standard_Boolean theCopyGeom = Standard_False,
                                           const Standard_Boolean theCopyMesh = Standard_False);
  
  //! Applies the geometric transformation defined at the
  //! time of construction of this framework to the shape S.
  //! - If the transformation T is direct and isometric, in
  //! other words, if the determinant of the vectorial part
  //! of T is equal to 1., and if theCopyGeom equals false (the
  //! default value), the resulting shape is the same as
  //! the original but with a new location assigned to it.
  //! - In all other cases, the transformation is applied to a duplicate of theShape.
  //! - If theCopyMesh is true, the triangulation will be copied,
  //! and the copy will be assigned to the result shape.
  //! Use the function Shape to access the result.
  //! Note: this framework can be reused to apply the same
  //! geometric transformation to other shapes. You only
  //! need to specify them by calling the function Perform again.
  Standard_EXPORT void Perform (const TopoDS_Shape&    theShape,
                                const Standard_Boolean theCopyGeom = Standard_False, 
                                const Standard_Boolean theCopyMesh = Standard_False);
  
  //! Returns the modified shape corresponding to <S>.
  Standard_EXPORT virtual TopoDS_Shape ModifiedShape (const TopoDS_Shape& S) const Standard_OVERRIDE;
  
  //! Returns the list  of shapes modified from the shape
  //! <S>.
  Standard_EXPORT virtual const TopTools_ListOfShape& Modified (const TopoDS_Shape& S) Standard_OVERRIDE;




protected:





private:



  gp_Trsf myTrsf;
  TopLoc_Location myLocation;
  Standard_Boolean myUseModif;


};







#endif // _BRepBuilderAPI_Transform_HeaderFile
