// Created on: 1996-03-29
// Created by: Laurent BOURESCHE
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Law_Constant.ixx>

//=======================================================================
//function : Law_Constant
//purpose  : 
//=======================================================================

Law_Constant::Law_Constant()
{
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void Law_Constant::Set(const Standard_Real Radius, 
		       const Standard_Real PFirst, 
		       const Standard_Real PLast)
{
  radius = Radius;
  first  = PFirst;
  last   = PLast;
}
//=======================================================================
//function : Continuity
//purpose  : 
//=======================================================================
GeomAbs_Shape Law_Constant::Continuity() const 
{
  return GeomAbs_CN;
}

//=======================================================================
//function : NbIntervals
//purpose  : 
//=======================================================================
//Standard_Integer Law_Constant::NbIntervals(const GeomAbs_Shape S) const 
Standard_Integer Law_Constant::NbIntervals(const GeomAbs_Shape ) const 
{
   return 1;
}

//=======================================================================
//function : Intervals
//purpose  : 
//=======================================================================
void Law_Constant::Intervals(TColStd_Array1OfReal& T,
//                             const GeomAbs_Shape S) const 
                             const GeomAbs_Shape ) const 
{
  T.SetValue(T.Lower(), first);
  T.SetValue(T.Upper(), last);
}

//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

Standard_Real Law_Constant::Value(const Standard_Real)
{
  return radius;
}

//=======================================================================
//function : D1
//purpose  : 
//=======================================================================

void Law_Constant::D1(const Standard_Real, 
		      Standard_Real& F, 
		      Standard_Real& D)
{
  F = radius;
  D = 0.;
}

//=======================================================================
//function : D2
//purpose  : 
//=======================================================================

void Law_Constant::D2(const Standard_Real, 
		      Standard_Real& F, 
		      Standard_Real& D,
		      Standard_Real& D2)
{
  F = radius;
  D = D2 = 0.;
}

//=======================================================================
//function : Trim
//purpose  : 
//=======================================================================

Handle(Law_Function) Law_Constant::Trim(const Standard_Real PFirst, 
				      const Standard_Real PLast, 
//				      const Standard_Real Tol) const 
				      const Standard_Real ) const 
{
  Handle(Law_Constant) l = new (Law_Constant)();
  l->Set(radius, PFirst, PLast);
  return l;
}

//=======================================================================
//function : Bounds
//purpose  : 
//=======================================================================

void Law_Constant::Bounds(Standard_Real& PFirst, 
			  Standard_Real& PLast)
{
  PFirst = first;
  PLast  = last;
}
