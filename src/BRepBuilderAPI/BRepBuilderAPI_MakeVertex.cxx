// File:	BRepBuilderAPI_MakeVertex.cxx
// Created:	Fri Jul 23 15:51:55 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepBuilderAPI_MakeVertex.ixx>
#include <BRepBuilderAPI.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>

//=======================================================================
//function : BRepBuilderAPI_MakeVertex
//purpose  : 
//=======================================================================

BRepBuilderAPI_MakeVertex::BRepBuilderAPI_MakeVertex(const gp_Pnt& P)
: myMakeVertex(P)
{
  if ( myMakeVertex.IsDone()) {
    Done();
    myShape = myMakeVertex.Shape();
  }
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepBuilderAPI_MakeVertex::Vertex()const 
{
  return myMakeVertex.Vertex();
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================


BRepBuilderAPI_MakeVertex::operator TopoDS_Vertex() const
{
  return Vertex();
}

