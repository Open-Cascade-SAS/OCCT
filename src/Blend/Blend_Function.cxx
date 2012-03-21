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

#include <Blend_Function.ixx>

Standard_Integer Blend_Function::NbVariables () const
{
  return 4;
}

const gp_Pnt& Blend_Function::Pnt1() const 
{
  return PointOnS1();
}

const gp_Pnt& Blend_Function::Pnt2() const 
{
  return PointOnS2(); 
}

Standard_Boolean Blend_Function::TwistOnS1() const
{
  return Standard_False;
}

Standard_Boolean Blend_Function::TwistOnS2() const
{
  return Standard_False;
}

Standard_Boolean Blend_Function::Section(const Blend_Point& P, 
					 TColgp_Array1OfPnt& Poles, 
					 TColgp_Array1OfVec& DPoles, 
					 TColgp_Array1OfVec& D2Poles, 
					 TColgp_Array1OfPnt2d& Poles2d, 
					 TColgp_Array1OfVec2d& DPoles2d, 
					 TColgp_Array1OfVec2d& D2Poles2d, 
					 TColStd_Array1OfReal& Weigths, 
					 TColStd_Array1OfReal& DWeigths,
					 TColStd_Array1OfReal& D2Weigths)
{
  return Standard_False;
}
