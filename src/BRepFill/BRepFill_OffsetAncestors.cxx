// File:	BRepFill_OffsetAncestors.cxx
// Created:	Fri Sep  1 14:32:44 1995
// Author:	Bruno DUMORTIER
//		<dub@fuegox>


#include <BRepFill_OffsetAncestors.ixx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Face.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>

//=======================================================================
//function : BRepFill_OffsetAncestors
//purpose  : 
//=======================================================================

BRepFill_OffsetAncestors::BRepFill_OffsetAncestors()
:myIsPerform(Standard_False)
{
}


//=======================================================================
//function : BRepFill_OffsetAncestors
//purpose  : 
//=======================================================================

BRepFill_OffsetAncestors::BRepFill_OffsetAncestors
(BRepFill_OffsetWire& Paral)
{
  Perform(Paral);
}


//=======================================================================
//function : Perform
//purpose  : 
//=======================================================================

void BRepFill_OffsetAncestors::Perform(BRepFill_OffsetWire& Paral)
{
  TopoDS_Face Spine = Paral.Spine();
  
  TopExp_Explorer                    Exp;
  TopTools_ListIteratorOfListOfShape it;

  // on itere sur les edges.
  for ( Exp.Init(Spine, TopAbs_EDGE); Exp.More(); Exp.Next()) {
    for (it.Initialize(Paral.GeneratedShapes(Exp.Current()));
	 it.More(); it.Next()) {
      myMap.Bind( it.Value(), Exp.Current());
    }
  }

  // on itere sur les vertex.
  for ( Exp.Init(Spine, TopAbs_VERTEX); Exp.More(); Exp.Next()) {
    for (it.Initialize(Paral.GeneratedShapes(Exp.Current()));
	 it.More(); it.Next()) {
      myMap.Bind( it.Value(), Exp.Current());
    }
  }

  myIsPerform = Standard_True;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_OffsetAncestors::IsDone() const
{
  return myIsPerform;
}

//=======================================================================
//function : IsDone
//purpose  : 
//=======================================================================

Standard_Boolean BRepFill_OffsetAncestors:: HasAncestor(const TopoDS_Edge& S1)
const
{
  return myMap.IsBound(S1);
}

//=======================================================================
//function : TopoDS_Shape&
//purpose  : 
//=======================================================================

const TopoDS_Shape& BRepFill_OffsetAncestors::Ancestor(const TopoDS_Edge& S1)
const
{
  StdFail_NotDone_Raise_if( !myIsPerform, "");
  return myMap(S1);
}
