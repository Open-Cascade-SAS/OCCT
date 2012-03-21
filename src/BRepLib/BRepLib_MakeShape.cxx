// Created on: 1993-07-23
// Created by: Remi LEQUETTE
// Copyright (c) 1993-1999 Matra Datavision
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



#include <BRepLib_MakeShape.ixx>

//=======================================================================
//function : BRepLib_MakeShape
//purpose  : 
//=======================================================================

BRepLib_MakeShape::BRepLib_MakeShape()
{
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepLib_MakeShape::Build()
{
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape&  BRepLib_MakeShape::Shape() const
{
  if (!IsDone()) {
    // the following is const cast away
    ((BRepLib_MakeShape*) (void*) this)->Build();
    Check();
  }
  return myShape;
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================

BRepLib_MakeShape::operator TopoDS_Shape() const
{
  return Shape();
}



//=======================================================================
//function : HasDescendants
//purpose  : 
//=======================================================================

Standard_Boolean BRepLib_MakeShape::HasDescendants(const TopoDS_Face&)const
{
  return (Standard_True);
}



//=======================================================================
//function : FaceStatus
//purpose  : 
//=======================================================================

BRepLib_ShapeModification BRepLib_MakeShape::FaceStatus
  (const TopoDS_Face&) const
{
  BRepLib_ShapeModification myStatus = BRepLib_Trimmed;
  return myStatus;
}


//=======================================================================
//function : GeneratedFaces
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepLib_MakeShape::DescendantFaces
  (const TopoDS_Face&)
{
  return myGenFaces;
}

//=======================================================================
//function : NbSurfaces
//purpose  : 
//=======================================================================

Standard_Integer BRepLib_MakeShape::NbSurfaces() const
{
  return (0);
}

//=======================================================================
//function : NewFaces
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepLib_MakeShape::NewFaces(const Standard_Integer)
{
  return myNewFaces;
}


//=======================================================================
//function : FacesFromEdges
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepLib_MakeShape::FacesFromEdges
  (const TopoDS_Edge&)
{
  return myEdgFaces;
}
