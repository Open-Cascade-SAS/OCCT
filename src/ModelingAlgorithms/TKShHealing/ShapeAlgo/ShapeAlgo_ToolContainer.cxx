// Created on: 2000-02-07
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <ShapeAlgo_ToolContainer.hxx>
#include <ShapeFix_EdgeProjAux.hxx>
#include <ShapeFix_Shape.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(ShapeAlgo_ToolContainer, Standard_Transient)

//=================================================================================================

ShapeAlgo_ToolContainer::ShapeAlgo_ToolContainer() {}

//=================================================================================================

Handle(ShapeFix_Shape) ShapeAlgo_ToolContainer::FixShape() const
{
  return new ShapeFix_Shape;
}

//=================================================================================================

Handle(ShapeFix_EdgeProjAux) ShapeAlgo_ToolContainer::EdgeProjAux() const
{
  return new ShapeFix_EdgeProjAux;
}
