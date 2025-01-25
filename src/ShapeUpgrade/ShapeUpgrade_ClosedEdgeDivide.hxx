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

#ifndef _ShapeUpgrade_ClosedEdgeDivide_HeaderFile
#define _ShapeUpgrade_ClosedEdgeDivide_HeaderFile

#include <Standard.hxx>
#include <Standard_Type.hxx>

#include <ShapeUpgrade_EdgeDivide.hxx>
class TopoDS_Edge;

class ShapeUpgrade_ClosedEdgeDivide;
DEFINE_STANDARD_HANDLE(ShapeUpgrade_ClosedEdgeDivide, ShapeUpgrade_EdgeDivide)

class ShapeUpgrade_ClosedEdgeDivide : public ShapeUpgrade_EdgeDivide
{

public:
  //! Empty constructor.
  Standard_EXPORT ShapeUpgrade_ClosedEdgeDivide();

  Standard_EXPORT virtual Standard_Boolean Compute(const TopoDS_Edge& anEdge) Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(ShapeUpgrade_ClosedEdgeDivide, ShapeUpgrade_EdgeDivide)

protected:
private:
};

#endif // _ShapeUpgrade_ClosedEdgeDivide_HeaderFile
