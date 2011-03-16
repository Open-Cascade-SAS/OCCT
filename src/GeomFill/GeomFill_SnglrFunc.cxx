// File:	GeomFill_SnglrFunc.cxx
// Created:	Thu Feb 26 11:21:25 1998
// Author:	Roman BORISOV
//		<rbv@ecolox.nnov.matra-dtv.fr>


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
#ifndef DEB
  GeomAbs_Shape HCS=GeomAbs_C0;
#else
  GeomAbs_Shape HCS;
#endif
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
#ifndef DEB
  GeomAbs_Shape HCS=GeomAbs_C0;
#else
  GeomAbs_Shape HCS ;
#endif
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


