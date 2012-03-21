// Created on: 1996-03-12
// Created by: Bruno DUMORTIER
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



#include <GeomConvert_BSplineSurfaceToBezierSurface.ixx>

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
      Standard_DomainError::Raise("GeomConvert_BSplineSurfaceToBezierSurface");

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
    Standard_OutOfRange::Raise("GeomConvert_BSplineSurfaceToBezierSurface");
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


