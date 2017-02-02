// Created on: 1998-03-04
// Created by: Roman BORISOV
// Copyright (c) 1998-1999 Matra Datavision
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


#include <Adaptor2d_HCurve2d.hxx>
#include <Adaptor3d_CurveOnSurface.hxx>
#include <Adaptor3d_CurveOnSurfacePtr.hxx>
#include <Adaptor3d_HCurve.hxx>
#include <Adaptor3d_HCurveOnSurface.hxx>
#include <Adaptor3d_HSurface.hxx>
#include <CSLib.hxx>
#include <Geom_UndefinedValue.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomFill_Darboux.hxx>
#include <GeomFill_TrihedronLaw.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>
#include <Standard_ConstructionError.hxx>
#include <Standard_OutOfRange.hxx>
#include <Standard_Type.hxx>
#include <TColgp_Array2OfVec.hxx>

IMPLEMENT_STANDARD_RTTIEXT(GeomFill_Darboux,GeomFill_TrihedronLaw)

//=======================================================================
//function : FDeriv
//purpose  : computes (F/|F|)'
//=======================================================================
static gp_Vec FDeriv(const gp_Vec& F, const gp_Vec& DF)
{
  Standard_Real Norma = F.Magnitude();
  gp_Vec Result = (DF - F*(F*DF)/(Norma*Norma))/Norma;
  return Result;
}

//=======================================================================
//function : DDeriv
//purpose  : computes (F/|F|)''
//=======================================================================
static gp_Vec DDeriv(const gp_Vec& F, const gp_Vec& DF, const gp_Vec& D2F)
{
  Standard_Real Norma = F.Magnitude();
  gp_Vec Result = (D2F - 2*DF*(F*DF)/(Norma*Norma))/Norma - 
     F*((DF.SquareMagnitude() + F*D2F 
        - 3*(F*DF)*(F*DF)/(Norma*Norma))/(Norma*Norma*Norma));
  return Result;
}

//=======================================================================
//function : NormalD0
//purpose  : computes Normal to Surface
//=======================================================================
static void NormalD0(const Standard_Real U, const Standard_Real V, const Handle(Adaptor3d_HSurface)& Surf, gp_Dir& Normal, Standard_Integer& OrderU, Standard_Integer& OrderV)
{
//  gp_Vec D1U,D1V,D2U,D2V,DUV;
  gp_Vec D1U,D1V;
  GeomAbs_Shape Cont = (Surf->Surface().UContinuity() < Surf->Surface().VContinuity()) ? 
    (Surf->Surface().UContinuity()) : (Surf->Surface().VContinuity());
  OrderU = OrderV = 0;
#ifdef CHECK  
  if (Cont == GeomAbs_C0) throw Geom_UndefinedValue();
#endif
  gp_Pnt P;
  Surf->D1(U, V, P, D1U, D1V);
  Standard_Real MagTol=0.000000001;
  CSLib_NormalStatus NStatus;
  CSLib::Normal(D1U, D1V, MagTol, NStatus, Normal);

  if (NStatus != CSLib_Defined) {
    if (Cont==GeomAbs_C0 || 
        Cont==GeomAbs_C1) {
      throw Geom_UndefinedValue();
      }
    Standard_Integer MaxOrder=3;
    TColgp_Array2OfVec DerNUV(0,MaxOrder,0,MaxOrder);
    TColgp_Array2OfVec DerSurf(0,MaxOrder+1,0,MaxOrder+1);
    Standard_Integer i,j;//OrderU,OrderV;
    Standard_Real Umin,Umax,Vmin,Vmax;
    Umin = Surf->Surface().FirstUParameter();
    Umax = Surf->Surface().LastUParameter();
    Vmin = Surf->Surface().FirstVParameter();
    Vmax = Surf->Surface().LastVParameter();
    for(i=1;i<=MaxOrder+1;i++){
      DerSurf.SetValue(i,0,Surf->DN(U,V,i,0));
    }
    
    for(i=0;i<=MaxOrder+1;i++)
      for(j=1;j<=MaxOrder+1;j++){
        DerSurf.SetValue(i,j,Surf->DN(U,V,i,j));
      }    
    
    for(i=0;i<=MaxOrder;i++)
      for(j=0;j<=MaxOrder;j++){
        DerNUV.SetValue(i,j,CSLib::DNNUV(i,j,DerSurf));
      }
    
    CSLib::Normal(MaxOrder,DerNUV,MagTol,U,V,Umin,Umax,Vmin,Vmax,
                  NStatus,Normal,OrderU,OrderV);
    
    if (NStatus != CSLib_Defined) {
#ifdef OCCT_DEBUG
      cout << U << ", " << V<< endl;
      for(i=0;i<=MaxOrder;i++)
	for(j=0;j<=MaxOrder;j++){
	  cout <<"("<<i <<"," << j << ") = "<< DerSurf(i,j).X() <<","
	       << DerSurf(i,j).Y() <<"," << DerSurf(i,j).Z() << endl;
	}
#endif
      throw Geom_UndefinedValue();
    }
  }
}

//=======================================================================
//function : NormalD1
//purpose  : computes Normal to Surface and its first derivative
//=======================================================================
void NormalD1 (const Standard_Real U, const Standard_Real V, 
               const Handle(Adaptor3d_HSurface)& Surf, gp_Dir& Normal, 
               gp_Vec& D1UNormal, gp_Vec& D1VNormal)
{                                    
#ifdef CHECK  
  GeomAbs_Shape Cont = (Surf->Surface().UContinuity() < Surf->Surface().VContinuity()) ? 
    (Surf->Surface().UContinuity()) : (Surf->Surface().VContinuity());
  if (Cont==GeomAbs_C0 || 
      Cont==GeomAbs_C1) { 
    throw Geom_UndefinedDerivative();
  }
#endif
  gp_Vec d2u, d2v, d2uv;
  gp_Pnt P;
  Surf->D2(U, V, P, D1UNormal, D1VNormal,  d2u, d2v, d2uv);
  Standard_Real MagTol=0.000000001;
  CSLib_NormalStatus NStatus;
  CSLib::Normal (D1UNormal, D1VNormal, MagTol, NStatus, Normal);
  Standard_Integer MaxOrder;
  if (NStatus == CSLib_Defined) 
    MaxOrder=0;
  else 
    MaxOrder=3;
  Standard_Integer OrderU,OrderV;
  TColgp_Array2OfVec DerNUV(0,MaxOrder+1,0,MaxOrder+1);
  TColgp_Array2OfVec DerSurf(0,MaxOrder+2,0,MaxOrder+2);
  Standard_Integer i,j;
  Standard_Real Umin,Umax,Vmin,Vmax;
  Umin = Surf->Surface().FirstUParameter();
  Umax = Surf->Surface().LastUParameter();
  Vmin = Surf->Surface().FirstVParameter();
  Vmax = Surf->Surface().LastVParameter();
  
  DerSurf.SetValue(1, 0, D1UNormal);
    DerSurf.SetValue(0, 1, D1VNormal);
  DerSurf.SetValue(1, 1, d2uv);
  DerSurf.SetValue(2, 0, d2u);
  DerSurf.SetValue(0, 2, d2v);
  for(i=0;i<=MaxOrder+1;i++)
    for(j=i; j<=MaxOrder+2; j++ )
      if (i+j > 2) {
        DerSurf.SetValue(i,j,Surf->DN(U,V,i,j));
        if (i!=j) DerSurf.SetValue(j,i,Surf->DN(U,V,j,i));
      }
  
  for(i=0;i<=MaxOrder+1;i++)
    for(j=0;j<=MaxOrder+1;j++){
      DerNUV.SetValue(i,j,CSLib::DNNUV(i,j,DerSurf));
      }
  
  CSLib::Normal(MaxOrder,DerNUV,MagTol,U,V,Umin,Umax,Vmin,Vmax,
                NStatus,Normal,OrderU,OrderV);
  if (NStatus != CSLib_Defined) throw Geom_UndefinedValue();

  D1UNormal = CSLib::DNNormal(1,0,DerNUV,OrderU,OrderV);
  D1VNormal = CSLib::DNNormal(0,1,DerNUV,OrderU,OrderV);
}

//=======================================================================
//function : NormalD2
//purpose  : computes Normal to Surface and its first and second derivatives
//=======================================================================
void NormalD2 (const Standard_Real U, const Standard_Real V, 
               const Handle(Adaptor3d_HSurface)& Surf, gp_Dir& Normal, 
               gp_Vec& D1UNormal, gp_Vec& D1VNormal,
               gp_Vec& D2UNormal, gp_Vec& D2VNormal, gp_Vec& D2UVNormal)
{
#ifdef CHECK  
  GeomAbs_Shape Cont = (Surf->Surface().UContinuity() < Surf->Surface().VContinuity()) ? 
    (Surf->Surface().UContinuity()) : (Surf->Surface().VContinuity());
     if (Cont == GeomAbs_C0 || Continuity == GeomAbs_C1 ||
         Cont == GeomAbs_C2) { throw Geom_UndefinedDerivative(); }
#endif
  gp_Vec d3u, d3uuv, d3uvv, d3v;
  gp_Pnt P;
  Surf->D3(U, V, P, D1UNormal, D1VNormal, 
           D2UNormal, D2VNormal, D2UVNormal,
           d3u,d3v, d3uuv, d3uvv);
  Standard_Real MagTol=0.000000001;
  CSLib_NormalStatus NStatus;
  CSLib::Normal (D1UNormal, D1VNormal, MagTol, NStatus, Normal);
  Standard_Integer MaxOrder;
  if (NStatus == CSLib_Defined) 
    MaxOrder=0;
  else 
    MaxOrder=3;
  Standard_Integer OrderU,OrderV;
  TColgp_Array2OfVec DerNUV(0,MaxOrder+2,0,MaxOrder+2);
  TColgp_Array2OfVec DerSurf(0,MaxOrder+3,0,MaxOrder+3);
  Standard_Integer i,j;
  
  Standard_Real Umin,Umax,Vmin,Vmax;
  Umin = Surf->Surface().FirstUParameter();
  Umax = Surf->Surface().LastUParameter();
  Vmin = Surf->Surface().FirstVParameter();
  Vmax = Surf->Surface().LastVParameter();
  
  DerSurf.SetValue(1, 0, D1UNormal);
  DerSurf.SetValue(0, 1, D1VNormal);
  DerSurf.SetValue(1, 1, D2UVNormal);
  DerSurf.SetValue(2, 0, D2UNormal);
  DerSurf.SetValue(0, 2, D2VNormal);
  DerSurf.SetValue(3, 0, d3u);
  DerSurf.SetValue(2, 1, d3uuv);
  DerSurf.SetValue(1, 2, d3uvv);
  DerSurf.SetValue(0, 3, d3v);
  for(i=0;i<=MaxOrder+2;i++)
    for(j=i; j<=MaxOrder+3; j++ )
      if (i+j > 3) {
        DerSurf.SetValue(i,j,Surf->DN(U,V,i,j));
        if (i!=j) DerSurf.SetValue(j,i,Surf->DN(U,V,j,i));
      }
  
  for(i=0;i<=MaxOrder+2;i++)
    for(j=0;j<=MaxOrder+2;j++){
      DerNUV.SetValue(i,j,CSLib::DNNUV(i,j,DerSurf));
    }
  
  CSLib::Normal(MaxOrder,DerNUV,MagTol,U,V,Umin,Umax,Vmin,Vmax,
                NStatus,Normal,OrderU,OrderV);
  if (NStatus != CSLib_Defined) throw Geom_UndefinedValue();
  
  D1UNormal = CSLib::DNNormal(1,0,DerNUV,OrderU,OrderV);
  D1VNormal = CSLib::DNNormal(0,1,DerNUV,OrderU,OrderV);
  D2UNormal = CSLib::DNNormal(2,0,DerNUV,OrderU,OrderV);
  D2VNormal = CSLib::DNNormal(0,2,DerNUV,OrderU,OrderV);
  D2UVNormal = CSLib::DNNormal(1,1,DerNUV,OrderU,OrderV);
}

GeomFill_Darboux::GeomFill_Darboux()
{
}

Handle(GeomFill_TrihedronLaw) GeomFill_Darboux::Copy() const
{
  Handle(GeomFill_Darboux) copy = new (GeomFill_Darboux)();
  if (!myCurve.IsNull()) copy->SetCurve(myCurve);
  return copy;
}

 Standard_Boolean GeomFill_Darboux::D0(const Standard_Real Param,gp_Vec& Tangent,gp_Vec& Normal,gp_Vec& BiNormal)
{
  gp_Pnt2d C2d;
  gp_Vec2d D2d;
  gp_Pnt S;
  gp_Vec dS_du, dS_dv;
  Handle(Adaptor2d_HCurve2d) myCurve2d = Adaptor3d_CurveOnSurfacePtr(&(myTrimmed->Curve()))->GetCurve();
  Handle(Adaptor3d_HSurface) mySupport = Adaptor3d_CurveOnSurfacePtr(&(myTrimmed->Curve()))->GetSurface();
  Standard_Integer OrderU, OrderV;
  myCurve2d->D1(Param, C2d, D2d);

//  Normal = dS_du.Crossed(dS_dv).Normalized();
  gp_Dir NormalDir;
  NormalD0(C2d.X(), C2d.Y(), mySupport, NormalDir, OrderU, OrderV);
  BiNormal.SetXYZ(NormalDir.XYZ()); 

  mySupport->D1(C2d.X(), C2d.Y(), S, dS_du, dS_dv);
//  if(D2d.Magnitude() <= Precision::Confusion())
//    DoTSingular(Param, Order);
  
  Tangent = D2d.X()*dS_du + D2d.Y()*dS_dv;
  Tangent.Normalize();

  Normal =  BiNormal;
  Normal ^= Tangent;
    

  return Standard_True;
}

 Standard_Boolean GeomFill_Darboux::D1(const Standard_Real Param,
				       gp_Vec& Tangent,gp_Vec& DTangent,
				       gp_Vec& Normal,gp_Vec& DNormal,
				       gp_Vec& BiNormal,gp_Vec& DBiNormal)
{
  gp_Pnt2d C2d;
  gp_Vec2d D2d, D2_2d;
  gp_Pnt S;
  gp_Vec dS_du, dS_dv, d2S_du, d2S_dv, d2S_duv, F, DF;
  Handle(Adaptor2d_HCurve2d) myCurve2d = Adaptor3d_CurveOnSurfacePtr(&(myTrimmed->Curve()))->GetCurve();
  Handle(Adaptor3d_HSurface) mySupport = Adaptor3d_CurveOnSurfacePtr(&(myTrimmed->Curve()))->GetSurface();
//  Standard_Integer Order;
  myCurve2d->D2(Param, C2d, D2d, D2_2d);
  mySupport->D2(C2d.X(), C2d.Y(), S, dS_du, dS_dv, 
		d2S_du, d2S_dv, d2S_duv);
//  if(D2d.Magnitude() <= Precision::Confusion())
//    DoTSingular(Param, Order);
  
  F = D2d.X()*dS_du + D2d.Y()*dS_dv;
  Tangent = F.Normalized();
  DF = D2_2d.X()*dS_du + D2_2d.Y()*dS_dv + 
       d2S_du*D2d.X()*D2d.X() + 2*d2S_duv*D2d.X()*D2d.Y() + 
       d2S_dv*D2d.Y()*D2d.Y();

  DTangent = FDeriv(F, DF);

  gp_Dir NormalDir;
  gp_Vec D1UNormal, D1VNormal;
  NormalD1(C2d.X(), C2d.Y(), mySupport, NormalDir, D1UNormal, D1VNormal);
  BiNormal.SetXYZ(NormalDir.XYZ());
  DBiNormal = D1UNormal*D2d.X() + D1VNormal*D2d.Y();
 
  Normal =  BiNormal;
  Normal ^= Tangent; 
  DNormal = BiNormal.Crossed(DTangent) + DBiNormal.Crossed(Tangent);
  
  return Standard_True;
}

 Standard_Boolean GeomFill_Darboux::D2(const Standard_Real Param,
				       gp_Vec& Tangent,gp_Vec& DTangent,gp_Vec& D2Tangent,
				       gp_Vec& Normal,gp_Vec& DNormal,gp_Vec& D2Normal,
				       gp_Vec& BiNormal,gp_Vec& DBiNormal,gp_Vec& D2BiNormal) 
{
  gp_Pnt2d C2d;
  gp_Vec2d D2d, D2_2d, D3_2d;
  gp_Pnt S;
  gp_Vec dS_du, dS_dv, d2S_du, d2S_dv, d2S_duv, 
         d3S_du, d3S_dv, d3S_duuv, d3S_duvv, F, DF, D2F;
  Handle(Adaptor2d_HCurve2d) myCurve2d = Adaptor3d_CurveOnSurfacePtr(&(myTrimmed->Curve()))->GetCurve();
  Handle(Adaptor3d_HSurface) mySupport = Adaptor3d_CurveOnSurfacePtr(&(myTrimmed->Curve()))->GetSurface();
//  Standard_Integer Order;
  myCurve2d->D3(Param, C2d, D2d, D2_2d, D3_2d);
  mySupport->D3(C2d.X(), C2d.Y(), S, dS_du, dS_dv, 
		d2S_du, d2S_dv, d2S_duv, d3S_du, d3S_dv, d3S_duuv, d3S_duvv);
//  if(D2d.Magnitude() <= Precision::Confusion())
//    DoTSingular(Param, Order);
  
  F = D2d.X()*dS_du + D2d.Y()*dS_dv;
  Tangent = F.Normalized();
  DF = D2_2d.X()*dS_du + D2_2d.Y()*dS_dv + 
       d2S_du*D2d.X()*D2d.X() + 2*d2S_duv*D2d.X()*D2d.Y() + 
       d2S_dv*D2d.Y()*D2d.Y();

  D2F = D3_2d.X()*dS_du + D3_2d.Y()*dS_dv + 
        D2_2d.X()*(D2d.X()*d2S_du + D2d.Y()*d2S_duv) +
	D2_2d.Y()*(D2d.X()*d2S_duv + D2d.Y()*d2S_dv) +
	D2d.X()*D2d.X()*(D2d.X()*d3S_du + D2d.Y()*d3S_duuv) +
	D2d.Y()*D2d.Y()*(D2d.X()*d3S_duvv + D2d.Y()*d3S_dv) +
	2*(
	   D2d.X()*D2_2d.X()*d2S_du + D2d.Y()*D2_2d.Y()*d2S_dv +
	   D2d.X()*D2d.Y()*(D2d.X()*d3S_duuv + D2d.Y()*d3S_duvv) +
	   d2S_duv*(D2_2d.X()*D2d.Y() + D2d.X()*D2_2d.Y())
	   );

  DTangent = FDeriv(F, DF);
  D2Tangent = DDeriv(F, DF, D2F);

  gp_Dir NormalDir;
  gp_Vec D1UNormal, D1VNormal, D2UNormal, D2VNormal, D2UVNormal;
  NormalD2(C2d.X(), C2d.Y(), mySupport, NormalDir, D1UNormal, D1VNormal, 
           D2UNormal, D2VNormal, D2UVNormal);
  BiNormal.SetXYZ(NormalDir.XYZ());
  DBiNormal = D1UNormal*D2d.X() + D1VNormal*D2d.Y();
  D2BiNormal = D1UNormal*D2_2d.X() + D1VNormal*D2_2d.Y() + D2UNormal*D2d.X()*D2d.X() +
    2*D2UVNormal*D2d.X()*D2d.Y() + D2VNormal*D2d.Y()*D2d.Y();

  Normal =  BiNormal;
  Normal ^= Tangent; 
  DNormal = BiNormal.Crossed(DTangent) + DBiNormal.Crossed(Tangent);
  D2Normal = BiNormal.Crossed(D2Tangent) + 2*DBiNormal.Crossed(DTangent) + D2BiNormal.Crossed(Tangent);

  return Standard_True;
}

 Standard_Integer GeomFill_Darboux::NbIntervals(const GeomAbs_Shape S) const
{
  return myCurve->NbIntervals(S);
}

 void GeomFill_Darboux::Intervals(TColStd_Array1OfReal& T,const GeomAbs_Shape S) const
{
  myCurve->Intervals(T, S);
}

 void GeomFill_Darboux::GetAverageLaw(gp_Vec& ATangent,gp_Vec& ANormal,gp_Vec& ABiNormal)
{
  Standard_Integer Num = 20; //order of digitalization
  gp_Vec T, N, BN;
  ATangent = gp_Vec(0, 0, 0);
  ANormal = gp_Vec(0, 0, 0);
  ABiNormal = gp_Vec(0, 0, 0);
  Standard_Real Step = (myTrimmed->LastParameter() - 
                        myTrimmed->FirstParameter()) / Num;
  Standard_Real Param;
  for (Standard_Integer i = 0; i <= Num; i++) {
    Param = myTrimmed->FirstParameter() + i*Step;
    if (Param > myTrimmed->LastParameter()) Param = myTrimmed->LastParameter();
    D0(Param, T, N, BN);
    ATangent += T;
    ANormal += N;
    ABiNormal += BN;
  }
  ATangent /= Num + 1;
  ANormal /= Num + 1;

  ATangent.Normalize();
  ABiNormal = ATangent.Crossed(ANormal).Normalized();
  ANormal = ABiNormal.Crossed(ATangent);  
}

 Standard_Boolean GeomFill_Darboux::IsConstant() const
{
  return (myCurve->GetType() == GeomAbs_Line);
}

 Standard_Boolean GeomFill_Darboux::IsOnlyBy3dCurve() const
{
  return Standard_False;
}
