// Created on: 1993-06-22
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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

//              .cxx

#include <GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface.ixx>
#include <StdFail_NotDone.hxx>
#include <Geom_BSplineSurface.hxx>
#include <StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <StepGeom_HArray2OfCartesianPoint.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <GeomAbs_BSplKnotDistribution.hxx>
#include <StepGeom_KnotType.hxx>
#include <TCollection_HAsciiString.hxx>

//=============================================================================
// Creation d' une bspline_Surface_with_knots_and_rational_bspline_Surface de
// prostep a partir d' une BSplineSurface de Geom
//=============================================================================

GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface::
  GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface( const
    Handle(Geom_BSplineSurface)& BS )
								      
{
  Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface) BSWK;
  Standard_Integer aUDegree, aVDegree, NU, NV, i, j, NUknots, NVknots, itampon;
  Standard_Real rtampon;
  Handle(StepGeom_CartesianPoint) Pt = new StepGeom_CartesianPoint; 
  Handle(StepGeom_HArray2OfCartesianPoint) aControlPointsList;
  StepGeom_BSplineSurfaceForm aSurfaceForm;
  StepData_Logical aUClosed, aVClosed, aSelfIntersect;
  Handle(TColStd_HArray1OfInteger) aUMultiplicities, aVMultiplicities;
  Handle(TColStd_HArray1OfReal) aUKnots, aVKnots;
  Handle(TColStd_HArray2OfReal) aWeightsData;
  GeomAbs_BSplKnotDistribution UDistribution, VDistribution;
  StepGeom_KnotType KnotSpec;

  aUDegree = BS->UDegree();
  aVDegree = BS->VDegree();

  NU = BS->NbUPoles();
  NV = BS->NbVPoles();
  TColgp_Array2OfPnt P(1,NU,1,NV);
  BS->Poles(P);
  aControlPointsList = new StepGeom_HArray2OfCartesianPoint(1,NU,1,NV);
  for ( i=P.LowerRow(); i<=P.UpperRow(); i++) {
    for ( j=P.LowerCol(); j<=P.UpperCol(); j++) { 
      GeomToStep_MakeCartesianPoint MkPoint(P.Value(i,j));
      Pt = MkPoint.Value();
      aControlPointsList->SetValue(i, j, Pt);
    }
  }

  aSurfaceForm = StepGeom_bssfUnspecified;
  
  if (BS->IsUClosed()) 
    aUClosed = StepData_LTrue;
  else
    aUClosed = StepData_LFalse;

  if (BS->IsVClosed()) 
    aVClosed = StepData_LTrue;
  else
    aVClosed = StepData_LFalse;

  aSelfIntersect = StepData_LFalse;

  NUknots = BS->NbUKnots();
  NVknots = BS->NbVKnots();
  TColStd_Array1OfInteger MU(1,NUknots);
  BS->UMultiplicities(MU);
  aUMultiplicities = new TColStd_HArray1OfInteger(1,NUknots);
  for ( i=MU.Lower(); i<=MU.Upper(); i++) { 
    itampon = MU.Value(i);
    aUMultiplicities->SetValue(i, itampon);
  }
  TColStd_Array1OfInteger MV(1,NVknots);
  BS->VMultiplicities(MV);
  aVMultiplicities = new TColStd_HArray1OfInteger(1,NVknots);
  for ( i=MV.Lower(); i<=MV.Upper(); i++) { 
    itampon = MV.Value(i);
    aVMultiplicities->SetValue(i, itampon);
  }
  
  TColStd_Array1OfReal KU(1,NUknots);
  TColStd_Array1OfReal KV(1,NVknots);
  BS->UKnots(KU);
  BS->VKnots(KV);
  aUKnots = new TColStd_HArray1OfReal(1,NUknots);
  aVKnots = new TColStd_HArray1OfReal(1,NVknots);
  for ( i=KU.Lower(); i<=KU.Upper(); i++) { 
    rtampon = KU.Value(i);
    aUKnots->SetValue(i, rtampon);
  }
  for ( i=KV.Lower(); i<=KV.Upper(); i++) { 
    rtampon = KV.Value(i);
    aVKnots->SetValue(i, rtampon);
  }
  
  UDistribution = BS->UKnotDistribution();
  VDistribution = BS->VKnotDistribution();
  if ( UDistribution == GeomAbs_NonUniform &&
       VDistribution == GeomAbs_NonUniform )
    KnotSpec = StepGeom_ktUnspecified;
  else if ( UDistribution == GeomAbs_Uniform &&
            VDistribution == GeomAbs_Uniform )
    KnotSpec = StepGeom_ktUniformKnots;
  else if ( UDistribution == GeomAbs_QuasiUniform &&
            VDistribution == GeomAbs_QuasiUniform )
    KnotSpec = StepGeom_ktQuasiUniformKnots;
  else if ( UDistribution == GeomAbs_PiecewiseBezier &&
            VDistribution == GeomAbs_PiecewiseBezier )
    KnotSpec = StepGeom_ktPiecewiseBezierKnots;
  else 
    KnotSpec = StepGeom_ktUnspecified;
  
  TColStd_Array2OfReal W(1,NU,1,NV);
  BS->Weights(W);
  aWeightsData = new TColStd_HArray2OfReal(1,NU,1,NV);
  for ( i=W.LowerRow(); i<=W.UpperRow(); i++) {
    for (j=W.LowerCol(); j<=W.UpperCol(); j++) {
      rtampon = W.Value(i,j);
      aWeightsData->SetValue(i, j, rtampon);
    }
  }
  
  BSWK = new StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface; 
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  BSWK->Init(name,aUDegree, aVDegree, aControlPointsList, aSurfaceForm, 
	     aUClosed, aVClosed, aSelfIntersect, aUMultiplicities, 
	     aVMultiplicities, aUKnots, aVKnots, KnotSpec, aWeightsData );
	     
  theBSplineSurfaceWithKnotsAndRationalBSplineSurface = BSWK;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_BSplineSurfaceWithKnotsAndRationalBSplineSurface) &
      GeomToStep_MakeBSplineSurfaceWithKnotsAndRationalBSplineSurface::Value() const
{
  StdFail_NotDone_Raise_if(!done == Standard_True,"");
  return theBSplineSurfaceWithKnotsAndRationalBSplineSurface;
}
