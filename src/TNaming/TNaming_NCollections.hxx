// Created on: 2010-03-25
// Created by: Sergey ZARITCHNY
// Copyright (c) 2010-2012 OPEN CASCADE SAS
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



#ifndef TNaming_NCollections_HeaderFile
#define TNaming_NCollections_HeaderFile
#include <NCollection_Map.hxx>   
#include <NCollection_DataMap.hxx>
typedef NCollection_Map<TopoDS_Shape> TNaming_MapOfShape; 
typedef TNaming_MapOfShape::Iterator TNaming_MapIteratorOfMapOfShape;
typedef NCollection_DataMap<TopoDS_Shape, TNaming_MapOfShape> TNaming_DataMapOfShapeMapOfShape; 
typedef TNaming_DataMapOfShapeMapOfShape::Iterator TNaming_DataMapIteratorOfDataMapOfShapeMapOfShape; 
//=======================================================================
//function : NCollection => IsEqual
//=======================================================================
Standard_Boolean IsEqual (const TopoDS_Shape& S1, const TopoDS_Shape& S2) 
{
  return S1.IsEqual(S2);
}

#endif
