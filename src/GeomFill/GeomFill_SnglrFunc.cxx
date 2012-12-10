// Created on: 1998-02-26
// Created by: Roman BORISOV
// Copyright (c) 1998-1999 Matra Datavision
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



#include <GeomFill_SnglrFunc.ixx>
#include <Precision.hxx>

GeomFill_SnglrFunc::GeomFill_SnglrFunc(const Handle(Adaptor3d_HCurve)& HC) : 
       myHCurve(HC), ratio(1)
{
}

void GeomFill_SnglrFunc::SetRatio(const Standard_Real Ratio)
{
  ratio = Ratio;
}

 Standard_Real GeomFill_SnglrFunc::FirstParameter() const
{
  return myHCurve->FirstParameter();
}

 Standard_Real GeomFill_SnglrFunc::LastParameter() const
{
  return myHCurve->LastParameter();
}

 Standard_Integer GeomFill_SnglrFunc::NbIntervals(const GeomAbs_Shape S) 
{
  GeomAbs_Shape HCS=GeomAbs_C0;
  switch(S) {
  case GeomAbs_C0: HCS = GeomAbs_C2; break;
  case GeomAbs_C1: HCS = GeomAbs_C3; break;
  case GeomAbs_C2: HCS = GeomAbs_CN; break;
  default: Standard_DomainError::Raise();
  }
  return myHCurve->NbIntervals(HCS);
}

 void GeomFill_SnglrFunc::Intervals(TColStd_Array1OfReal& T,const GeomAbs_Shape S) 
{
  GeomAbs_Shape HCS=GeomAbs_C0;
  switch(S) {
  case GeomAbs_C0: HCS = GeomAbs_C2; break;
  case GeomAbs_C1: HCS = GeomAbs_C3; break;
  case GeomAbs_C2: HCS = GeomAbs_CN; break;
  default: Standard_DomainError::Raise();
  }
  myHCurve->Intervals(T, HCS);
}

 Standard_Boolean GeomFill_SnglrFunc::IsPeriodic() const
{
  return myHCurve->IsPeriodic();
}

 Standard_Real GeomFill_SnglrFunc::Period() const
{
  return myHCurve->Period();
}


 gp_Pnt GeomFill_SnglrFunc::Value(const Standard_Real U) const
{
  gp_Pnt C;
  gp_Vec DC, D2C;
  myHCurve->D2(U, C, DC, D2C);
  DC *= ratio;
  return gp_Pnt(DC.Crossed(D2C).XYZ());
}

 void GeomFill_SnglrFunc::D0(const Standard_Real U,gp_Pnt& P) const
{
  gp_Pnt C;
  gp_Vec DC, D2C;
  myHCurve->D2(U, C, DC, D2C);
  DC *= ratio;
  P = gp_Pnt(DC.Crossed(D2C).XYZ());
}

 void GeomFill_SnglrFunc::D1(const Standard_Real U,gp_Pnt& P,gp_Vec& V) const
{
  gp_Pnt C;
  gp_Vec DC, D2C, D3C;
  myHCurve->D3(U, C, DC, D2C, D3C);
  DC *= ratio;
  P = gp_Pnt(DC.Crossed(D2C).XYZ());
  V = DC.Crossed(D3C);
}

 void GeomFill_SnglrFunc::D2(const Standard_Real U,gp_Pnt& P,gp_Vec& V1,gp_Vec& V2) const
{
  gp_Pnt C;
  gp_Vec DC, D2C, D3C, D4C;
  myHCurve->D3(U, C, DC, D2C, D3C);
  P = gp_Pnt(DC.Crossed(D2C).XYZ());
  V1 = DC.Crossed(D3C);
  D4C = myHCurve->DN(U, 4);
  V2 = D2C.Crossed(D3C) + DC.Crossed(D4C);

  P.ChangeCoord() *= ratio;
  V1 *= ratio;
  V2 *= ratio;
}

 Standard_Real GeomFill_SnglrFunc::Resolution(const Standard_Real R3D) const
{
  return Precision::Parametric(R3D);
}

 GeomAbs_CurveType GeomFill_SnglrFunc::GetType() const
{
  return GeomAbs_OtherCurve;
}


