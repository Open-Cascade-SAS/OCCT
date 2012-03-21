// Created on: 1993-03-24
// Created by: JCV
// Copyright (c) 1993-1999 Matra Datavision
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




#include <Geom2d_Curve.ixx>



typedef Geom2d_Curve         Curve;
typedef Handle(Geom2d_Curve) Handle(Curve);


//=======================================================================
//function : Reversed
//purpose  : 
//=======================================================================

Handle(Geom2d_Curve) Geom2d_Curve::Reversed () const
{
  Handle(Curve) C = Handle(Curve)::DownCast(Copy());
  C->Reverse();
  return C;
}

//=======================================================================
//function : TransformedParameter
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Curve::TransformedParameter(const Standard_Real U,
						 const gp_Trsf2d& ) const
{
  return U;
}

//=======================================================================
//function : ParametricTransformation
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Curve::ParametricTransformation(const gp_Trsf2d& ) const
{
  return 1.;
}

//=======================================================================
//function : Period
//purpose  : 
//=======================================================================

Standard_Real Geom2d_Curve::Period() const
{
  Standard_NoSuchObject_Raise_if
    ( !IsPeriodic(),"Geom2d_Curve::Period");

  return ( LastParameter() - FirstParameter());
}


//=======================================================================
//function : Value
//purpose  : 
//=======================================================================

gp_Pnt2d  Geom2d_Curve::Value(const Standard_Real U)const 
{
  gp_Pnt2d P;
  D0(U,P);
  return P;
}
