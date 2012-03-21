// Created on: 1997-06-25
// Created by: Philippe MANGIN
// Copyright (c) 1997-1999 Matra Datavision
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



#include <Approx_SweepFunction.ixx>

// Standard_Boolean Approx_SweepFunction::D1(const Standard_Real Param,const Standard_Real First,const Standard_Real Last,TColgp_Array1OfPnt& Poles,TColgp_Array1OfVec& DPoles,TColgp_Array1OfPnt2d& Poles2d,TColgp_Array1OfVec2d& DPoles2d,TColStd_Array1OfReal& Weigths,TColStd_Array1OfReal& DWeigths) 
Standard_Boolean Approx_SweepFunction::D1(const Standard_Real ,const Standard_Real ,const Standard_Real ,TColgp_Array1OfPnt& ,TColgp_Array1OfVec& ,TColgp_Array1OfPnt2d& ,TColgp_Array1OfVec2d& ,TColStd_Array1OfReal& ,TColStd_Array1OfReal& ) 
{
 Standard_NotImplemented::Raise("Approx_SweepFunction::D1");
 return Standard_False;
}

// Standard_Boolean Approx_SweepFunction::D2(const Standard_Real Param,const Standard_Real First,const Standard_Real Last,TColgp_Array1OfPnt& Poles,TColgp_Array1OfVec& DPoles,TColgp_Array1OfVec& D2Poles,TColgp_Array1OfPnt2d& Poles2d,TColgp_Array1OfVec2d& DPoles2d,TColgp_Array1OfVec2d& D2Poles2d,TColStd_Array1OfReal& Weigths,TColStd_Array1OfReal& DWeigths,TColStd_Array1OfReal& D2Weigths) 
 Standard_Boolean Approx_SweepFunction::D2(const Standard_Real ,const Standard_Real ,const Standard_Real ,TColgp_Array1OfPnt& ,TColgp_Array1OfVec& ,TColgp_Array1OfVec& ,TColgp_Array1OfPnt2d& ,TColgp_Array1OfVec2d& ,TColgp_Array1OfVec2d& ,TColStd_Array1OfReal& ,TColStd_Array1OfReal& ,TColStd_Array1OfReal& ) 
{
 Standard_NotImplemented::Raise("Approx_SweepFunction::D2");
 return Standard_False;
}

// void Approx_SweepFunction::Resolution(const Standard_Integer Index,const Standard_Real Tol,Standard_Real& TolU,Standard_Real& TolV) const
 void Approx_SweepFunction::Resolution(const Standard_Integer ,const Standard_Real ,Standard_Real& ,Standard_Real& ) const
{
 Standard_NotImplemented::Raise("Approx_SweepFunction::Resolution");  
}

 gp_Pnt Approx_SweepFunction::BarycentreOfSurf() const
{
 Standard_NotImplemented::Raise("Approx_SweepFunction::BarycentreOfSurf");
 return gp_Pnt(0.,0.,0.);
}

 Standard_Real Approx_SweepFunction::MaximalSection() const
{
  Standard_NotImplemented::Raise("Approx_SweepFunction::MaximalSection()");
  return 0;
}

// void Approx_SweepFunction::GetMinimalWeight(TColStd_Array1OfReal& Weigths) const
 void Approx_SweepFunction::GetMinimalWeight(TColStd_Array1OfReal& ) const
{
 Standard_NotImplemented::Raise("Approx_SweepFunction::GetMinimalWeight");
}
