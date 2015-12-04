// Created on: 1995-01-12
// Created by: Laurent BOURESCHE
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

// pmn -> 17/01/1996 added : Continuity, (Nb)Interals, D2, Trim

#include <Law_Function.hxx>
#include <Law_Linear.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Law_Linear,Law_Function)

Law_Linear::Law_Linear () {}


void Law_Linear::Set (const Standard_Real Pdeb,
		      const Standard_Real Valdeb,
		      const Standard_Real Pfin,
		      const Standard_Real Valfin)
{
  pdeb = Pdeb;
  pfin = Pfin;
  valdeb = Valdeb;
  valfin = Valfin;
}

//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================
GeomAbs_Shape Law_Linear::Continuity() const 
{
  return GeomAbs_CN;
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================
//Standard_Integer Law_Linear::NbIntervals(const GeomAbs_Shape S) const 
Standard_Integer Law_Linear::NbIntervals(const GeomAbs_Shape ) const 
{
   return 1;
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================
void Law_Linear::Intervals(TColStd_Array1OfReal& T,
//                           const GeomAbs_Shape S) const 
                           const GeomAbs_Shape ) const 
{
  T.SetValue(T.Lower(), pdeb);
  T.SetValue(T.Upper(), pfin);
}

Standard_Real Law_Linear::Value(const Standard_Real X)
{
  return ((X-pdeb)*valfin + (pfin-X)*valdeb)/(pfin-pdeb);
}

void Law_Linear::D1(const Standard_Real X,
		    Standard_Real& F,
		    Standard_Real& D)
{
  F = ((X-pdeb)*valfin + (pfin-X)*valdeb)/(pfin-pdeb);
  D = (valfin-valdeb)/(pfin-pdeb);
}
void Law_Linear::D2(const Standard_Real X,
		    Standard_Real& F,
		    Standard_Real& D,
		    Standard_Real& D2)
{
  F = ((X-pdeb)*valfin + (pfin-X)*valdeb)/(pfin-pdeb);
  D = (valfin-valdeb)/(pfin-pdeb);
  D2 = 0;
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Law_Function) Law_Linear::Trim(const Standard_Real PFirst, 
				      const Standard_Real PLast, 
//				      const Standard_Real Tol) const 
				      const Standard_Real ) const 
{
  Handle(Law_Linear) l = new (Law_Linear)();
  Standard_Real Vdeb, Vfin;
  Vdeb = (( PFirst-pdeb)*valfin + (pfin-PFirst)*valdeb)/(pfin-pdeb);
  Vfin = (( PLast-pdeb)*valfin + (pfin-PLast)*valdeb)/(pfin-pdeb);
  l->Set(PFirst, Vdeb, PLast, Vfin);

  return l;
}


void Law_Linear::Bounds(Standard_Real& PFirst,
			Standard_Real& PLast)
{
  PFirst = pdeb;
  PLast  = pfin;
}
