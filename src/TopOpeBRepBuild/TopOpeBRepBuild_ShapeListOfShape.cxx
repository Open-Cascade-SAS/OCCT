// Created on: 1997-06-26
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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


#include <TopOpeBRepBuild_ShapeListOfShape.ixx>

#define MTTdmiomoslos TopTools_DataMapIteratorOfDataMapOfShapeListOfShape
#define MTTdmoslos TopTools_DataMapOfShapeListOfShape
#define MTTmiomos TopTools_MapIteratorOfMapOfShape
#define MTTmos TopTools_MapOfShape
#define MTTliolos TopTools_ListIteratorOfListOfShape
#define MTTlos TopTools_ListOfShape
#define MTs TopoDS_Shape
#define MTf TopoDS_Face
#define MTe TopoDS_Edge
#define MTv TopoDS_Vertex
#define MDSlioloi TopOpeBRepDS_ListIteratorOfListOfInterference
#define MDSloi TopOpeBRepDS_ListOfInterference
#define MDShi Handle(TopOpeBRepDS_Interference)
#define MDSi TopOpeBRepDS_Interference
#define MDShssi Handle(TopOpeBRepDS_ShapeShapeInterference)
#define MDSssi TopOpeBRepDS_ShapeShapeInterference
#define MDSii TopOpeBRepDS_InterferenceIterator
#define MDShds Handle(TopOpeBRepDS_HDataStructure)
#define MDSds TopOpeBRepDS_DataStructure
#define MDSk TopOpeBRepDS_Kind
#define MDSsd TopOpeBRepDS_ShapeData
#define MDSmosd TopOpeBRepDS_MapOfShapeData
#define Msr Standard_Real
#define Msi Standard_Integer
#define Msb Standard_Boolean
#define Mso Standard_Ostream
#define Mtcas TCollection_AsciiString

//=======================================================================
//function : TopOpeBRepBuild_ShapeListOfShape
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ShapeListOfShape::TopOpeBRepBuild_ShapeListOfShape()
{
}

//=======================================================================
//function : TopOpeBRepBuild_ShapeListOfShape
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ShapeListOfShape::TopOpeBRepBuild_ShapeListOfShape
(const TopoDS_Shape& S)
{
  myShape = S;
}

//=======================================================================
//function : TopOpeBRepBuild_ShapeListOfShape
//purpose  : 
//=======================================================================

TopOpeBRepBuild_ShapeListOfShape::TopOpeBRepBuild_ShapeListOfShape
(const TopoDS_Shape& S, const TopTools_ListOfShape& L)
{
  myShape = S;
  myList = L;
}

//=======================================================================
//function : List
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& TopOpeBRepBuild_ShapeListOfShape::List() const
{
  return myList;
}

//=======================================================================
//function : ChangeList
//purpose  : 
//=======================================================================

TopTools_ListOfShape& TopOpeBRepBuild_ShapeListOfShape::ChangeList()
{
  return myList;
}

//=======================================================================
//function : Shape
//purpose  : 
//=======================================================================

const TopoDS_Shape& TopOpeBRepBuild_ShapeListOfShape::Shape() const
{
  return myShape;
}

//=======================================================================
//function : ChangeShape
//purpose  : 
//=======================================================================

TopoDS_Shape& TopOpeBRepBuild_ShapeListOfShape::ChangeShape()
{
  return myShape;
}

