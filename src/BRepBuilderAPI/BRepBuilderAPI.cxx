// Created on: 1999-10-13
// Created by: Atelier CAS2000
// Copyright (c) 1999-1999 Matra Datavision
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
#include <BRep_Tool.hxx>
#include <BRepBuilderAPI.hxx>
#include <BRepLib.hxx>
#include <BRepTools.hxx>
#include <Geom_Plane.hxx>
#include <gp.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>

//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================
void  BRepBuilderAPI::Plane(const Handle(Geom_Plane)& P)
{
  BRepLib::Plane(P);
}


//=======================================================================
//function : Plane
//purpose  : 
//=======================================================================

const Handle(Geom_Plane)&  BRepBuilderAPI::Plane()
{
  return BRepLib::Plane();
}


//=======================================================================
//function : Precision
//purpose  : 
//=======================================================================

void  BRepBuilderAPI::Precision(const Standard_Real P)
{
  BRepLib::Precision(P);
}


//=======================================================================
//function : Precision
//purpose  : 
//=======================================================================

Standard_Real  BRepBuilderAPI::Precision()
{
  return BRepLib::Precision();
}
