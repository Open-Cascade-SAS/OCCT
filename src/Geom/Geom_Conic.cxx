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


#include <Geom_Conic.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Pnt.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Geom_Conic,Geom_Curve)

typedef Geom_Conic         Conic;
typedef gp_Ax1 Ax1;
typedef gp_Ax2 Ax2;
typedef gp_Pnt Pnt;
typedef gp_Vec Vec;



void  Geom_Conic::Reverse () {

  gp_Dir Vz = pos.Direction ();
  Vz.Reverse();
  pos.SetDirection (Vz);
}

void Geom_Conic::SetAxis (const Ax1& A1) {  pos.SetAxis (A1); }

void Geom_Conic::SetLocation (const Pnt& O) { pos.SetLocation (O); }

void Geom_Conic::SetPosition (const Ax2& A2) { pos = A2; }

Ax1 Geom_Conic::Axis () const { return pos.Axis(); }

GeomAbs_Shape Geom_Conic::Continuity () const { return GeomAbs_CN; }

Pnt Geom_Conic::Location () const { return pos.Location(); }

const gp_Ax2& Geom_Conic::Position () const { return pos; }

Ax1 Geom_Conic::XAxis () const {return Ax1(pos.Location(), pos.XDirection());}

Ax1 Geom_Conic::YAxis () const {return Ax1(pos.Location(), pos.YDirection());}

Standard_Boolean Geom_Conic::IsCN (const Standard_Integer ) const { return Standard_True; }

