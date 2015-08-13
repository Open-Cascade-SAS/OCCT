// Created on: 1993-07-23
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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


#include <BRep_Builder.hxx>
#include <BRepLib.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

//=======================================================================
//function : BRepLib_MakeVertex
//purpose  : 
//=======================================================================
BRepLib_MakeVertex::BRepLib_MakeVertex(const gp_Pnt& P)
{
  BRep_Builder B;
  B.MakeVertex(TopoDS::Vertex(myShape),P,BRepLib::Precision());
  Done();
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepLib_MakeVertex::Vertex() 
{
  return TopoDS::Vertex(Shape());
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================


BRepLib_MakeVertex::operator TopoDS_Vertex()
{
  return Vertex();
}


