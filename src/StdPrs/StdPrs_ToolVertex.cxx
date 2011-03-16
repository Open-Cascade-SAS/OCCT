// File:	StdPrs_ToolVertex.cxx
// Created:	Tue Mai 18 18:45:41 1993
// Author:	Laurent PAINNOT
//		<lpa@metrox>


#include <StdPrs_ToolVertex.ixx>
#include <BRep_Tool.hxx>
#include <gp_Pnt.hxx>


void StdPrs_ToolVertex::Coord (const TopoDS_Vertex& aVertex,
			       Standard_Real& X,
			       Standard_Real& Y,
			       Standard_Real& Z) {
  gp_Pnt P = BRep_Tool::Pnt(aVertex);
  X = P.X(); Y = P.Y(); Z = P.Z();
}
