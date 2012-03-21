// Created on: 1995-01-12
// Created by: Laurent BOURESCHE
// Copyright (c) 1995-1999 Matra Datavision
// Copyright (c) 1999-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.


// pmn -> 17/01/1996 added : Continuity, (Nb)Interals, D2, Trim

#include <Law_Linear.ixx>


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
