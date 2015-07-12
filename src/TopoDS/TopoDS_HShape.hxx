// Created on: 1995-06-28
// Created by: Modelistation
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

#ifndef _TopoDS_HShape_HeaderFile
#define _TopoDS_HShape_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <TopoDS_Shape.hxx>
#include <MMgt_TShared.hxx>
class TopoDS_Shape;


class TopoDS_HShape;
DEFINE_STANDARD_HANDLE(TopoDS_HShape, MMgt_TShared)

//! Class to manipulate a Shape with  handle.
class TopoDS_HShape : public MMgt_TShared
{

public:

  
  //! Constructs an empty shape object
    TopoDS_HShape();
  
  //! Constructs a shape object defined by the shape aShape.
    TopoDS_HShape(const TopoDS_Shape& aShape);
  
  //! Loads this shape with the shape aShape
    void Shape (const TopoDS_Shape& aShape);
  
  //! Returns a reference to a constant TopoDS_Shape based on this shape.
    const TopoDS_Shape& Shape() const;
  

  //! Exchanges the TopoDS_Shape object defining this
  //! shape for another one referencing the same underlying shape
  //! Accesses the list of shapes within the underlying
  //! shape referenced by the TopoDS_Shape object.
  //! Returns a reference to a TopoDS_Shape based on
  //! this shape. The TopoDS_Shape can be modified.
    TopoDS_Shape& ChangeShape();




  DEFINE_STANDARD_RTTI(TopoDS_HShape,MMgt_TShared)

protected:




private:


  TopoDS_Shape myShape;


};


#include <TopoDS_HShape.lxx>





#endif // _TopoDS_HShape_HeaderFile
