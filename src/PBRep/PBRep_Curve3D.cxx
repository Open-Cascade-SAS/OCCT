// File:	PBRep_Curve3D.cxx
// Created:	Mon Jul 26 10:28:50 1993
// Author:	Remi LEQUETTE
//		<rle@nonox>
// Update:      Frederic MAUPAS

#include <PBRep_Curve3D.ixx>


//=======================================================================
//function : PBRep_Curve3D
//purpose  : 
//=======================================================================

PBRep_Curve3D::PBRep_Curve3D(const Handle(PGeom_Curve)& C,
			     const Standard_Real CF,
			     const Standard_Real CL,
			     const PTopLoc_Location& L) 
: PBRep_GCurve(L, CF, CL),
  myCurve3D(C)
{
}


//=======================================================================
//function : Curve3D
//purpose  : 
//=======================================================================

Handle(PGeom_Curve)  PBRep_Curve3D::Curve3D()const 
{
  return myCurve3D;
}

//=======================================================================
//function : IsCurve3D
//purpose  : 
//=======================================================================

Standard_Boolean  PBRep_Curve3D::IsCurve3D()const 
{
  return Standard_True;
}

