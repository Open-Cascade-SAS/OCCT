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

#include "BOPTest_Utilities.pxx"

//==================================================================================================
// Direct BOP Operations Tests (equivalent to bcut, bfuse, bcommon, btuc commands)
//==================================================================================================

class BOPAlgo_DirectOperationsTest : public BOPAlgo_TestBase
{
};

// Test direct cut operation: bcut result sphere box
TEST_F(BOPAlgo_DirectOperationsTest, DirectCut_SphereMinusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult = PerformDirectBOP(aSphere, aBox, BOPAlgo_CUT);
  EXPECT_FALSE(aResult.IsNull()) << "Result shape should not be null";

  const double aSurfaceArea = BOPTest_Utilities::GetSurfaceArea(aResult);
  EXPECT_GT(aSurfaceArea, 0.0) << "Cut result should have positive surface area";
}

// Test direct fuse operation: bfuse result sphere box
TEST_F(BOPAlgo_DirectOperationsTest, DirectFuse_SpherePlusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult = PerformDirectBOP(aSphere, aBox, BOPAlgo_FUSE);
  EXPECT_FALSE(aResult.IsNull()) << "Result shape should not be null";

  const double aVolume       = BOPTest_Utilities::GetVolume(aResult);
  const double aSphereVolume = BOPTest_Utilities::GetVolume(aSphere);
  const double aBoxVolume    = BOPTest_Utilities::GetVolume(aBox);
  EXPECT_GT(aVolume, aSphereVolume) << "Fuse result should be larger than sphere alone";
  EXPECT_GT(aVolume, aBoxVolume) << "Fuse result should be larger than box alone";
}

// Test direct common operation: bcommon result box1 box2
TEST_F(BOPAlgo_DirectOperationsTest, DirectCommon_OverlappingBoxes)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 2.0, 2.0, 2.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(1, 1, 1), 2.0, 2.0, 2.0);

  const TopoDS_Shape aResult = PerformDirectBOP(aBox1, aBox2, BOPAlgo_COMMON);
  ValidateResult(aResult, -1.0, 1.0); // Expected volume = 1.0
}

// Test direct tuc operation: btuc result box1 box2
TEST_F(BOPAlgo_DirectOperationsTest, DirectTUC_IdenticalBoxes)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformDirectBOP(aBox1, aBox2, BOPAlgo_CUT21);
  ValidateResult(aResult, -1.0, -1.0, true); // Expected empty
}

// Test with NURBS converted shapes
TEST_F(BOPAlgo_DirectOperationsTest, DirectCut_NurbsBoxMinusBox)
{
  TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  aBox1              = BOPTest_Utilities::ConvertToNurbs(aBox1);
  EXPECT_FALSE(aBox1.IsNull()) << "Failed to convert to NURBS";

  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 1, 0), 1.0, 0.5, 1.0);

  const TopoDS_Shape aResult      = PerformDirectBOP(aBox1, aBox2, BOPAlgo_CUT);
  const double       aSurfaceArea = BOPTest_Utilities::GetSurfaceArea(aResult);
  EXPECT_GT(aSurfaceArea, 0.0) << "NURBS cut result should have positive surface area";
}

//==================================================================================================
// Two-step BOP Operations Tests (equivalent to bop + bopXXX commands)
//==================================================================================================

class BOPAlgo_TwoStepOperationsTest : public BOPAlgo_TestBase
{
};

// Test two-step cut operation: bop sphere box; bopcut result
TEST_F(BOPAlgo_TwoStepOperationsTest, TwoStepCut_SphereMinusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult      = PerformTwoStepBOP(aSphere, aBox, BOPAlgo_CUT);
  const double       aSurfaceArea = BOPTest_Utilities::GetSurfaceArea(aResult);
  EXPECT_GT(aSurfaceArea, 0.0) << "Two-step cut result should have positive surface area";
}

// Test two-step fuse operation: bop sphere box; bopfuse result
TEST_F(BOPAlgo_TwoStepOperationsTest, TwoStepFuse_SpherePlusBox)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  const TopoDS_Shape aResult = PerformTwoStepBOP(aSphere, aBox, BOPAlgo_FUSE);
  EXPECT_FALSE(aResult.IsNull()) << "Result shape should not be null";

  const double aVolume       = BOPTest_Utilities::GetVolume(aResult);
  const double aSphereVolume = BOPTest_Utilities::GetVolume(aSphere);
  const double aBoxVolume    = BOPTest_Utilities::GetVolume(aBox);
  EXPECT_GT(aVolume, aSphereVolume) << "Two-step fuse result should be larger than sphere alone";
  EXPECT_GT(aVolume, aBoxVolume) << "Two-step fuse result should be larger than box alone";
}

// Test two-step common operation: bop box1 box2; bopcommon result
TEST_F(BOPAlgo_TwoStepOperationsTest, TwoStepCommon_OverlappingBoxes)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 2.0, 2.0, 2.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(1, 1, 1), 2.0, 2.0, 2.0);

  const TopoDS_Shape aResult = PerformTwoStepBOP(aBox1, aBox2, BOPAlgo_COMMON);
  ValidateResult(aResult, -1.0, 1.0); // Expected volume = 1.0
}

// Test two-step tuc operation: bop box1 box2; boptuc result
TEST_F(BOPAlgo_TwoStepOperationsTest, TwoStepTUC_IdenticalBoxes)
{
  const TopoDS_Shape aBox1 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);
  const TopoDS_Shape aBox2 = BOPTest_Utilities::CreateBox(gp_Pnt(0, 0, 0), 1.0, 1.0, 1.0);

  const TopoDS_Shape aResult = PerformTwoStepBOP(aBox1, aBox2, BOPAlgo_CUT21);
  ValidateResult(aResult, -1.0, -1.0, true); // Expected empty
}

//==================================================================================================
// Complex Operations Tests
//==================================================================================================

class BOPAlgo_ComplexOperationsTest : public BOPAlgo_TestBase
{
};

// Test multiple intersecting primitives
TEST_F(BOPAlgo_ComplexOperationsTest, MultipleIntersectingPrimitives)
{
  const TopoDS_Shape aSphere   = BOPTest_Utilities::CreateSphere(gp_Pnt(0, 0, 0), 1.5);
  const TopoDS_Shape aCylinder = BOPTest_Utilities::CreateCylinder(0.8, 3.0);
  const TopoDS_Shape aBox = BOPTest_Utilities::CreateBox(gp_Pnt(-0.5, -0.5, -0.5), 1.0, 1.0, 1.0);

  // First intersect sphere with cylinder
  const TopoDS_Shape aIntermediate = PerformDirectBOP(aSphere, aCylinder, BOPAlgo_COMMON);
  EXPECT_FALSE(aIntermediate.IsNull()) << "Intermediate result should not be null";

  // Then fuse with box
  const TopoDS_Shape aFinalResult = PerformDirectBOP(aIntermediate, aBox, BOPAlgo_FUSE);
  const double       aVolume      = BOPTest_Utilities::GetVolume(aFinalResult);
  EXPECT_GT(aVolume, 0.0) << "Complex operation result should have positive volume";
}

// Test comparison between direct and two-step operations
TEST_F(BOPAlgo_ComplexOperationsTest, DirectVsTwoStepComparison)
{
  const TopoDS_Shape aSphere = BOPTest_Utilities::CreateUnitSphere();
  const TopoDS_Shape aBox    = BOPTest_Utilities::CreateUnitBox();

  // Perform direct operation
  const TopoDS_Shape aDirectResult = PerformDirectBOP(aSphere, aBox, BOPAlgo_FUSE);

  // Perform two-step operation
  const TopoDS_Shape aTwoStepResult = PerformTwoStepBOP(aSphere, aBox, BOPAlgo_FUSE);

  // Results should be equivalent
  const double aDirectVolume  = BOPTest_Utilities::GetVolume(aDirectResult);
  const double aTwoStepVolume = BOPTest_Utilities::GetVolume(aTwoStepResult);

  EXPECT_NEAR(aDirectVolume, aTwoStepVolume, myTolerance)
    << "Direct and two-step operations should produce equivalent results";
}

#include <BOPAlgo_PaveFiller.hxx>
#include <BOPDS_DS.hxx>
#include <BOPDS_Curve.hxx>
#include <IntTools_Curve.hxx>

namespace
{
  //! Create a pill (capsule) shape: cylinder with hemispherical ends
  //! @param theRadius radius of cylinder and hemispheres
  //! @param theCylinderLength length of the cylindrical portion (total length = theCylinderLength + 2*theRadius)
  //! @param theAxis axis along which the pill is oriented (default: Z axis)
  TopoDS_Shape CreatePill(double theRadius, double theCylinderLength, const gp_Ax2& theAxis = gp_Ax2())
  {
    // Create cylinder along axis
    BRepPrimAPI_MakeCylinder aCylMaker(theAxis, theRadius, theCylinderLength);
    TopoDS_Shape             aCylinder = aCylMaker.Shape();

    // Create bottom hemisphere (at origin along axis)
    gp_Ax2                  aBottomAxis(theAxis.Location(), theAxis.Direction().Reversed());
    BRepPrimAPI_MakeSphere  aBottomSphereMaker(aBottomAxis, theRadius, M_PI / 2.0);
    TopoDS_Shape            aBottomHemi = aBottomSphereMaker.Shape();

    // Create top hemisphere (at end of cylinder)
    gp_Pnt aTopCenter = theAxis.Location().Translated(gp_Vec(theAxis.Direction()) * theCylinderLength);
    gp_Ax2 aTopAxis(aTopCenter, theAxis.Direction());
    BRepPrimAPI_MakeSphere aTopSphereMaker(aTopAxis, theRadius, M_PI / 2.0);
    TopoDS_Shape           aTopHemi = aTopSphereMaker.Shape();

    // Fuse all parts together
    BRepAlgoAPI_Fuse aFuse1(aCylinder, aBottomHemi);
    if (!aFuse1.IsDone())
      return TopoDS_Shape();

    BRepAlgoAPI_Fuse aFuse2(aFuse1.Shape(), aTopHemi);
    if (!aFuse2.IsDone())
      return TopoDS_Shape();

    return aFuse2.Shape();
  }
}

// Test pill-pill common operation
// Two overlapping pill shapes should produce a lens-shaped intersection (4 faces)
TEST_F(BOPAlgo_ComplexOperationsTest, PillPillCommon)
{
  // Load pill shapes from brep files
  const std::string aArgPath  = "/Users/dpasukhi/work/OCCT/arg1.brep";
  const std::string aToolPath = "/Users/dpasukhi/work/OCCT/tool1.brep";

  BRep_Builder   aBuilder;
  TopoDS_Shape   aPill1, aPill2;
  BRepTools::Read(aPill1, aArgPath.c_str(), aBuilder);
  BRepTools::Read(aPill2, aToolPath.c_str(), aBuilder);
  ASSERT_FALSE(aPill1.IsNull()) << "Failed to load arg1.brep";
  ASSERT_FALSE(aPill2.IsNull()) << "Failed to load tool1.brep";

  std::cout << "Loaded two pill shapes for Boolean Common operation" << std::endl;

  // Perform Common operation
  BRepAlgoAPI_Common aCommon(aPill1, aPill2);
  aCommon.SetFuzzyValue(1.0e-6);
  aCommon.Build();
  ASSERT_TRUE(aCommon.IsDone()) << "Common operation failed";

  const TopoDS_Shape aResult = aCommon.Shape();
  ASSERT_FALSE(aResult.IsNull()) << "Result shape is null";

  // Count faces, edges, and solids using explorer
  int aNbFaces = 0, aNbEdges = 0, aNbSolids = 0;
  for (TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next())
    ++aNbFaces;
  for (TopExp_Explorer anExp(aResult, TopAbs_EDGE); anExp.More(); anExp.Next())
    ++aNbEdges;
  for (TopExp_Explorer anExp(aResult, TopAbs_SOLID); anExp.More(); anExp.Next())
    ++aNbSolids;

  std::cout << "Result: " << aNbSolids << " solids, "
            << aNbFaces << " faces, "
            << aNbEdges << " edges" << std::endl;

  // Expected: lens shape with 4 faces (2 from each pill's cylinder)
  // The intersection should produce a lens bounded by 2 arcs from each cylinder
  EXPECT_EQ(aNbSolids, 1) << "Expected 1 solid";
  EXPECT_EQ(aNbFaces, 4) << "Expected 4 faces (lens shape)";

  // Verify positive volume
  const double aVolume = BOPTest_Utilities::GetVolume(aResult);
  EXPECT_GT(aVolume, 0.0) << "Result should have positive volume";
}

TEST_F(BOPAlgo_ComplexOperationsTest, OwnTest)
{
  std::string aFolderPath = "/Users/dpasukhi/Downloads/";
  // std::string anArgPath   = aFolderPath + "arg1.brep";
  // std::string aToolPath   = aFolderPath + "tool1.brep";
  // std::string aCommonPath = aFolderPath + "common.brep";

  // BRep_Builder aBuilder;
  // TopoDS_Shape anArgShape;
  // BRepTools::Read(anArgShape, anArgPath.c_str(), aBuilder);
  // TopoDS_Shape aToolShape;
  // BRepTools::Read(aToolShape, aToolPath.c_str(), aBuilder);

  // BRepAlgoAPI_Common aCommon;
  // TopTools_ListOfShape anArgShapes;
  // anArgShapes.Append(anArgShape);
  // aCommon.SetArguments(anArgShapes);
  // TopTools_ListOfShape aToolShapes;
  // aToolShapes.Append(aToolShape);
  // aCommon.SetTools(aToolShapes);
  // if (theArgc >= 2)
  // {
  //   int aFuzzyExp = Draw::Atoi(theArgv[1]);
  //   if (aFuzzyExp <= 0)
  //     aFuzzyExp = 1;
  //   const double aFuzzyValue = std::pow(10.0, -aFuzzyExp);
  //   aCommon.SetFuzzyValue(aFuzzyValue);
  // }
  // else
  // {
  //   aCommon.SetFuzzyValue(1.0e-6);
  // }
  // aCommon.Build();
  // if (!aCommon.IsDone())
  // {
  //   std::cout << "Common is not done\n";
  //   return 1;
  // }
  // // ==========================================
  // // Write common shape
  // // ==========================================
  // TopoDS_Shape aCommonShape = aCommon.Shape();
  // if (aCommonShape.IsNull())
  // {
  //   std::cout << "Common shape is null\n";
  //   return 1;
  // }
  // BRepTools::Write(aCommonShape, aCommonPath.c_str());
  // // Print statistics
  // BOPAlgo_PaveFiller& aPaveFiller = *aCommon.DSFiller();

  std::string  anArgPath = aFolderPath + "FF_0_Face1.brep";
  std::string  aToolPath = aFolderPath + "FF_0_Face2.brep";
  BRep_Builder aBuilder;
  TopoDS_Shape anArgShape;
  BRepTools::Read(anArgShape, anArgPath.c_str(), aBuilder);
  TopoDS_Shape aToolShape;
  BRepTools::Read(aToolShape, aToolPath.c_str(), aBuilder);
  NCollection_List<TopoDS_Shape> anArgShapes;
  anArgShapes.Append(anArgShape);
  anArgShapes.Append(aToolShape);
  BOPAlgo_PaveFiller aPaveFiller;
  aPaveFiller.SetArguments(anArgShapes);
  aPaveFiller.SetFuzzyValue(1.0e-6);
  aPaveFiller.Perform();

  BOPDS_DS&                                 aDS       = const_cast<BOPDS_DS&>(aPaveFiller.DS());
  const NCollection_Vector<BOPDS_InterfVV>& aVVInterf = aDS.InterfVV();
  std::cout << "Number of VV intersections: " << aVVInterf.Length() << "\n";
  const NCollection_Vector<BOPDS_InterfEE>& aEEInterf = aDS.InterfEE();
  std::cout << "Number of EE intersections: " << aEEInterf.Length() << "\n";
  const NCollection_Vector<BOPDS_InterfVE>& aVEInterf = aDS.InterfVE();
  std::cout << "Number of VE intersections: " << aVEInterf.Length() << "\n";
  const NCollection_Vector<BOPDS_InterfEF>& aEFInterf = aDS.InterfEF();
  std::cout << "Number of EF intersections: " << aEFInterf.Length() << "\n";
  const NCollection_Vector<BOPDS_InterfFF>& aFFInterf = aDS.InterfFF();
  std::cout << "Number of FF intersections: " << aFFInterf.Length() << "\n";
  const NCollection_Vector<BOPDS_InterfVF>& aVFInterf = aDS.InterfVF();
  std::cout << "Number of VF intersections: " << aVFInterf.Length() << "\n";

  //==========================================
  // Edfe-Edge intersections
  //==========================================
  std::set<std::pair<int, int>> aWrittenEEElements;
  // Write EE interference shapes to files
  for (int i = 0; i < aEEInterf.Length(); ++i)
  {
    const BOPDS_InterfEE& anEEInt    = aEEInterf(i);
    int                   anEdge1Idx = 0;
    int                   anEdge2Idx = 0;
    anEEInt.Indices(anEdge1Idx, anEdge2Idx);

    if (aWrittenEEElements.emplace(anEdge1Idx, anEdge2Idx).second == false)
    {
      // This EE interference has already been written
      continue;
    }

    const TopoDS_Shape& anEdge1 = aDS.Shape(anEdge1Idx);
    const TopoDS_Shape& anEdge2 = aDS.Shape(anEdge2Idx);

    std::string anEdge1File = aFolderPath + "EE_" + std::to_string(i) + "_Edge1.brep";
    std::string anEdge2File = aFolderPath + "EE_" + std::to_string(i) + "_Edge2.brep";

    BRepTools::Write(anEdge1, anEdge1File.c_str());
    BRepTools::Write(anEdge2, anEdge2File.c_str());

    std::cout << "Written EE interference " << i << ": Edge1 index " << anEdge1Idx << ", Edge2 index "
          << anEdge2Idx << "\n";
  }

  //==========================================
  // Edge-Face intersections
  //==========================================
  std::set<std::pair<int, int>> aWrittenEFElements;
  // Write EF interference shapes to files
  for (int i = 0; i < aEFInterf.Length(); ++i)
  {
    const BOPDS_InterfEF& anEFInt   = aEFInterf(i);
    int                   anEdgeIdx = 0;
    int                   aFaceIdx  = 0;
    anEFInt.Indices(anEdgeIdx, aFaceIdx);

    if (aWrittenEFElements.emplace(anEdgeIdx, aFaceIdx).second == false)
    {
      // This EF interference has already been written
      continue;
    }

    const TopoDS_Shape& anEdge = aDS.Shape(anEdgeIdx);
    const TopoDS_Shape& aFace  = aDS.Shape(aFaceIdx);

    std::string anEdgeFile = aFolderPath + "EF_" + std::to_string(i) + "_Edge.brep";
    std::string aFaceFile  = aFolderPath + "EF_" + std::to_string(i) + "_Face.brep";

    BRepTools::Write(anEdge, anEdgeFile.c_str());
    BRepTools::Write(aFace, aFaceFile.c_str());

    std::cout << "Written EF interference " << i << ": Edge index " << anEdgeIdx << ", Face index "
          << aFaceIdx << "\n";
  }

  //==========================================
  // Face-Face intersections
  //==========================================
  for (int i = 0; i < aFFInterf.Length(); ++i)
  {
    const BOPDS_InterfFF& anFFInt   = aFFInterf(i);
    int                   aFace1Idx = 0;
    int                   aFace2Idx = 0;
    anFFInt.Indices(aFace1Idx, aFace2Idx);

    const TopoDS_Shape& aFace1 = aDS.Shape(aFace1Idx);
    const TopoDS_Shape& aFace2 = aDS.Shape(aFace2Idx);

    std::string aFace1File = aFolderPath + "FF_" + std::to_string(i) + "_Face1.brep";
    std::string aFace2File = aFolderPath + "FF_" + std::to_string(i) + "_Face2.brep";

    BRepTools::Write(aFace1, aFace1File.c_str());
    BRepTools::Write(aFace2, aFace2File.c_str());

    std::cout << "Written FF interference " << i << ": Face1 index " << aFace1Idx << ", Face2 index "
          << aFace2Idx;

    // Check if faces are tangent
    if (anFFInt.TangentFaces())
    {
      std::cout << " (tangent)";
    }

    // Report number of intersection curves and points
    std::cout << ", Curves: " << anFFInt.Curves().Length() << ", Points: " << anFFInt.Points().Length()
          << "\n";

    // Print detailed information for each intersection curve
    const NCollection_Vector<BOPDS_Curve>& aCurves = anFFInt.Curves();
    for (int c = 0; c < aCurves.Length(); ++c)
    {
      const BOPDS_Curve&    aBOPDSCurve = aCurves(c);
      const IntTools_Curve& aCurve      = aBOPDSCurve.Curve();

      std::cout << "  Curve " << c << ": ";

      // Print curve type
      GeomAbs_CurveType aCurveType = aCurve.Type();
      switch (aCurveType)
      {
        case GeomAbs_Line:
          std::cout << "Line";
          break;
        case GeomAbs_Circle:
          std::cout << "Circle";
          break;
        case GeomAbs_Ellipse:
          std::cout << "Ellipse";
          break;
        case GeomAbs_Hyperbola:
          std::cout << "Hyperbola";
          break;
        case GeomAbs_Parabola:
          std::cout << "Parabola";
          break;
        case GeomAbs_BezierCurve:
          std::cout << "BezierCurve";
          break;
        case GeomAbs_BSplineCurve:
          std::cout << "BSplineCurve";
          break;
        case GeomAbs_OffsetCurve:
          std::cout << "OffsetCurve";
          break;
        case GeomAbs_OtherCurve:
          std::cout << "OtherCurve";
          break;
        default:
          std::cout << "Unknown";
          break;
      }

      // Print tolerance
      std::cout << ", Tolerance: " << aBOPDSCurve.Tolerance()
            << ", Tangential Tolerance: " << aCurve.TangentialTolerance();

      // Print bounds if available
      double aFirst = 0.0;
      double aLast  = 0.0;
      if (aCurve.HasBounds())
      {
        gp_Pnt aFirstPnt;
        gp_Pnt aLastPnt;
        if (aCurve.Bounds(aFirst, aLast, aFirstPnt, aLastPnt))
        {
          std::cout << ", Bounds: [" << aFirst << ", " << aLast << "]";
          std::cout << ", First point: (" << aFirstPnt.X() << ", " << aFirstPnt.Y() << ", "
                << aFirstPnt.Z() << ")";
          std::cout << ", Last point: (" << aLastPnt.X() << ", " << aLastPnt.Y() << ", " << aLastPnt.Z()
                << ")";
        }
      }

      // Create and write TopoDS_Edge from the curve
      const Handle(Geom_Curve)& a3dCurve = aCurve.Curve();
      if (!a3dCurve.IsNull())
      {
        TopoDS_Edge anEdge;
        if (aCurve.HasBounds() && aFirst != aLast)
        {
          BRepBuilderAPI_MakeEdge anEdgeMaker(a3dCurve, aFirst, aLast);
          if (anEdgeMaker.IsDone())
          {
            anEdge = anEdgeMaker.Edge();
          }
        }
        else
        {
          BRepBuilderAPI_MakeEdge anEdgeMaker(a3dCurve);
          if (anEdgeMaker.IsDone())
          {
            anEdge = anEdgeMaker.Edge();
          }
        }

        if (!anEdge.IsNull())
        {
          std::string aCurveFile =
            aFolderPath + "FF_" + std::to_string(i) + "_Curve" + std::to_string(c) + ".brep";
          BRepTools::Write(anEdge, aCurveFile.c_str());
          std::cout << ", Written to: FF_" << i << "_Curve" << c << ".brep";
        }
      }

      std::cout << "\n";
    }
  }
}