// Created on: 1993-03-24
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



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
