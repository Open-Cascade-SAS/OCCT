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

Blend_Point::Blend_Point()
    : istgt(true)
{
}

Blend_Point::Blend_Point(const gp_Pnt&   P1,
                         const gp_Pnt&   P2,
                         const double    Param,
                         const double    U1,
                         const double    V1,
                         const double    U2,
                         const double    V2,
                         const gp_Vec&   Tg1,
                         const gp_Vec&   Tg2,
                         const gp_Vec2d& Tg12d,
                         const gp_Vec2d& Tg22d)
    : pt1(P1),
      pt2(P2),
      tg1(Tg1),
      tg2(Tg2),
      prm(Param),
      u1(U1),
      v1(V1),
      u2(U2),
      v2(V2),
      utg12d(Tg12d.X()),
      vtg12d(Tg12d.Y()),
      utg22d(Tg22d.X()),
      vtg22d(Tg22d.Y()),
      hass1(true),
      hass2(true),
      hasc1(false),
      hasc2(false),
      istgt(false)
{
}

Blend_Point::Blend_Point(const gp_Pnt& P1,
                         const gp_Pnt& P2,
                         const double  Param,
                         const double  U1,
                         const double  V1,
                         const double  U2,
                         const double  V2)
    : pt1(P1),
      pt2(P2),
      prm(Param),
      u1(U1),
      v1(V1),
      u2(U2),
      v2(V2),
      hass1(true),
      hass2(true),
      hasc1(false),
      hasc2(false),
      istgt(true)
{
}

void Blend_Point::SetValue(const gp_Pnt&   P1,
                           const gp_Pnt&   P2,
                           const double    Param,
                           const double    U1,
                           const double    V1,
                           const double    U2,
                           const double    V2,
                           const gp_Vec&   Tg1,
                           const gp_Vec&   Tg2,
                           const gp_Vec2d& Tg12d,
                           const gp_Vec2d& Tg22d)
{
  pt1    = P1;
  pt2    = P2;
  prm    = Param;
  u1     = U1;
  v1     = V1;
  hass1  = true;
  u2     = U2;
  v2     = V2;
  hass2  = true;
  hasc1  = false;
  hasc2  = false;
  istgt  = false;
  tg1    = Tg1;
  tg2    = Tg2;
  utg12d = Tg12d.X();
  vtg12d = Tg12d.Y();
  utg22d = Tg22d.X();
  vtg22d = Tg22d.Y();
}

void Blend_Point::SetValue(const gp_Pnt& P1,
                           const gp_Pnt& P2,
                           const double  Param,
                           const double  U1,
                           const double  V1,
                           const double  U2,
                           const double  V2)
{
  pt1   = P1;
  pt2   = P2;
  prm   = Param;
  u1    = U1;
  v1    = V1;
  hass1 = true;
  u2    = U2;
  v2    = V2;
  hass2 = true;
  hasc1 = false;
  hasc2 = false;
  istgt = true;
}

Blend_Point::Blend_Point(const gp_Pnt&   Ps,
                         const gp_Pnt&   Pc,
                         const double    Param,
                         const double    U,
                         const double    V,
                         const double    W,
                         const gp_Vec&   Tgs,
                         const gp_Vec&   Tgc,
                         const gp_Vec2d& Tg2d)
    : pt1(Ps),
      pt2(Pc),
      tg1(Tgs),
      tg2(Tgc),
      prm(Param),
      u1(U),
      v1(V),
      pc2(W),
      utg12d(Tg2d.X()),
      vtg12d(Tg2d.Y()),
      hass1(true),
      hass2(false),
      hasc1(false),
      hasc2(true),
      istgt(false)
{
}

Blend_Point::Blend_Point(const gp_Pnt& Ps,
                         const gp_Pnt& Pc,
                         const double  Param,
                         const double  U,
                         const double  V,
                         const double  W)
    : pt1(Ps),
      pt2(Pc),
      prm(Param),
      u1(U),
      v1(V),
      pc2(W),
      hass1(true),
      hass2(false),
      hasc1(false),
      hasc2(true),
      istgt(true)
{
}

void Blend_Point::SetValue(const gp_Pnt&   Ps,
                           const gp_Pnt&   Pc,
                           const double    Param,
                           const double    U,
                           const double    V,
                           const double    W,
                           const gp_Vec&   Tgs,
                           const gp_Vec&   Tgc,
                           const gp_Vec2d& Tg2d)
{
  pt1    = Ps;
  pt2    = Pc;
  prm    = Param;
  u1     = U;
  v1     = V;
  hass1  = true;
  hass2  = false;
  hasc1  = false;
  pc2    = W;
  hasc2  = true;
  istgt  = false;
  tg1    = Tgs;
  tg2    = Tgc;
  utg12d = Tg2d.X();
  vtg12d = Tg2d.Y();
}

void Blend_Point::SetValue(const gp_Pnt& Ps,
                           const gp_Pnt& Pc,
                           const double  Param,
                           const double  U,
                           const double  V,
                           const double  W)
{
  pt1   = Ps;
  pt2   = Pc;
  prm   = Param;
  u1    = U;
  v1    = V;
  hass1 = true;
  hass2 = false;
  hasc1 = false;
  pc2   = W;
  hasc2 = true;
  istgt = true;
}

Blend_Point::Blend_Point(const gp_Pnt&   P1,
                         const gp_Pnt&   P2,
                         const double    Param,
                         const double    U1,
                         const double    V1,
                         const double    U2,
                         const double    V2,
                         const double    PC,
                         const gp_Vec&   Tg1,
                         const gp_Vec&   Tg2,
                         const gp_Vec2d& Tg12d,
                         const gp_Vec2d& Tg22d)
    : pt1(P1),
      pt2(P2),
      tg1(Tg1),
      tg2(Tg2),
      prm(Param),
      u1(U1),
      v1(V1),
      u2(U2),
      v2(V2),
      pc2(PC),
      utg12d(Tg12d.X()),
      vtg12d(Tg12d.Y()),
      utg22d(Tg22d.X()),
      vtg22d(Tg22d.Y()),
      hass1(true),
      hass2(true),
      hasc1(false),
      hasc2(true),
      istgt(false)
{
}

Blend_Point::Blend_Point(const gp_Pnt& P1,
                         const gp_Pnt& P2,
                         const double  Param,
                         const double  U1,
                         const double  V1,
                         const double  U2,
                         const double  V2,
                         const double  PC)
    : pt1(P1),
      pt2(P2),
      prm(Param),
      u1(U1),
      v1(V1),
      u2(U2),
      v2(V2),
      pc2(PC),
      hass1(true),
      hass2(true),
      hasc1(false),
      hasc2(true),
      istgt(true)
{
}

void Blend_Point::SetValue(const gp_Pnt&   P1,
                           const gp_Pnt&   P2,
                           const double    Param,
                           const double    U1,
                           const double    V1,
                           const double    U2,
                           const double    V2,
                           const double    PC,
                           const gp_Vec&   Tg1,
                           const gp_Vec&   Tg2,
                           const gp_Vec2d& Tg12d,
                           const gp_Vec2d& Tg22d)
{
  pt1    = P1;
  pt2    = P2;
  prm    = Param;
  u1     = U1;
  v1     = V1;
  hass1  = true;
  u2     = U2;
  v2     = V2;
  hass2  = true;
  hasc1  = false;
  pc2    = PC;
  hasc2  = true;
  istgt  = false;
  tg1    = Tg1;
  tg2    = Tg2;
  utg12d = Tg12d.X();
  vtg12d = Tg12d.Y();
  utg22d = Tg22d.X();
  vtg22d = Tg22d.Y();
}

void Blend_Point::SetValue(const gp_Pnt& P1,
                           const gp_Pnt& P2,
                           const double  Param,
                           const double  U1,
                           const double  V1,
                           const double  U2,
                           const double  V2,
                           const double  PC)
{
  pt1   = P1;
  pt2   = P2;
  prm   = Param;
  u1    = U1;
  v1    = V1;
  hass1 = true;
  u2    = U2;
  v2    = V2;
  hass2 = true;
  hasc1 = false;
  pc2   = PC;
  hasc2 = true;
  istgt = true;
}

Blend_Point::Blend_Point(const gp_Pnt&   P1,
                         const gp_Pnt&   P2,
                         const double    Param,
                         const double    U1,
                         const double    V1,
                         const double    U2,
                         const double    V2,
                         const double    PC1,
                         const double    PC2,
                         const gp_Vec&   Tg1,
                         const gp_Vec&   Tg2,
                         const gp_Vec2d& Tg12d,
                         const gp_Vec2d& Tg22d)
    : pt1(P1),
      pt2(P2),
      tg1(Tg1),
      tg2(Tg2),
      prm(Param),
      u1(U1),
      v1(V1),
      u2(U2),
      v2(V2),
      pc1(PC1),
      pc2(PC2),
      utg12d(Tg12d.X()),
      vtg12d(Tg12d.Y()),
      utg22d(Tg22d.X()),
      vtg22d(Tg22d.Y()),
      hass1(true),
      hass2(true),
      hasc1(true),
      hasc2(true),
      istgt(false)
{
}

Blend_Point::Blend_Point(const gp_Pnt& P1,
                         const gp_Pnt& P2,
                         const double  Param,
                         const double  U1,
                         const double  V1,
                         const double  U2,
                         const double  V2,
                         const double  PC1,
                         const double  PC2)
    : pt1(P1),
      pt2(P2),
      prm(Param),
      u1(U1),
      v1(V1),
      u2(U2),
      v2(V2),
      pc1(PC1),
      pc2(PC2),
      hass1(true),
      hass2(true),
      hasc1(true),
      hasc2(true),
      istgt(true)
{
}

void Blend_Point::SetValue(const gp_Pnt&   P1,
                           const gp_Pnt&   P2,
                           const double    Param,
                           const double    U1,
                           const double    V1,
                           const double    U2,
                           const double    V2,
                           const double    PC1,
                           const double    PC2,
                           const gp_Vec&   Tg1,
                           const gp_Vec&   Tg2,
                           const gp_Vec2d& Tg12d,
                           const gp_Vec2d& Tg22d)
{
  pt1    = P1;
  pt2    = P2;
  prm    = Param;
  u1     = U1;
  v1     = V1;
  hass1  = true;
  u2     = U2;
  v2     = V2;
  hass2  = true;
  pc1    = PC1;
  hasc1  = true;
  pc2    = PC2;
  hasc2  = true;
  istgt  = false;
  tg1    = Tg1;
  tg2    = Tg2;
  utg12d = Tg12d.X();
  vtg12d = Tg12d.Y();
  utg22d = Tg22d.X();
  vtg22d = Tg22d.Y();
}

void Blend_Point::SetValue(const gp_Pnt& P1,
                           const gp_Pnt& P2,
                           const double  Param,
                           const double  U1,
                           const double  V1,
                           const double  U2,
                           const double  V2,
                           const double  PC1,
                           const double  PC2)
{
  pt1   = P1;
  pt2   = P2;
  prm   = Param;
  u1    = U1;
  v1    = V1;
  hass1 = true;
  u2    = U2;
  v2    = V2;
  hass2 = true;
  pc1   = PC1;
  hasc1 = true;
  pc2   = PC2;
  hasc2 = true;
  istgt = true;
}

void Blend_Point::SetValue(const gp_Pnt& P1,
                           const gp_Pnt& P2,
                           const double  Param,
                           const double  PC1,
                           const double  PC2)
{
  pt1   = P1;
  pt2   = P2;
  prm   = Param;
  hass1 = false;
  hass2 = false;
  pc1   = PC1;
  hasc1 = true;
  pc2   = PC2;
  hasc2 = true;
  istgt = true;
}
