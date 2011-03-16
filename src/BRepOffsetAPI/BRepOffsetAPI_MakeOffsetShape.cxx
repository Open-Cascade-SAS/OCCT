// File:	BRepOffsetAPI_MakeOffsetShape.cxx
// Created:	Tue Feb 13 14:21:59 1996
// Author:	Yves FRICAUD
//		<yfr@stylox>


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
      // Reverse les Shape generes  dans le cas des solides minces.
      // Utile seulement pour les faces mais sans incidence sur les autres.
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
