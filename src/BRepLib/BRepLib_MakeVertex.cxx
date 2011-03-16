// File:	BRepLib_MakeVertex.cxx
// Created:	Fri Jul 23 15:51:55 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <BRepLib_MakeVertex.ixx>
#include <BRepLib.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS.hxx>

//=======================================================================
//function : BRepLib_MakeVertex
//purpose  : 
//=======================================================================

BRepLib_MakeVertex::BRepLib_MakeVertex(const gp_Pnt& P)
{
  BRep_Builder B;
  B.MakeVertex(TopoDS::Vertex(myShape),P,BRepLib::Precision());
  Done();
}


//=======================================================================
//function : Vertex
//purpose  : 
//=======================================================================

const TopoDS_Vertex&  BRepLib_MakeVertex::Vertex()const 
{
  return TopoDS::Vertex(Shape());
}


//=======================================================================
//function : operator
//purpose  : 
//=======================================================================


BRepLib_MakeVertex::operator TopoDS_Vertex() const
{
  return Vertex();
}


