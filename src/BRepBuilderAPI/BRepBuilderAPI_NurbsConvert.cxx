// Created on: 1994-12-09
// Created by: Jacques GOUSSARD
// Copyright (c) 1994-1999 Matra Datavision
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
#include <BRepBuilderAPI_NurbsConvert.hxx>
#include <BRepLib.hxx>
#include <BRepTools_NurbsConvertModification.hxx>
#include <TopAbs.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>
#include <TopTools_MapOfShape.hxx>

//#include <gp.hxx>
//=======================================================================
//function : BRepBuilderAPI_NurbsConvert
//purpose  : 
//=======================================================================
BRepBuilderAPI_NurbsConvert::BRepBuilderAPI_NurbsConvert () 
     
{
  myModification = new BRepTools_NurbsConvertModification();
}

//=======================================================================
//function : BRepBuilderAPI_NurbsConvert
//purpose  : 
//=======================================================================

BRepBuilderAPI_NurbsConvert::BRepBuilderAPI_NurbsConvert (const TopoDS_Shape& S,
                                                          const Standard_Boolean Copy) 
     
{
  myModification = new BRepTools_NurbsConvertModification();
  Perform(S,Copy);
}

//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepBuilderAPI_NurbsConvert::Perform(const TopoDS_Shape& S,
                                          const Standard_Boolean /*Copy*/)
{
  Handle(BRepTools_NurbsConvertModification) theModif = 
    Handle(BRepTools_NurbsConvertModification)::DownCast(myModification);
  DoModif(S,myModification);
}



