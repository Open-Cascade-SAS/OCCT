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

#include <inspector/ShapeView_Tools.hxx>

#include <BRep_Builder.hxx>
#include <BRepTools.hxx>

#include <AIS_Shape.hxx>
// =======================================================================
// function : ReadShape
// purpose :
// =======================================================================
TopoDS_Shape ShapeView_Tools::ReadShape (const TCollection_AsciiString& theFileName)
{
  TopoDS_Shape aShape;

  BRep_Builder aBuilder;
  BRepTools::Read (aShape, theFileName.ToCString(), aBuilder);
  return aShape;
}

// =======================================================================
// function : CreatePresentation
// purpose :
// =======================================================================
Handle(Standard_Transient) ShapeView_Tools::CreatePresentation (const TopoDS_Shape& theShape)
{
  return new AIS_Shape (theShape);
}
