// File:	DBRep_Edge.cxx
// Created:	Thu Jul 15 11:31:24 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>


#include <DBRep_Edge.ixx>

//=======================================================================
//function : DBRep_Edge
//purpose  : 
//=======================================================================

DBRep_Edge::DBRep_Edge(const TopoDS_Edge& E, const Draw_Color& C) :
       myEdge(E),
       myColor(C)
{
}


