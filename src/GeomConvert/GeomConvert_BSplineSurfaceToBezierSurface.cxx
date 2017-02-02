// Created on: 1996-03-12
// Created by: Bruno DUMORTIER
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


#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <GeomConvert_BSplineSurfaceToBezierSurface.hxx>
#include <Standard_DimensionError.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_OutOfRange.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array2OfReal.hxx>

//=======================================================================
//function : GeomConvert_BSplineSurfaceToBezierSurface
//purpose  : 
//=======================================================================
GeomConvert_BSplineSurfaceToBezierSurface::GeomConvert_BSplineSurfaceToBezierSurface(const Handle(Geom_BSplineSurface)& BasisSurface)
{
  mySurface = Handle(Geom_BSplineSurface)::DownCast(BasisSurface->Copy());
  Standard_Real U1,U2,V1,V2;
  mySurface->Bounds(U1,U2,V1,V2);
  mySurface->Segment(U1,U2,V1,V2);
  mySurface->IncreaseUMultiplicity(mySurface->FirstUKnotIndex(),
				   mySurface->LastUKnotIndex(),
				   mySurface->UDegree());
  mySurface->IncreaseVMultiplicity(mySurface->FirstVKnotIndex(),
				   mySurface->LastVKnotIndex(),
				   mySurface->VDegree());
}


//=======================================================================
//function : GeomConvert_BSplineSurfaceToBezierSurface
//purpose  : 
//=======================================================================

GeomConvert_BSplineSurfaceToBezierSurface::GeomConvert_BSplineSurfaceToBezierSurface
(const Handle(Geom_BSplineSurface)& BasisSurface,
 const Standard_Real U1, 
 const Standard_Real U2, 
 const Standard_Real V1, 
 const Standard_Real V2, 
 const Standard_Real ParametricTolerance)
{
  if ( (U2 - U1 <  ParametricTolerance) ||
       (V2 - V1 <  ParametricTolerance) )
      throw Standard_DomainError("GeomConvert_BSplineSurfaceToBezierSurface");

  Standard_Real Uf=U1, Ul=U2, Vf=V1, Vl=V2, PTol = ParametricTolerance/2;
  Standard_Integer I1, I2;

  mySurface = Handle(Geom_BSplineSurface)::DownCast(BasisSurface->Copy());

  mySurface->LocateU(U1, PTol, I1, I2);
  if (I1==I2) { // On est sur le noeud
    if ( mySurface->UKnot(I1) > U1) Uf = mySurface->UKnot(I1);
  }    

  mySurface->LocateU(U2, PTol, I1, I2);
  if (I1==I2) { // On est sur le noeud
    if ( mySurface->UKnot(I1) < U2) Ul = mySurface->UKnot(I1);
  }

  mySurface->LocateV(V1, PTol, I1, I2);
  if (I1==I2) { // On est sur le noeud
    if ( mySurface->VKnot(I1) > V1) Vf = mySurface->VKnot(I1);
  }    

  mySurface->LocateV(V2, PTol, I1, I2);
  if (I1==I2) { // On est sur le noeud
    if ( mySurface->VKnot(I1) < V2) Vl = mySurface->VKnot(I1);
  }

  mySurface->Segment(Uf, Ul, Vf, Vl);
  mySurface->IncreaseUMultiplicity(mySurface->FirstUKnotIndex(),
				   mySurface->LastUKnotIndex(),
				   mySurface->UDegree());
  mySurface->IncreaseVMultiplicity(mySurface->FirstVKnotIndex(),
				   mySurface->LastVKnotIndex(),
				   mySurface->VDegree());
}


//=======================================================================
//function : Patch
//purpose  : 
//=======================================================================

Handle(Geom_BezierSurface) GeomConvert_BSplineSurfaceToBezierSurface::Patch
(const Standard_Integer UIndex,
 const Standard_Integer VIndex)
{
  if (UIndex < 1 || UIndex > mySurface->NbUKnots()-1 ||
      VIndex < 1 || VIndex > mySurface->NbVKnots()-1   ) {
    throw Standard_OutOfRange("GeomConvert_BSplineSurfaceToBezierSurface");
  }
  Standard_Integer UDeg = mySurface->UDegree();
  Standard_Integer VDeg = mySurface->VDegree();

  TColgp_Array2OfPnt Poles(1,UDeg+1,1,VDeg+1);

  Handle(Geom_BezierSurface) S;
  if ( mySurface->IsURational() || mySurface->IsVRational()) {
    TColStd_Array2OfReal Weights(1,UDeg+1,1,VDeg+1);
    for ( Standard_Integer i = 1; i <= UDeg+1; i++) {
      Standard_Integer CurI = i+UDeg*(UIndex-1);
      for ( Standard_Integer j = 1; j <= VDeg+1; j++) {
	Poles(i,j)   = mySurface->Pole(CurI,j+VDeg*(VIndex-1));
	Weights(i,j) = mySurface->Weight(CurI,j+VDeg*(VIndex-1));
      }
    }
    S = new Geom_BezierSurface(Poles,Weights);
  }
  else {
    for ( Standard_Integer i = 1; i <= UDeg+1; i++) {
      Standard_Integer CurI = i+UDeg*(UIndex-1);
      for ( Standard_Integer j = 1; j <= VDeg+1; j++) {
	Poles(i,j)   = mySurface->Pole(CurI,j+VDeg*(VIndex-1));
      }
    }
    S = new Geom_BezierSurface(Poles);
  }
  return S;
}


//=======================================================================
//function : Patches
//purpose  : 
//=======================================================================

void GeomConvert_BSplineSurfaceToBezierSurface::Patches
(TColGeom_Array2OfBezierSurface& Surfaces)
{
  Standard_Integer NbU = NbUPatches();
  Standard_Integer NbV = NbVPatches();
  for (Standard_Integer i = 1; i <= NbU; i++) {
    for (Standard_Integer j = 1; j <= NbV; j++) {
      Surfaces(i,j) = Patch(i,j);
    }
  }
}

//=======================================================================
//function : UKnots
//purpose  : 
//=======================================================================

void GeomConvert_BSplineSurfaceToBezierSurface::UKnots
     (TColStd_Array1OfReal& TKnots) const
{
 Standard_Integer ii, kk;
  for (ii = 1, kk = TKnots.Lower();
       ii <= mySurface->NbUKnots(); ii++, kk++)
    TKnots(kk) = mySurface->UKnot(ii);
}

//=======================================================================
//function : VKnots
//purpose  : 
//=======================================================================

void GeomConvert_BSplineSurfaceToBezierSurface::VKnots
     (TColStd_Array1OfReal& TKnots) const
{
 Standard_Integer ii, kk;
  for (ii = 1, kk = TKnots.Lower();
       ii <= mySurface->NbVKnots(); ii++, kk++)
    TKnots(kk) = mySurface->VKnot(ii);
}

//=======================================================================
//function : NbUPatches
//purpose  : 
//=======================================================================

Standard_Integer GeomConvert_BSplineSurfaceToBezierSurface::NbUPatches() const 
{
  return (mySurface->NbUKnots()-1);
}


//=======================================================================
//function : NbVPatches
//purpose  : 
//=======================================================================

Standard_Integer GeomConvert_BSplineSurfaceToBezierSurface::NbVPatches() const 
{
  return (mySurface->NbVKnots()-1);
}


