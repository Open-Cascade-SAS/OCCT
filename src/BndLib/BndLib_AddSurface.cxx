// Created on: 1995-07-24
// Created by: Modelistation
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

//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503


#include <BndLib_AddSurface.ixx>


#include <Adaptor3d_HSurface.hxx>
#include <GeomAbs_SurfaceType.hxx>
#include <BndLib.hxx>
#include <gp_Pnt.hxx>
#include <gp_Pln.hxx>
#include <ElSLib.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_BezierSurface.hxx>

#include <Precision.hxx>

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BndLib_AddSurface::Add(const Adaptor3d_Surface& S,
			    const Standard_Real Tol,
			    Bnd_Box& B ) 
{

  BndLib_AddSurface::Add(S,
			 S.FirstUParameter(),
			 S.LastUParameter (),
			 S.FirstVParameter(),
			 S.LastVParameter (),Tol,B);
}
//=======================================================================
//function : NbUSamples
//purpose  : 
//=======================================================================

static Standard_Integer NbUSamples(const Adaptor3d_Surface& S) 
{
  Standard_Integer N;
  GeomAbs_SurfaceType Type = S.GetType();
  switch (Type) {
  case GeomAbs_BezierSurface: 
    {
      N = 2*S.NbUPoles();
      break;
    }
  case GeomAbs_BSplineSurface: 
    {
      const Handle(Geom_BSplineSurface)& BS = S.BSpline();
      N = 2*(BS->UDegree() + 1)*(BS->NbUKnots() -1);
      break;
    }
  default:
    N = 33;
  }
  return Min (50,N);
}

//=======================================================================
//function : NbVSamples
//purpose  : 
//=======================================================================

static Standard_Integer NbVSamples(const Adaptor3d_Surface& S) 
{
  Standard_Integer N;
  GeomAbs_SurfaceType Type = S.GetType();
  switch (Type) {
  case GeomAbs_BezierSurface:
    {
      N = 2*S.NbVPoles();
      break;
    }
  case GeomAbs_BSplineSurface:
    {
      const Handle(Geom_BSplineSurface)& BS = S.BSpline();
      N = 2*(BS->VDegree() + 1)*(BS->NbVKnots() - 1) ;
      break;
    }
  default:
    N = 33;
  }
  return Min(50,N);
}

//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 Begin
static gp_Pnt BaryCenter(const gp_Pln        &aPlane,
			 const Standard_Real  aUMin,
			 const Standard_Real  aUMax,
			 const Standard_Real  aVMin,
			 const Standard_Real  aVMax)
{
  Standard_Real aU, aV;
  Standard_Boolean isU1Inf = Precision::IsInfinite(aUMin);
  Standard_Boolean isU2Inf = Precision::IsInfinite(aUMax);
  Standard_Boolean isV1Inf = Precision::IsInfinite(aVMin);
  Standard_Boolean isV2Inf = Precision::IsInfinite(aVMax);

  if (isU1Inf && isU2Inf)
    aU = 0;
  else if (isU1Inf)
    aU = aUMax - 10.;
  else if (isU2Inf)
    aU = aUMin + 10.;
  else
    aU = (aUMin + aUMax)/2.;
    
  if (isV1Inf && isV2Inf)
    aV = 0;
  else if (isV1Inf)
    aV = aVMax - 10.;
  else if (isV2Inf)
    aV = aVMin + 10.;
  else
    aV = (aVMin + aVMax)/2.;

  gp_Pnt aCenter = ElSLib::Value(aU, aV, aPlane);

  return aCenter;
}

static void TreatInfinitePlane(const gp_Pln        &aPlane,
			       const Standard_Real  aUMin,
			       const Standard_Real  aUMax,
			       const Standard_Real  aVMin,
			       const Standard_Real  aVMax,
			       const Standard_Real  aTol,
			             Bnd_Box       &aB)
{
  // Get 3 coordinate axes of the plane.
  const gp_Dir        &aNorm        = aPlane.Axis().Direction();
  const Standard_Real  anAngularTol = RealEpsilon();

  // Get location of the plane as its barycenter
  gp_Pnt aLocation = BaryCenter(aPlane, aUMin, aUMax, aVMin, aVMax);

  if (aNorm.IsParallel(gp::DX(), anAngularTol)) {
    aB.Add(aLocation);
    aB.OpenYmin();
    aB.OpenYmax();
    aB.OpenZmin();
    aB.OpenZmax();
  } else if (aNorm.IsParallel(gp::DY(), anAngularTol)) {
    aB.Add(aLocation);
    aB.OpenXmin();
    aB.OpenXmax();
    aB.OpenZmin();
    aB.OpenZmax();
  } else if (aNorm.IsParallel(gp::DZ(), anAngularTol)) {
    aB.Add(aLocation);
    aB.OpenXmin();
    aB.OpenXmax();
    aB.OpenYmin();
    aB.OpenYmax();
  } else {
    aB.SetWhole();
    return;
  }

  aB.Enlarge(aTol);
}
//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 End

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

void BndLib_AddSurface::Add(const Adaptor3d_Surface& S,
			    const Standard_Real UMin,
			    const Standard_Real UMax,
			    const Standard_Real VMin,
			    const Standard_Real VMax,
			    const Standard_Real Tol,
			    Bnd_Box& B ) 
{
  GeomAbs_SurfaceType Type = S.GetType(); // skv OCC6503

  if (Precision::IsInfinite(VMin) ||
      Precision::IsInfinite(VMax) ||
      Precision::IsInfinite(UMin) ||
      Precision::IsInfinite(UMax)   ) {
//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 Begin
//     B.SetWhole();
//     return;
    switch (Type) {
    case GeomAbs_Plane: 
      {
	TreatInfinitePlane(S.Plane(), UMin, UMax, VMin, VMax, Tol, B);
	return;
      }
    default: 
      {
	B.SetWhole();
	return;
      }
    }
//  Modified by skv - Fri Aug 27 12:29:04 2004 OCC6503 End
  }

//   GeomAbs_SurfaceType Type = S.GetType(); // skv OCC6503

  switch (Type) {
    
  case GeomAbs_Plane: 
    {
      gp_Pln Plan = S.Plane();
      B.Add(ElSLib::Value(UMin,VMin,Plan)); 
      B.Add(ElSLib::Value(UMin,VMax,Plan)); 
      B.Add(ElSLib::Value(UMax,VMin,Plan)); 
      B.Add(ElSLib::Value(UMax,VMax,Plan)); 
      B.Enlarge(Tol);
      break;
    }
  case GeomAbs_Cylinder: 
    {
      BndLib::Add(S.Cylinder(),UMin,UMax,VMin,VMax,Tol,B);
      break;
    }
  case GeomAbs_Cone: 
    {
      BndLib::Add(S.Cone(),UMin,UMax,VMin,VMax,Tol,B);
      break;
    }
  case GeomAbs_Torus: 
    {
      BndLib::Add(S.Torus(),UMin,UMax,VMin,VMax,Tol,B);
      break;
    }
  case GeomAbs_Sphere: 
    {
      if (Abs(UMin) < Precision::Angular() &&
	  Abs(UMax - 2.*M_PI) < Precision::Angular() &&
	  Abs(VMin + M_PI/2.) < Precision::Angular() &&
	  Abs(VMax - M_PI/2.) < Precision::Angular()) // a whole sphere
	BndLib::Add(S.Sphere(),Tol,B);
      else
	BndLib::Add(S.Sphere(),UMin,UMax,VMin,VMax,Tol,B);
      break;
    }
  case GeomAbs_OffsetSurface: 
    {
      Handle(Adaptor3d_HSurface) HS = S.BasisSurface();
      Add (HS->Surface(),UMin,UMax,VMin,VMax,Tol,B);
      B.Enlarge(S.OffsetValue());
      B.Enlarge(Tol);
      break;
    } 
  case GeomAbs_BezierSurface:
  case GeomAbs_BSplineSurface: 
    {
      Standard_Real PTol = Precision::Parametric(Precision::Confusion());
      if (Abs(UMin-S.FirstUParameter()) < PTol &&
	  Abs(VMin-S.FirstVParameter()) < PTol &&
	  Abs(UMax-S.LastUParameter ()) < PTol &&
	  Abs(VMax-S.LastVParameter ()) < PTol ) {
	TColgp_Array2OfPnt Tp(1,S.NbUPoles(),1,S.NbVPoles());
	if (Type == GeomAbs_BezierSurface) {
	  S.Bezier()->Poles(Tp);
	}
	else {
	  S.BSpline()->Poles(Tp);
	}
	for (Standard_Integer i = Tp.LowerRow();i<=Tp.UpperRow();i++) {
	  for (Standard_Integer j = Tp.LowerCol();j<=Tp.UpperCol();j++) {
	    B.Add(Tp(i,j));
	  }
	}
	B.Enlarge(Tol);
	break;
      }
    }
  default: 
    {
      Standard_Integer Nu = NbUSamples(S);
      Standard_Integer Nv = NbVSamples(S);
      gp_Pnt P;
      for (Standard_Integer i =1 ;i<=Nu;i++){
	Standard_Real U = UMin + ((UMax-UMin)*(i-1)/(Nu-1));
	for (Standard_Integer j=1 ;j<=Nv;j++){
	  Standard_Real V = VMin + ((VMax-VMin)*(j-1)/(Nv-1));
	  S.D0(U,V,P);
	  B.Add(P);
	}
      }
      B.Enlarge(Tol);
    }
  }
}
