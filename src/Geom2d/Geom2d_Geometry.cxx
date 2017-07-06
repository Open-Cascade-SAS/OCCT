// Created on: 1993-03-24
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Geom2d_Geometry.hxx>
#include <gp_Ax2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Trsf2d.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom2d_Geometry,Standard_Transient)

typedef Geom2d_Geometry Geometry;
typedef gp_Ax2d   Ax2d;
typedef gp_Pnt2d  Pnt2d;
typedef gp_Vec2d  Vec2d;
typedef gp_Trsf2d Trsf2d;

void Geom2d_Geometry::Mirror (const gp_Pnt2d& P) {
   
  Trsf2d T;
  T.SetMirror (P);
  Transform (T);
}



void Geom2d_Geometry::Mirror (const gp_Ax2d& A) {

  Trsf2d T;
  T.SetMirror (A);
  Transform (T);
}


void Geom2d_Geometry::Rotate (const gp_Pnt2d& P, const Standard_Real Ang) {

  Trsf2d T;
  T.SetRotation (P, Ang);
  Transform (T);
}


void Geom2d_Geometry::Scale (const gp_Pnt2d& P, const Standard_Real S) {

  Trsf2d T;
  T.SetScale (P, S);
  Transform (T);
}


void Geom2d_Geometry::Translate (const gp_Vec2d& V) {

  Trsf2d T;
  T.SetTranslation (V);
  Transform (T);
}


void Geom2d_Geometry::Translate (const gp_Pnt2d& P1, const gp_Pnt2d& P2) {

  Vec2d V (P1, P2);
  Translate (V);
}


 Handle(Geom2d_Geometry) Geom2d_Geometry::Mirrored (const gp_Pnt2d& P) const
 {
  Handle(Geom2d_Geometry) G = Copy();
  G->Mirror (P);
  return G;
}


Handle(Geom2d_Geometry) Geom2d_Geometry::Mirrored (const gp_Ax2d& A) const
{
  Handle(Geom2d_Geometry) G = Copy();
  G->Mirror (A);
  return G;
}


Handle(Geom2d_Geometry) Geom2d_Geometry::Rotated (const gp_Pnt2d& P, const Standard_Real Ang) const
{
  Handle(Geom2d_Geometry) G = Copy();
  G->Rotate (P, Ang);
  return G;
}


Handle(Geom2d_Geometry) Geom2d_Geometry::Scaled (const gp_Pnt2d& P,  const Standard_Real S) const
{
  Handle(Geom2d_Geometry) G = Copy();
  G->Scale (P, S);
  return G;
}


Handle(Geom2d_Geometry) Geom2d_Geometry::Transformed (const gp_Trsf2d& T) const
{
  Handle(Geom2d_Geometry) G = Copy();
  G->Transform (T);
  return G;
}


Handle(Geom2d_Geometry) Geom2d_Geometry::Translated (const gp_Vec2d& V) const
{
  Handle(Geom2d_Geometry) G = Copy();
  G->Translate (V);
  return G;
}


Handle(Geom2d_Geometry) Geom2d_Geometry::Translated (const gp_Pnt2d& P1, const gp_Pnt2d& P2) const
{
  Handle(Geom2d_Geometry) G = Copy();
  G->Translate (P1, P2);
  return G;
}
