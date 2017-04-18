// Created on: 1993-08-05
// Created by: Martine LANGLOIS
// Copyright (c) 1993-1999 Matra Datavision
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


#include <Geom_BSplineSurface.hxx>
#include <GeomAbs_BSplKnotDistribution.hxx>
#include <GeomToStep_MakeBSplineSurfaceWithKnots.hxx>
#include <GeomToStep_MakeCartesianPoint.hxx>
#include <StdFail_NotDone.hxx>
#include <StepGeom_BSplineSurfaceWithKnots.hxx>
#include <StepGeom_CartesianPoint.hxx>
#include <StepGeom_HArray2OfCartesianPoint.hxx>
#include <StepGeom_KnotType.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray2OfReal.hxx>

//=============================================================================
// Creation d' une bspline_Surface_with_knots_and_rational_bspline_Surface de
// prostep a partir d' une BSplineSurface de Geom
//=============================================================================
GeomToStep_MakeBSplineSurfaceWithKnots::
  GeomToStep_MakeBSplineSurfaceWithKnots( const
    Handle(Geom_BSplineSurface)& BS )
								      
{
  Handle(StepGeom_BSplineSurfaceWithKnots) BSWK;
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
  
  BSWK = new StepGeom_BSplineSurfaceWithKnots; 
  Handle(TCollection_HAsciiString) name = new TCollection_HAsciiString("");
  BSWK->Init(name, aUDegree, aVDegree, aControlPointsList, aSurfaceForm, 
	     aUClosed, aVClosed, aSelfIntersect, aUMultiplicities, 
	     aVMultiplicities, aUKnots, aVKnots, KnotSpec );
	     
  theBSplineSurfaceWithKnots = BSWK;
  done = Standard_True;
}

//=============================================================================
// renvoi des valeurs
//=============================================================================

const Handle(StepGeom_BSplineSurfaceWithKnots) &
      GeomToStep_MakeBSplineSurfaceWithKnots::Value() const
{
  StdFail_NotDone_Raise_if (!done, "GeomToStep_MakeBSplineSurfaceWithKnots::Value() - no result");
  return theBSplineSurfaceWithKnots;
}
