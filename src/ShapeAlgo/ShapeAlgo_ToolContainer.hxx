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

#ifndef _ShapeAlgo_ToolContainer_HeaderFile
#define _ShapeAlgo_ToolContainer_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <Standard_Transient.hxx>
class ShapeFix_Shape;
class ShapeFix_EdgeProjAux;

class ShapeAlgo_ToolContainer;
DEFINE_STANDARD_HANDLE(ShapeAlgo_ToolContainer, Standard_Transient)

//! Returns tools used by AlgoContainer
class ShapeAlgo_ToolContainer : public Standard_Transient
{

public:
  //! Empty constructor
  Standard_EXPORT ShapeAlgo_ToolContainer();

  //! Returns ShapeFix_Shape
  Standard_EXPORT virtual Handle(ShapeFix_Shape) FixShape() const;

  //! Returns ShapeFix_EdgeProjAux
  Standard_EXPORT virtual Handle(ShapeFix_EdgeProjAux) EdgeProjAux() const;

  DEFINE_STANDARD_RTTIEXT(ShapeAlgo_ToolContainer, Standard_Transient)

protected:
private:
};

#endif // _ShapeAlgo_ToolContainer_HeaderFile
