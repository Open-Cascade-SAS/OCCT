// Created on: 1993-03-10
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

//JCV 09/07/92 portage sur C1

#include <Geom_Geometry.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Geometry,Standard_Transient)

typedef Geom_Geometry         Geometry;
typedef gp_Pnt                Pnt;
typedef gp_Vec                Vec;
typedef gp_Ax1                Ax1;
typedef gp_Ax2                Ax2;
typedef gp_Trsf               Trsf;

Handle(Geom_Geometry) Geom_Geometry::Copy() const {

   Handle(Geom_Geometry) G;
   throw Standard_ConstructionError();
}


void Geom_Geometry::Mirror (const gp_Pnt& P) {
   
  Trsf T;
  T.SetMirror (P);
  Transform (T);
}



void Geom_Geometry::Mirror (const gp_Ax1& A1) {

  Trsf T;
  T.SetMirror (A1);
  Transform (T);
}


void Geom_Geometry::Mirror (const gp_Ax2& A2) {

  Trsf T;
  T.SetMirror (A2);
  Transform (T);
}


void Geom_Geometry::Rotate (const gp_Ax1& A1, const Standard_Real Ang) {

  Trsf T;
  T.SetRotation (A1, Ang);
  Transform (T);
}


void Geom_Geometry::Scale (const gp_Pnt& P, const Standard_Real S) {

  Trsf T;
  T.SetScale (P, S);
  Transform (T);
}


void Geom_Geometry::Translate (const gp_Vec& V) {

  Trsf T;
  T.SetTranslation (V);
  Transform (T);
}


void Geom_Geometry::Translate (const gp_Pnt& P1, const gp_Pnt& P2) {

  Vec V (P1, P2);
  Translate (V);
}


Handle(Geom_Geometry) Geom_Geometry::Mirrored (const gp_Pnt& P) const
{
  Handle(Geom_Geometry) G  = Copy();
  G->Mirror (P);
  return G;
}


Handle(Geom_Geometry) Geom_Geometry::Mirrored (const gp_Ax1& A1) const
{
  Handle(Geom_Geometry) G = Copy();
  G->Mirror (A1);
  return G;
}


Handle(Geom_Geometry) Geom_Geometry::Mirrored (const gp_Ax2& A2) const
{
  Handle(Geom_Geometry) G = Copy();
  G->Mirror (A2);
  return G;
}



Handle(Geom_Geometry) Geom_Geometry::Rotated (const gp_Ax1& A1, const Standard_Real Ang) const
{
  Handle(Geom_Geometry) G = Copy();
  G->Rotate (A1, Ang);
  return G;
}



Handle(Geom_Geometry) Geom_Geometry::Scaled (const gp_Pnt& P, const Standard_Real S) const
{
  Handle(Geom_Geometry) G = Copy();
  G->Scale (P, S);
  return G;
}



Handle(Geom_Geometry) Geom_Geometry::Transformed (const gp_Trsf& T) const
{
  Handle(Geom_Geometry) G = Copy();
  G->Transform (T);
  return G;
}



Handle(Geom_Geometry) Geom_Geometry::Translated (const gp_Vec& V) const
{
  Handle(Geom_Geometry) G = Copy();
  G->Translate (V);
  return G;
}


Handle(Geom_Geometry) Geom_Geometry::Translated (const gp_Pnt& P1, const gp_Pnt& P2) const
{
   Handle(Geom_Geometry) G = Copy();
   G->Translate (P1, P2);
   return G;
}


