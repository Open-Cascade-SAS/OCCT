// Copyright (c) 1995-1999 Matra Datavision
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


#include <ElCLib.hxx>
#include <gp.hxx>
#include <gp_Circ.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <GProp.hxx>
#include <GProp_CelGProps.hxx>
#include <math_Jacobi.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Standard_NotImplemented.hxx>

GProp_CelGProps::GProp_CelGProps(){}

void GProp_CelGProps::SetLocation(const gp_Pnt& CLocation)
{
  loc = CLocation;
}


void GProp_CelGProps::Perform(const gp_Circ& C, 			      
			      const Standard_Real U1,
			      const Standard_Real U2)
{
  Standard_Real X0,Y0,Z0,Xa1,Ya1,Za1,Xa2,Ya2,Za2,Xa3,Ya3,Za3;
  C.Location().Coord(X0,Y0,Z0);
  C.XAxis().Direction().Coord(Xa1,Ya1,Za1);
  C.YAxis().Direction().Coord(Xa2,Ya2,Za2);
  C.Axis().Direction().Coord(Xa3,Ya3,Za3);
  Standard_Real Ray = C.Radius();

  dim = Ray*Abs(U2-U1);
  Standard_Real xloc =Ray*(Sin(U2)-Sin(U1))/(U2-U1);
  Standard_Real yloc =Ray*(Cos(U1)-Cos(U2))/(U2-U1);

  g.SetCoord(xloc*Xa1+yloc*Xa2+X0,xloc*Ya1+yloc*Ya2+Y0,Z0);

  math_Matrix Dm(1,3,1,3);
  Dm(1,1)= Ray*Ray*Ray*(U2/2-U1/2-Sin(2*U2)/4+Sin(2*U1)/4);
  Dm(2,2)= Ray*Ray*Ray*(U2/2-U1/2+Sin(2*U2)/4-Sin(2*U1)/4);
  Dm(3,3)= Ray*Ray*dim; 
  Dm(2,1)= Dm(1,2) = - Ray*Ray*Ray*(Cos(2*U1)/4-Cos(2*U2)/4);
  Dm(3,1) = Dm(1,3) = 0.;
  Dm(3,2) = Dm(2,3) = 0.;
  math_Matrix Passage (1,3,1,3);
  Passage(1,1) = Xa1; Passage(1,2) = Xa2 ;Passage(1,3) = Xa3;
  Passage(2,1) = Ya1; Passage(2,2) = Ya2 ;Passage(2,3) = Ya3;
  Passage(3,1) = Za1; Passage(3,2) = Za2 ;Passage(3,3) = Za3;

  math_Jacobi J(Dm);
  math_Vector V1(1,3),V2(1,3),V3(1,3);
  J.Vector(1,V1);
  V1.Multiply(Passage,V1);
  V1.Multiply(J.Value(1));
  J.Vector(2,V2);
  V2.Multiply(Passage,V2);
  V2.Multiply(J.Value(2));
  J.Vector(3,V3);
  V3.Multiply(Passage,V3);
  V3.Multiply(J.Value(3));

  inertia = gp_Mat (gp_XYZ(V1(1),V2(1),V3(1)),
		    gp_XYZ(V1(2),V2(2),V3(2)),
		    gp_XYZ(V1(3),V2(3),V3(3)));

  gp_Mat Hop;
  GProp::HOperator(g,loc,dim,Hop);
  inertia = inertia+Hop;
}


void GProp_CelGProps::Perform(const gp_Lin& C, 			      
			      const Standard_Real U1,
			      const Standard_Real U2)
{
  gp_Ax1 Pos = C.Position();
  gp_Pnt P1 = ElCLib::LineValue(U1,Pos);
  dim =Abs(U2-U1);
  gp_Pnt P2 = ElCLib::LineValue(U2,Pos);
  g.SetCoord((P1.X()+P2.X())/2.,(P1.Y()+P2.Y())/2.,(P1.Z()+P2.Z())/2.);
  Standard_Real Vx,Vy,Vz,X0,Y0,Z0;
  Pos.Direction().Coord(Vx,Vy,Vz);
  Pos.Location().Coord(X0,Y0,Z0);
  Standard_Real alfa1 = (Vz*Vz+Vy*Vy)/3.;
  Standard_Real alfa2 = Vy*Y0+Vz*Z0;
  Standard_Real alfa3 = Y0*Y0+Z0*Z0;
  Standard_Real Ixx =  (U2*(U2*(U2*alfa1+alfa2)+alfa3)) -
              (U1*(U1*(U1*alfa1+alfa2)+alfa3));
  alfa1 = (Vz*Vz+Vx*Vx)/3.;
  alfa2 = Vx*X0+Vz*Z0;
  alfa3 = X0*X0+Z0*Z0;
  Standard_Real Iyy =  (U2*(U2*(U2*alfa1+alfa2)+alfa3)) -
              (U1*(U1*(U1*alfa1+alfa2)+alfa3));
  alfa1 = (Vy*Vy+Vx*Vx)/3.;
  alfa2 = Vy*Y0+Vz*Z0;
  alfa3 = Y0*Y0+Z0*Z0;
  Standard_Real Izz =  (U2*(U2*(U2*alfa1+alfa2)+alfa3)) -
                       (U1*(U1*(U1*alfa1+alfa2)+alfa3));
  alfa1 = (Vy*Vx)/3.;
  alfa2 = (Vy*X0+Vx*Y0)/2.;
  alfa3 = Y0*X0;
  Standard_Real Ixy =  (U2*(U2*(U2*alfa1+alfa2)+alfa3)) -
                       (U1*(U1*(U1*alfa1+alfa2)+alfa3));
  alfa1 = (Vz*Vx)/3.;
  alfa2 = (Vz*X0+Vx*Z0)/2;
  alfa3 = Z0*X0;
  Standard_Real Ixz =  (U2*(U2*(U2*alfa1+alfa2)+alfa3)) -
                       (U1*(U1*(U1*alfa1+alfa2)+alfa3));
  alfa1 = (Vy*Vz)/3.;
  alfa2 = (Vy*Z0+Vz*Y0)/2.;
  alfa3 = Y0*Z0;
  Standard_Real Iyz =  (U2*(U2*(U2*alfa1+alfa2)+alfa3)) -
                       (U1*(U1*(U1*alfa1+alfa2)+alfa3));

  inertia = gp_Mat (gp_XYZ (Ixx, -Ixy,-Ixz),
                    gp_XYZ (-Ixy, Iyy,-Iyz),
                    gp_XYZ (-Ixz,-Iyz, Izz));
}


GProp_CelGProps::GProp_CelGProps (const gp_Circ& C, const gp_Pnt& CLocation) 
{
  SetLocation(CLocation);
  Perform(C,0.,2.*M_PI);
}

GProp_CelGProps::GProp_CelGProps (const gp_Circ& C, 
				  const Standard_Real U1, 
				  const Standard_Real U2,
				  const gp_Pnt& CLocation)
{
  SetLocation(CLocation);
  Perform(C,U1,U2);
}

GProp_CelGProps::GProp_CelGProps (const gp_Lin& C, 
				  const Standard_Real U1, 
				  const Standard_Real U2,
				  const gp_Pnt& CLocation)
{
  SetLocation(CLocation);
  Perform(C,U1,U2);
}

