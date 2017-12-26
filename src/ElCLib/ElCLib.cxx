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

// Evolutions   JCV Dec 1991 ajout de calculs de derivees et traitement
//              d'entites 2d
//              JCV Mars 1992 ajout method SetLinearForm

#define No_Standard_OutOfRange


#include <ElCLib.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Circ.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Dir.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Elips.hxx>
#include <gp_Elips2d.hxx>
#include <gp_Hypr.hxx>
#include <gp_Hypr2d.hxx>
#include <gp_Lin.hxx>
#include <gp_Lin2d.hxx>
#include <gp_Parab.hxx>
#include <gp_Parab2d.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

static Standard_Real PIPI = M_PI + M_PI;

//=======================================================================
//function : InPeriod
//purpose  : Value theULast is never returned.
//          Example of some case (checked on WIN64 platform)
//          with some surface having period 2*PI = 6.2831853071795862.
//            Let theUFirst be equal to 6.1645624650899675. Then,
//          theULast must be equal to
//              6.1645624650899675+6.2831853071795862=12.4477477722695537.
//
//          However, real result is 12.447747772269555.
//          Therefore, new period value to adjust will be equal to
//              12.447747772269555-6.1645624650899675=6.2831853071795871.
//
//          As we can see, (6.2831853071795871 != 6.2831853071795862).
//
//          According to above said, this method should be used carefully.
//          In order to increase reliability of this method, input arguments
//          needs to be replaced with following: 
//            (theU, theUFirst, thePeriod). theULast parameter is excess.
//=======================================================================
Standard_Real  ElCLib::InPeriod(const Standard_Real theU, 
                                const Standard_Real theUFirst, 
                                const Standard_Real theULast)
{
  if( Precision::IsInfinite(theU) ||
      Precision::IsInfinite(theUFirst) ||
      Precision::IsInfinite(theULast))
  {//In order to avoid FLT_Overflow exception
    return theU;
  }

  const Standard_Real aPeriod = theULast - theUFirst;

  if(aPeriod < Epsilon(theULast))
    return theU;

  return Max(theUFirst, theU + aPeriod*Ceiling((theUFirst-theU)/aPeriod));
}

//=======================================================================
//function : AdjustPeriodic
//purpose  : 
//=======================================================================

void ElCLib::AdjustPeriodic(const Standard_Real UFirst,
                            const Standard_Real ULast,
                            const Standard_Real Preci,
                            Standard_Real& U1,
                            Standard_Real& U2)
{
  Standard_Real period = ULast - UFirst;
  U1 -= Floor((U1-UFirst)/period) * period;
  if (ULast - U1 < Preci) U1 -= period;
  U2 -= Floor((U2-U1)/period) * period;
  if (U2 - U1 < Preci) U2 += period;
}

//=======================================================================
//function : LineValue
//purpose  : 
//=======================================================================

gp_Pnt ElCLib::LineValue (const Standard_Real U,
			  const gp_Ax1& Pos)
{
  const gp_XYZ& ZDir = Pos.Direction().XYZ();
  const gp_XYZ& PLoc = Pos.Location ().XYZ();
  return gp_Pnt(U * ZDir.X() + PLoc.X(),
		U * ZDir.Y() + PLoc.Y(),
		U * ZDir.Z() + PLoc.Z());
}

//=======================================================================
//function : CircleValue
//purpose  : 
//=======================================================================

gp_Pnt ElCLib::CircleValue (const Standard_Real U,
			    const gp_Ax2& Pos,
			    const Standard_Real Radius)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real A1 = Radius * cos(U);
  Standard_Real A2 = Radius * sin(U);
  return gp_Pnt(A1 * XDir.X() + A2 * YDir.X() + PLoc.X(),
		A1 * XDir.Y() + A2 * YDir.Y() + PLoc.Y(),
		A1 * XDir.Z() + A2 * YDir.Z() + PLoc.Z());
}

//=======================================================================
//function : EllipseValue
//purpose  : 
//=======================================================================

gp_Pnt ElCLib::EllipseValue (const Standard_Real U,
			     const gp_Ax2& Pos, 
			     const Standard_Real MajorRadius,
			     const Standard_Real MinorRadius)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real A1 = MajorRadius * cos(U);
  Standard_Real A2 = MinorRadius * sin(U);
  return gp_Pnt(A1 * XDir.X() + A2 * YDir.X() + PLoc.X(),
		A1 * XDir.Y() + A2 * YDir.Y() + PLoc.Y(),
		A1 * XDir.Z() + A2 * YDir.Z() + PLoc.Z());
}

//=======================================================================
//function : HyperbolaValue
//purpose  : 
//=======================================================================

gp_Pnt ElCLib::HyperbolaValue (const Standard_Real U,
			       const gp_Ax2& Pos,
			       const Standard_Real MajorRadius,
			       const Standard_Real MinorRadius)
{
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real A1 = MajorRadius * Cosh(U);
  Standard_Real A2 = MinorRadius * Sinh(U);
  return gp_Pnt(A1 * XDir.X() + A2 * YDir.X() + PLoc.X(),
		A1 * XDir.Y() + A2 * YDir.Y() + PLoc.Y(),
		A1 * XDir.Z() + A2 * YDir.Z() + PLoc.Z());
}

//=======================================================================
//function : ParabolaValue
//purpose  : 
//=======================================================================

gp_Pnt ElCLib::ParabolaValue (const Standard_Real U,
			      const gp_Ax2& Pos,
			      const Standard_Real Focal)
{
  if (Focal == 0.0) {
    const gp_XYZ& XDir = Pos.XDirection().XYZ();
    const gp_XYZ& PLoc = Pos.Location  ().XYZ();
    return gp_Pnt(U * XDir.X() + PLoc.X(),
		  U * XDir.Y() + PLoc.Y(),
		  U * XDir.Z() + PLoc.Z());
  }
  const gp_XYZ& XDir = Pos.XDirection().XYZ();
  const gp_XYZ& YDir = Pos.YDirection().XYZ();
  const gp_XYZ& PLoc = Pos.Location  ().XYZ();
  Standard_Real A1 = U * U / (4.0 * Focal);
  return gp_Pnt(A1 * XDir.X() + U * YDir.X() + PLoc.X(),
		A1 * XDir.Y() + U * YDir.Y() + PLoc.Y(),
		A1 * XDir.Z() + U * YDir.Z() + PLoc.Z());
}

//=======================================================================
//function : LineD1
//purpose  : 
//=======================================================================

void ElCLib::LineD1 (const Standard_Real U,
		     const gp_Ax1& Pos,
		     gp_Pnt& P,
		     gp_Vec& V1)
{
  gp_XYZ Coord = Pos.Direction().XYZ();
  V1.SetXYZ (Coord);
  Coord.SetLinearForm (U, Coord, Pos.Location().XYZ());
  P.SetXYZ (Coord);
}

//=======================================================================
//function : CircleD1
//purpose  : 
//=======================================================================

void ElCLib::CircleD1 (const Standard_Real U,
		       const gp_Ax2& Pos,
		       const Standard_Real Radius, 
		       gp_Pnt& P,
		       gp_Vec& V1)
{
  Standard_Real Xc = Radius * Cos (U);
  Standard_Real Yc = Radius * Sin (U);
  gp_XYZ Coord0;
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  gp_XYZ Coord2 (Pos.YDirection().XYZ());
  //Point courant :
  Coord0.SetLinearForm (Xc, Coord1, Yc, Coord2, Pos.Location().XYZ());
  P.SetXYZ (Coord0);
  //D1 :
  Coord0.SetLinearForm (-Yc, Coord1, Xc, Coord2);
  V1.SetXYZ (Coord0);
}

//=======================================================================
//function : EllipseD1
//purpose  : 
//=======================================================================

void ElCLib::EllipseD1 (const Standard_Real U,
			const gp_Ax2& Pos,
			const Standard_Real MajorRadius,
			const Standard_Real MinorRadius,
			gp_Pnt& P,
			gp_Vec& V1)
{
  Standard_Real Xc = Cos (U);
  Standard_Real Yc = Sin (U);
  gp_XYZ Coord0;
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  gp_XYZ Coord2 (Pos.YDirection().XYZ());
  //Point courant :
  Coord0.SetLinearForm (Xc*MajorRadius, Coord1, 
			Yc*MinorRadius, Coord2,
			Pos.Location().XYZ());
  P.SetXYZ (Coord0);
  //D1 :
  Coord0.SetLinearForm (-Yc*MajorRadius, Coord1, Xc*MinorRadius, Coord2);
  V1.SetXYZ (Coord0);
}

//=======================================================================
//function : HyperbolaD1
//purpose  : 
//=======================================================================

void ElCLib::HyperbolaD1 (const Standard_Real U,
			  const gp_Ax2& Pos,
			  const Standard_Real MajorRadius,
			  const Standard_Real MinorRadius, 
			  gp_Pnt& P,
			  gp_Vec& V1)
{
  Standard_Real Xc = Cosh (U);
  Standard_Real Yc = Sinh (U);
  gp_XYZ Coord0;
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  gp_XYZ Coord2 (Pos.YDirection().XYZ());
  //Point courant :
  Coord0.SetLinearForm (Xc*MajorRadius, Coord1, 
			Yc*MinorRadius, Coord2,
			Pos.Location().XYZ());
  P.SetXYZ (Coord0);
  //D1 :
  Coord0.SetLinearForm (Yc*MajorRadius, Coord1, Xc*MinorRadius, Coord2);
  V1.SetXYZ (Coord0);
}

//=======================================================================
//function : ParabolaD1
//purpose  : 
//=======================================================================

void ElCLib::ParabolaD1 (const Standard_Real U,
			 const gp_Ax2& Pos,
			 const Standard_Real Focal,
			 gp_Pnt& P,
			 gp_Vec& V1)
{
  gp_XYZ Coord0;
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  if (Focal == 0.0) {      //Parabole degenere en une droite
    V1.SetXYZ (Coord1);
    Coord1.Multiply (U);
    Coord1.Add (Pos.Location().XYZ());
    P.SetXYZ (Coord1);
  }
  else {
    gp_XYZ Coord2 (Pos.YDirection().XYZ());
    Coord0.SetLinearForm (U / (2.0 * Focal), Coord1, Coord2);
    V1.SetXYZ (Coord0);
    Coord0.SetLinearForm ((U * U) / (4.0 * Focal), Coord1, 
			  U, Coord2,
			  Pos.Location().XYZ());
    P.SetXYZ (Coord0);
  }
}

//=======================================================================
//function : CircleD2
//purpose  : 
//=======================================================================

void ElCLib::CircleD2 (const Standard_Real U,
		       const gp_Ax2& Pos,
		       const Standard_Real Radius,
		       gp_Pnt& P,
		       gp_Vec& V1,
		       gp_Vec& V2)
{
  Standard_Real Xc = Radius * cos(U);
  Standard_Real Yc = Radius * sin(U);
  gp_XYZ Coord0;
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  gp_XYZ Coord2 (Pos.YDirection().XYZ());
  //Point courant :
  Coord0.SetLinearForm (Xc, Coord1, Yc, Coord2, Pos.Location().XYZ());
  P.SetXYZ (Coord0);
  //D1 :
  Coord0.SetLinearForm (-Yc, Coord1, Xc, Coord2);
  V1.SetXYZ (Coord0);
  //D2 :
  Coord0.SetLinearForm (-Xc, Coord1, -Yc, Coord2);
  V2.SetXYZ (Coord0);
}

//=======================================================================
//function : EllipseD2
//purpose  : 
//=======================================================================

void ElCLib::EllipseD2 (const Standard_Real U,
			const gp_Ax2& Pos,
			const Standard_Real MajorRadius,
			const Standard_Real MinorRadius,
			gp_Pnt& P,
			gp_Vec& V1,
			gp_Vec& V2)
{
  Standard_Real Xc = cos(U);
  Standard_Real Yc = sin(U);
  gp_XYZ Coord0; 
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  gp_XYZ Coord2 (Pos.YDirection().XYZ());
  //Point courant :
  Coord0.SetLinearForm (Xc*MajorRadius, Coord1, 
			Yc*MinorRadius, Coord2,
			Pos.Location().XYZ());
  P.SetXYZ (Coord0);
  //D1 :
  Coord0.SetLinearForm (-Yc*MajorRadius, Coord1, Xc*MinorRadius, Coord2);
  V1.SetXYZ (Coord0);
  //D2 :
  Coord0.SetLinearForm (-Xc*MajorRadius, Coord1, -Yc*MinorRadius, Coord2);
  V2.SetXYZ (Coord0);
}

//=======================================================================
//function : HyperbolaD2
//purpose  : 
//=======================================================================

void ElCLib::HyperbolaD2 (const Standard_Real U,
			  const gp_Ax2& Pos,
			  const Standard_Real MajorRadius,
			  const Standard_Real MinorRadius,
			  gp_Pnt& P,
			  gp_Vec& V1,
			  gp_Vec& V2)
{
  Standard_Real Xc = Cosh(U);
  Standard_Real Yc = Sinh(U);
  gp_XYZ Coord0; 
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  gp_XYZ Coord2 (Pos.YDirection().XYZ());

  //Point courant et D2:
  Coord0.SetLinearForm (Xc*MajorRadius, Coord1, Yc*MinorRadius, Coord2);
  V2.SetXYZ (Coord0);
  Coord0.Add (Pos.Location().XYZ());
  P.SetXYZ (Coord0);
  //D1 :
  Coord0.SetLinearForm (Yc*MajorRadius, Coord1, Xc*MinorRadius, Coord2);
  V1.SetXYZ (Coord0);
}

//=======================================================================
//function : ParabolaD2
//purpose  : 
//=======================================================================

void ElCLib::ParabolaD2 (const Standard_Real U,
			 const gp_Ax2& Pos,
			 const Standard_Real Focal,
			 gp_Pnt& P,
			 gp_Vec& V1,
			 gp_Vec& V2)
{
  gp_XYZ Coord0(0.0, 0.0, 0.0);
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  if (Focal == 0.0) {
    V2.SetCoord (0.0, 0.0, 0.0);
    V1.SetXYZ (Coord1);
    Coord1.Multiply (U);
    Coord1.Add (Pos.Location().XYZ());        
    P.SetXYZ (Coord1);//was: P.SetXYZ (Coord0);
    
  }
  else {
    gp_XYZ Coord2 (Pos.YDirection().XYZ());
    Coord0.SetLinearForm ((U * U) / (4.0 * Focal), Coord1, 
			  U, Coord2,
			  Pos.Location().XYZ());
    P.SetXYZ (Coord0);
    Coord0.SetLinearForm (U / (2.0 * Focal), Coord1, Coord2);
    V1.SetXYZ (Coord0);
    Coord1.Multiply (1.0 / (2.0 * Focal));
    V2.SetXYZ (Coord1);
  }
}

//=======================================================================
//function : CircleD3
//purpose  : 
//=======================================================================

void ElCLib::CircleD3 (const Standard_Real U,
		       const gp_Ax2& Pos,
		       const Standard_Real Radius, 
		       gp_Pnt& P,
		       gp_Vec& V1,
		       gp_Vec& V2,
		       gp_Vec& V3)
{
  Standard_Real Xc = Radius * cos(U);
  Standard_Real Yc = Radius * sin(U);
  gp_XYZ Coord0;
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  gp_XYZ Coord2 (Pos.YDirection().XYZ());
  //Point Courant :
  Coord0.SetLinearForm (Xc, Coord1, Yc, Coord2, Pos.Location().XYZ());
  P.SetXYZ (Coord0);
  //D1 :
  Coord0.SetLinearForm (-Yc, Coord1, Xc, Coord2);
  V1.SetXYZ (Coord0);
  //D2 :
  Coord0.SetLinearForm (-Xc, Coord1, -Yc, Coord2);
  V2.SetXYZ (Coord0);
  //D3 :
  Coord0.SetLinearForm (Yc, Coord1, -Xc, Coord2);
  V3.SetXYZ (Coord0);
}

//=======================================================================
//function : EllipseD3
//purpose  : 
//=======================================================================

void ElCLib::EllipseD3 (const Standard_Real U,
			const gp_Ax2& Pos,
			const Standard_Real MajorRadius,
			const Standard_Real MinorRadius,
			gp_Pnt& P,
			gp_Vec& V1,
			gp_Vec& V2,
			gp_Vec& V3)
{
  Standard_Real Xc = cos(U);
  Standard_Real Yc = sin(U);
  gp_XYZ Coord0;
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  gp_XYZ Coord2 (Pos.YDirection().XYZ());
  //Point Courant :
  Coord0.SetLinearForm (Xc*MajorRadius, Coord1,
			Yc*MinorRadius, Coord2,
			Pos.Location().XYZ());
  P.SetXYZ (Coord0);
  //D1 :
  Coord0.SetLinearForm (-Yc*MajorRadius, Coord1, Xc*MinorRadius, Coord2);
  V1.SetXYZ (Coord0);
  //D2 :
  Coord0.SetLinearForm (-Xc*MajorRadius, Coord1, -Yc*MinorRadius, Coord2);
  V2.SetXYZ (Coord0);
  //D3
  Coord0.SetLinearForm (Yc*MajorRadius, Coord1, -Xc*MinorRadius, Coord2);
  V3.SetXYZ (Coord0);
}

//=======================================================================
//function : HyperbolaD3
//purpose  : 
//=======================================================================

void ElCLib::HyperbolaD3 (const Standard_Real U,
			  const gp_Ax2& Pos,
			  const Standard_Real MajorRadius,
			  const Standard_Real MinorRadius,
			  gp_Pnt& P,
			  gp_Vec& V1,
			  gp_Vec& V2,
			  gp_Vec& V3)
{
  Standard_Real Xc = Cosh(U);
  Standard_Real Yc = Sinh(U);
  gp_XYZ Coord0;
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  gp_XYZ Coord2 (Pos.YDirection().XYZ());
  //Point courant et D2 :
  Coord0.SetLinearForm (Xc*MajorRadius, Coord1, Yc*MinorRadius, Coord2);
  V2.SetXYZ (Coord0);
  Coord0.Add (Pos.Location().XYZ());
  P.SetXYZ (Coord0);
  //D1 et D3 :
  Coord0.SetLinearForm (Yc*MajorRadius, Coord1, Xc*MinorRadius, Coord2);
  V1.SetXYZ (Coord0);
  V3.SetXYZ (Coord0);
}

//=======================================================================
//function : LineValue
//purpose  : 
//=======================================================================

gp_Pnt2d ElCLib::LineValue (const Standard_Real U,
			    const gp_Ax2d& Pos)
{
  const gp_XY& ZDir = Pos.Direction().XY();
  const gp_XY& PLoc = Pos.Location ().XY();
  return gp_Pnt2d(U * ZDir.X() + PLoc.X(),
		  U * ZDir.Y() + PLoc.Y());
}

//=======================================================================
//function : CircleValue
//purpose  : 
//=======================================================================

gp_Pnt2d ElCLib::CircleValue (const Standard_Real U,
			      const gp_Ax22d& Pos,
			      const Standard_Real Radius)
{
  const gp_XY& XDir = Pos.XDirection().XY();
  const gp_XY& YDir = Pos.YDirection().XY();
  const gp_XY& PLoc = Pos.Location  ().XY();
  Standard_Real A1 = Radius * cos(U);
  Standard_Real A2 = Radius * sin(U);
  return gp_Pnt2d(A1 * XDir.X() + A2 * YDir.X() + PLoc.X(),
		  A1 * XDir.Y() + A2 * YDir.Y() + PLoc.Y());
}

//=======================================================================
//function : EllipseValue
//purpose  : 
//=======================================================================

gp_Pnt2d ElCLib::EllipseValue (const Standard_Real U,
			       const gp_Ax22d& Pos,
			       const Standard_Real MajorRadius,
			       const Standard_Real MinorRadius)
{
  const gp_XY& XDir = Pos.XDirection().XY();
  const gp_XY& YDir = Pos.YDirection().XY();
  const gp_XY& PLoc = Pos.Location  ().XY();
  Standard_Real A1 = MajorRadius * cos(U);
  Standard_Real A2 = MinorRadius * sin(U);
  return gp_Pnt2d(A1 * XDir.X() + A2 * YDir.X() + PLoc.X(),
		  A1 * XDir.Y() + A2 * YDir.Y() + PLoc.Y());
}

//=======================================================================
//function : HyperbolaValue
//purpose  : 
//=======================================================================

gp_Pnt2d ElCLib::HyperbolaValue (const Standard_Real U,
				 const gp_Ax22d& Pos,
				 const Standard_Real MajorRadius,
				 const Standard_Real MinorRadius)
{
  const gp_XY& XDir = Pos.XDirection().XY();
  const gp_XY& YDir = Pos.YDirection().XY();
  const gp_XY& PLoc = Pos.Location  ().XY();
  Standard_Real A1 = MajorRadius * Cosh(U);
  Standard_Real A2 = MinorRadius * Sinh(U);
  return gp_Pnt2d(A1 * XDir.X() + A2 * YDir.X() + PLoc.X(),
		  A1 * XDir.Y() + A2 * YDir.Y() + PLoc.Y());
}

//=======================================================================
//function : ParabolaValue
//purpose  : 
//=======================================================================

gp_Pnt2d ElCLib::ParabolaValue (const Standard_Real U,
				const gp_Ax22d& Pos,
				const Standard_Real Focal)
{
  if (Focal == 0.0) {
    const gp_XY& XDir = Pos.XDirection().XY();
    const gp_XY& PLoc = Pos.Location  ().XY();
    return gp_Pnt2d(U * XDir.X() + PLoc.X(),
		    U * XDir.Y() + PLoc.Y());
  }
  const gp_XY& XDir = Pos.XDirection().XY();
  const gp_XY& YDir = Pos.YDirection().XY();
  const gp_XY& PLoc = Pos.Location  ().XY();
  Standard_Real A1 = U * U / (4.0 * Focal);
  return gp_Pnt2d(A1 * XDir.X() + U * YDir.X() + PLoc.X(),
		  A1 * XDir.Y() + U * YDir.Y() + PLoc.Y());
}

//=======================================================================
//function : LineD1
//purpose  : 
//=======================================================================

void ElCLib::LineD1(const Standard_Real U,
		    const gp_Ax2d& Pos,
		    gp_Pnt2d& P,
		    gp_Vec2d& V1)
{
  gp_XY Coord = Pos.Direction().XY();
  V1.SetXY (Coord);
  Coord.SetLinearForm (U, Coord, Pos.Location().XY());
  P.SetXY (Coord);
}

//=======================================================================
//function : CircleD1
//purpose  : 
//=======================================================================

void ElCLib::CircleD1 (const Standard_Real U,
		       const gp_Ax22d& Pos,
		       const Standard_Real Radius,
		       gp_Pnt2d& P,
		       gp_Vec2d& V1)
{
  gp_XY Vxy;
  gp_XY Xdir (Pos.XDirection().XY());
  gp_XY Ydir (Pos.YDirection().XY());
  Standard_Real Xc = Radius * cos(U);
  Standard_Real Yc = Radius * sin(U);
  //Point courant :
  Vxy.SetLinearForm (Xc, Xdir, Yc, Ydir, Pos.Location().XY());
  P.SetXY (Vxy);
  //V1 :
  Vxy.SetLinearForm (-Yc, Xdir, Xc, Ydir);
  V1.SetXY (Vxy);
}

//=======================================================================
//function : EllipseD1
//purpose  : 
//=======================================================================

void ElCLib::EllipseD1 (const Standard_Real U,
			const gp_Ax22d& Pos,
			const Standard_Real MajorRadius,
			const Standard_Real MinorRadius,
			gp_Pnt2d& P,
			gp_Vec2d& V1)
{
  gp_XY Vxy;
  gp_XY Xdir ((Pos.XDirection()).XY());
  gp_XY Ydir ((Pos.YDirection()).XY());
  Standard_Real Xc = cos(U);
  Standard_Real Yc = sin(U);
  //Point courant :
  Vxy.SetLinearForm (Xc*MajorRadius, Xdir,
		     Yc*MinorRadius, Ydir,
		     Pos.Location().XY());
  P.SetXY (Vxy);
  
  //V1 :
  Vxy.SetLinearForm (-Yc*MajorRadius, Xdir, Xc*MinorRadius, Ydir);
  V1.SetXY (Vxy);
}

//=======================================================================
//function : HyperbolaD1
//purpose  : 
//=======================================================================

void ElCLib::HyperbolaD1 (const Standard_Real U,
			  const gp_Ax22d& Pos,
			  const Standard_Real MajorRadius,
			  const Standard_Real MinorRadius,
			  gp_Pnt2d& P,
			  gp_Vec2d& V1)
{
  gp_XY Vxy;
  gp_XY Xdir ((Pos.XDirection()).XY());
  gp_XY Ydir ((Pos.YDirection()).XY());
  Standard_Real Xc = Cosh(U);
  Standard_Real Yc = Sinh(U);
  //Point courant :
  Vxy.SetLinearForm (Xc*MajorRadius, Xdir, 
		     Yc*MinorRadius, Ydir,
		     Pos.Location().XY());
  P.SetXY (Vxy);
  
  //V1 :
  Vxy.SetLinearForm (Yc*MajorRadius, Xdir, Xc*MinorRadius, Ydir);
  V1.SetXY (Vxy);
}

//=======================================================================
//function : ParabolaD1
//purpose  : 
//=======================================================================

void ElCLib::ParabolaD1 (const Standard_Real U,
			 const gp_Ax22d& Pos,
			 const Standard_Real Focal,
			 gp_Pnt2d& P,
			 gp_Vec2d& V1)
{
  gp_XY Vxy;
  gp_XY Xdir (Pos.XDirection().XY());
  if (Focal == 0.0) {      //Parabole degenere en une droite
    V1.SetXY (Xdir);
    Vxy.SetLinearForm (U, Xdir, Pos.Location().XY());
  }
  else {
    gp_XY Ydir (Pos.YDirection().XY());
    Vxy.SetLinearForm (U / (2.0 * Focal), Xdir, Ydir);
    V1.SetXY (Vxy);
    Vxy.SetLinearForm ((U * U) / (4.0 * Focal), Xdir, 
		       U, Ydir,
		       Pos.Location().XY());
  }
  P.SetXY (Vxy);
}

//=======================================================================
//function : CircleD2
//purpose  : 
//=======================================================================

void ElCLib::CircleD2 (const Standard_Real U,
		       const gp_Ax22d& Pos,
		       const Standard_Real Radius, 
		       gp_Pnt2d& P,
		       gp_Vec2d& V1,
		       gp_Vec2d& V2)
{
  gp_XY Vxy;
  gp_XY Xdir (Pos.XDirection().XY());
  gp_XY Ydir (Pos.YDirection().XY());
  Standard_Real Xc = Radius * cos(U);
  Standard_Real Yc = Radius * sin(U);
  //V2 :
  Vxy.SetLinearForm (Xc, Xdir, Yc, Ydir);
  V2.SetXY (Vxy);
  V2.Reverse();
  Vxy.Add (Pos.Location().XY());
  P.SetXY (Vxy);

  //V1 :
  Vxy.SetLinearForm (-Yc, Xdir, Xc, Ydir);
  V1.SetXY (Vxy);
}

//=======================================================================
//function : EllipseD2
//purpose  : 
//=======================================================================

void ElCLib::EllipseD2 (const Standard_Real U,
			const gp_Ax22d& Pos,
			const Standard_Real MajorRadius,
			const Standard_Real MinorRadius,
			gp_Pnt2d& P,
			gp_Vec2d& V1,
			gp_Vec2d& V2)
{
  gp_XY Vxy;
  gp_XY Xdir (Pos.XDirection().XY());
  gp_XY Ydir (Pos.YDirection().XY());
  Standard_Real Xc = cos(U);
  Standard_Real Yc = sin(U);

  //V2 :
  Vxy.SetLinearForm (Xc*MajorRadius, Xdir, Yc*MinorRadius, Ydir);
  V2.SetXY (Vxy);
  V2.Reverse ();

  //Point courant :
  Vxy.Add (Pos.Location().XY());
  P.SetXY (Vxy);

  //V1 :
  Vxy.SetLinearForm (-Yc*MajorRadius, Xdir, Xc*MinorRadius, Ydir);
  V1.SetXY (Vxy);
}

//=======================================================================
//function : HyperbolaD2
//purpose  : 
//=======================================================================

void ElCLib::HyperbolaD2 (const Standard_Real U,
			  const gp_Ax22d& Pos,
			  const Standard_Real MajorRadius,
			  const Standard_Real MinorRadius,
			  gp_Pnt2d& P,
			  gp_Vec2d& V1,
			  gp_Vec2d& V2)
{
  gp_XY Vxy;
  gp_XY Xdir (Pos.XDirection().XY()); 
  gp_XY Ydir (Pos.YDirection().XY()); 
  Standard_Real Xc = Cosh(U);
  Standard_Real Yc = Sinh(U);

  //V2 :
  Vxy.SetLinearForm (Xc*MajorRadius, Xdir, Yc*MinorRadius, Ydir);
  V2.SetXY (Vxy);

  //Point courant :
  Vxy.Add (Pos.Location().XY());
  P.SetXY (Vxy);

  //V1 :
  Vxy.SetLinearForm (Yc*MajorRadius, Xdir, Xc*MinorRadius, Ydir);
  V1.SetXY (Vxy);
}

//=======================================================================
//function : ParabolaD2
//purpose  : 
//=======================================================================

void ElCLib::ParabolaD2 (const Standard_Real U,
			 const gp_Ax22d& Pos,
			 const Standard_Real Focal,
			 gp_Pnt2d& P,
			 gp_Vec2d& V1,
			 gp_Vec2d& V2)
{
  gp_XY Vxy;
  gp_XY Xdir (Pos.XDirection().XY());
  if (Focal == 0.0) {
    V2.SetCoord (0.0, 0.0);
    V1.SetXY (Xdir);
    Vxy.SetLinearForm (U, Xdir, Pos.Location().XY());
  }
  else {
    gp_XY Ydir (Pos.YDirection().XY());
    Vxy = Xdir.Multiplied (1.0 / (2.0 * Focal));
    V2.SetXY (Vxy);
    Vxy.SetLinearForm (U, Vxy, Ydir);
    V1.SetXY (Vxy);
    Vxy.SetLinearForm (U * U / (4.0 * Focal), Xdir, U, Ydir);
    Vxy.Add (Pos.Location().XY());
  }
  P.SetXY (Vxy);
}

//=======================================================================
//function : CircleD3
//purpose  : 
//=======================================================================

void ElCLib::CircleD3 (const Standard_Real U,
		       const gp_Ax22d& Pos,
		       const Standard_Real Radius,
		       gp_Pnt2d& P,
		       gp_Vec2d& V1,
		       gp_Vec2d& V2,
		       gp_Vec2d& V3)
{
  gp_XY Vxy;
  gp_XY Xdir (Pos.XDirection().XY());
  gp_XY Ydir (Pos.YDirection().XY());
  Standard_Real Xc = Radius * cos(U);
  Standard_Real Yc = Radius * sin(U);

  //V2 :
  Vxy.SetLinearForm (Xc, Xdir, Yc, Ydir);
  V2.SetXY (Vxy);
  V2.Reverse();

  //Point courant :
  Vxy.Add (Pos.Location().XY());
  P.SetXY (Vxy);

  //V1 :
  Vxy.SetLinearForm (-Yc, Xdir, Xc, Ydir);
  V1.SetXY (Vxy);

  //V3 :
  V3.SetXY (Vxy);
  V3.Reverse ();
}

//=======================================================================
//function : EllipseD3
//purpose  : 
//=======================================================================

void ElCLib::EllipseD3 (const Standard_Real U,
			const gp_Ax22d& Pos,
			const Standard_Real MajorRadius,
			const Standard_Real MinorRadius,
			gp_Pnt2d& P,
			gp_Vec2d& V1,
			gp_Vec2d& V2,
			gp_Vec2d& V3)
{
  gp_XY Vxy;
  gp_XY Xdir (Pos.XDirection().XY());
  gp_XY Ydir (Pos.YDirection().XY());
  Standard_Real Xc = cos(U);
  Standard_Real Yc = sin(U);

  //V2 :
  Vxy.SetLinearForm (Xc*MajorRadius, Xdir, Yc*MinorRadius, Ydir);
  V2.SetXY (Vxy);
  V2.Reverse ();

  //Point courant :
  Vxy.Add (Pos.Location().XY());
  P.SetXY (Vxy);

  //V1 :
  Vxy.SetLinearForm (-Yc*MajorRadius, Xdir, Xc*MinorRadius, Ydir);
  V1.SetXY (Vxy);

  //V3 :
  V3.SetXY (Vxy);
  V3.Reverse ();
}

//=======================================================================
//function : HyperbolaD3
//purpose  : 
//=======================================================================

void ElCLib::HyperbolaD3 (const Standard_Real U,
			  const gp_Ax22d& Pos,
			  const Standard_Real MajorRadius,
			  const Standard_Real MinorRadius,
			  gp_Pnt2d& P,
			  gp_Vec2d& V1,
			  gp_Vec2d& V2,
			  gp_Vec2d& V3)
{
  gp_XY Vxy;
  gp_XY Xdir (Pos.XDirection().XY());
  gp_XY Ydir (Pos.YDirection().XY());
  Standard_Real Xc = Cosh(U);
  Standard_Real Yc = Sinh(U);

  //V2 :
  Vxy.SetLinearForm (Xc*MajorRadius, Xdir, Yc*MinorRadius, Ydir);
  V2.SetXY (Vxy);

  //Point courant :
  Vxy.Add (Pos.Location().XY());
  P.SetXY (Vxy);

  //V1 :
  Vxy.SetLinearForm (Yc*MajorRadius, Xdir, Xc*MinorRadius, Ydir);
  V1.SetXY (Vxy);

  //V3 :
  V3.SetXY (Vxy);
}

//=======================================================================
//function : LineDN
//purpose  : 
//=======================================================================

gp_Vec ElCLib::LineDN (const Standard_Real,
		       const gp_Ax1& Pos,
		       const Standard_Integer N) 
{
  if (N == 1) { return gp_Vec(Pos.Direction()); }
  return gp_Vec (0., 0., 0.);
}

//=======================================================================
//function : CircleDN
//purpose  : 
//=======================================================================

gp_Vec ElCLib::CircleDN (const Standard_Real U,
			 const gp_Ax2& Pos,
			 const Standard_Real Radius,
			 const Standard_Integer N) 
{
  Standard_Real Xc=0, Yc=0;
  if (N == 1) {       
    Xc = Radius * -sin(U);
    Yc = Radius *  cos(U);
  }
  else if ((N + 2) % 4 == 0) {
    Xc = Radius * -cos(U);
    Yc = Radius * -sin(U);
  }
  else if ((N + 1) % 4 == 0) {
    Xc = Radius *  sin(U);
    Yc = Radius * -cos(U);
  }
  else if (N % 4 == 0) {
    Xc = Radius *  cos(U);
    Yc = Radius *  sin(U);
  }
  else if ((N-1) % 4 == 0) {
    Xc = Radius * -sin(U);
    Yc = Radius *  cos(U);
  }
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  Coord1.SetLinearForm (Xc, Coord1, Yc, Pos.YDirection().XYZ());
  return gp_Vec (Coord1);
}

//=======================================================================
//function : EllipseDN
//purpose  : 
//=======================================================================

gp_Vec ElCLib::EllipseDN (const Standard_Real U,
			  const gp_Ax2& Pos,
			  const Standard_Real MajorRadius,
			  const Standard_Real MinorRadius,
			  const Standard_Integer N)
{
  Standard_Real Xc=0, Yc=0;
  if (N == 1) {       
    Xc = MajorRadius * -sin(U);
    Yc = MinorRadius *  cos(U);
  }
  else if ((N + 2) % 4 == 0) {
    Xc = MajorRadius * -cos(U);
    Yc = MinorRadius * -sin(U);
  }
  else if ((N + 1) % 4 == 0) {
    Xc = MajorRadius *  sin(U);
    Yc = MinorRadius * -cos(U);
  }
  else if (N % 4 == 0) {
    Xc = MajorRadius *  cos(U);
    Yc = MinorRadius *  sin(U);
  }
  else if ((N-1) % 4 == 0) {
    Xc = MajorRadius * -sin(U);
    Yc = MinorRadius *  cos(U);
  }
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  Coord1.SetLinearForm (Xc, Coord1, Yc, Pos.YDirection().XYZ());
  return gp_Vec (Coord1);
}

//=======================================================================
//function : HyperbolaDN
//purpose  : 
//=======================================================================

gp_Vec ElCLib::HyperbolaDN (const Standard_Real U,
			    const gp_Ax2& Pos,
			    const Standard_Real MajorRadius,
			    const Standard_Real MinorRadius,
			    const Standard_Integer N)
{
  Standard_Real Xc=0, Yc=0;
  if (IsOdd (N)) {       
    Xc = MajorRadius * Sinh(U);
    Yc = MinorRadius * Cosh(U);
  }
  else if (IsEven (N)) {
    Xc = MajorRadius * Cosh(U);
    Yc = MinorRadius * Sinh(U);
  }
  gp_XYZ Coord1 (Pos.XDirection().XYZ());
  Coord1.SetLinearForm (Xc, Coord1, Yc, Pos.YDirection().XYZ());
  return gp_Vec (Coord1);
}

//=======================================================================
//function : ParabolaDN
//purpose  : 
//=======================================================================

gp_Vec ElCLib::ParabolaDN (const Standard_Real U,
			   const gp_Ax2& Pos,
			   const Standard_Real Focal,
			   const Standard_Integer N) 
{
  if (N <= 2) {
    gp_XYZ Coord1 (Pos.XDirection().XYZ());
    if (N == 1) {
      if (Focal == 0.0) { 
	return gp_Vec(Coord1);
      }
      else {
	Coord1.SetLinearForm
	  (U / (2.0 * Focal), Coord1, Pos.YDirection().XYZ());
	return gp_Vec (Coord1);
      }
    }
    else if (N == 2) {
      if (Focal == 0.0) { 
	return gp_Vec(0.0, 0.0, 0.0);
      }
      else {
	Coord1.Multiply ( 1.0 / (2.0 * Focal));
	return gp_Vec (Coord1);
      }
    }
  }
  return gp_Vec (0.,0.,0.);
}

//=======================================================================
//function : LineDN
//purpose  : 
//=======================================================================

gp_Vec2d ElCLib::LineDN (const Standard_Real,
			 const gp_Ax2d& Pos,
			 const Standard_Integer N) 
{
  if (N == 1) { return gp_Vec2d (Pos.Direction ()); }
  return gp_Vec2d (0.0, 0.0);
}

//=======================================================================
//function : CircleDN
//purpose  : 
//=======================================================================

gp_Vec2d ElCLib::CircleDN (const Standard_Real U,
			   const gp_Ax22d& Pos,
			   const Standard_Real Radius,
			   const Standard_Integer N)
{
  Standard_Real Xc=0, Yc=0;
  if (N == 1) {       
    Xc = Radius * -sin(U);
    Yc = Radius *  cos(U);
  }
  else if ((N + 2) % 4 == 0) {
    Xc = Radius * -cos(U);
    Yc = Radius * -sin(U);
  }
  else if ((N + 1) % 4 == 0) {
    Xc = Radius *  sin(U);
    Yc = Radius * -cos(U);
  }
  else if (N % 4 == 0) {
    Xc = Radius *  cos(U);
    Yc = Radius *  sin(U);
  }
  else if ((N-1) % 4 == 0) {
    Xc = Radius * -sin(U);
    Yc = Radius *  cos(U);
  }
  gp_XY Xdir (Pos.XDirection().XY());
  gp_XY Ydir (Pos.YDirection().XY());
  Xdir.SetLinearForm (Xc, Xdir, Yc, Ydir);
  return gp_Vec2d (Xdir);
}

//=======================================================================
//function : EllipseDN
//purpose  : 
//=======================================================================

gp_Vec2d ElCLib::EllipseDN (const Standard_Real U,
			    const gp_Ax22d& Pos,
			    const Standard_Real MajorRadius,
			    const Standard_Real MinorRadius,
			    const Standard_Integer N)
{
  Standard_Real Xc=0, Yc=0;
  if (N == 1) {       
    Xc = MajorRadius * -sin(U);
    Yc = MinorRadius *  cos(U);
  }
  else if ((N + 2) % 4 == 0) {
    Xc = MajorRadius * -cos(U);
    Yc = MinorRadius * -sin(U);
  }
  else if ((N + 1) % 4 == 0) {
    Xc = MajorRadius *  sin(U);
    Yc = MinorRadius * -cos(U);
  }
  else if (N % 4 == 0) {
    Xc = MajorRadius *  cos(U);
    Yc = MinorRadius *  sin(U);
  }
  else if ((N-1) % 4 == 0) {
    Xc = MajorRadius * -sin(U);
    Yc = MinorRadius *  cos(U);
  }
  gp_XY Xdir (Pos.XDirection().XY());
  gp_XY Ydir (Pos.YDirection().XY());
  Xdir.SetLinearForm (Xc, Xdir, Yc, Ydir);
  return gp_Vec2d (Xdir);
}

//=======================================================================
//function : HyperbolaDN
//purpose  : 
//=======================================================================

gp_Vec2d ElCLib::HyperbolaDN (const Standard_Real U,
			      const gp_Ax22d& Pos,
			      const Standard_Real MajorRadius,
			      const Standard_Real MinorRadius,
			      const Standard_Integer N)
{
  Standard_Real Xc=0, Yc=0;
  if (IsOdd (N)) {       
    Xc = MajorRadius * Sinh(U);
    Yc = MinorRadius * Cosh(U);
  }
  else if (IsEven (N)) {
    Xc = MajorRadius * Cosh(U);
    Yc = MinorRadius * Sinh(U);
  }
  gp_XY Xdir (Pos.XDirection().XY());
  gp_XY Ydir (Pos.YDirection().XY());
  Xdir.SetLinearForm (Xc, Xdir, Yc, Ydir);
  return gp_Vec2d (Xdir);
}

//=======================================================================
//function : ParabolaDN
//purpose  : 
//=======================================================================

gp_Vec2d ElCLib::ParabolaDN (const Standard_Real U,
			     const gp_Ax22d& Pos,
			     const Standard_Real Focal,
			     const Standard_Integer N)
{
  if (N <= 2)  {
    gp_XY Xdir (Pos.XDirection().XY());
    if (N == 1) {
      if (Focal == 0.0) { 
	return gp_Vec2d (Xdir);
      }
      else {
	gp_XY Ydir (Pos.YDirection().XY());
	Xdir.SetLinearForm (U / (2.0 * Focal), Xdir,  Ydir);
	return gp_Vec2d (Xdir);
      }
    }
    else if (N == 2) {
      if (Focal == 0.0) { 
	return gp_Vec2d (0.0, 0.0);
      }
      else {
	Xdir.Multiply ( 1.0 / (2.0 * Focal));
	return gp_Vec2d (Xdir);
      }
    }
  }
  return gp_Vec2d (0.0, 0.0); 
}

//=======================================================================
//function : LineParameter
//purpose  : 
//=======================================================================

Standard_Real ElCLib::LineParameter (const gp_Ax1& L, const gp_Pnt& P)
{
  return (P.XYZ() - L.Location().XYZ()).Dot (L.Direction().XYZ());
}

//=======================================================================
//function : CircleParameter
//purpose  : 
//=======================================================================
Standard_Real ElCLib::CircleParameter(const gp_Ax2& Pos,
                                      const gp_Pnt& P)
{
  gp_Vec aVec(Pos.Location(), P);
  if (aVec.SquareMagnitude() < gp::Resolution())
    // coinciding points -> infinite number of parameters
    return 0.0;

  const gp_Dir& dir = Pos.Direction();
  // Project vector on circle's plane
  gp_XYZ aVProj = dir.XYZ().CrossCrossed(aVec.XYZ(), dir.XYZ());

  if (aVProj.SquareModulus() < gp::Resolution())
    return 0.0;

  // Angle between X direction and projected vector
  Standard_Real Teta = (Pos.XDirection()).AngleWithRef(aVProj, dir);

  if      (Teta < -1.e-16)  Teta += PIPI;
  else if (Teta < 0)        Teta = 0;
  return Teta;
}

//=======================================================================
//function : EllipseParameter
//purpose  : 
//=======================================================================

Standard_Real ElCLib::EllipseParameter (const gp_Ax2& Pos,
					const Standard_Real MajorRadius,
					const Standard_Real MinorRadius,
					const gp_Pnt& P)
{
  gp_XYZ  OP = P.XYZ()-Pos.Location().XYZ();
  gp_XYZ xaxis = Pos.XDirection().XYZ();
  gp_XYZ yaxis = Pos.YDirection().XYZ();
  Standard_Real NY = OP.Dot(yaxis);
  Standard_Real NX = OP.Dot(xaxis); 

  if(  (Abs(NX) <= gp::Resolution())
     &&(Abs(NY) <= gp::Resolution()))
    //-- The point P is on the Axis of the Ellipse. 
    return(0.0);
  
  yaxis.Multiply (NY * (MajorRadius/MinorRadius));
  gp_XYZ Om = xaxis.Multiplied (NX);
  Om.Add (yaxis); 
  Standard_Real Teta = gp_Vec(xaxis).AngleWithRef
    (gp_Vec(Om), gp_Vec(Pos.Direction()));
  if      (Teta < -1.e-16)  Teta += PIPI;
  else if (Teta < 0)        Teta = 0;
  return Teta;
}

//=======================================================================
//function : HyperbolaParameter
//purpose  : 
//=======================================================================

Standard_Real ElCLib::HyperbolaParameter (const gp_Ax2& Pos,
					  const Standard_Real,
					  const Standard_Real MinorRadius,
					  const gp_Pnt& P)
{
  Standard_Real sht = 
    gp_Vec(Pos.Location (), P).Dot
      (gp_Vec (Pos.YDirection())) / MinorRadius;

#if __QNX__
  return std::asinh(sht);
#else
  return asinh(sht);
#endif
}

//=======================================================================
//function : ParabolaParameter
//purpose  : 
//=======================================================================

Standard_Real ElCLib::ParabolaParameter (const gp_Ax2& Pos,
					 const gp_Pnt& P)
{
  return gp_Vec(Pos.Location(),P).Dot(gp_Vec(Pos.YDirection()));
}

//=======================================================================
//function : LineParameter
//purpose  : 
//=======================================================================

Standard_Real ElCLib::LineParameter (const gp_Ax2d& L,
				     const gp_Pnt2d& P)
{
  gp_XY Coord = P.XY();
  Coord.Subtract (L.Location().XY());
  return Coord.Dot (L.Direction().XY());
}

//=======================================================================
//function : CircleParameter
//purpose  : 
//=======================================================================

Standard_Real ElCLib::CircleParameter (const gp_Ax22d& Pos,
				       const gp_Pnt2d& P)
{
  Standard_Real Teta = (Pos.XDirection()).Angle
    (gp_Vec2d (Pos.Location(),P));
  Teta = ((Pos.XDirection() ^ Pos.YDirection()) >= 0.0) ? Teta: -Teta;
  if      (Teta < -1.e-16)  Teta += PIPI;
  else if (Teta < 0)        Teta = 0;
  return Teta;
}

//=======================================================================
//function : EllipseParameter
//purpose  : 
//=======================================================================

Standard_Real ElCLib::EllipseParameter (const gp_Ax22d& Pos,
					const Standard_Real MajorRadius,
					const Standard_Real MinorRadius, 
					const gp_Pnt2d& P)
{
  gp_XY  OP = P.XY();
  OP.Subtract (Pos.Location().XY());
  gp_XY xaxis = Pos.XDirection().XY();
  gp_XY yaxis = Pos.YDirection().XY();
  gp_XY Om = xaxis.Multiplied (OP.Dot(xaxis));
  yaxis.Multiply ( (OP.Dot(yaxis)) * (MajorRadius/MinorRadius) );
  Om.Add (yaxis); 
  Standard_Real Teta = gp_Vec2d(xaxis).Angle (gp_Vec2d(Om));
  Teta = ((Pos.XDirection() ^ Pos.YDirection()) >= 0.0) ? Teta: -Teta;
  if      (Teta < -1.e-16)  Teta += PIPI;
  else if (Teta < 0)        Teta = 0;
  return Teta;
}

//=======================================================================
//function : HyperbolaParameter
//purpose  : 
//=======================================================================

Standard_Real ElCLib::HyperbolaParameter (const gp_Ax22d& Pos,
					  const Standard_Real,
					  const Standard_Real MinorRadius, 
					  const gp_Pnt2d& P)
{
  gp_Vec2d V (Pos.YDirection().XY());
  Standard_Real sht = gp_Vec2d(Pos.Location(),P).Dot(V) /MinorRadius;
#if __QNX__
  return std::asinh(sht);
#else
  return asinh(sht);
#endif
}

//=======================================================================
//function : ParabolaParameter
//purpose  : 
//=======================================================================

Standard_Real ElCLib::ParabolaParameter (const gp_Ax22d& Pos,
					 const gp_Pnt2d& P)
{
  gp_Vec2d Directrix (Pos.YDirection().XY());
  return gp_Vec2d(Pos.Location(),P).Dot(Directrix);
}

//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

gp_Pnt ElCLib::To3d (const gp_Ax2& Pos, const gp_Pnt2d& P)
{
  gp_XYZ Vxy = Pos.XDirection().XYZ();
  Vxy.SetLinearForm (P.X(), Vxy, P.Y(), Pos.YDirection().XYZ(), 
		     Pos.Location().XYZ());
  return gp_Pnt (Vxy);

}

//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

gp_Dir ElCLib::To3d (const gp_Ax2& Pos, const gp_Dir2d& V)
{
  gp_Vec Vx = Pos.XDirection();
  gp_Vec Vy = Pos.YDirection();
  Vx.Multiply (V.X());
  Vy.Multiply (V.Y());
  Vx.Add(Vy);
  return gp_Dir (Vx);
}

//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

gp_Vec ElCLib::To3d (const gp_Ax2& Pos, const gp_Vec2d& V)
{
  gp_Vec Vx = Pos.XDirection();
  gp_Vec Vy = Pos.YDirection();
  Vx.Multiply (V.X());
  Vy.Multiply (V.Y());
  Vx.Add(Vy);
  return Vx;
}

//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

gp_Ax1 ElCLib::To3d (const gp_Ax2& Pos, const gp_Ax2d& A)
{
  gp_Pnt P  = ElCLib::To3d (Pos, A.Location());
  gp_Vec V  = ElCLib::To3d (Pos, A.Direction());
  return gp_Ax1 (P, V);
}

//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

gp_Ax2 ElCLib::To3d (const gp_Ax2& Pos, const gp_Ax22d& A)
{
  gp_Pnt P  = ElCLib::To3d (Pos, A.Location());
  gp_Vec VX = ElCLib::To3d (Pos, A.XDirection());
  gp_Vec VY = ElCLib::To3d (Pos, A.YDirection());
  return gp_Ax2 (P, VX.Crossed(VY), VX);
}

//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

gp_Lin ElCLib::To3d (const gp_Ax2& Pos, const gp_Lin2d& L)
{
  return gp_Lin (ElCLib::To3d (Pos, L.Position()));
}

//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

gp_Circ ElCLib::To3d (const gp_Ax2& Pos, const gp_Circ2d& C)
{
  return gp_Circ (ElCLib::To3d (Pos, C.Axis()), C.Radius());
}

//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

gp_Elips ElCLib::To3d (const gp_Ax2& Pos, const gp_Elips2d& E)
{
  return gp_Elips (ElCLib::To3d (Pos, E.Axis ()), E.MajorRadius (),
		   E.MinorRadius ());
}

//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

gp_Hypr ElCLib::To3d (const gp_Ax2& Pos, const gp_Hypr2d& H)
{
  return gp_Hypr (ElCLib::To3d (Pos, H.Axis ()), H.MajorRadius (),
		  H.MinorRadius ());
}

//=======================================================================
//function : To3d
//purpose  : 
//=======================================================================

gp_Parab ElCLib::To3d (const gp_Ax2& Pos, const gp_Parab2d& Prb)
{
  return gp_Parab ( ElCLib::To3d (Pos, Prb.Axis ()), Prb.Focal ());
}

