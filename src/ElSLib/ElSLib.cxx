// Created on: 1991-09-09
// Created by: Michel Chauvat
// Copyright (c) 1991-1999 Matra Datavision
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

//  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620


#ifndef No_Exception
#define No_Exception
#endif


#include <ElSLib.hxx>
#include <gp.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Cone.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>
#include <gp_Torus.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_XYZ.hxx>

static Standard_Real PIPI = M_PI + M_PI;

gp_Pnt ElSLib::PlaneValue (const Standard_Real U,
			   const Standard_Real V,
			   const gp_Ax3& Pos)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  return gp_Pnt(U * XDir.X() + V * YDir.X() + PLoc.X(),
		U * XDir.Y() + V * YDir.Y() + PLoc.Y(),
		U * XDir.Z() + V * YDir.Z() + PLoc.Z());
}

gp_Pnt ElSLib::ConeValue (const Standard_Real U, 
			  const Standard_Real V,
			  const gp_Ax3& Pos,
			  const Standard_Real Radius,
			  const Standard_Real SAngle) 
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real R  = Radius + V * sin(SAngle);
  Standard_Real A3 =          V * cos(SAngle);
  Standard_Real A1 = R * cos(U);
  Standard_Real A2 = R * sin(U);
  return gp_Pnt(A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X(),
		A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y(),
		A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
}

gp_Pnt ElSLib::CylinderValue (const Standard_Real U,
			      const Standard_Real V, 
			      const gp_Ax3& Pos, 
			      const Standard_Real Radius)
{
  // M(u,v) = C + Radius * ( Xdir * Cos(u) + Ydir * Sin(u)) + V * Zdir
  // where C is the location point of the Axis2placement
  // Xdir, Ydir ,Zdir are the directions of the local coordinates system

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real A1 =  Radius * cos(U);
  Standard_Real A2 =  Radius * sin(U);
  return gp_Pnt(A1 * XDir.X() + A2 * YDir.X() + V * ZDir.X() + PLoc.X(),
		A1 * XDir.Y() + A2 * YDir.Y() + V * ZDir.Y() + PLoc.Y(),
		A1 * XDir.Z() + A2 * YDir.Z() + V * ZDir.Z() + PLoc.Z());
}

gp_Pnt ElSLib::SphereValue (const Standard_Real U,
			    const Standard_Real V,
			    const gp_Ax3& Pos,
			    const Standard_Real Radius)
{
  //M(U,V) = Location +
  //         R * CosV (CosU * XDirection + SinU * YDirection) +
  //         R * SinV * Direction

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real R  = Radius * cos(V);
  Standard_Real A3 = Radius * sin(V);
  Standard_Real A1 = R * cos(U);
  Standard_Real A2 = R * sin(U);
  return gp_Pnt(A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X(),
		A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y(),
		A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
}

gp_Pnt ElSLib::TorusValue (const Standard_Real U,
			   const Standard_Real V,
			   const gp_Ax3& Pos,
			   const Standard_Real MajorRadius,
			   const Standard_Real MinorRadius)
{
  //M(U,V) = 
  //  Location +
  //  (MajRadius+MinRadius*Cos(V)) * (Cos(U)*XDirection + Sin(U)*YDirection) +
  //  MinorRadius * Sin(V) * Direction

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real R  = MajorRadius + MinorRadius * cos(V);
  Standard_Real A3 =               MinorRadius * sin(V);
  Standard_Real A1 = R * cos(U);
  Standard_Real A2 = R * sin(U);
  //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
  Standard_Real eps = 10.*(MinorRadius + MajorRadius)*RealEpsilon();

  if (Abs(A1) <= eps)
    A1 = 0.;

  if (Abs(A2) <= eps)
    A2 = 0.;

  if (Abs(A3) <= eps)
    A3 = 0.;

  //  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620 End
  return gp_Pnt(A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X(),
		A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y(),
		A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
}

gp_Vec ElSLib::PlaneDN (const Standard_Real,
			const Standard_Real,
			const gp_Ax3& Pos,
			const Standard_Integer Nu,
			const Standard_Integer Nv)
{
  if      (Nu == 0 && Nv == 1) { return gp_Vec (Pos.YDirection()); }
  else if (Nu == 1 && Nv == 0) { return gp_Vec (Pos.XDirection()); }
  return gp_Vec (0., 0., 0.); 
}

gp_Vec ElSLib::ConeDN (const Standard_Real    U, 
		       const Standard_Real    V,
		       const gp_Ax3&    Pos, 
		       const Standard_Real    Radius,
		       const Standard_Real    SAngle, 
		       const Standard_Integer Nu,
		       const Standard_Integer Nv)
{
   gp_XYZ Xdir = Pos.XDirection().XYZ();
   gp_XYZ Ydir = Pos.YDirection().XYZ(); 
   Standard_Real Um = U + Nu * M_PI_2;  // M_PI * 0.5
   Xdir.Multiply(cos(Um));
   Ydir.Multiply(sin(Um));
   Xdir.Add(Ydir);
   if(Nv == 0) { 
     Xdir.Multiply(Radius + V * sin(SAngle));
     if(Nu == 0) Xdir.Add(Pos.Location().XYZ());
     return gp_Vec(Xdir);
   }
   else if(Nv == 1) { 
     Xdir.Multiply(sin(SAngle));
     return gp_Vec(Xdir);     
   }
   return gp_Vec(0.0,0.0,0.0);
}

gp_Vec ElSLib::CylinderDN (const Standard_Real    U,
			   const Standard_Real,
			   const gp_Ax3&    Pos,
			   const Standard_Real    Radius,
			   const Standard_Integer Nu,
			   const Standard_Integer Nv)
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0) { return gp_Vec(); }
  if (Nv == 0) {
    Standard_Real RCosU = Radius * cos(U);
    Standard_Real RSinU = Radius * sin(U);
    gp_XYZ Xdir = Pos.XDirection().XYZ();
    gp_XYZ Ydir = Pos.YDirection().XYZ();
    if ((Nu + 6) % 4 == 0) {
      Xdir.Multiply (-RCosU);
      Ydir.Multiply (-RSinU);
    }
    else if ((Nu + 5) % 4 == 0) {
      Xdir.Multiply ( RSinU);
      Ydir.Multiply (-RCosU);
    }
    else if ((Nu + 3) % 4 == 0) {
      Xdir.Multiply (-RSinU);
      Ydir.Multiply ( RCosU);
    }
    else if (Nu % 4 == 0) {
      Xdir.Multiply ( RCosU);
      Ydir.Multiply ( RSinU);
    }
    Xdir.Add (Ydir);
    return gp_Vec (Xdir);
  }
  else if (Nv == 1 && Nu == 0) { return gp_Vec (Pos.Direction()); }
  else { return gp_Vec (0.0, 0.0, 0.0); }
}

gp_Vec ElSLib::SphereDN (const Standard_Real    U,
			 const Standard_Real    V,
			 const gp_Ax3&    Pos,
			 const Standard_Real    Radius,
			 const Standard_Integer Nu, 
			 const Standard_Integer Nv)
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0) { return gp_Vec(); }
  Standard_Real CosU  = cos(U);
  Standard_Real SinU  = sin(U);
  Standard_Real RCosV = Radius * cos(V);
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  Standard_Real A1,A2,A3,X,Y,Z;
  if (Nu == 0) {
    Standard_Real RSinV = Radius * sin(V);
    if (IsOdd (Nv)) { A1 = - RSinV * CosU; A2 = - RSinV * SinU; A3 =   RCosV; }
    else            { A1 = - RCosV * CosU; A2 = - RCosV * SinU; A3 = - RSinV; }
    X = A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X();
    Y = A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y();
    Z = A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z();
    if (!( (Nv + 2) % 4 == 0 || (Nv + 3) % 4 == 0 ))
      { X = - X; Y = - Y; Z = - Z; }
  }
  else if (Nv == 0) {
    if (IsOdd (Nu)) { A1 = - RCosV * SinU; A2 =   RCosV * CosU; }
    else            { A1 =   RCosV * CosU; A2 =   RCosV * SinU; }
    X = A1 * XDir.X() + A2 * YDir.X();
    Y = A1 * XDir.Y() + A2 * YDir.Y();
    Z = A1 * XDir.Z() + A2 * YDir.Z();
    if ( (Nu + 2) % 4 == 0 || (Nu + 1) % 4 == 0 )
      { X = - X; Y = - Y; Z = - Z; }
  }
  else {
    Standard_Real RSinV = Radius * sin(V);
    if (IsOdd (Nu)) { A1 = - SinU; A2 =   CosU; }
    else            { A1 = - CosU; A2 = - SinU; }
    if (IsOdd (Nv)) A3 = - RSinV;
    else            A3 = - RCosV;
    X = (A1 * XDir.X() + A2 * YDir.X()) * A3;
    Y = (A1 * XDir.Y() + A2 * YDir.Y()) * A3;
    Z = (A1 * XDir.Z() + A2 * YDir.Z()) * A3;
    if ((!((Nu + 2) % 4 == 0 || (Nu + 3) % 4 == 0) &&
	 ((Nv + 2) % 4 == 0 || (Nv + 3) % 4 == 0)) ||
	(((Nu + 2) % 4 == 0 || (Nu + 3) % 4 == 0) &&
	 !((Nv + 2) % 4 == 0 || (Nv + 3) % 4 == 0)))
      { X = - X; Y = - Y; Z = - Z; }
  }
  return gp_Vec(X,Y,Z);
}

gp_Vec ElSLib::TorusDN (const Standard_Real    U,
			const Standard_Real    V,
			const gp_Ax3&    Pos,
			const Standard_Real    MajorRadius,
			const Standard_Real    MinorRadius,
			const Standard_Integer Nu, 
			const Standard_Integer Nv)
{
  if (Nu + Nv < 1 || Nu < 0 || Nv < 0) { return gp_Vec(); }
  Standard_Real CosU  = cos(U);
  Standard_Real SinU  = sin(U);
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  Standard_Real A1,A2,A3,X=0,Y=0,Z=0;
  //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
  Standard_Real eps = 10.*(MinorRadius + MajorRadius)*RealEpsilon();
  //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 End
  if (Nv == 0) {
    Standard_Real R = MajorRadius + MinorRadius * cos(V);
    if (IsOdd (Nu)) { A1 = - R * SinU; A2 =   R * CosU; }
    else            { A1 = - R * CosU; A2 = - R * SinU; }
    //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
    if (Abs(A1) <= eps)
      A1 = 0.;

    if (Abs(A2) <= eps)
      A2 = 0.;
    //  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620 End
    X = A1 * XDir.X() + A2 * YDir.X();
    Y = A1 * XDir.Y() + A2 * YDir.Y();
    Z = A1 * XDir.Z() + A2 * YDir.Z();
    if (!((Nu + 2) % 4 == 0 || (Nu + 3) % 4 == 0))
      { X = - X; Y = - Y; Z = - Z; }
  }
  else if (Nu == 0) {
    Standard_Real RCosV = MinorRadius * cos(V);
    Standard_Real RSinV = MinorRadius * sin(V);
    if (IsOdd (Nv)) { A1 = - RSinV * CosU; A2 = - RSinV * SinU; A3 =   RCosV; }
    else            { A1 = - RCosV * CosU; A2 = - RCosV * SinU; A3 = - RSinV; }
    //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
    if (Abs(A1) <= eps)
      A1 = 0.;

    if (Abs(A2) <= eps)
      A2 = 0.;

    if (Abs(A3) <= eps)
      A3 = 0.;
    //  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620 End
    X = A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X();
    Y = A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y();
    Z = A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z();
    if (!((Nv + 2) % 4 == 0 || (Nv + 3) % 4 == 0))
      { X = - X; Y = - Y; Z = - Z; }
  }
  else {
    if (IsOdd (Nu) &&
	IsOdd (Nv)) {
      Standard_Real RSinV = MinorRadius * sin(V);
      A1 = RSinV * SinU; A2 = - RSinV * CosU;
      //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
      if (Abs(A1) <= eps)
	A1 = 0.;

      if (Abs(A2) <= eps)
	A2 = 0.;
      //  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620 End
      X = A1 * XDir.X() + A2 * YDir.X();
      Y = A1 * XDir.Y() + A2 * YDir.Y();
      Z = A1 * XDir.Z() + A2 * YDir.Z();
    }
    else if (IsEven (Nu) && IsEven (Nv)) {
      Standard_Real RCosV = MinorRadius * cos(V);
      A1 = RCosV * CosU; A2 =   RCosV * SinU;
      //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
      if (Abs(A1) <= eps)
	A1 = 0.;

      if (Abs(A2) <= eps)
	A2 = 0.;
      //  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620 End
      X = A1 * XDir.X() + A2 * YDir.X();
      Y = A1 * XDir.Y() + A2 * YDir.Y();
      Z = A1 * XDir.Z() + A2 * YDir.Z();
    }
    else if (IsEven (Nv) && IsOdd (Nu)) {
      Standard_Real RCosV = MinorRadius * cos(V);
      A1 = RCosV * SinU; A2 = - RCosV * CosU;
      //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
      if (Abs(A1) <= eps)
	A1 = 0.;

      if (Abs(A2) <= eps)
	A2 = 0.;
      //  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620 End
      X = A1 * XDir.X() + A2 * YDir.X();
      Y = A1 * XDir.Y() + A2 * YDir.Y();
      Z = A1 * XDir.Z() + A2 * YDir.Z();
      if (((Nv + Nu + 3) % 4) == 0)
	{ X = - X; Y = - Y; Z = - Z; }
    }
    else if (IsOdd (Nv) && IsEven (Nu)) {
      Standard_Real RSinV = MinorRadius * sin(V);
      A1 = RSinV * CosU; A2 =   RSinV * SinU;
      //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
      if (Abs(A1) <= eps)
	A1 = 0.;

      if (Abs(A2) <= eps)
	A2 = 0.;
      //  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620 End
      X = A1 * XDir.X() + A2 * YDir.X();
      Y = A1 * XDir.Y() + A2 * YDir.Y();
      Z = A1 * XDir.Z() + A2 * YDir.Z();
      if (((Nu + Nv + 3) % 4) == 0)
	{ X = - X; Y = - Y; Z = - Z; }
    }
  }
  return gp_Vec (X,Y,Z);
}

void ElSLib::PlaneD0 (const Standard_Real U,
		      const Standard_Real V, 
		      const gp_Ax3& Pos,
		      gp_Pnt& P) 
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  P.SetX(U * XDir.X() + V * YDir.X() + PLoc.X());
  P.SetY(U * XDir.Y() + V * YDir.Y() + PLoc.Y());
  P.SetZ(U * XDir.Z() + V * YDir.Z() + PLoc.Z());
}

void ElSLib::ConeD0 (const Standard_Real U,
		     const Standard_Real V,
		     const gp_Ax3& Pos, 
		     const Standard_Real Radius,
		     const Standard_Real SAngle,
		     gp_Pnt& P)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real R  = Radius + V * sin(SAngle);
  Standard_Real A3 =          V * cos(SAngle);
  Standard_Real A1 = R * cos(U);
  Standard_Real A2 = R * sin(U);
  P.SetX(A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X());
  P.SetY(A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y());
  P.SetZ(A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
}

void ElSLib::CylinderD0 (const Standard_Real U,
			 const Standard_Real V,
			 const gp_Ax3& Pos, 
			 const Standard_Real Radius,
			 gp_Pnt& P)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real A1 = Radius * cos(U);
  Standard_Real A2 = Radius * sin(U);
  P.SetX(A1 * XDir.X() + A2 * YDir.X() + V * ZDir.X() + PLoc.X());
  P.SetY(A1 * XDir.Y() + A2 * YDir.Y() + V * ZDir.Y() + PLoc.Y());
  P.SetZ(A1 * XDir.Z() + A2 * YDir.Z() + V * ZDir.Z() + PLoc.Z());
}

void ElSLib::SphereD0 (const Standard_Real U,
		       const Standard_Real V,
		       const gp_Ax3& Pos, 
		       const Standard_Real Radius, gp_Pnt& P)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real R  = Radius * cos(V);
  Standard_Real A3 = Radius * sin(V);
  Standard_Real A1 = R * cos(U);
  Standard_Real A2 = R * sin(U);
  P.SetX(A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X());
  P.SetY(A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y());
  P.SetZ(A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
}

void ElSLib::TorusD0 (const Standard_Real U,
		      const Standard_Real V,
		      const gp_Ax3& Pos,
		      const Standard_Real MajorRadius,
		      const Standard_Real MinorRadius, 
		      gp_Pnt& P )
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real R  = MajorRadius + MinorRadius * cos(V);
  Standard_Real A3 =               MinorRadius * sin(V);
  Standard_Real A1 = R * cos(U);
  Standard_Real A2 = R * sin(U);
  //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
  Standard_Real eps = 10.*(MinorRadius + MajorRadius)*RealEpsilon();

  if (Abs(A1) <= eps)
    A1 = 0.;

  if (Abs(A2) <= eps)
    A2 = 0.;

  if (Abs(A3) <= eps)
    A3 = 0.;
  //  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620 End
  P.SetX(A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X());
  P.SetY(A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y());
  P.SetZ(A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
}                      

void ElSLib::PlaneD1 (const Standard_Real U, 
		      const Standard_Real V, 
		      const gp_Ax3& Pos,
		      gp_Pnt&       P,
		      gp_Vec&       Vu,
		      gp_Vec&       Vv)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  P.SetX(U * XDir.X() + V * YDir.X() + PLoc.X());
  P.SetY(U * XDir.Y() + V * YDir.Y() + PLoc.Y());
  P.SetZ(U * XDir.Z() + V * YDir.Z() + PLoc.Z());
  Vu.SetX(XDir.X());
  Vu.SetY(XDir.Y());
  Vu.SetZ(XDir.Z());
  Vv.SetX(YDir.X());
  Vv.SetY(YDir.Y());
  Vv.SetZ(YDir.Z());
}

void ElSLib::ConeD1 (const Standard_Real U,
		     const Standard_Real V,
		     const gp_Ax3& Pos, 
		     const Standard_Real Radius,
		     const Standard_Real SAngle,
		     gp_Pnt& P,
		     gp_Vec& Vu,
		     gp_Vec& Vv)
{
  // Z = V * Cos(SAngle)
  // M(U,V) = Location() + V * Cos(SAngle) * ZDirection() +
  // (Radius + V*Sin(SAng)) * (Cos(U) * XDirection() + Sin(U) * YDirection())

  // D1U = 
  //(Radius + V*Sin(SAng)) * (-Sin(U) * XDirection() + Cos(U) * YDirection())

  // D1V = 
  // Direction() *Cos(SAngle) + Sin(SAng) * (Cos(U) * XDirection() + 
  // Sin(U) * YDirection())

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real CosU = cos(U);
  Standard_Real SinU = sin(U);
  Standard_Real CosA = cos(SAngle);
  Standard_Real SinA = sin(SAngle);
  Standard_Real R  = Radius + V * SinA;
  Standard_Real A3 =          V * CosA;
  Standard_Real A1 = R * CosU;
  Standard_Real A2 = R * SinU;
  Standard_Real R1 = SinA * CosU;
  Standard_Real R2 = SinA * SinU;
  P .SetX(  A1 * XDir.X() + A2 * YDir.X() + A3 * ZDir.X() + PLoc.X());
  P .SetY(  A1 * XDir.Y() + A2 * YDir.Y() + A3 * ZDir.Y() + PLoc.Y());
  P .SetZ(  A1 * XDir.Z() + A2 * YDir.Z() + A3 * ZDir.Z() + PLoc.Z());
  Vu.SetX(- A2 * XDir.X() + A1 * YDir.X());
  Vu.SetY(- A2 * XDir.Y() + A1 * YDir.Y());
  Vu.SetZ(- A2 * XDir.Z() + A1 * YDir.Z());
  Vv.SetX(  R1 * XDir.X() + R2 * YDir.X() + CosA * ZDir.X());
  Vv.SetY(  R1 * XDir.Y() + R2 * YDir.Y() + CosA * ZDir.Y());
  Vv.SetZ(  R1 * XDir.Z() + R2 * YDir.Z() + CosA * ZDir.Z());
}

void ElSLib::CylinderD1 (const Standard_Real U,
			 const Standard_Real V,
			 const gp_Ax3& Pos,
			 const Standard_Real Radius,
			 gp_Pnt&       P,
			 gp_Vec&       Vu,
			 gp_Vec&       Vv)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real A1 = Radius * cos(U);
  Standard_Real A2 = Radius * sin(U);
  P .SetX(  A1 * XDir.X() + A2 * YDir.X() + V * ZDir.X() + PLoc.X());
  P .SetY(  A1 * XDir.Y() + A2 * YDir.Y() + V * ZDir.Y() + PLoc.Y());
  P .SetZ(  A1 * XDir.Z() + A2 * YDir.Z() + V * ZDir.Z() + PLoc.Z());
  Vu.SetX(- A2 * XDir.X() + A1 * YDir.X());
  Vu.SetY(- A2 * XDir.Y() + A1 * YDir.Y());
  Vu.SetZ(- A2 * XDir.Z() + A1 * YDir.Z());
  Vv.SetX(  ZDir.X());
  Vv.SetY(  ZDir.Y());
  Vv.SetZ(  ZDir.Z());
}

void ElSLib::SphereD1 (const Standard_Real U, 
		       const Standard_Real V,
		       const gp_Ax3& Pos,
		       const Standard_Real Radius,
		       gp_Pnt&       P, 
		       gp_Vec&       Vu, 
		       gp_Vec&       Vv)
{
  // Vxy = CosU * XDirection + SinU * YDirection
  // DVxy = -SinU * XDirection + CosU * YDirection

  // P(U,V) = Location +  R * CosV * Vxy  +   R * SinV * Direction
  
  // Vu = R * CosV * DVxy

  // Vv = -R * SinV * Vxy + R * CosV * Direction

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real CosU = cos(U);
  Standard_Real SinU = sin(U);
  Standard_Real R1 = Radius * cos(V);
  Standard_Real R2 = Radius * sin(V);
  Standard_Real A1 = R1 * CosU;
  Standard_Real A2 = R1 * SinU;
  Standard_Real A3 = R2 * CosU;
  Standard_Real A4 = R2 * SinU;
  P .SetX(  A1 * XDir.X() + A2 * YDir.X() + R2 * ZDir.X() + PLoc.X());
  P .SetY(  A1 * XDir.Y() + A2 * YDir.Y() + R2 * ZDir.Y() + PLoc.Y());
  P .SetZ(  A1 * XDir.Z() + A2 * YDir.Z() + R2 * ZDir.Z() + PLoc.Z());
  Vu.SetX(- A2 * XDir.X() + A1 * YDir.X());
  Vu.SetY(- A2 * XDir.Y() + A1 * YDir.Y());
  Vu.SetZ(- A2 * XDir.Z() + A1 * YDir.Z());
  Vv.SetX(- A3 * XDir.X() - A4 * YDir.X() + R1 * ZDir.X());
  Vv.SetY(- A3 * XDir.Y() - A4 * YDir.Y() + R1 * ZDir.Y());
  Vv.SetZ(- A3 * XDir.Z() - A4 * YDir.Z() + R1 * ZDir.Z());
}

void ElSLib::TorusD1 ( const Standard_Real U,
		      const Standard_Real V,
		      const gp_Ax3& Pos,
		      const Standard_Real MajorRadius,
		      const Standard_Real MinorRadius, 
		      gp_Pnt&       P,
		      gp_Vec&       Vu,
		      gp_Vec&       Vv)
{

  //P(U,V) = 
  //  Location +
  //  (MajorRadius+MinorRadius*Cos(V)) *
  //  (Cos(U)*XDirection + Sin(U)*YDirection) +
  //  MinorRadius * Sin(V) * Direction

  //Vv = -MinorRadius * Sin(V) * (Cos(U)*XDirection + Sin(U)*YDirection) +
  //      MinorRadius * Cos(V) * Direction

  //Vu =
  // (MajorRadius+MinorRadius*Cos(V)) *
  // (-Sin(U)*XDirection + Cos(U)*YDirection)

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real CosU = cos(U);
  Standard_Real SinU = sin(U);
  Standard_Real R1 = MinorRadius * cos(V);
  Standard_Real R2 = MinorRadius * sin(V);
  Standard_Real R  = MajorRadius + R1;
  Standard_Real A1 = R * CosU;
  Standard_Real A2 = R * SinU;
  Standard_Real A3 = R2 * CosU;
  Standard_Real A4 = R2 * SinU;
  //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
  Standard_Real eps = 10.*(MinorRadius + MajorRadius)*RealEpsilon();

  if (Abs(A1) <= eps)
    A1 = 0.;

  if (Abs(A2) <= eps)
    A2 = 0.;

  if (Abs(A3) <= eps)
    A3 = 0.;

  if (Abs(A4) <= eps)
    A4 = 0.;
  //  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620 End
  P .SetX(  A1 * XDir.X() + A2 * YDir.X() + R2 * ZDir.X() + PLoc.X());
  P .SetY(  A1 * XDir.Y() + A2 * YDir.Y() + R2 * ZDir.Y() + PLoc.Y());
  P .SetZ(  A1 * XDir.Z() + A2 * YDir.Z() + R2 * ZDir.Z() + PLoc.Z());
  Vu.SetX(- A2 * XDir.X() + A1 * YDir.X());
  Vu.SetY(- A2 * XDir.Y() + A1 * YDir.Y());
  Vu.SetZ(- A2 * XDir.Z() + A1 * YDir.Z());
  Vv.SetX(- A3 * XDir.X() - A4 * YDir.X() + R1 * ZDir.X());
  Vv.SetY(- A3 * XDir.Y() - A4 * YDir.Y() + R1 * ZDir.Y());
  Vv.SetZ(- A3 * XDir.Z() - A4 * YDir.Z() + R1 * ZDir.Z());
}                      

void ElSLib::ConeD2 (const Standard_Real U,
		     const Standard_Real V,
		     const gp_Ax3& Pos, 
		     const Standard_Real Radius,
		     const Standard_Real SAngle, 
		     gp_Pnt&       P,
		     gp_Vec&       Vu, 
		     gp_Vec&       Vv,
		     gp_Vec&       Vuu,
		     gp_Vec&       Vvv,
		     gp_Vec&       Vuv)
{
  // Z = V * Cos(SAngle)
  // M(U,V) = Location() + V * Cos(SAngle) * Direction() +
  // (Radius + V*Sin(SAng)) * (Cos(U) * XDirection() + Sin(U) * YDirection())

  // DU = 
  //(Radius + V*Sin(SAng)) * (-Sin(U) * XDirection() + Cos(U) * YDirection())

  // DV = 
  // Direction() *Cos(SAngle) + Sin(SAng) * (Cos(U) * XDirection() + 
  // Sin(U) * YDirection())

  // D2U =
  //(Radius + V*Sin(SAng)) * (-Cos(U) * XDirection() - Sin(U) * YDirection())

  // D2V = 0.0   
  
  // DUV = 
  //Sin(SAng) * (-Sin(U) * XDirection() + Cos(U) * YDirection())

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real CosU = cos(U);
  Standard_Real SinU = sin(U);
  Standard_Real CosA = cos(SAngle);
  Standard_Real SinA = sin(SAngle);
  Standard_Real R  = Radius + V * SinA;
  Standard_Real A3 =          V * CosA;
  Standard_Real A1 = R * CosU;
  Standard_Real A2 = R * SinU;
  Standard_Real R1 = SinA * CosU;
  Standard_Real R2 = SinA * SinU;
  Standard_Real Som1X = A1 * XDir.X() + A2 * YDir.X();
  Standard_Real Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  Standard_Real Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  P  .SetX(  Som1X + A3 * ZDir.X() + PLoc.X());
  P  .SetY(  Som1Y + A3 * ZDir.Y() + PLoc.Y());
  P  .SetZ(  Som1Z + A3 * ZDir.Z() + PLoc.Z());
  Vu .SetX(- A2 * XDir.X() + A1 * YDir.X());
  Vu .SetY(- A2 * XDir.Y() + A1 * YDir.Y());
  Vu .SetZ(- A2 * XDir.Z() + A1 * YDir.Z());
  Vv .SetX(  R1 * XDir.X() + R2 * YDir.X() + CosA * ZDir.X());
  Vv .SetY(  R1 * XDir.Y() + R2 * YDir.Y() + CosA * ZDir.Y());
  Vv .SetZ(  R1 * XDir.Z() + R2 * YDir.Z() + CosA * ZDir.Z());
  Vuu.SetX(- Som1X);
  Vuu.SetY(- Som1Y);
  Vuu.SetZ(- Som1Z);
  Vvv.SetX(  0.0);
  Vvv.SetY(  0.0);
  Vvv.SetZ(  0.0);
  Vuv.SetX(- R2 * XDir.X() + R1 * YDir.X());
  Vuv.SetY(- R2 * XDir.Y() + R1 * YDir.Y());
  Vuv.SetZ(- R2 * XDir.Z() + R1 * YDir.Z());
}

void ElSLib::CylinderD2 (const Standard_Real U,
			 const Standard_Real V,
			 const gp_Ax3& Pos,
			 const Standard_Real Radius,
			 gp_Pnt&       P,
			 gp_Vec&       Vu,
			 gp_Vec&       Vv,
			 gp_Vec&       Vuu,
			 gp_Vec&       Vvv,
			 gp_Vec&       Vuv)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real A1 = Radius * cos(U);
  Standard_Real A2 = Radius * sin(U);
  Standard_Real Som1X = A1 * XDir.X() + A2 * YDir.X();
  Standard_Real Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  Standard_Real Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  P  .SetX(  Som1X + V * ZDir.X() + PLoc.X());
  P  .SetY(  Som1Y + V * ZDir.Y() + PLoc.Y());
  P  .SetZ(  Som1Z + V * ZDir.Z() + PLoc.Z());
  Vu .SetX(- A2 * XDir.X() + A1 * YDir.X());
  Vu .SetY(- A2 * XDir.Y() + A1 * YDir.Y());
  Vu .SetZ(- A2 * XDir.Z() + A1 * YDir.Z());
  Vv .SetX(  ZDir.X());
  Vv .SetY(  ZDir.Y());
  Vv .SetZ(  ZDir.Z());
  Vuu.SetX(- Som1X);
  Vuu.SetY(- Som1Y);
  Vuu.SetZ(- Som1Z);
  Vvv.SetX(  0.0);
  Vvv.SetY(  0.0);
  Vvv.SetZ(  0.0);
  Vuv.SetX(  0.0);
  Vuv.SetY(  0.0);
  Vuv.SetZ(  0.0);
}

void ElSLib::SphereD2 (const Standard_Real U,
		       const Standard_Real V,
		       const gp_Ax3& Pos, 
		       const Standard_Real Radius,
		       gp_Pnt&       P, 
		       gp_Vec&       Vu,
		       gp_Vec&       Vv,
		       gp_Vec&       Vuu,
		       gp_Vec&       Vvv,
		       gp_Vec&       Vuv)
{
  // Vxy = CosU * XDirection + SinU * YDirection
  // DVxy = -SinU * XDirection + CosU * YDirection

  // P(U,V) = Location +  R * CosV * Vxy  +   R * SinV * Direction
  
  // Vu = R * CosV * DVxy

  // Vuu = - R * CosV * Vxy

  // Vv = -R * SinV * Vxy + R * CosV * Direction

  // Vvv = -R * CosV * Vxy - R * SinV * Direction

  // Vuv = - R * SinV * DVxy

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real CosU = cos(U);
  Standard_Real SinU = sin(U);
  Standard_Real R1 = Radius * cos(V);
  Standard_Real R2 = Radius * sin(V);
  Standard_Real A1 = R1 * CosU;
  Standard_Real A2 = R1 * SinU;
  Standard_Real A3 = R2 * CosU;
  Standard_Real A4 = R2 * SinU;
  Standard_Real Som1X = A1 * XDir.X() + A2 * YDir.X();
  Standard_Real Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  Standard_Real Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  Standard_Real R2ZX = R2 * ZDir.X();
  Standard_Real R2ZY = R2 * ZDir.Y();
  Standard_Real R2ZZ = R2 * ZDir.Z();
  P  .SetX(  Som1X + R2ZX + PLoc.X());
  P  .SetY(  Som1Y + R2ZY + PLoc.Y());
  P  .SetZ(  Som1Z + R2ZZ + PLoc.Z());
  Vu .SetX(- A2 * XDir.X() + A1 * YDir.X());
  Vu .SetY(- A2 * XDir.Y() + A1 * YDir.Y());
  Vu .SetZ(- A2 * XDir.Z() + A1 * YDir.Z());
  Vv .SetX(- A3 * XDir.X() - A4 * YDir.X() + R1 * ZDir.X());
  Vv .SetY(- A3 * XDir.Y() - A4 * YDir.Y() + R1 * ZDir.Y());
  Vv .SetZ(- A3 * XDir.Z() - A4 * YDir.Z() + R1 * ZDir.Z());
  Vuu.SetX(- Som1X);
  Vuu.SetY(- Som1Y);
  Vuu.SetZ(- Som1Z);
  Vvv.SetX(- Som1X - R2ZX);
  Vvv.SetY(- Som1Y - R2ZY);
  Vvv.SetZ(- Som1Z - R2ZZ);
  Vuv.SetX(  A4 * XDir.X() - A3 * YDir.X());
  Vuv.SetY(  A4 * XDir.Y() - A3 * YDir.Y());
  Vuv.SetZ(  A4 * XDir.Z() - A3 * YDir.Z());
}

void ElSLib::TorusD2 (const Standard_Real U,
		      const Standard_Real V,
		      const gp_Ax3& Pos,
		      const Standard_Real MajorRadius, 
		      const Standard_Real MinorRadius,
		      gp_Pnt&       P,
		      gp_Vec&       Vu,
		      gp_Vec&       Vv,
		      gp_Vec&       Vuu,
		      gp_Vec&       Vvv,
		      gp_Vec&       Vuv)
{
  //P(U,V) = 
  //  Location +
  //  (MajorRadius+MinorRadius*Cos(V)) *
  //  (Cos(U)*XDirection + Sin(U)*YDirection) +
  //  MinorRadius * Sin(V) * Direction
  
  //Vv = -MinorRadius * Sin(V) * (Cos(U)*XDirection + Sin(U)*YDirection) +
  //      MinorRadius * Cos(V) * Direction
  
  //Vu =
  // (MajorRadius+MinorRadius*Cos(V)) * 
  // (-Sin(U)*XDirection + Cos(U)*YDirection)
  
  
  //Vvv = -MinorRadius * Cos(V) * (Cos(U)*XDirection + Sin(U)*YDirection) 
  //      -MinorRadius * Sin(V) * Direction

  //Vuu =
  // -(MajorRadius+MinorRadius*Cos(V)) * 
  // (Cos(U)*XDirection + Sin(U)*YDirection)

  //Vuv = MinorRadius * Sin(V) * (Sin(U)*XDirection - Cos(U)*YDirection)

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real CosU = cos(U);
  Standard_Real SinU = sin(U);
  Standard_Real R1 = MinorRadius * cos(V);
  Standard_Real R2 = MinorRadius * sin(V);
  Standard_Real R  = MajorRadius + R1;
  Standard_Real A1 = R  * CosU;
  Standard_Real A2 = R  * SinU;
  Standard_Real A3 = R2 * CosU;
  Standard_Real A4 = R2 * SinU;
  Standard_Real A5 = R1 * CosU;
  Standard_Real A6 = R1 * SinU;
  //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
  Standard_Real eps = 10.*(MinorRadius + MajorRadius)*RealEpsilon();

  if (Abs(A1) <= eps)
    A1 = 0.;

  if (Abs(A2) <= eps)
    A2 = 0.;

  if (Abs(A3) <= eps)
    A3 = 0.;

  if (Abs(A4) <= eps)
    A4 = 0.;

  if (Abs(A5) <= eps)
    A5 = 0.;

  if (Abs(A6) <= eps)
    A6 = 0.;
  //  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620 End
  Standard_Real Som1X = A1 * XDir.X() + A2 * YDir.X();
  Standard_Real Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  Standard_Real Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  Standard_Real R2ZX = R2 * ZDir.X();
  Standard_Real R2ZY = R2 * ZDir.Y();
  Standard_Real R2ZZ = R2 * ZDir.Z();
  P  .SetX(  Som1X + R2ZX + PLoc.X());
  P  .SetY(  Som1Y + R2ZY + PLoc.Y());
  P  .SetZ(  Som1Z + R2ZZ + PLoc.Z());
  Vu .SetX(- A2 * XDir.X() + A1 * YDir.X());
  Vu .SetY(- A2 * XDir.Y() + A1 * YDir.Y());
  Vu .SetZ(- A2 * XDir.Z() + A1 * YDir.Z());
  Vv .SetX(- A3 * XDir.X() - A4 * YDir.X() + R1 * ZDir.X());
  Vv .SetY(- A3 * XDir.Y() - A4 * YDir.Y() + R1 * ZDir.Y());
  Vv .SetZ(- A3 * XDir.Z() - A4 * YDir.Z() + R1 * ZDir.Z());
  Vuu.SetX(- Som1X);
  Vuu.SetY(- Som1Y);
  Vuu.SetZ(- Som1Z);
  Vvv.SetX(- A5 * XDir.X() - A6 * YDir.X() - R2ZX);
  Vvv.SetY(- A5 * XDir.Y() - A6 * YDir.Y() - R2ZY);
  Vvv.SetZ(- A5 * XDir.Z() - A6 * YDir.Z() - R2ZZ);
  Vuv.SetX(  A4 * XDir.X() - A3 * YDir.X());
  Vuv.SetY(  A4 * XDir.Y() - A3 * YDir.Y());
  Vuv.SetZ(  A4 * XDir.Z() - A3 * YDir.Z());
}

void ElSLib::ConeD3 (const Standard_Real U,
		     const Standard_Real V,
		     const gp_Ax3& Pos,
		     const Standard_Real Radius,
		     const Standard_Real SAngle,
		     gp_Pnt& P, 
		     gp_Vec& Vu, gp_Vec& Vv, 
		     gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv,
		     gp_Vec& Vuuu, gp_Vec& Vvvv,
		     gp_Vec& Vuuv, gp_Vec& Vuvv)
{
  // Z = V * Cos(SAngle)
  // M(U,V) = Location() + V * Cos(SAngle) * Direction() +
  // (Radius + V*Sin(SAng)) * (Cos(U) * XDirection() + Sin(U) * YDirection())

  // DU = 
  //(Radius + V*Sin(SAng)) * (-Sin(U) * XDirection() + Cos(U) * YDirection())

  // DV = 
  // Direction() *Cos(SAngle) + Sin(SAng) * (Cos(U) * XDirection() + 
  // Sin(U) * YDirection())

  // D2U =
  //(Radius + V*Sin(SAng)) * (-Cos(U) * XDirection() - Sin(U) * YDirection())

  // D2V = 0.0   
  
  // DUV = 
  //Sin(SAng) * (-Sin(U) * XDirection() + Cos(U) * YDirection()) 

  // D3U =
  //(Radius + V*Sin(SAng)) * (Sin(U) * XDirection() - Cos(U) * YDirection())

  // DUVV = 0.0

  // D3V = 0.0

  // DUUV =  Sin(SAng) * (-Cos(U)*XDirection()-Sin(U) * YDirection()) +


  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real CosU = cos(U);
  Standard_Real SinU = sin(U);
  Standard_Real CosA = cos(SAngle);
  Standard_Real SinA = sin(SAngle);
  Standard_Real R  = Radius + V * SinA;
  Standard_Real A3 =          V * CosA;
  Standard_Real A1 = R * CosU;
  Standard_Real A2 = R * SinU;
  Standard_Real R1 = SinA * CosU;
  Standard_Real R2 = SinA * SinU;
  Standard_Real Som1X = A1 * XDir.X() + A2 * YDir.X();
  Standard_Real Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  Standard_Real Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  Standard_Real Som2X = R1 * XDir.X() + R2 * YDir.X();
  Standard_Real Som2Y = R1 * XDir.Y() + R2 * YDir.Y();
  Standard_Real Som2Z = R1 * XDir.Z() + R2 * YDir.Z();
  Standard_Real Dif1X = A2 * XDir.X() - A1 * YDir.X();
  Standard_Real Dif1Y = A2 * XDir.Y() - A1 * YDir.Y();
  Standard_Real Dif1Z = A2 * XDir.Z() - A1 * YDir.Z();
  P   .SetX(  Som1X + A3 * ZDir.X() + PLoc.X());
  P   .SetY(  Som1Y + A3 * ZDir.Y() + PLoc.Y());
  P   .SetZ(  Som1Z + A3 * ZDir.Z() + PLoc.Z());
  Vu  .SetX(- Dif1X);
  Vu  .SetY(- Dif1Y);
  Vu  .SetZ(- Dif1Z);
  Vv  .SetX(  Som2X + CosA * ZDir.X());
  Vv  .SetY(  Som2Y + CosA * ZDir.Y());
  Vv  .SetZ(  Som2Z + CosA * ZDir.Z());
  Vuu .SetX(- Som1X);
  Vuu .SetY(- Som1Y);
  Vuu .SetZ(- Som1Z);
  Vvv .SetX(  0.0);
  Vvv .SetY(  0.0);
  Vvv .SetZ(  0.0);
  Vuv .SetX(- R2 * XDir.X() + R1 * YDir.X());
  Vuv .SetY(- R2 * XDir.Y() + R1 * YDir.Y());
  Vuv .SetZ(- R2 * XDir.Z() + R1 * YDir.Z());
  Vuuu.SetX(  Dif1X);
  Vuuu.SetY(  Dif1Y);
  Vuuu.SetZ(  Dif1Z);
  Vvvv.SetX(  0.0);
  Vvvv.SetY(  0.0);
  Vvvv.SetZ(  0.0);
  Vuvv.SetX(  0.0);
  Vuvv.SetY(  0.0);
  Vuvv.SetZ(  0.0);
  Vuuv.SetX(- Som2X);
  Vuuv.SetY(- Som2Y);
  Vuuv.SetZ(- Som2Z);
}

void ElSLib::CylinderD3 (const Standard_Real U,
			 const Standard_Real V,
			 const gp_Ax3& Pos,
			 const Standard_Real Radius,
			 gp_Pnt& P,
			 gp_Vec& Vu, gp_Vec& Vv,
			 gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv,
			 gp_Vec& Vuuu, gp_Vec& Vvvv,
			 gp_Vec& Vuuv, gp_Vec& Vuvv)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real A1 = Radius * cos(U);
  Standard_Real A2 = Radius * sin(U);
  Standard_Real Som1X = A1 * XDir.X() + A2 * YDir.X();
  Standard_Real Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  Standard_Real Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  Standard_Real Dif1X = A2 * XDir.X() - A1 * YDir.X();
  Standard_Real Dif1Y = A2 * XDir.Y() - A1 * YDir.Y();
  Standard_Real Dif1Z = A2 * XDir.Z() - A1 * YDir.Z();
  P   .SetX(  Som1X + V * ZDir.X() + PLoc.X());
  P   .SetY(  Som1Y + V * ZDir.Y() + PLoc.Y());
  P   .SetZ(  Som1Z + V * ZDir.Z() + PLoc.Z());
  Vu  .SetX(- Dif1X);
  Vu  .SetY(- Dif1Y);
  Vu  .SetZ(- Dif1Z);
  Vv  .SetX(  ZDir.X());
  Vv  .SetY(  ZDir.Y());
  Vv  .SetZ(  ZDir.Z());
  Vuu .SetX(- Som1X);
  Vuu .SetY(- Som1Y);
  Vuu .SetZ(- Som1Z);
  Vvv .SetX(  0.0);
  Vvv .SetY(  0.0);
  Vvv .SetZ(  0.0);
  Vuv .SetX(  0.0);
  Vuv .SetY(  0.0);
  Vuv .SetZ(  0.0);
  Vuuu.SetX(  Dif1X);
  Vuuu.SetY(  Dif1Y);
  Vuuu.SetZ(  Dif1Z);
  Vvvv.SetX(  0.0);
  Vvvv.SetY(  0.0);
  Vvvv.SetZ(  0.0);
  Vuvv.SetX(  0.0);
  Vuvv.SetY(  0.0);
  Vuvv.SetZ(  0.0);
  Vuuv.SetX(  0.0);
  Vuuv.SetY(  0.0);
  Vuuv.SetZ(  0.0);
}

void ElSLib::SphereD3 (const Standard_Real U,
		       const Standard_Real V,
		       const gp_Ax3& Pos, 
		       const Standard_Real Radius,
		       gp_Pnt& P,
		       gp_Vec& Vu, gp_Vec& Vv,
		       gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv,
		       gp_Vec& Vuuu, gp_Vec& Vvvv,
		       gp_Vec& Vuuv, gp_Vec& Vuvv)
{

  // Vxy = CosU * XDirection + SinU * YDirection
  // DVxy = -SinU * XDirection + CosU * YDirection

  // P(U,V) = Location +  R * CosV * Vxy  +   R * SinV * Direction
  
  // Vu = R * CosV * DVxy

  // Vuu = - R * CosV * Vxy

  // Vuuu = - Vu

  // Vv = -R * SinV * Vxy + R * CosV * Direction

  // Vvv = -R * CosV * Vxy - R * SinV * Direction

  // Vvvv = -Vv

  // Vuv = - R * SinV * DVxy

  // Vuuv = R * SinV * Vxy

  // Vuvv = - R * CosV * DVxy = Vuuu = -Vu

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real CosU = cos(U);
  Standard_Real SinU = sin(U);
  Standard_Real R1 = Radius * cos(V);
  Standard_Real R2 = Radius * sin(V);
  Standard_Real A1 = R1 * CosU;
  Standard_Real A2 = R1 * SinU;
  Standard_Real A3 = R2 * CosU;
  Standard_Real A4 = R2 * SinU;
  Standard_Real Som1X = A1 * XDir.X() + A2 * YDir.X();
  Standard_Real Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  Standard_Real Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  Standard_Real Som3X = A3 * XDir.X() + A4 * YDir.X();
  Standard_Real Som3Y = A3 * XDir.Y() + A4 * YDir.Y();
  Standard_Real Som3Z = A3 * XDir.Z() + A4 * YDir.Z();
  Standard_Real Dif1X = A2 * XDir.X() - A1 * YDir.X();
  Standard_Real Dif1Y = A2 * XDir.Y() - A1 * YDir.Y();
  Standard_Real Dif1Z = A2 * XDir.Z() - A1 * YDir.Z();
  Standard_Real R1ZX = R1 * ZDir.X();
  Standard_Real R1ZY = R1 * ZDir.Y();
  Standard_Real R1ZZ = R1 * ZDir.Z();
  Standard_Real R2ZX = R2 * ZDir.X();
  Standard_Real R2ZY = R2 * ZDir.Y();
  Standard_Real R2ZZ = R2 * ZDir.Z();
  P   .SetX(  Som1X + R2ZX + PLoc.X());
  P   .SetY(  Som1Y + R2ZY + PLoc.Y());
  P   .SetZ(  Som1Z + R2ZZ + PLoc.Z());
  Vu  .SetX(- Dif1X);
  Vu  .SetY(- Dif1Y);
  Vu  .SetZ(- Dif1Z);
  Vv  .SetX(- Som3X + R1ZX);
  Vv  .SetY(- Som3Y + R1ZY);
  Vv  .SetZ(- Som3Z + R1ZZ);
  Vuu .SetX(- Som1X);
  Vuu .SetY(- Som1Y);
  Vuu .SetZ(- Som1Z);
  Vvv .SetX(- Som1X - R2ZX);
  Vvv .SetY(- Som1Y - R2ZY);
  Vvv .SetZ(- Som1Z - R2ZZ);
  Vuv .SetX(  A4 * XDir.X() - A3 * YDir.X());
  Vuv .SetY(  A4 * XDir.Y() - A3 * YDir.Y());
  Vuv .SetZ(  A4 * XDir.Z() - A3 * YDir.Z());
  Vuuu.SetX(  Dif1X);
  Vuuu.SetY(  Dif1Y);
  Vuuu.SetZ(  Dif1Z);
  Vvvv.SetX(  Som3X - R1ZX);
  Vvvv.SetY(  Som3Y - R1ZY);
  Vvvv.SetZ(  Som3Z - R1ZZ);
  Vuvv.SetX(  Dif1X);
  Vuvv.SetY(  Dif1Y);
  Vuvv.SetZ(  Dif1Z);
  Vuuv.SetX(  Som3X);
  Vuuv.SetY(  Som3Y);
  Vuuv.SetZ(  Som3Z);
}

void ElSLib::TorusD3 (const Standard_Real U,
		      const Standard_Real V,
		      const gp_Ax3& Pos,
		      const Standard_Real MajorRadius,
		      const Standard_Real MinorRadius,
		      gp_Pnt& P,
		      gp_Vec& Vu, gp_Vec& Vv,
		      gp_Vec& Vuu, gp_Vec& Vvv, gp_Vec& Vuv,
		      gp_Vec& Vuuu, gp_Vec& Vvvv,
		      gp_Vec& Vuuv, gp_Vec& Vuvv)
{

  //P(U,V) = 
  //  Location +
  //  (MajorRadius+MinorRadius*Cos(V)) * 
  //  (Cos(U)*XDirection + Sin(U)*YDirection) +
  //   MinorRadius * Sin(V) * Direction

  //Vv = -MinorRadius * Sin(V) * (Cos(U)*XDirection + Sin(U)*YDirection) +
  //     MinorRadius * Cos(V) * Direction

  //Vvv = -MinorRadius * Cos(V) * (Cos(U)*XDirection + Sin(U)*YDirection) 
  //      -MinorRadius * Sin(V) * Direction

  //Vvvv = - Vv

  //Vu =
  // (MajorRadius+MinorRadius*Cos(V)) *
  // (-Sin(U)*XDirection + Cos(U)*YDirection)
  
  //Vuu =
  // -(MajorRadius+MinorRadius*Cos(V)) *
  // (Cos(U)*XDirection + Sin(U)*YDirection)

  //Vuuu = -Vu

  //Vuv = MinorRadius * Sin(V) * (Sin(U)*XDirection - Cos(U)*YDirection)

  //Vuvv = MinorRadius * Cos(V) * (Sin(U)*XDirection - Cos(U)*YDirection)

  //Vuuv = MinorRadius * Sin(V) * (Cos(U)*XDirection + Sin(U)*YDirection)

  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& ZDir = Pos.Direction ().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real CosU = cos(U);
  Standard_Real SinU = sin(U);
  Standard_Real R1 = MinorRadius * cos(V);
  Standard_Real R2 = MinorRadius * sin(V);
  Standard_Real R  = MajorRadius + R1;
  Standard_Real A1 = R * CosU;
  Standard_Real A2 = R * SinU;
  Standard_Real A3 = R2 * CosU;
  Standard_Real A4 = R2 * SinU;
  Standard_Real A5 = R1 * CosU;
  Standard_Real A6 = R1 * SinU;
  //  Modified by skv - Tue Sep  9 15:10:34 2003 OCC620 Begin
  Standard_Real eps = 10.*(MinorRadius + MajorRadius)*RealEpsilon();

  if (Abs(A1) <= eps)
    A1 = 0.;

  if (Abs(A2) <= eps)
    A2 = 0.;

  if (Abs(A3) <= eps)
    A3 = 0.;

  if (Abs(A4) <= eps)
    A4 = 0.;

  if (Abs(A5) <= eps)
    A5 = 0.;

  if (Abs(A6) <= eps)
    A6 = 0.;
  //  Modified by skv - Tue Sep  9 15:10:35 2003 OCC620 End
  Standard_Real Som1X = A1 * XDir.X() + A2 * YDir.X();
  Standard_Real Som1Y = A1 * XDir.Y() + A2 * YDir.Y();
  Standard_Real Som1Z = A1 * XDir.Z() + A2 * YDir.Z();
  Standard_Real Som3X = A3 * XDir.X() + A4 * YDir.X();
  Standard_Real Som3Y = A3 * XDir.Y() + A4 * YDir.Y();
  Standard_Real Som3Z = A3 * XDir.Z() + A4 * YDir.Z();
  Standard_Real Dif1X = A2 * XDir.X() - A1 * YDir.X();
  Standard_Real Dif1Y = A2 * XDir.Y() - A1 * YDir.Y();
  Standard_Real Dif1Z = A2 * XDir.Z() - A1 * YDir.Z();
  Standard_Real R1ZX = R1 * ZDir.X();
  Standard_Real R1ZY = R1 * ZDir.Y();
  Standard_Real R1ZZ = R1 * ZDir.Z();
  Standard_Real R2ZX = R2 * ZDir.X();
  Standard_Real R2ZY = R2 * ZDir.Y();
  Standard_Real R2ZZ = R2 * ZDir.Z();
  P   .SetX(  Som1X + R2ZX + PLoc.X());
  P   .SetY(  Som1Y + R2ZY + PLoc.Y());
  P   .SetZ(  Som1Z + R2ZZ + PLoc.Z());
  Vu  .SetX(- Dif1X);
  Vu  .SetY(- Dif1Y);
  Vu  .SetZ(- Dif1Z);
  Vv  .SetX(- Som3X + R1ZX);
  Vv  .SetY(- Som3Y + R1ZY);
  Vv  .SetZ(- Som3Z + R1ZZ);
  Vuu .SetX(- Som1X);
  Vuu .SetY(- Som1Y);
  Vuu .SetZ(- Som1Z);
  Vvv .SetX(- A5 * XDir.X() - A6 * YDir.X() - R2ZX);
  Vvv .SetY(- A5 * XDir.Y() - A6 * YDir.Y() - R2ZY);
  Vvv .SetZ(- A5 * XDir.Z() - A6 * YDir.Z() - R2ZZ);
  Vuv .SetX(  A4 * XDir.X() - A3 * YDir.X());
  Vuv .SetY(  A4 * XDir.Y() - A3 * YDir.Y());
  Vuv .SetZ(  A4 * XDir.Z() - A3 * YDir.Z());
  Vuuu.SetX(  Dif1X);
  Vuuu.SetY(  Dif1Y);
  Vuuu.SetZ(  Dif1Z);
  Vvvv.SetX(  Som3X - R1ZX);
  Vvvv.SetY(  Som3Y - R1ZY);
  Vvvv.SetZ(  Som3Z - R1ZZ);
  Vuuv.SetX(  Som3X);
  Vuuv.SetY(  Som3Y);
  Vuuv.SetZ(  Som3Z);
  Vuvv.SetX(  A6 * XDir.X() - A5 * YDir.X() );
  Vuvv.SetY(  A6 * XDir.Y() - A5 * YDir.Y() );
  Vuvv.SetZ(  A6 * XDir.Z() - A5 * YDir.Z() );
}

//=======================================================================
//function : PlaneParameters
//purpose  : 
//=======================================================================

void ElSLib::PlaneParameters (const gp_Ax3& Pos,
			      const gp_Pnt& P,
			      Standard_Real& U,
			      Standard_Real& V)
{
  gp_Trsf T;
  T.SetTransformation (Pos);
  gp_Pnt Ploc = P.Transformed (T);
  U = Ploc.X();
  V = Ploc.Y();
}

//=======================================================================
//function : CylindreParameters
//purpose  : 
//=======================================================================

void ElSLib::CylinderParameters (const gp_Ax3& Pos,
				 const Standard_Real,
				 const gp_Pnt& P,
				 Standard_Real& U,
				 Standard_Real& V)
{
  gp_Trsf T;
  T.SetTransformation (Pos);
  gp_Pnt Ploc = P.Transformed (T);
  U = atan2(Ploc.Y(),Ploc.X());
  if      (U < -1.e-16)  U += PIPI;
  else if (U < 0)        U = 0;
  V = Ploc.Z();
}

//=======================================================================
//function : ConeParameters
//purpose  : 
//=======================================================================

void ElSLib::ConeParameters(const gp_Ax3& Pos,
			    const Standard_Real Radius,
			    const Standard_Real SAngle,
			    const gp_Pnt& P,
			    Standard_Real& U,
			    Standard_Real& V)
{
  gp_Trsf T;
  T.SetTransformation (Pos);
  gp_Pnt Ploc = P.Transformed (T);

  if(Ploc.X() ==0.0  &&  Ploc.Y()==0.0 ) {
    U = 0.0;
  }
  else if ( -Radius > Ploc.Z()* Tan(SAngle) ) {
    // the point is at the wrong side of the apex
    U = atan2(-Ploc.Y(), -Ploc.X());
  }
  else {
    U = atan2(Ploc.Y(),Ploc.X());
  }
  if      (U < -1.e-16)  U += PIPI;
  else if (U < 0)        U = 0;

  // Evaluate V as follows :
  // P0 = Cone.Value(U,0)
  // P1 = Cone.Value(U,1)
  // V = P0 P1 . P0 Ploc
  // After simplification obtain:
  // V = Sin(Sang) * ( x cosU + y SinU - R) + z * Cos(Sang)
  // Method that permits to find V of the projected point if the point
  // is not actually on the cone.

  V =  sin(SAngle) * ( Ploc.X() * cos(U) + Ploc.Y() * sin(U) - Radius)
    + cos(SAngle) * Ploc.Z();
}

//=======================================================================
//function : SphereParameters
//purpose  : 
//=======================================================================

void ElSLib::SphereParameters(const gp_Ax3& Pos,
			      const Standard_Real,
			      const gp_Pnt& P,
			      Standard_Real& U,
			      Standard_Real& V)
{
  gp_Trsf T;
  T.SetTransformation (Pos);
  gp_Pnt Ploc = P.Transformed (T);
  Standard_Real x, y, z;
  Ploc.Coord (x, y, z);
  Standard_Real l = sqrt (x * x + y * y);
  if (l < gp::Resolution()) {    // point on axis Z of the sphere
    if (z > 0.)
      V =   M_PI_2; // PI * 0.5
    else
      V = - M_PI_2; // PI * 0.5
    U = 0.;
  }
  else {
    V = atan(z/l);
    U = atan2(y,x);
    if      (U < -1.e-16)  U += PIPI;
    else if (U < 0)        U = 0;
  }
}

//=======================================================================
//function : TorusParameters
//purpose  : 
//=======================================================================

void ElSLib::TorusParameters(const gp_Ax3& Pos,
			     const Standard_Real MajorRadius,
			     const Standard_Real MinorRadius,
			     const gp_Pnt& P,
			     Standard_Real& U,
			     Standard_Real& V)
{
  gp_Trsf Tref;
  Tref.SetTransformation (Pos);
  gp_Pnt Ploc = P.Transformed (Tref);
  Standard_Real x, y, z;
  Ploc.Coord (x, y, z);

  // all that to process case of  Major < Minor.
  U = atan2(y,x);
  if (MajorRadius < MinorRadius){
    Standard_Real cosu = cos(U);
    Standard_Real sinu = sin(U);
    Standard_Real z2 = z * z;
    Standard_Real MinR2 = MinorRadius * MinorRadius;
    Standard_Real RCosU = MajorRadius * cosu;
    Standard_Real RSinU = MajorRadius * sinu;
    Standard_Real xm = x - RCosU;
    Standard_Real ym = y - RSinU;
    Standard_Real xp = x + RCosU;
    Standard_Real yp = y + RSinU;
    Standard_Real D1 = xm * xm + ym * ym + z2 - MinR2;
    Standard_Real D2 = xp * xp + yp * yp + z2 - MinR2;
    Standard_Real AD1 = D1;
    if (AD1 < 0) AD1 = - AD1;
    Standard_Real AD2 = D2;
    if (AD2 < 0) AD2 = - AD2;
    if (AD2 < AD1) U += M_PI;
  }
  if      (U < -1.e-16)  U += PIPI;
  else if (U < 0)        U = 0;
  Standard_Real cosu = cos(U);
  Standard_Real sinu = sin(U);
  gp_Dir dx(cosu,sinu,0.);
  gp_Dir dP(x - MajorRadius * cosu,
	    y - MajorRadius * sinu,
	    z);
  V = dx.AngleWithRef(dP,dx^gp::DZ());
  if      (V < -1.e-16)  V += PIPI;
  else if (V < 0)        V = 0;
}

//=======================================================================
//function : PlaneUIso
//purpose  : 
//=======================================================================

gp_Lin  ElSLib::PlaneUIso(const gp_Ax3& Pos, 
			  const Standard_Real U)
{
  gp_Lin L(Pos.Location(),Pos.YDirection());
  gp_Vec Ve(Pos.XDirection());
  Ve *= U;
  L.Translate(Ve);
  return L;
}

//=======================================================================
//function : CylinderUIso
//purpose  : 
//=======================================================================

gp_Lin  ElSLib::CylinderUIso(const gp_Ax3& Pos, 
			     const Standard_Real Radius, 
			     const Standard_Real U)
{
  gp_Pnt P;
  gp_Vec DU,DV;
  CylinderD1(U,0.,Pos,Radius,P,DU,DV);
  gp_Lin L(P,DV);
  return L;
}

//=======================================================================
//function : ConeUIso
//purpose  : 
//=======================================================================

gp_Lin  ElSLib::ConeUIso(const gp_Ax3& Pos, 
			 const Standard_Real Radius, 
			 const Standard_Real SAngle, 
			 const Standard_Real U)
{
  gp_Pnt P;
  gp_Vec DU,DV;
  ConeD1(U,0,Pos,Radius,SAngle,P,DU,DV);
  gp_Lin L(P,DV);
  return L;
}

//=======================================================================
//function : SphereUIso
//purpose  : 
//=======================================================================

gp_Circ  ElSLib::SphereUIso(const gp_Ax3& Pos, 
			    const Standard_Real Radius, 
			    const Standard_Real U)
{
  gp_Vec dx = Pos.XDirection();
  gp_Vec dy = Pos.YDirection();
  gp_Dir dz = Pos.Direction ();
  gp_Dir cx = cos(U) * dx + sin(U) * dy;
  gp_Ax2 axes(Pos.Location(),
	      cx.Crossed(dz),
	      cx);
  gp_Circ Circ(axes,Radius);
  return Circ;
}

//=======================================================================
//function : TorusUIso
//purpose  : 
//=======================================================================

gp_Circ  ElSLib::TorusUIso(const gp_Ax3& Pos, 
			   const Standard_Real MajorRadius, 
			   const Standard_Real MinorRadius, 
			   const Standard_Real U)
{
  gp_Vec dx = Pos.XDirection();
  gp_Vec dy = Pos.YDirection();
  gp_Dir dz = Pos.Direction ();
  gp_Dir cx = cos(U) * dx + sin(U) * dy;
  gp_Ax2 axes(Pos.Location(),
	      cx.Crossed(dz),
	      cx);
  gp_Vec Ve = cx;
  Ve *= MajorRadius;
  axes.Translate(Ve);
  gp_Circ Circ(axes,MinorRadius);
  return Circ;
}

//=======================================================================
//function : PlaneVIso
//purpose  : 
//=======================================================================

gp_Lin  ElSLib::PlaneVIso(const gp_Ax3& Pos, 
			  const Standard_Real V)
{
  gp_Lin L(Pos.Location(),Pos.XDirection());
  gp_Vec Ve(Pos.YDirection());
  Ve *= V;
  L.Translate(Ve);
  return L;
}

//=======================================================================
//function : CylinderVIso
//purpose  : 
//=======================================================================

gp_Circ  ElSLib::CylinderVIso(const gp_Ax3& Pos, 
			      const Standard_Real Radius, 
			      const Standard_Real V)
{
  gp_Ax2 axes = Pos.Ax2();
  gp_Vec Ve(Pos.Direction());
  Ve.Multiply(V);
  axes.Translate(Ve);
  gp_Circ C(axes,Radius);
  return C;
}

//=======================================================================
//function : ConeVIso
//purpose  : 
//=======================================================================

gp_Circ  ElSLib::ConeVIso(const gp_Ax3& Pos, 
			  const Standard_Real Radius, 
			  const Standard_Real SAngle, 
			  const Standard_Real V)
{
  gp_Ax3 axes(Pos);
  gp_Vec Ve(Pos.Direction());
  Ve.Multiply(V * cos(SAngle));
  axes.Translate(Ve);
  Standard_Real R = Radius + V * sin(SAngle);
  if (R < 0) {
    axes.XReverse();
    axes.YReverse();
    R = - R;
  }
  gp_Circ C(axes.Ax2(),R);
  return C;
}

//=======================================================================
//function : SphereVIso
//purpose  : 
//=======================================================================

gp_Circ  ElSLib::SphereVIso(const gp_Ax3& Pos, 
			    const Standard_Real Radius, 
			    const Standard_Real V)
{
  gp_Ax2 axes = Pos.Ax2();
  gp_Vec Ve(Pos.Direction());
  Ve.Multiply(Radius * sin(V));
  axes.Translate(Ve);
  Standard_Real radius = Radius * cos(V);
  // #23170: if V is even slightly (e.g. by double epsilon) greater than PI/2,
  // radius will become negative and constructor of gp_Circ will raise exception.
  // Lets try to create correct isoline even on analytical continuation for |V| > PI/2...
  if (radius < 0.)
  {
    axes.SetDirection (-axes.Direction());
    radius = -radius;
  }
  gp_Circ Circ(axes,radius);
  return Circ;
}

//=======================================================================
//function : TorusVIso
//purpose  : 
//=======================================================================

gp_Circ  ElSLib::TorusVIso(const gp_Ax3& Pos, 
			   const Standard_Real MajorRadius, 
			   const Standard_Real MinorRadius, 
			   const Standard_Real V)
{
  gp_Ax3 axes = Pos.Ax2();
  gp_Vec Ve(Pos.Direction());
  Ve.Multiply(MinorRadius * sin(V));
  axes.Translate(Ve);
  Standard_Real R = MajorRadius + MinorRadius * cos(V);
  if (R < 0) {
    axes.XReverse();
    axes.YReverse();
    R = - R;
  }
  gp_Circ Circ(axes.Ax2(),R);
  return Circ;
}

