// Created on: 1992-07-01
// Created by: Laurent BUCHARD
// Copyright (c) 1992-1999 Matra Datavision
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

#ifndef OCCT_DEBUG
#define No_Standard_RangeError
#define No_Standard_OutOfRange
#endif


#include <gp_Ax3.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Pln.hxx>
#include <gp_Sphere.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <IntAna_Quadric.hxx>

//----------------------------------------------------------------------
//-- 
//--  Equation generale des quadriques :  
//-- 
//--   f(x,y,z) =       CXX X**2  + CYY Y**2  + CZZ Z**2
//--              
//--             + 2 (  CXY X Y   + CXZ X Z   + CYZ Y Z )
//-- 
//--             + 2 (  CX  X     + CY  Y     + CZ  Z)
//-- 
//--             + CCte
//-- 
//-- 
//----------------------------------------------------------------------
//----------------------------------------------------------------------
//-- Quadric Vide
//----------------------------------------------------------------------
IntAna_Quadric::IntAna_Quadric(void) {
  CXX=CYY=CZZ=CXY=CXZ=CYZ=CX=CY=CZ=0.0;   CCte=1.0;
}
//----------------------------------------------------------------------
//-- Pln  -----> Quadric
//-- 
//-- Coefficients sur gp_Pln retourne A,B,C,D 
//-- 	          avec f(x,y,z)  =    A x    + B y   +  C z   + D 
//-- 
//--  que l on identifie a        2(  CX x   + CY y  +  CZ z ) + CCte
//----------------------------------------------------------------------
void IntAna_Quadric::SetQuadric(const gp_Pln& P) {
  P.Coefficients(CX,CY,CZ,CCte);
  CX*=0.5; 
  CY*=0.5;
  CZ*=0.5;
  CXX=CYY=CZZ=CXY=CXZ=CYZ=0.0;
}

IntAna_Quadric::IntAna_Quadric(const gp_Pln& P) {
  P.Coefficients(CX,CY,CZ,CCte);  
  CX*=0.5; 
  CY*=0.5;
  CZ*=0.5;
  CXX=CYY=CZZ=CXY=CXZ=CYZ=0.0;
}
//----------------------------------------------------------------------
//-- Cylinder -----> Quadric
//----------------------------------------------------------------------
void IntAna_Quadric::SetQuadric(const gp_Cylinder& Cyl) {
  Cyl.Coefficients(CXX,CYY,CZZ,CXY,CXZ,CYZ,CX,CY,CZ,CCte);  
}

IntAna_Quadric::IntAna_Quadric(const gp_Cylinder& Cyl) {
  Cyl.Coefficients(CXX,CYY,CZZ,CXY,CXZ,CYZ,CX,CY,CZ,CCte);  
}
//----------------------------------------------------------------------
//-- Cone -----> Quadric
//----------------------------------------------------------------------
IntAna_Quadric::IntAna_Quadric(const gp_Cone& Cone) {
  Cone.Coefficients(CXX,CYY,CZZ,CXY,CXZ,CYZ,CX,CY,CZ,CCte);  
}

void IntAna_Quadric::SetQuadric(const gp_Cone& Cone) {
  Cone.Coefficients(CXX,CYY,CZZ,CXY,CXZ,CYZ,CX,CY,CZ,CCte);  
}
//----------------------------------------------------------------------
//-- Sphere -----> Quadric
//----------------------------------------------------------------------
void IntAna_Quadric::SetQuadric(const gp_Sphere& Sph) {
  Sph.Coefficients(CXX,CYY,CZZ,CXY,CXZ,CYZ,CX,CY,CZ,CCte);
}

IntAna_Quadric::IntAna_Quadric(const gp_Sphere& Sph) {
  Sph.Coefficients(CXX,CYY,CZZ,CXY,CXZ,CYZ,CX,CY,CZ,CCte);
}
//----------------------------------------------------------------------
//-- Returns the Coefficients of the Quadric
//----------------------------------------------------------------------
void IntAna_Quadric::Coefficients(Standard_Real& _CXX,Standard_Real& _CYY,Standard_Real& _CZZ,
				  Standard_Real& _CXY,Standard_Real& _CXZ,Standard_Real& _CYZ,
				  Standard_Real& _CX, Standard_Real& _CY, Standard_Real& _CZ,
				  Standard_Real& _CCte) const 
{
  _CXX=CXX; _CYY=CYY; _CZZ=CZZ;
  _CXY=CXY; _CXZ=CXZ; _CYZ=CYZ;
  _CX=CX;   _CY=CY;   _CZ=CZ;
  _CCte=CCte;
}
//----------------------------------------------------------------------
//-- Computes the Coefficients in a new coordinate system
//----------------------------------------------------------------------
void IntAna_Quadric::NewCoefficients( Standard_Real& _CXX,Standard_Real& _CYY,Standard_Real& _CZZ
				     ,Standard_Real& _CXY,Standard_Real& _CXZ,Standard_Real& _CYZ
				     ,Standard_Real& _CX, Standard_Real& _CY, Standard_Real& _CZ
				     ,Standard_Real& _CCte
				     ,const gp_Ax3& Axis) const 
{
  Standard_Real t11,t12,t13,t14;                  // x = t11 X + t12 Y + t13 Z + t14
  Standard_Real t21,t22,t23,t24;                  // y = t21 X + t22 Y + t23 Z + t24
  Standard_Real t31,t32,t33,t34;                  // z = t31 X + t32 Y + t33 Z + t34
                                 
                                                  //   = X DirX + Y DirY + Z DirZ + Loc
  
  Standard_Real Cxx,Cyy,Czz,Cxy,Cxz,Cyz,Cx,Cy,Cz,Ccte;            
  
  gp_Trsf Trans;
  Trans.SetTransformation(Axis);
  Trans.Invert();
  t11=Trans.Value(1,1);  t12=Trans.Value(1,2); t13=Trans.Value(1,3); t14=Trans.Value(1,4);
  t21=Trans.Value(2,1);  t22=Trans.Value(2,2); t23=Trans.Value(2,3); t24=Trans.Value(2,4);
  t31=Trans.Value(3,1);  t32=Trans.Value(3,2); t33=Trans.Value(3,3); t34=Trans.Value(3,4);
  
  
  //      P0(x,y,z)=_CXX x x + _CYY y y + ... + _CCte =0  (x,y,z "absolute" coordinates)
  // and  P1(X(x,y,z),Y(x,y,z),Z(x,y,z))=P0(x,y,z)
  //
  // with P1(X,Y,Z)= Cxx X X + Cyy Y Y + Czz Z Z + 2 Cxy X Y ... + Ccte
  //               = _CXX  x x + _CYY  y y  +  ...  +  _CCte
  
  Standard_Real t11_P2=t11*t11;  Standard_Real t21_P2=t21*t21;  Standard_Real t31_P2=t31*t31;
  Standard_Real t12_P2=t12*t12;  Standard_Real t22_P2=t22*t22;  Standard_Real t32_P2=t32*t32;
  Standard_Real t13_P2=t13*t13;  Standard_Real t23_P2=t23*t23;  Standard_Real t33_P2=t33*t33;
  Standard_Real t14_P2=t14*t14;  Standard_Real t24_P2=t24*t24;  Standard_Real t34_P2=t34*t34;

  Ccte = ((_CCte) 
	  + t14_P2* (_CXX) 
	  + t24_P2* (_CYY) 
	  + t34_P2* (_CZZ)
	  + 2.0 * ( t14*( (_CX) 
			 +t24* (_CXY) 
			 +t34* (_CXZ))
		   +t24*( (_CY) 
			 +t34* (_CYZ))
		   +t34* (_CZ)));
  
  Cxx = (  t11_P2* (_CXX) 
	 + t21_P2* (_CYY)
	 + t31_P2* (_CZZ)
	 + 2.0*( t11*( t21* (_CXY) 
		      +t31* (_CXZ))
 		+t21*t31* (_CYZ)));
  
  Cyy = (t12_P2* (_CXX) 
	 + t22_P2* (_CYY) 
	 + t32_P2* (_CZZ)
	 + 2.0*( t12*( t22* (_CXY) 
		      +t32* (_CXZ))
		+t22*t32* (_CYZ)));

  Czz = (t13_P2* (_CXX) 
	 + t33_P2* (_CZZ)
	 + t23_P2* (_CYY) 
	 + 2.0*( t13*( t23* (_CXY) 
		      +t33* (_CXZ))
		+t23*t33* (_CYZ)));

  Cz = (t13* (_CX) 
	+ t13*( t14* (_CXX) 
	       + t24* (_CXY) 
	       + t34* (_CXZ))
	+ t14*( t23* (_CXY) 
	       +t33* (_CXZ))
	+ t23*( (_CY) 
	       +t24* (_CYY) 
	       +t34* (_CYZ))
	+ t33*( t24* (_CYZ) 
	       +(_CZ)  
	       + t34* (_CZZ)));

  Cx = (t11* ((_CX) 
	      +t14* (_CXX) 
	      +t24* (_CXY) 
	      +t34* (_CXZ))
	+ t14*( t21* (_CXY) 
	       +t31* (_CXZ))
	+ t21*( (_CY) 
	       +t24* (_CYY)
	       +t34* (_CYZ))
	+ t31*( t24* (_CYZ) 
	       +(_CZ)
	       +t34* (_CZZ)));

  Cxy = (t11*( t12* (_CXX) 
	      +t22* (_CXY) 
	      +t32* (_CXZ))
	 + t12*( t21* (_CXY) 
		+t31* (_CXZ))
	 + t21*( t22* (_CYY) 
		+t32* (_CYZ))
	 + t31*( t22* (_CYZ) 
		+t32* (_CZZ)));

  Cxz = (t11*( t13* (_CXX) 
	      +t23* (_CXY) 
	      +t33* (_CXZ))
	 + t13*( t21* (_CXY) 
		+t31* (_CXZ))
	 + t21*( t23* (_CYY) 
		+t33* (_CYZ))
	 + t31*( t23* (_CYZ) 
		+t33* (_CZZ)));

  Cy = (t12* ( (_CX) 
	      +t14* (_CXX) 
	      +t24* (_CXY) 
	      +t34* (_CXZ))
	+ t14*( t22* (_CXY) 
	       +t32* (_CXZ))
	+ t22*( (_CY) 
	       +t24* (_CYY) 
	       +t34* (_CYZ))
	+ t32*( (_CZ) 
	       + t24* (_CYZ) 
	       + t34* (_CZZ)));

  Cyz = (t12*( t13* (_CXX) 
	      +t23* (_CXY) 
	      +t33* (_CXZ))
	 + t13*( t22* (_CXY) 
		+t32* (_CXZ))
	 + t22*( t23* (_CYY) 
		+t33* (_CYZ))
	 + t32*( t23* (_CYZ) 
		+t33* (_CZZ)));
  

  _CXX=Cxx; _CYY=Cyy; _CZZ=Czz;   _CX=Cx;   _CY=Cy;   _CZ=Cz;
  _CXY=Cxy; _CXZ=Cxz; _CYZ=Cyz;  _CCte=Ccte; 


}
  
