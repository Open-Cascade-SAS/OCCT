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

#include <Geom2d_BezierCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Geom2dGcc_Circ2d2TanOn.hxx>
#include <Geom2dGcc_QualifiedCurve.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <GccEnt.hxx>
#include <NCollection_List.hxx>
#include <TColgp_Array1OfPnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec2d.hxx>

#include <gtest/gtest.h>

TEST(Geom2dGcc_Circ2d2TanOn_Test, OCC27357_NoExceptions)
{
  // Bug OCC27357: Geom2dGcc_Circ2d2TanOn: check status of sub-algorithms to avoid exceptions
  // This test verifies that the algorithm handles edge cases without throwing exceptions

  TColgp_Array1OfPnt2d aPoles(1, 3);
  aPoles.SetValue(1, gp_Pnt2d(0., 0.));
  aPoles.SetValue(2, gp_Pnt2d(0., 1.));
  aPoles.SetValue(3, gp_Pnt2d(6., 0.));

  Handle(Geom2d_BezierCurve) aCurve1 = new Geom2d_BezierCurve(aPoles);
  aPoles.SetValue(2, gp_Pnt2d(0., 1.5));
  Handle(Geom2d_BezierCurve)         aCurve2 = new Geom2d_BezierCurve(aPoles);
  NCollection_List<Standard_Integer> aDummyList;
  int                                nP = 100;

  for (int i = 0; i < nP; i++)
  {
    Standard_Real u = i / (nP - 1.);
    gp_Pnt2d      aP1;
    gp_Vec2d      aTangent;
    aCurve1->D1(u, aP1, aTangent);
    gp_Vec2d                 aNormal(-aTangent.Y(), aTangent.X());
    Handle(Geom2d_Line)      aNormalLine = new Geom2d_Line(aP1, gp_Dir2d(aNormal));
    Geom2dGcc_QualifiedCurve aQualifiedC1(Geom2dAdaptor_Curve(aCurve1), GccEnt_unqualified);
    Geom2dGcc_QualifiedCurve aQualifiedC2(Geom2dAdaptor_Curve(aCurve2), GccEnt_unqualified);

    // This should not throw an exception even in edge cases
    EXPECT_NO_THROW({
      Geom2dAPI_ProjectPointOnCurve aProjPc1(aP1, aCurve1);
      double                        g1 = aProjPc1.LowerDistanceParameter();
      Geom2dAPI_ProjectPointOnCurve aProjPc3(aP1, aNormalLine);
      double                        g3 = aProjPc3.LowerDistanceParameter();
      Geom2dGcc_Circ2d2TanOn        aCircleBuilder(aQualifiedC1,
                                            aQualifiedC2,
                                            Geom2dAdaptor_Curve(aNormalLine),
                                            1e-9,
                                            g1,
                                            g1,
                                            g3);
      aDummyList.Append(aCircleBuilder.NbSolutions());
    });
  }

  // Test completed without exceptions
  SUCCEED();
}
