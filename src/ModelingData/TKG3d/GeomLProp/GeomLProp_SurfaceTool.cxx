// Created on: 1992-08-18
// Created by: Herve LEGRAND
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

#include <Geom_Surface.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomLProp_SurfaceTool.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

void GeomLProp_SurfaceTool::Value(const occ::handle<Geom_Surface>& S,
                                  const double         U,
                                  const double         V,
                                  gp_Pnt&                     P)
{
  P = S->Value(U, V);
}

void GeomLProp_SurfaceTool::D1(const occ::handle<Geom_Surface>& S,
                               const double         U,
                               const double         V,
                               gp_Pnt&                     P,
                               gp_Vec&                     D1U,
                               gp_Vec&                     D1V)
{
  S->D1(U, V, P, D1U, D1V);
}

void GeomLProp_SurfaceTool::D2(const occ::handle<Geom_Surface>& S,
                               const double         U,
                               const double         V,
                               gp_Pnt&                     P,
                               gp_Vec&                     D1U,
                               gp_Vec&                     D1V,
                               gp_Vec&                     D2U,
                               gp_Vec&                     D2V,
                               gp_Vec&                     DUV)
{
  S->D2(U, V, P, D1U, D1V, D2U, D2V, DUV);
}

//=================================================================================================

gp_Vec GeomLProp_SurfaceTool::DN(const occ::handle<Geom_Surface>& S,
                                 const double         U,
                                 const double         V,
                                 const int      IU,
                                 const int      IV)
{
  return S->DN(U, V, IU, IV);
}

int GeomLProp_SurfaceTool::Continuity(const occ::handle<Geom_Surface>& S)
{
  GeomAbs_Shape s = S->Continuity();
  switch (s)
  {
    case GeomAbs_C0:
      return 0;
    case GeomAbs_C1:
      return 1;
    case GeomAbs_C2:
      return 2;
    case GeomAbs_C3:
      return 3;
    case GeomAbs_G1:
      return 0;
    case GeomAbs_G2:
      return 0;
    case GeomAbs_CN:
      return 3;
  };
  return 0;
}

void GeomLProp_SurfaceTool::Bounds(const occ::handle<Geom_Surface>& S,
                                   double&              U1,
                                   double&              V1,
                                   double&              U2,
                                   double&              V2)
{
  S->Bounds(U1, U2, V1, V2);
}
