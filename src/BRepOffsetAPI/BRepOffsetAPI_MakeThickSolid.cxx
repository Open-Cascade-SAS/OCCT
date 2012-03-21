// Created on: 1996-02-13
// Created by: Yves FRICAUD
// Copyright (c) 1996-1999 Matra Datavision
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



#include <BRepOffsetAPI_MakeThickSolid.ixx>
#include <BRepOffset_MakeOffset.hxx>
#include <Standard_ConstructionError.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopoDS.hxx>


//=======================================================================
//function : BRepOffsetAPI_MakeThickSolid
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeThickSolid::BRepOffsetAPI_MakeThickSolid()
{
}


//=======================================================================
//function : BRepOffsetAPI_MakeThickSolid
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeThickSolid::BRepOffsetAPI_MakeThickSolid
(const TopoDS_Shape&         S, 
 const TopTools_ListOfShape& ClosingFaces,
 const Standard_Real         Offset, 
 const Standard_Real         Tol, 
 const BRepOffset_Mode       Mode,
 const Standard_Boolean      Intersection,
 const Standard_Boolean      SelfInter,
 const GeomAbs_JoinType      Join)
{
  myOffsetShape.Initialize (S,Offset,Tol,Mode,Intersection,SelfInter,Join);
  TopTools_ListIteratorOfListOfShape it(ClosingFaces);
  for (; it.More(); it.Next()) {
    myOffsetShape.AddFace(TopoDS::Face(it.Value()));
  }
  Build();
}

//=======================================================================
//function : virtual
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeThickSolid::Build()
{
  if (!IsDone()) {
    myOffsetShape.MakeThickSolid();
    if (!myOffsetShape.IsDone()) return;
    myShape  = myOffsetShape.Shape();
    Done();
  }
}



//=======================================================================
//function : Modified
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffsetAPI_MakeThickSolid::Modified (const TopoDS_Shape& F) 

{
  myGenerated.Clear();
  if (myOffsetShape.OffsetFacesFromShapes().HasImage(F)) {
    if (myOffsetShape.ClosingFaces().Contains(F)) { 
      myOffsetShape.OffsetFacesFromShapes().LastImage (F, myGenerated); 
      // Les face du resultat sont orientees comme dans la piece initiale.
      //si offset a l interieur.
      TopTools_ListIteratorOfListOfShape it(myGenerated);
      for (; it.More(); it.Next())
	it.Value().Reverse();
   
    }
  }
  return myGenerated;
}
