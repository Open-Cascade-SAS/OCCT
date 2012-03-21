// Created on: 1999-10-13
// Created by: Atelier CAS2000
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <BRepBuilderAPI.ixx>

#include <BRepLib.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepTools.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Compound.hxx>
#include <gp.hxx>


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
