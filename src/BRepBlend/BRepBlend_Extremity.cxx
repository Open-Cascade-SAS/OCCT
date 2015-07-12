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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_HVertex.hxx>
#include <BRepBlend_Extremity.hxx>
#include <BRepBlend_PointOnRst.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <IntSurf_Transition.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>

BRepBlend_Extremity::BRepBlend_Extremity (): 
       pt(gp_Pnt(0,0,0)),
       tang(gp_Vec(0,0,0)),
       param(0.0),  u(0.0), v(0.0), tol(0.0),
       isvtx(Standard_False), hastang(Standard_False)
{
}

BRepBlend_Extremity::BRepBlend_Extremity (const gp_Pnt& P,
				  const Standard_Real U,
				  const Standard_Real V,
				  const Standard_Real Param,
				  const Standard_Real Tol) :
       pt(P),
       tang(gp_Vec(0,0,0)),
       param(Param),u(U),v(V),tol(Tol),isvtx(Standard_False),
       hastang(Standard_False)
{
}


BRepBlend_Extremity::BRepBlend_Extremity (const gp_Pnt& P,
				  const Standard_Real U,
				  const Standard_Real V,
				  const Standard_Real Param,
				  const Standard_Real Tol,
				  const Handle(Adaptor3d_HVertex)& Vtx) :
       vtx(Vtx),pt(P),
       tang(gp_Vec(0,0,0)),
       param(Param),u(U),v(V),tol(Tol),isvtx(Standard_True),
       hastang(Standard_False)
{}


BRepBlend_Extremity::BRepBlend_Extremity (const gp_Pnt& P,
				  const Standard_Real W,
				  const Standard_Real Param,
				  const Standard_Real Tol) :
       pt(P),
       tang(gp_Vec(0,0,0)),
       param(Param),u(W),tol(Tol),isvtx(Standard_False),
       hastang(Standard_False)
{}


void BRepBlend_Extremity::SetValue (const gp_Pnt& P,
				const Standard_Real U,
				const Standard_Real V,
				const Standard_Real Param,
				const Standard_Real Tol)
{
  pt    = P;
  u     = U;
  v     = V;
  param = Param;
  tol   = Tol;
  isvtx = Standard_False;
  seqpt.Clear();
}


void BRepBlend_Extremity::SetValue (const gp_Pnt& P,
				const Standard_Real U,
				const Standard_Real V,
				const Standard_Real Param,
				const Standard_Real Tol,
				const Handle(Adaptor3d_HVertex)& Vtx)
{
  pt    = P;
  u     = U;
  v     = V;
  param = Param;
  tol   = Tol;
  isvtx = Standard_True;
  vtx   = Vtx;
  seqpt.Clear();
}

void BRepBlend_Extremity::SetValue (const gp_Pnt& P,
				const Standard_Real W,
				const Standard_Real Param,
				const Standard_Real Tol)
{
  pt    = P;
  u     = W;
  param = Param;
  tol   = Tol;
  isvtx = Standard_False;
  seqpt.Clear();
}


void BRepBlend_Extremity::SetVertex (const Handle(Adaptor3d_HVertex)& V)
{
  isvtx = Standard_True;
  vtx   = V;
}

void BRepBlend_Extremity::AddArc (const Handle(Adaptor2d_HCurve2d)& A,
			      const Standard_Real Param,
			      const IntSurf_Transition& TLine,
			      const IntSurf_Transition& TArc)
{
  seqpt.Append(BRepBlend_PointOnRst(A,Param,TLine,TArc));
}

