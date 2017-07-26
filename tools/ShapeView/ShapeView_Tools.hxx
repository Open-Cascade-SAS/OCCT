// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef ShapeView_Tools_H
#define ShapeView_Tools_H

#include <Standard.hxx>

#include <TCollection_AsciiString.hxx>
#include <Standard_Transient.hxx>
#include <TopoDS_Shape.hxx>

//! \class ShapeView_Tools
//! It gives auxiliary methods for TopoDS_Shape manipulation
class ShapeView_Tools
{
public:

  //! Read Shape using BREP reader
  //! \param theFileName a file name
  //! \return shape or NULL
  Standard_EXPORT static TopoDS_Shape ReadShape (const TCollection_AsciiString& theFileName);

  //! Creates AIS_Shape for the shape
  //! \param theShape a shape
  //! \return presentation
  Standard_EXPORT static Handle(Standard_Transient) CreatePresentation (const TopoDS_Shape& theShape);
};

#endif
