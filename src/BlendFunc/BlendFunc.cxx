// Created on: 1995-10-04
// Created by: Jacques GOUSSARD
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

// 09/09/1996 PMN Ajout des methodes GetCircle et GetTolerance.
// 30/12/1996 PMN Ajout de GetMinimalWeight
// 23/09/1997 PMN Supprimme GetCircle et GetTol (passe dans GeomFill)

#include <Adaptor3d_HSurface.hxx>
#include <BlendFunc.hxx>
#include <CSLib.hxx>
#include <CSLib_NormalStatus.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomConvert.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <Standard_ConstructionError.hxx>
#include <TColgp_Array2OfVec.hxx>

//=======================================================================
//function : GetShape
//purpose  : 
//=======================================================================
void BlendFunc::GetShape (const BlendFunc_SectionShape SShape,
			  const Standard_Real MaxAng,
			  Standard_Integer& NbPoles,
			  Standard_Integer& NbKnots,
			  Standard_Integer& Degree,
			  Convert_ParameterisationType& TConv)
{
  switch (SShape) {
  case BlendFunc_Rational:
    {
      Standard_Integer NbSpan =
	(Standard_Integer)(Ceiling(3.*Abs(MaxAng)/2./M_PI));
      NbPoles = 2*NbSpan+1;
      NbKnots = NbSpan+1;
      Degree = 2;
      if (NbSpan == 1) {
	TConv = Convert_TgtThetaOver2_1;
      }
      else { // QuasiAngular affin d'etre C1 (et meme beaucoup plus)
	NbPoles = 7 ;
	NbKnots = 2 ;
	Degree  = 6 ;
	TConv = Convert_QuasiAngular;
      }
    }
    break;
  case BlendFunc_QuasiAngular:
    {
      NbPoles = 7 ;
      NbKnots = 2 ;
      Degree  = 6 ;
      TConv = Convert_QuasiAngular;
    }
    break;
  case BlendFunc_Polynomial:
    {
      NbPoles = 8;
      NbKnots = 2;
      Degree = 7;
      TConv = Convert_Polynomial;
    }
    break;
  case BlendFunc_Linear:
    {
      NbPoles = 2;
      NbKnots = 2;
      Degree = 1;
    }
    break;
  }

}

//=======================================================================
//function : GetMinimalWeights
//purpose  : On suppose les extremum de poids sont obtenus pour les
//           extremums d'angles (A verifier eventuelement pour Quasi-Angular)
//=======================================================================

void BlendFunc::GetMinimalWeights(const BlendFunc_SectionShape SShape,
				  const Convert_ParameterisationType  TConv,
				  const Standard_Real MinAng,
				  const Standard_Real MaxAng,
				  TColStd_Array1OfReal& Weights)

{
  switch (SShape) {
  case BlendFunc_Polynomial:
  case BlendFunc_Linear:
    {
      Weights.Init(1);
    }
    break;
  case BlendFunc_Rational:
  case BlendFunc_QuasiAngular:
    {
      gp_Ax2 popAx2(gp_Pnt(0, 0, 0), gp_Dir(0,0,1));
      gp_Circ C (popAx2, 1);
      Handle(Geom_TrimmedCurve) Sect1 = 
	new Geom_TrimmedCurve(new Geom_Circle(C), 0., MaxAng);
      Handle(Geom_BSplineCurve) CtoBspl = 
      GeomConvert::CurveToBSplineCurve(Sect1, TConv);
      CtoBspl->Weights(Weights);
      
      TColStd_Array1OfReal poids (Weights.Lower(),  Weights.Upper());
      Standard_Real angle_min = Max(Precision::PConfusion(), MinAng);
   
      Handle(Geom_TrimmedCurve) Sect2 = 
	new Geom_TrimmedCurve(new Geom_Circle(C), 0., angle_min);
      CtoBspl = GeomConvert::CurveToBSplineCurve(Sect2, TConv);
      CtoBspl->Weights(poids);

      for (Standard_Integer ii=Weights.Lower(); ii<=Weights.Upper(); ii++) {
	if (poids(ii) < Weights(ii)) {
	  Weights(ii) = poids(ii);
	}
      }
    }
    break;
  }
}


//=======================================================================
//function : IncrementeShape
//purpose  : 
//=======================================================================

GeomAbs_Shape BlendFunc::NextShape (const GeomAbs_Shape S)
{
  switch (S)
  {
    case GeomAbs_C0 : return  GeomAbs_C1;
    case GeomAbs_C1 : return  GeomAbs_C2;
    case GeomAbs_C2 : return  GeomAbs_C3;
    default : break;
  }
  return GeomAbs_CN;
}


//=======================================================================
//function : ComputeNormal
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc::ComputeNormal (const Handle(Adaptor3d_HSurface)& Surf,
                                           const gp_Pnt2d& p2d, gp_Vec& Normal)
{
  const Standard_Integer MaxOrder=3;
  const Standard_Real U = p2d.X();
  const Standard_Real V = p2d.Y();

  Standard_Integer i,j;

  TColgp_Array2OfVec DerSurf(0,MaxOrder+1,0,MaxOrder+1);
  for(i=1;i<=MaxOrder+1;i++)
    DerSurf.SetValue(i,0,Surf->DN(U,V,i,0));
  for(i=0;i<=MaxOrder+1;i++)
    for(j=1;j<=MaxOrder+1;j++)
      DerSurf.SetValue(i,j,Surf->DN(U,V,i,j));

  TColgp_Array2OfVec DerNUV(0,MaxOrder,0,MaxOrder);
  for(i=0;i<=MaxOrder;i++)
    for(j=0;j<=MaxOrder;j++)
      DerNUV.SetValue(i,j,CSLib::DNNUV(i,j,DerSurf));

  gp_Dir thenormal;
  CSLib_NormalStatus stat;
  Standard_Integer OrderU,OrderV;
  const Standard_Real Umin = Surf->FirstUParameter();
  const Standard_Real Umax = Surf->LastUParameter();
  const Standard_Real Vmin = Surf->FirstVParameter();
  const Standard_Real Vmax = Surf->LastVParameter(); //szv: was FirstVParameter!
  CSLib::Normal(MaxOrder,DerNUV,Standard_Real(1.e-9),U,V,Umin,Umax,Vmin,Vmax,
                stat,thenormal,OrderU,OrderV);
  if (stat == CSLib_Defined)
  {
    Normal.SetXYZ(thenormal.XYZ());
    return Standard_True;
  }
  return Standard_False;
}


//=======================================================================
//function : ComputeDNormal
//purpose  : 
//=======================================================================

Standard_Boolean BlendFunc::ComputeDNormal (const Handle(Adaptor3d_HSurface)& Surf,
                                            const gp_Pnt2d& p2d, gp_Vec& Normal,
                                            gp_Vec& DNu, gp_Vec& DNv)
{
  const Standard_Integer MaxOrder=3;
  const Standard_Real U = p2d.X();
  const Standard_Real V = p2d.Y();

  Standard_Integer i,j;

  TColgp_Array2OfVec DerSurf(0,MaxOrder+1,0,MaxOrder+1);
  for(i=1;i<=MaxOrder+1;i++)
    DerSurf.SetValue(i,0,Surf->DN(U,V,i,0));
  for(i=0;i<=MaxOrder+1;i++)
    for(j=1;j<=MaxOrder+1;j++)
      DerSurf.SetValue(i,j,Surf->DN(U,V,i,j));

  TColgp_Array2OfVec DerNUV(0,MaxOrder,0,MaxOrder);
  for(i=0;i<=MaxOrder;i++)
    for(j=0;j<=MaxOrder;j++)
      DerNUV.SetValue(i,j,CSLib::DNNUV(i,j,DerSurf));

  gp_Dir thenormal;
  CSLib_NormalStatus stat;
  Standard_Integer OrderU,OrderV;
  const Standard_Real Umin = Surf->FirstUParameter();
  const Standard_Real Umax = Surf->LastUParameter();
  const Standard_Real Vmin = Surf->FirstVParameter();
  const Standard_Real Vmax = Surf->LastVParameter(); //szv: was FirstVParameter!
  CSLib::Normal(MaxOrder,DerNUV,Standard_Real(1.e-9),U,V,Umin,Umax,Vmin,Vmax,
                stat,thenormal,OrderU,OrderV);
  if (stat == CSLib_Defined)
  {
    Normal.SetXYZ(thenormal.XYZ());
    DNu = CSLib::DNNormal(1, 0, DerNUV, OrderU, OrderV);
    DNv = CSLib::DNNormal(0, 1, DerNUV, OrderU, OrderV);
    return Standard_True;
  }
  return Standard_False;
}
