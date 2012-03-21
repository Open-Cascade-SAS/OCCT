// Created on: 1997-11-21
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



#include <GeomFill_SectionLaw.ixx>

Standard_Boolean GeomFill_SectionLaw::D1(const Standard_Real,TColgp_Array1OfPnt&,TColgp_Array1OfVec&,TColStd_Array1OfReal&,TColStd_Array1OfReal& ) 
{
  Standard_NotImplemented::Raise("GeomFill_SectionLaw::D1");
  return 0;
}

 Standard_Boolean GeomFill_SectionLaw::D2(const Standard_Real,TColgp_Array1OfPnt& ,TColgp_Array1OfVec&,TColgp_Array1OfVec&,TColStd_Array1OfReal&,TColStd_Array1OfReal&,TColStd_Array1OfReal&) 
{
  Standard_NotImplemented::Raise("GeomFill_SectionLaw::D2");
  return 0;  
}

 Handle(Geom_BSplineSurface) GeomFill_SectionLaw::BSplineSurface() const
{
 Handle(Geom_BSplineSurface) BS;
 BS.Nullify();
 return BS;
}

 void GeomFill_SectionLaw::SetTolerance(const Standard_Real ,const Standard_Real) 
{
  //Ne fait Rien
}

 gp_Pnt GeomFill_SectionLaw::BarycentreOfSurf() const
{

  Standard_NotImplemented::Raise("GeomFill_SectionLaw::BarycentreOfSurf");
    return gp_Pnt(0.0, 0.0, 0.0);
}

 void GeomFill_SectionLaw::GetMinimalWeight(TColStd_Array1OfReal&) const
{
  Standard_NotImplemented::Raise("GeomFill_SectionLaw::GetMinimalWeight");
}

 Standard_Boolean GeomFill_SectionLaw::IsConstant(Standard_Real& Error) const
{
  Error = 0.;
  return Standard_False;
}

 Handle(Geom_Curve)  GeomFill_SectionLaw::ConstantSection() const
{
 Handle(Geom_Curve) C;
 Standard_DomainError::Raise("GeomFill_SectionLaw::ConstantSection");
 return C;
}

 Standard_Boolean GeomFill_SectionLaw::IsConicalLaw(Standard_Real& Error) const
{
  Error = 0.;
  return Standard_False;
}

 Handle(Geom_Curve) GeomFill_SectionLaw::
 CirclSection(const Standard_Real) const
{
 Handle(Geom_Curve) C;
 Standard_DomainError::Raise("GeomFill_SectionLaw::CirclSection");
 return C;
}
