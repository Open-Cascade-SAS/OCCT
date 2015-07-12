// Created on: 1999-03-08
// Created by: Fabrice SERVANT
// Copyright (c) 1999-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.


#include <Adaptor3d_HSurface.hxx>
#include <IntPolyh_Point.hxx>

#include <stdio.h>
//=======================================================================
//function : IntPolyh_Point
//purpose  : 
//=======================================================================
IntPolyh_Point::IntPolyh_Point() 
: 
  x(0),y(0),z(0),u(0),v(0),POC(1), 
  myDegenerated(Standard_False) 
{
}

//=======================================================================
//function : IntPolyh_Point
//purpose  : 
//=======================================================================
IntPolyh_Point::IntPolyh_Point(const Standard_Real _x,
			       const Standard_Real _y,
			       const Standard_Real _z,
			       const Standard_Real _u,
			       const Standard_Real _v)
:
  POC(1),
  myDegenerated(Standard_False) 
{ 
  x=_x; y=_y; z=_z; u=_u; v=_v;
}
//=======================================================================
//function : X
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Point::X() const 
{ 
  return x; 
} 
//=======================================================================
//function : Y
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Point::Y() const 
{
  return y;   
}
//=======================================================================
//function : Z
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Point::Z() const 
{
  return z;
 } 
//=======================================================================
//function : U
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Point::U() const 
{ 
  return u;
} 
//=======================================================================
//function : V
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Point::V() const 
{ 
  return v; 
}
//=======================================================================
//function : PartOfCommon
//purpose  : 
//=======================================================================
Standard_Integer IntPolyh_Point::PartOfCommon() const 
{
  return POC;
}
//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
void IntPolyh_Point::Set(const Standard_Real _x,
			 const Standard_Real _y,
			 const Standard_Real _z,
			 const Standard_Real _u, 
			 const Standard_Real _v, 
			 const Standard_Integer II) 
{ 
  x=_x; 
  y=_y; 
  z=_z; 
  u=_u; 
  v=_v; 
  POC=II;
}
//=======================================================================
//function : Equal
//purpose  : 
//=======================================================================
void IntPolyh_Point::Equal(const IntPolyh_Point &Pt) 
{
  x = Pt.x;
  y = Pt.y;
  z = Pt.z;
  u = Pt.u;
  v = Pt.v;
}
//=======================================================================
//function : SetX
//purpose  : 
//=======================================================================
void IntPolyh_Point::SetX(const Standard_Real _x) 
{
  x=_x;
} 
//=======================================================================
//function : SetY
//purpose  : 
//=======================================================================
void IntPolyh_Point::SetY(const Standard_Real _y) 
{
  y=_y; 
} 
//=======================================================================
//function : SetZ
//purpose  : 
//=======================================================================
void IntPolyh_Point::SetZ(const Standard_Real _z) 
{
  z=_z; 
} 
//=======================================================================
//function : SetU
//purpose  : 
//=======================================================================
void IntPolyh_Point::SetU(const Standard_Real _u) 
{
  u=_u;
} 
//=======================================================================
//function : SetV
//purpose  : 
//=======================================================================
void IntPolyh_Point::SetV(const Standard_Real _v) 
{
  v=_v;
} 
//=======================================================================
//function : SetPartOfCommon
//purpose  : 
//=======================================================================
void IntPolyh_Point::SetPartOfCommon(const Standard_Integer ii) 
{
  POC=ii;
}


//=======================================================================
//function : Middle
//purpose  : 
//=======================================================================
void IntPolyh_Point::Middle(const Handle(Adaptor3d_HSurface)& MySurface,
				      const IntPolyh_Point & Point1, 
				      const IntPolyh_Point & Point2){
  u = (Point1.U()+Point2.U())*0.5;
  v = (Point1.V()+Point2.V())*0.5;
  
  gp_Pnt PtXYZ = (MySurface)->Value(u, v);

  x=PtXYZ.X();
  y=PtXYZ.Y(); 
  z=PtXYZ.Z();
}
//=======================================================================
//function : Add
//purpose  : 
//=======================================================================
IntPolyh_Point IntPolyh_Point::Add(const IntPolyh_Point &P1)const
{
  IntPolyh_Point res;
  //
  res.SetX(x+P1.X());
  res.SetY(y+P1.Y());
  res.SetZ(z+P1.Z());
  res.SetU(u+P1.U());
  res.SetV(v+P1.V());
  return res;
}  

//=======================================================================
//function : Sub
//purpose  : 
//=======================================================================
IntPolyh_Point IntPolyh_Point::Sub(const IntPolyh_Point &P1)const
{ 
  IntPolyh_Point res;
  //
  res.SetX(x-P1.X());
  res.SetY(y-P1.Y());
  res.SetZ(z-P1.Z());
  res.SetU(u-P1.U());
  res.SetV(v-P1.V());
  return res;
} 
//=======================================================================
//function : Divide
//purpose  : 
//=======================================================================
IntPolyh_Point IntPolyh_Point::Divide(const Standard_Real RR)const
{ 
  IntPolyh_Point res;
  //
  if (Abs(RR)>10.0e-20) {
    res.SetX(x/RR);
    res.SetY(y/RR);
    res.SetZ(z/RR);
    res.SetU(u/RR);
    res.SetV(v/RR);
  }
  else { 
   printf("Division par zero RR=%f\n",RR);
  }
  return res;
}
//=======================================================================
//function : Multiplication
//purpose  : 
//=======================================================================
IntPolyh_Point IntPolyh_Point::Multiplication(const Standard_Real RR)const
{ 
  IntPolyh_Point res;
  //
  res.SetX(x*RR);
  res.SetY(y*RR);
  res.SetZ(z*RR);
  res.SetU(u*RR);
  res.SetV(v*RR);
  return res;
}
//=======================================================================
//function : SquareModulus
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Point::SquareModulus()const
{
  Standard_Real res=x*x+y*y+z*z;
  return res;
}

//=======================================================================
//function : SquareDistance
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Point::SquareDistance(const IntPolyh_Point &P2)const
{
  Standard_Real res=(x-P2.x)*(x-P2.x)+(y-P2.y)*(y-P2.y)+(z-P2.z)*(z-P2.z);
  return res;
}
//=======================================================================
//function : Dot
//purpose  : 
//=======================================================================
Standard_Real IntPolyh_Point::Dot(const IntPolyh_Point &b ) const
{ 
  Standard_Real t=x*b.x+y*b.y+z*b.z;
  return t;
}
//=======================================================================
//function : Cross
//purpose  : 
//=======================================================================
void IntPolyh_Point::Cross(const IntPolyh_Point &a,const IntPolyh_Point &b){ 
  x=a.y*b.z-a.z*b.y;
  y=a.z*b.x-a.x*b.z;
  z=a.x*b.y-a.y*b.x;
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void IntPolyh_Point::Dump() const
{ 
  printf("\nPoint : x=%+8.3eg y=%+8.3eg z=%+8.3eg u=%+8.3eg v=%+8.3eg\n",x,y,z,u,v);
}
//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
void IntPolyh_Point::Dump(const Standard_Integer i) const
{ 
  printf("\nPoint(%3d) : x=%+8.3eg y=%+8.3eg z=%+8.3eg u=%+8.3eg v=%+8.3eg poc=%3d\n",
	 i,x,y,z,u,v,POC);
}
//modified by NIZNHY-PKV Fri Jan 20 12:13:03 2012f
//=======================================================================
//function : SetDegenerated
//purpose  : 
//=======================================================================
void IntPolyh_Point::SetDegenerated(const Standard_Boolean theFlag) 
{
  myDegenerated=theFlag;
}
//=======================================================================
//function : Degenerated
//purpose  : 
//=======================================================================
Standard_Boolean IntPolyh_Point::Degenerated()const 
{
  return myDegenerated;
}







