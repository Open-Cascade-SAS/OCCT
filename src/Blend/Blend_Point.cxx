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


#include <Blend_Point.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_DomainError.hxx>

Blend_Point::Blend_Point ():istgt(Standard_True) {}

Blend_Point::Blend_Point(const gp_Pnt& P1,
			 const gp_Pnt& P2,
			 const Standard_Real Param,
			 const Standard_Real U1,
			 const Standard_Real V1,
			 const Standard_Real U2,
			 const Standard_Real V2,
			 const gp_Vec& Tg1,
			 const gp_Vec& Tg2,
			 const gp_Vec2d& Tg12d,
			 const gp_Vec2d& Tg22d) :
       pt1(P1),pt2(P2),tg1(Tg1),tg2(Tg2),prm(Param),
       u1(U1),v1(V1),u2(U2),v2(V2),
       utg12d(Tg12d.X()),vtg12d(Tg12d.Y()),
       utg22d(Tg22d.X()),vtg22d(Tg22d.Y()),
       hass1(Standard_True), hass2(Standard_True),
       hasc1(Standard_False),hasc2(Standard_False),
       istgt(Standard_False)
{}

Blend_Point::Blend_Point(const gp_Pnt& P1,
			 const gp_Pnt& P2,
			 const Standard_Real Param,
			 const Standard_Real U1,
			 const Standard_Real V1,
			 const Standard_Real U2,
			 const Standard_Real V2) :
       pt1(P1),pt2(P2),prm(Param),
       u1(U1),v1(V1),u2(U2),v2(V2),
       hass1(Standard_True),hass2(Standard_True),
       hasc1(Standard_False),hasc2(Standard_False),
       istgt(Standard_True)
{}

void Blend_Point::SetValue(const gp_Pnt& P1,
			   const gp_Pnt& P2,
			   const Standard_Real Param,
			   const Standard_Real U1,
			   const Standard_Real V1,
			   const Standard_Real U2,
			   const Standard_Real V2,
			   const gp_Vec& Tg1,
			   const gp_Vec& Tg2,
			   const gp_Vec2d& Tg12d,
			   const gp_Vec2d& Tg22d)
{
  pt1   = P1;
  pt2   = P2;
  prm   = Param;
  u1    = U1;
  v1    = V1;
  hass1 = Standard_True;
  u2    = U2;
  v2    = V2;
  hass2 = Standard_True;
  hasc1 = Standard_False;
  hasc2 = Standard_False;
  istgt = Standard_False;
  tg1   = Tg1;
  tg2   = Tg2;
  utg12d = Tg12d.X();
  vtg12d = Tg12d.Y();
  utg22d = Tg22d.X();
  vtg22d = Tg22d.Y();
}

void Blend_Point::SetValue(const gp_Pnt& P1,
			   const gp_Pnt& P2,
			   const Standard_Real Param,
			   const Standard_Real U1,
			   const Standard_Real V1,
			   const Standard_Real U2,
			   const Standard_Real V2)
{
  pt1   = P1;
  pt2   = P2;
  prm   = Param;
  u1    = U1;
  v1    = V1;
  hass1 = Standard_True;
  u2    = U2;
  v2    = V2;
  hass2 = Standard_True;
  hasc1 = Standard_False;
  hasc2 = Standard_False;
  istgt = Standard_True;
}

Blend_Point::Blend_Point(const gp_Pnt& Ps,
			 const gp_Pnt& Pc,
			 const Standard_Real Param,
			 const Standard_Real U,
			 const Standard_Real V,
			 const Standard_Real W,
			 const gp_Vec& Tgs,
			 const gp_Vec& Tgc,
			 const gp_Vec2d& Tg2d) :
       pt1(Ps),pt2(Pc),tg1(Tgs),tg2(Tgc),prm(Param),
       u1(U),v1(V),pc2(W),
       utg12d(Tg2d.X()),vtg12d(Tg2d.Y()),
       hass1(Standard_True),
       hass2(Standard_False),
       hasc1(Standard_False),
       hasc2(Standard_True),
       istgt(Standard_False)
{}

Blend_Point::Blend_Point(const gp_Pnt& Ps,
			 const gp_Pnt& Pc,
			 const Standard_Real Param,
			 const Standard_Real U,
			 const Standard_Real V,
			 const Standard_Real W) :
       pt1(Ps),pt2(Pc),prm(Param),
       u1(U),v1(V),       pc2(W),
       hass1(Standard_True),
       hass2(Standard_False),
       hasc1(Standard_False),hasc2(Standard_True),
       istgt(Standard_True)
{}

void Blend_Point::SetValue(const gp_Pnt& Ps,
			   const gp_Pnt& Pc,
			   const Standard_Real Param,
			   const Standard_Real U,
			   const Standard_Real V,
			   const Standard_Real W,
			   const gp_Vec& Tgs,
			   const gp_Vec& Tgc,
			   const gp_Vec2d& Tg2d)
{
  pt1    = Ps;
  pt2    = Pc;
  prm    = Param;
  u1     = U;
  v1     = V;
  hass1  = Standard_True;
  hass2  = Standard_False;
  hasc1  = Standard_False;
  pc2    = W;
  hasc2  = Standard_True;
  istgt  = Standard_False;
  tg1    = Tgs;
  tg2    = Tgc;
  utg12d = Tg2d.X();
  vtg12d = Tg2d.Y();
}

void Blend_Point::SetValue(const gp_Pnt& Ps,
			   const gp_Pnt& Pc,
			   const Standard_Real Param,
			   const Standard_Real U,
			   const Standard_Real V,
			   const Standard_Real W)
{
  pt1   = Ps;
  pt2   = Pc;
  prm   = Param;
  u1    = U;
  v1    = V;
  hass1  = Standard_True;
  hass2  = Standard_False;
  hasc1  = Standard_False;
  pc2    = W;
  hasc2  = Standard_True;
  istgt = Standard_True;
}

Blend_Point::Blend_Point(const gp_Pnt& P1,
			 const gp_Pnt& P2,
			 const Standard_Real Param,
			 const Standard_Real U1,
			 const Standard_Real V1,
			 const Standard_Real U2,
			 const Standard_Real V2,
			 const Standard_Real PC,
			 const gp_Vec& Tg1,
			 const gp_Vec& Tg2,
			 const gp_Vec2d& Tg12d,
			 const gp_Vec2d& Tg22d) :
       pt1(P1),pt2(P2),tg1(Tg1),tg2(Tg2),prm(Param),
       u1(U1),v1(V1),
       u2(U2),v2(V2),pc2(PC),
       utg12d(Tg12d.X()),vtg12d(Tg12d.Y()),
       utg22d(Tg22d.X()),vtg22d(Tg22d.Y()),
       hass1(Standard_True),hass2(Standard_True),
       hasc1(Standard_False),
       hasc2(Standard_True),
       istgt(Standard_False)
{}

Blend_Point::Blend_Point(const gp_Pnt& P1,
			 const gp_Pnt& P2,
			 const Standard_Real Param,
			 const Standard_Real U1,
			 const Standard_Real V1,
			 const Standard_Real U2,
			 const Standard_Real V2,
			 const Standard_Real PC) :
       pt1(P1),pt2(P2),prm(Param),
       u1(U1),v1(V1),
       u2(U2),v2(V2),pc2(PC),hass1(Standard_True),hass2(Standard_True),
       hasc1(Standard_False),
       hasc2(Standard_True),
       istgt(Standard_True)
{}

void Blend_Point::SetValue(const gp_Pnt& P1,
			   const gp_Pnt& P2,
			   const Standard_Real Param,
			   const Standard_Real U1,
			   const Standard_Real V1,
			   const Standard_Real U2,
			   const Standard_Real V2,
			   const Standard_Real PC,
			   const gp_Vec& Tg1,
			   const gp_Vec& Tg2,
			   const gp_Vec2d& Tg12d,
			   const gp_Vec2d& Tg22d)
{
  pt1   = P1;
  pt2   = P2;
  prm   = Param;
  u1    = U1;
  v1    = V1;
  hass1 = Standard_True;
  u2    = U2;
  v2    = V2;
  hass2 = Standard_True;
  hasc1 = Standard_False;
  pc2   = PC;
  hasc2 = Standard_True;
  istgt = Standard_False;
  tg1   = Tg1;
  tg2   = Tg2;
  utg12d = Tg12d.X();
  vtg12d = Tg12d.Y();
  utg22d = Tg22d.X();
  vtg22d = Tg22d.Y();
}

void Blend_Point::SetValue(const gp_Pnt& P1,
			   const gp_Pnt& P2,
			   const Standard_Real Param,
			   const Standard_Real U1,
			   const Standard_Real V1,
			   const Standard_Real U2,
			   const Standard_Real V2,
			   const Standard_Real PC)
{
  pt1   = P1;
  pt2   = P2;
  prm   = Param;
  u1    = U1;
  v1    = V1;
  hass1 = Standard_True;
  u2    = U2;
  v2    = V2;
  hass2 = Standard_True;
  hasc1 = Standard_False;
  pc2   = PC;
  hasc2 = Standard_True;
  istgt = Standard_True;
}

Blend_Point::Blend_Point(const gp_Pnt& P1,
			 const gp_Pnt& P2,
			 const Standard_Real Param,
			 const Standard_Real U1,
			 const Standard_Real V1,
			 const Standard_Real U2,
			 const Standard_Real V2,
			 const Standard_Real PC1,
			 const Standard_Real PC2,
			 const gp_Vec& Tg1,
			 const gp_Vec& Tg2,
			 const gp_Vec2d& Tg12d,
			 const gp_Vec2d& Tg22d) :
       pt1(P1),pt2(P2),tg1(Tg1),tg2(Tg2),prm(Param),
       u1(U1),v1(V1),u2(U2),v2(V2),
       pc1(PC1),pc2(PC2),
       utg12d(Tg12d.X()),vtg12d(Tg12d.Y()),
       utg22d(Tg22d.X()),vtg22d(Tg22d.Y()),
       hass1(Standard_True),hass2(Standard_True),
       hasc1(Standard_True),hasc2(Standard_True),
       istgt(Standard_False)
{}

Blend_Point::Blend_Point(const gp_Pnt& P1,
			 const gp_Pnt& P2,
			 const Standard_Real Param,
			 const Standard_Real U1,
			 const Standard_Real V1,
			 const Standard_Real U2,
			 const Standard_Real V2,
			 const Standard_Real PC1,
			 const Standard_Real PC2) :
       pt1(P1),pt2(P2),prm(Param),
       u1(U1),v1(V1),
       u2(U2),v2(V2),pc1(PC1),pc2(PC2),
       hass1(Standard_True),hass2(Standard_True),
       hasc1(Standard_True),hasc2(Standard_True),
       istgt(Standard_True)
{}

void Blend_Point::SetValue(const gp_Pnt& P1,
			   const gp_Pnt& P2,
			   const Standard_Real Param,
			   const Standard_Real U1,
			   const Standard_Real V1,
			   const Standard_Real U2,
			   const Standard_Real V2,
			   const Standard_Real PC1,
			   const Standard_Real PC2,
			   const gp_Vec& Tg1,
			   const gp_Vec& Tg2,
			   const gp_Vec2d& Tg12d,
			   const gp_Vec2d& Tg22d)
{
  pt1   = P1;
  pt2   = P2;
  prm   = Param;
  u1    = U1;
  v1    = V1;
  hass1 = Standard_True;
  u2    = U2;
  v2    = V2;
  hass2 = Standard_True;
  pc1   = PC1;
  hasc1 = Standard_True;
  pc2   = PC2;
  hasc2 = Standard_True;
  istgt = Standard_False;
  tg1   = Tg1;
  tg2   = Tg2;
  utg12d = Tg12d.X();
  vtg12d = Tg12d.Y();
  utg22d = Tg22d.X();
  vtg22d = Tg22d.Y();
}

void Blend_Point::SetValue(const gp_Pnt& P1,
			   const gp_Pnt& P2,
			   const Standard_Real Param,
			   const Standard_Real U1,
			   const Standard_Real V1,
			   const Standard_Real U2,
			   const Standard_Real V2,
			   const Standard_Real PC1,
			   const Standard_Real PC2)
{
  pt1   = P1;
  pt2   = P2;
  prm   = Param;
  u1    = U1;
  v1    = V1;
  hass1 = Standard_True;
  u2    = U2;
  v2    = V2;
  hass2 = Standard_True;
  pc1   = PC1;
  hasc1 = Standard_True;
  pc2   = PC2;
  hasc2 = Standard_True;
  istgt = Standard_True;
}

void Blend_Point::SetValue(const gp_Pnt& P1,
			   const gp_Pnt& P2,
			   const Standard_Real Param,
			   const Standard_Real PC1,
			   const Standard_Real PC2)
{
  pt1   = P1;
  pt2   = P2;
  prm   = Param;
  hass1 = Standard_False;
  hass2 = Standard_False;
  pc1   = PC1;
  hasc1 = Standard_True;
  pc2   = PC2;
  hasc2 = Standard_True;
  istgt = Standard_True;
}
