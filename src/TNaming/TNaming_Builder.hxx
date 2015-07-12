// Created on: 1996-12-16
// Created by: Remi Lequette
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _TNaming_Builder_HeaderFile
#define _TNaming_Builder_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

class TNaming_UsedShapes;
class TNaming_NamedShape;
class Standard_ConstructionError;
class TDF_Label;
class TopoDS_Shape;


//! A tool to create and maintain topological attributes.
//! Constructor creates an empty
//! TNaming_NamedShape attribute at the given
//! label. It allows adding "old shape" and "new
//! shape" pairs with the specified evolution to this
//! named shape. One evolution type per one
//! builder must be used.
class TNaming_Builder 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Create an   Builder.
  //! Warning:  Before Addition copies the current Value, and clear
  Standard_EXPORT TNaming_Builder(const TDF_Label& aLabel);
  
  //! Records the shape newShape which was
  //! generated during a topological construction.
  //! As an example, consider the case of a face
  //! generated in construction of a box.
  Standard_EXPORT void Generated (const TopoDS_Shape& newShape);
  
  //! Records the shape newShape which was
  //! generated from the shape oldShape during a topological construction.
  //! As an example, consider the case of a face
  //! generated from an edge in construction of a prism.
  Standard_EXPORT void Generated (const TopoDS_Shape& oldShape, const TopoDS_Shape& newShape);
  
  //! Records the shape oldShape which was deleted from the current label.
  //! As an example, consider the case of a face removed by a Boolean operation.
  Standard_EXPORT void Delete (const TopoDS_Shape& oldShape);
  
  //! Records the shape newShape which is a
  //! modification of the shape oldShape.
  //! As an example, consider the case of a face split
  //! or merged in a Boolean operation.
  Standard_EXPORT void Modify (const TopoDS_Shape& oldShape, const TopoDS_Shape& newShape);
  
  //! Add a  Shape to the current label ,  This Shape is
  //! unmodified.  Used for example  to define a set
  //! of shapes under a label.
  Standard_EXPORT void Select (const TopoDS_Shape& aShape, const TopoDS_Shape& inShape);
  
  //! Returns the NamedShape which has been built or is under construction.
  Standard_EXPORT Handle(TNaming_NamedShape) NamedShape() const;




protected:





private:



  Handle(TNaming_UsedShapes) myShapes;
  Handle(TNaming_NamedShape) myAtt;


};







#endif // _TNaming_Builder_HeaderFile
