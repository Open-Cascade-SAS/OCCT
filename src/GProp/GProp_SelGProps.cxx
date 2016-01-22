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


#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <GProp.hxx>
#include <GProp_SelGProps.hxx>
#include <math_Jacobi.hxx>
#include <math_Matrix.hxx>
#include <math_Vector.hxx>
#include <Standard_NotImplemented.hxx>

GProp_SelGProps::GProp_SelGProps(){}

void GProp_SelGProps::SetLocation(const gp_Pnt& SLocation )
{
  loc = SLocation;
}


void GProp_SelGProps::Perform(const gp_Cylinder& S, 
			      const Standard_Real         Alpha1,
			      const Standard_Real         Alpha2, 
			      const Standard_Real         Z1,
			      const Standard_Real         Z2)
 {
  Standard_Real X0,Y0,Z0,Xa1,Ya1,Za1,Xa2,Ya2,Za2,Xa3,Ya3,Za3;
  S.Location().Coord(X0,Y0,Z0);
  Standard_Real R = S.Radius();
  S.Position().XDirection().Coord(Xa1,Ya1,Za1);
  S.Position().YDirection().Coord(Xa2,Ya2,Za2);
  S.Position().Direction().Coord(Xa3,Ya3,Za3);
  dim = R*(Z2-Z1)*(Alpha2-Alpha1);
  Standard_Real SA2 = Sin(Alpha2);
  Standard_Real SA1 = Sin(Alpha1);
  Standard_Real CA2 = Cos(Alpha2);
  Standard_Real CA1 = Cos(Alpha1);
  Standard_Real Ix = R*(SA2-SA1)/(Alpha2-Alpha1);
  Standard_Real Iy = R*(CA1-CA2)/(Alpha2-Alpha1);
  g.SetCoord( X0 + Ix*Xa1+Iy*Xa2 + Xa3*(Z2+Z1)/2.,
	      Y0 + Ix*Ya1+Iy*Ya2 + Ya3*(Z2+Z1)/2.,
	      Z0 + Ix*Za1+Iy*Za2 + Za3*(Z2+Z1)/2.);
  Standard_Real ICn2 =R*R*( Alpha2-Alpha1 + SA2*CA2 - SA1*CA1 )/2.;  
  Standard_Real ISn2 =R*R*( Alpha2-Alpha1 - SA2*CA2 + SA1*CA1 )/2.;  
  Standard_Real IZ2 = (Alpha2-Alpha1)*(Z2*Z2+Z2*Z1+Z1*Z1)/3.;
  Standard_Real ICnSn= R*R*( SA2*SA2-SA1*SA1)/2.;
  Standard_Real ICnz = (Z2+Z1)*(SA2-SA1)/2.;
  Standard_Real ISnz = (Z2+Z1)*(CA1-CA2)/2.;

  math_Matrix Dm(1,3,1,3);

  Dm(1,1) = ISn2 + IZ2;
  Dm(2,2) = ICn2 + IZ2;
  Dm(3,3) = Alpha2-Alpha1;
  Dm(1,2) = Dm(2,1) = -ICnSn;
  Dm(1,3) = Dm(3,1) = -ICnz;
  Dm(3,2) = Dm(2,3) = -ISnz;

  math_Matrix Passage (1,3,1,3);
  Passage(1,1) = Xa1; Passage(1,2) = Xa2 ;Passage(1,3) = Xa3;
  Passage(2,1) = Ya1; Passage(2,2) = Ya2 ;Passage(2,3) = Ya3;
  Passage(3,1) = Za1; Passage(3,2) = Za2 ;Passage(3,3) = Za3;

  math_Jacobi J(Dm);
  R = R*(Z2-Z1);
  math_Vector V1(1,3), V2(1,3), V3(1,3);
  J.Vector(1,V1);
  V1.Multiply(Passage,V1);
  V1.Multiply(R*J.Value(1));
  J.Vector(2,V2);
  V2.Multiply(Passage,V2);
  V2.Multiply(R*J.Value(2));
  J.Vector(3,V3);
  V3.Multiply(Passage,V3);
  V3.Multiply(R*J.Value(3));

  inertia = gp_Mat (gp_XYZ(V1(1),V2(1),V3(1)),
		    gp_XYZ(V1(2),V2(2),V3(2)),
		    gp_XYZ(V1(3),V2(3),V3(3)));
  gp_Mat Hop;
  GProp::HOperator(g,loc,dim,Hop);
  inertia = inertia+Hop;
}

void GProp_SelGProps::Perform (const gp_Cone& S, 
			       const Standard_Real     Alpha1,
			       const Standard_Real     Alpha2,
			       const Standard_Real     Z1,
			       const Standard_Real     Z2)

{
  Standard_Real X0,Y0,Z0,Xa1,Ya1,Za1,Xa2,Ya2,Za2,Xa3,Ya3,Za3;
  S.Location().Coord(X0,Y0,Z0);
  S.Position().XDirection().Coord(Xa1,Ya1,Za1);
  S.Position().YDirection().Coord(Xa2,Ya2,Za2);
  S.Position().Direction().Coord(Xa3,Ya3,Za3);
  Standard_Real t =S.SemiAngle();
  Standard_Real Cnt = Cos(t);
  Standard_Real Snt = Sin(t); 
  Standard_Real R = S.RefRadius();
  Standard_Real Sn2 = Sin(Alpha2);
  Standard_Real Sn1 = Sin(Alpha1);
  Standard_Real Cn2 = Cos(Alpha2);
  Standard_Real Cn1 = Cos(Alpha1);

  Standard_Real Auxi1 = R + (Z2+Z1)*Snt/2.;
  Standard_Real Auxi2 = (Z2*Z2+Z1*Z2+Z1*Z1)/3.;
  dim = (Alpha2-Alpha1)*Cnt*(Z2-Z1)*Auxi1; 

  Standard_Real Ix  =
    (R*R+R*(Z2+Z1)*Snt + Snt*Auxi2)/Auxi1;
  Standard_Real Iy = Ix*(Cn1-Cn2)/(Alpha2-Alpha1);
  Ix = Ix*(Sn2-Sn1)/(Alpha2-Alpha1);
  Standard_Real Iz = Cnt*(R*(Z2+Z1)/2.+Snt*Auxi2)/Auxi1;

  g.SetCoord(X0 + Xa1*Ix+ Xa2*Iy+ Xa3*Iz,
	     Y0 + Ya1*Ix+ Ya2*Iy+ Ya3*Iz,
	     Z0 + Za1*Ix+ Za2*Iy+ Za3*Iz);
 
 Standard_Real R1 = R+Z1*Snt;
 Standard_Real R2 = R+Z2*Snt;
 Standard_Real ZZ = (Z2-Z1)*Cnt;
 Standard_Real IR2 = ZZ*Snt*(R1*R1*R1+R1*R1*R2+R1*R2*R2+R2*R2*R2)/4.;
 Standard_Real ICn2  = IR2*(Alpha2-Alpha1+Cn2*Sn2-Cn1*Sn1)/2.;
 Standard_Real ISn2 = IR2*(Alpha2-Alpha1+Cn2*Sn2-Cn1*Sn1)/2.;
 Standard_Real IZ2 = ZZ*Cnt*Cnt*(Z2-Z1)*(Alpha2-Alpha1)*
   (R*Auxi2 +Snt*(Z2*Z2*Z2+Z2*Z2*Z1+Z2*Z1*Z1+Z1*Z1*Z1))/4.;
 Standard_Real ICnSn = IR2*(Cn2*Cn2-Cn1*Cn1);
 Standard_Real ICnz = Cnt*Snt*ZZ*(R*(Z1+Z2)/2.+Auxi2)*(Sn2-Sn1);
 Standard_Real ISnz = Cnt*Snt*ZZ*(R*(Z1+Z2)/2.+Auxi2)*(Cn1-Cn2);    

  math_Matrix Dm(1,3,1,3);
  Dm(1,1) = ISn2 + IZ2;
  Dm(2,2) = ICn2 + IZ2;
  Dm(3,3) = IR2*(Alpha2-Alpha1);
  Dm(1,2) = Dm(2,1) = -ICnSn;
  Dm(1,3) = Dm(3,1) = -ICnz;
  Dm(3,2) = Dm(2,3) = -ISnz;

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


void GProp_SelGProps::Perform(const gp_Sphere& S,
			      const Standard_Real       Teta1,
			      const Standard_Real       Teta2,
			      const Standard_Real       Alpha1,
			      const Standard_Real       Alpha2)
{
  Standard_Real X0,Y0,Z0,Xa1,Ya1,Za1,Xa2,Ya2,Za2,Xa3,Ya3,Za3;
  S.Location().Coord(X0,Y0,Z0);
  S.Position().XDirection().Coord(Xa1,Ya1,Za1);
  S.Position().YDirection().Coord(Xa2,Ya2,Za2);
  S.Position().Direction().Coord(Xa3,Ya3,Za3);
  Standard_Real R = S.Radius();
  Standard_Real Cnt1 = Cos(Teta1);
  Standard_Real Snt1 = Sin(Teta1);
  Standard_Real Cnt2 = Cos(Teta2);
  Standard_Real Snt2 = Sin(Teta2);
  Standard_Real Cnf1 = Cos(Alpha1);
  Standard_Real Snf1 = Sin(Alpha1);
  Standard_Real Cnf2 = Cos(Alpha2);
  Standard_Real Snf2 = Sin(Alpha2);
  dim = R*R*(Teta2-Teta1)*(Snf2-Snf1);
  Standard_Real Ix = 
    R*(Snt2-Snt1)/(Teta2-Teta1)*
    (Alpha2-Alpha1+Snf2*Cnf2-Snf1*Cnf1)/(Snf2-Snf1)/2.;
  Standard_Real Iy = 
    R*(Cnt1-Cnt2)/(Teta2-Teta1)*
    (Alpha2-Alpha1+Snf2*Cnf2-Snf1*Cnf1)/(Snf2-Snf1)/2.;
  Standard_Real Iz = R*(Snf2+Snf1)/2.;
  g.SetCoord(
   X0 + Ix*Xa1 + Iy*Xa2 + Iz*Xa3,
   Y0 + Ix*Ya1 + Iy*Ya2 + Iz*Ya3,
   Z0 + Ix*Za1 + Iy*Za2 + Iz*Za3);

  Standard_Real IR2 = ( Cnf2*Snf2*(Cnf2+1.)- Cnf1*Snf1*(Cnf1+1.) +
                       Alpha2-Alpha1 )/3.;
  Standard_Real ICn2 = (Teta2-Teta1+ Cnt2*Snt2-Cnt1*Snt1)*IR2/2.;
  Standard_Real ISn2 = (Teta2-Teta1-Cnt2*Snt2+Cnt1*Snt1)*IR2/2.;
  Standard_Real ICnSn = ( Snt2*Snt2-Snt1*Snt1)*IR2/2.;
  Standard_Real IZ2 = (Teta2-Teta1)*(Snf2*Snf2*Snf2-Snf1*Snf1*Snf1)/3.;
  Standard_Real ICnz =(Snt2-Snt1)*(Cnf1*Cnf1*Cnf1-Cnf2*Cnf2*Cnf2)/3.;
  Standard_Real ISnz =(Cnt1-Cnt2)*(Cnf1*Cnf1*Cnf1-Cnf2*Cnf2*Cnf2)/3.;

  math_Matrix Dm(1,3,1,3);
  Dm(1,1) = ISn2 +IZ2; 
  Dm(2,2) = ICn2 +IZ2;
  Dm(3,3) = IR2*(Teta2-Teta1);
  Dm(1,2) = Dm(2,1) = -ICnSn;
  Dm(1,3) = Dm(3,1) = -ICnz;
  Dm(3,2) = Dm(2,3) = -ISnz;

  math_Matrix Passage (1,3,1,3);
  Passage(1,1) = Xa1; Passage(1,2) = Xa2 ;Passage(1,3) = Xa3;
  Passage(2,1) = Ya1; Passage(2,2) = Ya2 ;Passage(2,3) = Ya3;
  Passage(3,1) = Za1; Passage(3,2) = Za2 ;Passage(3,3) = Za3;

  math_Jacobi J(Dm);
  R = R*R*R*R;
  math_Vector V1(1,3), V2(1,3), V3(1,3);
  J.Vector(1,V1);
  V1.Multiply(Passage,V1);
  V1.Multiply(R*J.Value(1));
  J.Vector(2,V2);
  V2.Multiply(Passage,V2);
  V2.Multiply(R*J.Value(2));
  J.Vector(3,V3);
  V3.Multiply(Passage,V3);
  V3.Multiply(R*J.Value(3));

  inertia = gp_Mat (gp_XYZ(V1(1),V2(1),V3(1)),
		    gp_XYZ(V1(2),V2(2),V3(2)),
		    gp_XYZ(V1(3),V2(3),V3(3)));
  gp_Mat Hop;
  GProp::HOperator(g,loc,dim,Hop);
  inertia = inertia+Hop;

 
}


void GProp_SelGProps::Perform (const gp_Torus& S, 
			       const Standard_Real      Teta1,
			       const Standard_Real      Teta2,
			       const Standard_Real      Alpha1,
			       const Standard_Real      Alpha2)
{
  Standard_Real X0,Y0,Z0,Xa1,Ya1,Za1,Xa2,Ya2,Za2,Xa3,Ya3,Za3;
  S.Location().Coord(X0,Y0,Z0);
  S.XAxis().Direction().Coord(Xa1,Ya1,Za1);
  S.YAxis().Direction().Coord(Xa2,Ya2,Za2);
  S.Axis().Direction().Coord(Xa3,Ya3,Za3);
  Standard_Real RMax = S.MajorRadius();
  Standard_Real Rmin = S.MinorRadius();
  Standard_Real Cnt1 = Cos(Teta1);
  Standard_Real Snt1 = Sin(Teta1);
  Standard_Real Cnt2 = Cos(Teta2);
  Standard_Real Snt2 = Sin(Teta2);
  Standard_Real Cnf1 = Cos(Alpha1);
  Standard_Real Snf1 = Sin(Alpha1);
  Standard_Real Cnf2 = Cos(Alpha2);
  Standard_Real Snf2 = Sin(Alpha2);


  dim = RMax*Rmin*(Teta2-Teta1)*(Alpha2-Alpha1);
  Standard_Real Ix = 
    (Snt2-Snt1)/(Teta2-Teta1)*(Rmin*(Snf2-Snf1)/(Alpha2-Alpha1) + RMax);
  Standard_Real Iy = 
    (Cnt1-Cnt2)/(Teta2-Teta1)*(Rmin*(Snf2-Snf1)/(Alpha2-Alpha1) + RMax);
   Standard_Real Iz = Rmin*(Cnf1-Cnf2)/(Alpha2-Alpha1);
  
   g.SetCoord(
	     X0+Ix*Xa1+Iy*Xa2+Iz*Xa3,
	     Y0+Ix*Ya1+Iy*Ya2+Iz*Ya3,
	     Z0+Ix*Za1+Iy*Za2+Iz*Za3);

  Standard_Real IR2 = RMax*RMax + 2.*RMax*Rmin*(Snf2-Snf1) +
		      Rmin*Rmin/2.*(Snf2*Cnf2-Snf1*Cnf1);
  Standard_Real ICn2 = IR2*(Teta2-Teta1 +Snt2*Cnt2-Snt1*Cnt1)/2.;
  Standard_Real ISn2 = IR2*(Teta2-Teta1 -Snt2*Cnt2+Snt1*Cnt1)/2.;
  Standard_Real ICnSn = IR2*(Snt2*Snt2-Snt1*Snt1)/2.;
  Standard_Real IZ2 = 
     (Teta2-Teta1)*Rmin*Rmin*(Alpha2-Alpha1-Snf2*Cnf2+Snf1*Cnf1)/2.;
  Standard_Real ICnz = Rmin*(Snt2-Snt1)*(Cnf1-Cnf2)*(RMax+Rmin*(Cnf1+Cnf2)/2.);
  Standard_Real ISnz = Rmin*(Cnt2-Cnt1)*(Cnf1-Cnf2)*(RMax+Rmin*(Cnf1+Cnf2)/2.);

  math_Matrix Dm(1,3,1,3);
  Dm(1,1) = ISn2 + IZ2; 
  Dm(2,2) = ICn2 + IZ2;
  Dm(3,3) = IR2*(Teta2-Teta1);
  Dm(1,2) = Dm(2,1) = -ICnSn;
  Dm(1,3) = Dm(3,1) = -ICnz;
  Dm(3,2) = Dm(2,3) = -ISnz;

  math_Matrix Passage (1,3,1,3);
  Passage(1,1) = Xa1; Passage(1,2) = Xa2 ;Passage(1,3) = Xa3;
  Passage(2,1) = Ya1; Passage(2,2) = Ya2 ;Passage(2,3) = Ya3;
  Passage(3,1) = Za1; Passage(3,2) = Za2 ;Passage(3,3) = Za3;

  math_Jacobi J(Dm);
  RMax = RMax*Rmin;
  math_Vector V1(1,3), V2(1,3), V3(1,3);
  J.Vector(1,V1);
  V1.Multiply(Passage,V1);
  V1.Multiply(RMax*J.Value(1));
  J.Vector(2,V2);
  V2.Multiply(Passage,V2);
  V2.Multiply(RMax*J.Value(2));
  J.Vector(3,V3);
  V3.Multiply(Passage,V3);
  V3.Multiply(RMax*J.Value(3));

  inertia = gp_Mat (gp_XYZ(V1(1),V2(1),V3(1)),
		    gp_XYZ(V1(2),V2(2),V3(2)),
		    gp_XYZ(V1(3),V2(3),V3(3)));
  gp_Mat Hop;
  GProp::HOperator(g,loc,dim,Hop);
  inertia = inertia+Hop;


}

   

GProp_SelGProps::GProp_SelGProps (const gp_Cone& S, 
				  const Standard_Real     Alpha1,
				  const Standard_Real     Alpha2,
				  const Standard_Real     Z1,
				  const Standard_Real     Z2,
				  const gp_Pnt& SLocation )
{
  SetLocation(SLocation);
  Perform(S,Alpha1,Alpha2,Z1,Z2);
}


GProp_SelGProps::GProp_SelGProps (const gp_Cylinder& S, 
				  const Standard_Real         Alpha1,
				  const Standard_Real         Alpha2, 
				  const Standard_Real         Z1,
				  const Standard_Real         Z2,
				  const gp_Pnt& SLocation)
 {
  SetLocation(SLocation);
  Perform(S,Alpha1,Alpha2,Z1,Z2);
}


GProp_SelGProps::GProp_SelGProps (const gp_Sphere& S,
				  const Standard_Real       Teta1,
				  const Standard_Real       Teta2,
				  const Standard_Real       Alpha1,
				  const Standard_Real       Alpha2,
				  const gp_Pnt& SLocation)
 {
  SetLocation(SLocation);
  Perform(S,Teta1,Teta2,Alpha1,Alpha2);
}


GProp_SelGProps::GProp_SelGProps (const gp_Torus& S, 
				  const Standard_Real      Teta1,
				  const Standard_Real      Teta2,
				  const Standard_Real      Alpha1,
				  const Standard_Real      Alpha2,
				  const gp_Pnt& SLocation)
 {
  SetLocation(SLocation);
  Perform(S,Teta1,Teta2,Alpha1,Alpha2);
}

   

