// Created on: 2000-05-25
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

#ifndef _ShapeUpgrade_ShapeDivideClosedEdges_HeaderFile
#define _ShapeUpgrade_ShapeDivideClosedEdges_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>

#include <ShapeUpgrade_ShapeDivide.hxx>
#include <Standard_Integer.hxx>
class TopoDS_Shape;

class ShapeUpgrade_ShapeDivideClosedEdges : public ShapeUpgrade_ShapeDivide
{
public:
  DEFINE_STANDARD_ALLOC

  //! Initialises tool with shape and default parameter.
  Standard_EXPORT ShapeUpgrade_ShapeDivideClosedEdges(const TopoDS_Shape& S);

  //! Sets the number of cuts applied to divide closed edges.
  //! The number of resulting faces will be num+1.
  Standard_EXPORT void SetNbSplitPoints(const Standard_Integer num);

protected:
private:
};

#endif // _ShapeUpgrade_ShapeDivideClosedEdges_HeaderFile
