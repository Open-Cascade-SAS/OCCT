// File:	MeshAlgo_Circ.cxx
// Created:	Mon Aug  9 17:48:36 1993
// Author:	Didier PIFFAULT
//		<dpf@zerox>

#include <MeshAlgo_Circ.ixx>

MeshAlgo_Circ::MeshAlgo_Circ()
{}

MeshAlgo_Circ::MeshAlgo_Circ(const gp_XY& loc, const Standard_Real rad)
     : location(loc), radius(rad)
{}

void MeshAlgo_Circ::SetLocation(const gp_XY& loc)
{
  location=loc;
}

void MeshAlgo_Circ::SetRadius(const Standard_Real rad)
{
  radius=rad;
}
