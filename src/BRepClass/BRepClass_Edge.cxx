// File:	BRepClass_Edge.cxx
// Created:	Thu Nov 19 14:41:41 1992
// Author:	Remi LEQUETTE
//		<rle@phylox>


#include <BRepClass_Edge.ixx>


//=======================================================================
//function : BRepClass_Edge
//purpose  : 
//=======================================================================

BRepClass_Edge::BRepClass_Edge()
{
}

//=======================================================================
//function : BRepClass_Edge
//purpose  : 
//=======================================================================

BRepClass_Edge::BRepClass_Edge(const TopoDS_Edge& E,
			       const TopoDS_Face& F) :
       myEdge(E),
       myFace(F)
{
}

