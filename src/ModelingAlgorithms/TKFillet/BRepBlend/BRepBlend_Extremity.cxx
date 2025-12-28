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

#include <BRepBlend_Extremity.hxx>

#include <Adaptor2d_Curve2d.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <BRepBlend_PointOnRst.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntSurf_Transition.hxx>

BRepBlend_Extremity::BRepBlend_Extremity()
    : pt(gp_Pnt(0, 0, 0)),
      tang(gp_Vec(0, 0, 0)),
      param(0.0),
      u(0.0),
      v(0.0),
      tol(0.0),
      isvtx(false),
      hastang(false)
{
}

BRepBlend_Extremity::BRepBlend_Extremity(const gp_Pnt&       P,
                                         const double U,
                                         const double V,
                                         const double Param,
                                         const double Tol)
    : pt(P),
      tang(gp_Vec(0, 0, 0)),
      param(Param),
      u(U),
      v(V),
      tol(Tol),
      isvtx(false),
      hastang(false)
{
}

BRepBlend_Extremity::BRepBlend_Extremity(const gp_Pnt&                    P,
                                         const double              U,
                                         const double              V,
                                         const double              Param,
                                         const double              Tol,
                                         const occ::handle<Adaptor3d_HVertex>& Vtx)
    : vtx(Vtx),
      pt(P),
      tang(gp_Vec(0, 0, 0)),
      param(Param),
      u(U),
      v(V),
      tol(Tol),
      isvtx(true),
      hastang(false)
{
}

BRepBlend_Extremity::BRepBlend_Extremity(const gp_Pnt&       P,
                                         const double W,
                                         const double Param,
                                         const double Tol)
    : pt(P),
      tang(gp_Vec(0, 0, 0)),
      param(Param),
      u(W),
      v(0.0),
      tol(Tol),
      isvtx(false),
      hastang(false)
{
}

void BRepBlend_Extremity::SetValue(const gp_Pnt&       P,
                                   const double U,
                                   const double V,
                                   const double Param,
                                   const double Tol)
{
  pt    = P;
  u     = U;
  v     = V;
  param = Param;
  tol   = Tol;
  isvtx = false;
  seqpt.Clear();
}

void BRepBlend_Extremity::SetValue(const gp_Pnt&                    P,
                                   const double              U,
                                   const double              V,
                                   const double              Param,
                                   const double              Tol,
                                   const occ::handle<Adaptor3d_HVertex>& Vtx)
{
  pt    = P;
  u     = U;
  v     = V;
  param = Param;
  tol   = Tol;
  isvtx = true;
  vtx   = Vtx;
  seqpt.Clear();
}

void BRepBlend_Extremity::SetValue(const gp_Pnt&       P,
                                   const double W,
                                   const double Param,
                                   const double Tol)
{
  pt    = P;
  u     = W;
  param = Param;
  tol   = Tol;
  isvtx = false;
  seqpt.Clear();
}

void BRepBlend_Extremity::SetVertex(const occ::handle<Adaptor3d_HVertex>& V)
{
  isvtx = true;
  vtx   = V;
}

void BRepBlend_Extremity::AddArc(const occ::handle<Adaptor2d_Curve2d>& A,
                                 const double              Param,
                                 const IntSurf_Transition&        TLine,
                                 const IntSurf_Transition&        TArc)
{
  seqpt.Append(BRepBlend_PointOnRst(A, Param, TLine, TArc));
}
