// File:	Geom2d_Conic.cxx
// Created:	Wed Mar 24 19:20:16 1993
// Author:	JCV
//		<fid@sdsun2>
// Copyright:	Matra Datavision 1993

//File Geom2d_Conic.cx, JCV 12/07/91

#include <Geom2d_Conic.ixx>
#include <gp_Dir2d.hxx>

typedef Geom2d_Conic         Conic;
typedef Handle(Geom2d_Conic) Handle(Conic);

typedef gp_Ax2d  Ax2d;
typedef gp_Dir2d Dir2d;
typedef gp_Pnt2d Pnt2d;
typedef gp_Vec2d Vec2d;






//=======================================================================
//function : SetAxis
//purpose  : 
//=======================================================================

void Geom2d_Conic::SetAxis(const gp_Ax22d& A)
{
  pos.SetAxis(A); 
}

//=======================================================================
//function : SetXAxis
//purpose  : 
//=======================================================================

void Geom2d_Conic::SetXAxis (const Ax2d& A) 
{ 
  pos.SetXAxis(A); 
}

//=======================================================================
//function : SetYAxis
//purpose  : 
//=======================================================================

void Geom2d_Conic::SetYAxis (const Ax2d& A)
{ 
  pos.SetYAxis(A);
}

//=======================================================================
//function : SetLocation
//purpose  : 
//=======================================================================

void Geom2d_Conic::SetLocation (const Pnt2d& P) 
{
  pos.SetLocation (P); 
}

//=======================================================================
//function : XAxis
//purpose  : 
//=======================================================================

Ax2d Geom2d_Conic::XAxis () const 
{ 
  return gp_Ax2d(pos.Location(), pos.XDirection()); 
}

//=======================================================================
//function : YAxis
//purpose  : 
//=======================================================================

Ax2d Geom2d_Conic::YAxis () const 
{
   return gp_Ax2d(pos.Location(), pos.YDirection());
}

//=======================================================================
//function : Location
//purpose  : 
//=======================================================================

Pnt2d Geom2d_Conic::Location () const 
{
 return pos.Location(); 
}

//=======================================================================
//function : Position
//purpose  : 
//=======================================================================

const gp_Ax22d& Geom2d_Conic::Position () const 
{
  return pos; 
}


//=======================================================================
//function : Reverse
//purpose  : 
//=======================================================================

void Geom2d_Conic::Reverse () { 

  Dir2d Temp = pos.YDirection ();
  Temp.Reverse ();
  pos.SetAxis(gp_Ax22d(pos.Location(), pos.XDirection(), Temp));
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================

GeomAbs_Shape Geom2d_Conic::Continuity () const 
{
  return GeomAbs_CN; 
}

//=======================================================================
//function : IsCN
//purpose  : 
//=======================================================================

Standard_Boolean Geom2d_Conic::IsCN (const Standard_Integer ) const 
{
  return Standard_True; 
}
