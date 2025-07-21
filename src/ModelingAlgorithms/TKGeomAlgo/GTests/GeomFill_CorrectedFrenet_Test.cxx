// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <GeomFill_CorrectedFrenet.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_Curve.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <BRepAdaptor_CompCurve.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <GC_MakeSegment.hxx>
#include <ShapeExtend_WireData.hxx>
#include <TopoDS_Edge.hxx>
#include <TColgp_Array1OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <gp_Pnt.hxx>
#include <Standard_Real.hxx>

#include <gtest/gtest.h>

//==================================================================================================

TEST(GeomFill_CorrectedFrenet, EndlessLoopPrevention)
{
  TColgp_Array1OfPnt aPoles(1, 4);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1.0, 0.0, 0.0);
  aPoles(3) = gp_Pnt(1.0, 1.0, 0.0);
  aPoles(4) = gp_Pnt(0.0, 1.0, 0.0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  Handle(Geom_BSplineCurve) aCurve    = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  Handle(GeomAdaptor_Curve) anAdaptor = new GeomAdaptor_Curve(aCurve);

  GeomFill_CorrectedFrenet aCorrectedFrenet(Standard_False);
  aCorrectedFrenet.SetCurve(anAdaptor);

  Standard_Real aParam1 = 0.0;
  Standard_Real aParam2 = 1.0;

  gp_Vec aTangent1, aNormal1, aBinormal1;
  gp_Vec aTangent2, aNormal2, aBinormal2;

  EXPECT_NO_THROW({
    aCorrectedFrenet.D0(aParam1, aTangent1, aNormal1, aBinormal1);
    aCorrectedFrenet.D0(aParam2, aTangent2, aNormal2, aBinormal2);
  });

  EXPECT_GT(aTangent1.Magnitude(), 1e-10);
  EXPECT_GT(aNormal1.Magnitude(), 1e-10);
  EXPECT_GT(aBinormal1.Magnitude(), 1e-10);

  EXPECT_GT(aTangent2.Magnitude(), 1e-10);
  EXPECT_GT(aNormal2.Magnitude(), 1e-10);
  EXPECT_GT(aBinormal2.Magnitude(), 1e-10);
}

//==================================================================================================

TEST(GeomFill_CorrectedFrenet, SmallStepHandling)
{
  TColgp_Array1OfPnt aPoles(1, 2);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(1e-10, 0.0, 0.0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 2;
  aMults(2) = 2;

  Handle(Geom_BSplineCurve) aCurve    = new Geom_BSplineCurve(aPoles, aKnots, aMults, 1);
  Handle(GeomAdaptor_Curve) anAdaptor = new GeomAdaptor_Curve(aCurve);

  GeomFill_CorrectedFrenet aCorrectedFrenet(Standard_False);
  aCorrectedFrenet.SetCurve(anAdaptor);

  gp_Vec aTangent, aNormal, aBinormal;

  EXPECT_NO_THROW({ aCorrectedFrenet.D0(0.5, aTangent, aNormal, aBinormal); });
}

//==================================================================================================

TEST(GeomFill_CorrectedFrenet, ParameterProgressionGuarantee)
{
  TColgp_Array1OfPnt aPoles(1, 3);
  aPoles(1) = gp_Pnt(0.0, 0.0, 0.0);
  aPoles(2) = gp_Pnt(0.5, 0.5, 0.0);
  aPoles(3) = gp_Pnt(1.0, 0.0, 0.0);

  TColStd_Array1OfReal aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  TColStd_Array1OfInteger aMults(1, 2);
  aMults(1) = 3;
  aMults(2) = 3;

  Handle(Geom_BSplineCurve) aCurve    = new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
  Handle(GeomAdaptor_Curve) anAdaptor = new GeomAdaptor_Curve(aCurve);

  GeomFill_CorrectedFrenet aCorrectedFrenet(Standard_False);
  aCorrectedFrenet.SetCurve(anAdaptor);

  for (Standard_Real aParam = 0.1; aParam <= 0.9; aParam += 0.1)
  {
    gp_Vec aTangent, aNormal, aBinormal;

    EXPECT_NO_THROW({ aCorrectedFrenet.D0(aParam, aTangent, aNormal, aBinormal); });

    EXPECT_GT(aTangent.Magnitude(), 1e-12);
    EXPECT_GT(aNormal.Magnitude(), 1e-12);
    EXPECT_GT(aBinormal.Magnitude(), 1e-12);
  }
}

//==================================================================================================

TEST(GeomFill_CorrectedFrenet, ActualReproducerCase)
{
  TColgp_Array1OfPnt aPoints(1, 4);
  aPoints(1) = gp_Pnt(-1, -1, 0);
  aPoints(2) = gp_Pnt(0, -2, 0);
  aPoints(3) = gp_Pnt(0, -2, -1);
  aPoints(4) = gp_Pnt(0, -1, -1);

  ShapeExtend_WireData anExtend;
  for (Standard_Integer i = 2; i <= aPoints.Length(); i++)
  {
    Handle(Geom_Curve) aCurve = GC_MakeSegment(aPoints(i - 1), aPoints(i)).Value();
    TopoDS_Edge        anEdge = BRepBuilderAPI_MakeEdge(aCurve).Edge();
    anExtend.Add(anEdge);
  }

  BRepAdaptor_CompCurve anAdaptor(anExtend.WireAPIMake());

  GeomFill_CorrectedFrenet aCorrectedFrenet(Standard_False);

  // This SetCurve call should not hang (was causing infinite loops)
  EXPECT_NO_THROW({ aCorrectedFrenet.SetCurve(anAdaptor.ShallowCopy()); });

  // Verify we can evaluate the trihedron at various parameters
  gp_Vec aTangent, aNormal, aBinormal;

  EXPECT_NO_THROW({ aCorrectedFrenet.D0(0.0, aTangent, aNormal, aBinormal); });

  EXPECT_NO_THROW({ aCorrectedFrenet.D0(0.5, aTangent, aNormal, aBinormal); });

  EXPECT_NO_THROW({ aCorrectedFrenet.D0(1.0, aTangent, aNormal, aBinormal); });
}
