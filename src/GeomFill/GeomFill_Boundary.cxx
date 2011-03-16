// File:	GeomFill_Boundary.cxx
// Created:	Fri Nov  3 15:13:27 1995
// Author:	Laurent BOURESCHE
//		<lbo@phylox>


#include <GeomFill_Boundary.ixx>


//=======================================================================
//function : GeomFill_Boundary
//purpose  : 
//=======================================================================

GeomFill_Boundary::GeomFill_Boundary(const Standard_Real Tol3d, 
				     const Standard_Real Tolang):
 myT3d(Tol3d), myTang(Tolang)
{
}


//=======================================================================
//function : HasNormals
//purpose  : 
//=======================================================================

Standard_Boolean GeomFill_Boundary::HasNormals() const 
{
  return Standard_False;
}


//=======================================================================
//function : Norm
//purpose  : 
//=======================================================================

gp_Vec GeomFill_Boundary::Norm(const Standard_Real ) const 
{
  Standard_Failure::Raise("GeomFill_Boundary::Norm : Undefined normals");
  return gp_Vec();
}


//=======================================================================
//function : D1Norm
//purpose  : 
//=======================================================================

void GeomFill_Boundary::D1Norm(const Standard_Real , gp_Vec& , gp_Vec& ) const 
{
  Standard_Failure::Raise("GeomFill_Boundary::Norm : Undefined normals");
}


//=======================================================================
//function : Points
//purpose  : 
//=======================================================================

void GeomFill_Boundary::Points(gp_Pnt& PFirst, gp_Pnt& PLast) const
{
  Standard_Real f,l;
  Bounds(f,l);
  PFirst = Value(f);
  PLast  = Value(l);
}


//=======================================================================
//function : Tol3d
//purpose  : 
//=======================================================================

Standard_Real GeomFill_Boundary::Tol3d() const 
{
  return myT3d;
}


//=======================================================================
//function : Tol3d
//purpose  : 
//=======================================================================

void GeomFill_Boundary::Tol3d(const Standard_Real Tol)
{
  myT3d = Tol;
}


//=======================================================================
//function : Tolang
//purpose  : 
//=======================================================================

Standard_Real GeomFill_Boundary::Tolang() const 
{
  return myTang;
}


//=======================================================================
//function : Tolang
//purpose  : 
//=======================================================================

void GeomFill_Boundary::Tolang(const Standard_Real Tol)
{
  myTang = Tol;
}


