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



//  Modified by skv - Tue Mar 15 16:20:43 2005
// Add methods for supporting history.

#include <BRepOffsetAPI_MakeOffsetShape.ixx>
#include <BRepOffset_MakeOffset.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <Standard_ConstructionError.hxx>

//=======================================================================
//function : BRepOffsetAPI_MakeOffsetShape
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeOffsetShape::BRepOffsetAPI_MakeOffsetShape()
{
}

//=======================================================================
//function : BRepOffsetAPI_MakeOffsetShape
//purpose  : 
//=======================================================================

BRepOffsetAPI_MakeOffsetShape::BRepOffsetAPI_MakeOffsetShape
(const TopoDS_Shape&    S, 
 const Standard_Real    Offset, 
 const Standard_Real    Tol, 
 const BRepOffset_Mode  Mode, 
 const Standard_Boolean Intersection,
 const Standard_Boolean SelfInter,
 const GeomAbs_JoinType Join)
{
  myOffsetShape.Initialize (S,Offset,Tol,Mode,Intersection,SelfInter,Join);
  Build();
}


//=======================================================================
//function :MakeOffset
//purpose  : 
//=======================================================================

const BRepOffset_MakeOffset& BRepOffsetAPI_MakeOffsetShape::MakeOffset() const 
{
  return myOffsetShape;
}

//=======================================================================
//function : Build
//purpose  : 
//=======================================================================

void BRepOffsetAPI_MakeOffsetShape::Build()
{
  if (!IsDone()) {
    myOffsetShape.MakeOffsetShape();
    if (!myOffsetShape.IsDone()) return;
    myShape  = myOffsetShape.Shape();
    Done();
  }
}


//=======================================================================
//function : Generated
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffsetAPI_MakeOffsetShape::Generated (const TopoDS_Shape& S) 

{  
  myGenerated.Clear();
  if (!myOffsetShape.ClosingFaces().Contains(S)) {
    myOffsetShape.OffsetFacesFromShapes ().LastImage (S, myGenerated);
   
    if (!myOffsetShape.ClosingFaces().IsEmpty()) {
      // Reverse generated shapes in case of small solids.
      // Useful only for faces without influence on others.
      TopTools_ListIteratorOfListOfShape it(myGenerated);
      for (; it.More(); it.Next())
	it.Value().Reverse();
    }
  }
  return myGenerated;
}


//  Modified by skv - Tue Mar 15 16:20:43 2005 Begin

//=======================================================================
//function : GeneratedEdge
//purpose  : 
//=======================================================================

const TopTools_ListOfShape& BRepOffsetAPI_MakeOffsetShape::GeneratedEdge (const TopoDS_Shape& S) 

{  
  myGenerated.Clear();
  myOffsetShape.OffsetEdgesFromShapes ().LastImage (S, myGenerated);

  if (!myGenerated.IsEmpty()) {
    if (S.IsSame(myGenerated.First()))
      myGenerated.RemoveFirst();
  }

  return myGenerated;
}


//=======================================================================
//function : GetJoinType
//purpose  : Query offset join type.
//=======================================================================

GeomAbs_JoinType BRepOffsetAPI_MakeOffsetShape::GetJoinType() const
{
  return myOffsetShape.GetJoinType();
}

//  Modified by skv - Tue Mar 15 16:20:43 2005 End
