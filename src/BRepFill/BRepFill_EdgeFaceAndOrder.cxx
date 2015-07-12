// Created on: 1998-10-02
// Created by: Julia GERASIMOVA
// Copyright (c) 1998-1999 Matra Datavision
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


#include <BRepFill_EdgeFaceAndOrder.hxx>
#include <BRepFill_Filling.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>

BRepFill_EdgeFaceAndOrder::BRepFill_EdgeFaceAndOrder()
{
}

BRepFill_EdgeFaceAndOrder::BRepFill_EdgeFaceAndOrder( const TopoDS_Edge& anEdge,
						      const TopoDS_Face& aFace,
						      const GeomAbs_Shape anOrder )
{
  myEdge  = anEdge;
  myFace  = aFace;
  myOrder = anOrder;
}
