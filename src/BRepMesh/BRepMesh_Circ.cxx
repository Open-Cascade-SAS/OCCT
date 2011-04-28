// File:        BRepMesh_Circ.cxx
// Created:     Mon Aug  9 17:48:36 1993
// Author:      Didier PIFFAULT
//              <dpf@zerox>

#include <BRepMesh_Circ.hxx>

BRepMesh_Circ::BRepMesh_Circ()
{}

BRepMesh_Circ::BRepMesh_Circ(const gp_XY& loc, const Standard_Real rad)
: location(loc), radius(rad)
{}

void BRepMesh_Circ::SetLocation(const gp_XY& loc)
{
  location=loc;
}

void BRepMesh_Circ::SetRadius(const Standard_Real rad)
{
  radius=rad;
}
