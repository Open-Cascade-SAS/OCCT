// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCone.hxx>
#include <BRepAlgoAPI_Fuse.hxx>
#include <STEPControl_Writer.hxx>
#include <TopExp_Explorer.hxx>
#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#define ENABLE_DEBUG_STEP_OUTPUT 0

//==================================================================================================

class BFusePolarPatternTest : public BRepAlgoAPI_TestBase
{
protected:
  //! Create a tapered cone along the X-axis
  TopoDS_Shape makeCone()
  {
    const gp_Ax2 aConeAxis(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
    const double aBaseRadius = 6.0;
    const double aTopRadius = 3.0;
    const double aHeight = 40.0;

    BRepPrimAPI_MakeCone aConeMaker(aConeAxis, aBaseRadius, aTopRadius, aHeight);
    return aConeMaker.Shape();
  }

  //! Export shape to STEP; return success status
  bool exportToSTEP(const TopoDS_Shape& theShape, const char* theFilename)
  {
    if (theShape.IsNull())
    {
      return false;
    }

    STEPControl_Writer aWriter;
    IFSelect_ReturnStatus aStatus1 = aWriter.Transfer(theShape, STEPControl_AsIs);
    if (aStatus1 != IFSelect_RetDone)
    {
      return false;
    }

    IFSelect_ReturnStatus aStatus2 = aWriter.Write(theFilename);
    return (aStatus2 == IFSelect_RetDone);
  }

  //! Count the number of solids in a shape
  int countSolids(const TopoDS_Shape& theShape)
  {
    if (theShape.IsNull())
    {
      return 0;
    }

    int aCount = 0;
    for (TopExp_Explorer anExp(theShape, TopAbs_SOLID); anExp.More(); anExp.Next())
    {
      aCount++;
    }
    return aCount;
  }

  //! Get surface area of a shape
  double getSurfaceArea(const TopoDS_Shape& theShape)
  {
    return BOPTest_Utilities::GetSurfaceArea(theShape);
  }

  //! Create a polar pattern result by sequentially fusing rotated cones
  TopoDS_Shape createPatternResult(int theOccurrences)
  {
    // Start with the base cone
    const TopoDS_Shape aBaseShape = makeCone();
    if (aBaseShape.IsNull())
    {
      return TopoDS_Shape();
    }

    TopoDS_Shape aResult = aBaseShape;
    const double anAngleStep = 360.0 / theOccurrences;

    // Sequentially fuse rotated copies
    for (int i = 1; i < theOccurrences; ++i)
    {
      const TopoDS_Shape aRotated = BOPTest_Utilities::RotateZ(aBaseShape, anAngleStep * i);
      BRepAlgoAPI_Fuse aFuser(aResult, aRotated);
      if (!aFuser.IsDone())
        return TopoDS_Shape();
      aResult = aFuser.Shape();
    }

    return aResult;
  }
};

//==================================================================================================

const double EXPECTED_SURFACE_AREA_ONE_SHAPE = 1275.5214;

//==================================================================================================

TEST_F(BFusePolarPatternTest, OneOccurrences0Degrees)
{
  const TopoDS_Shape aResult = createPatternResult(1);
  
  EXPECT_FALSE(aResult.IsNull());
  
  int aSolidCount = countSolids(aResult);
  EXPECT_EQ(1, aSolidCount);
  
  double aSurfaceArea = getSurfaceArea(aResult);
  EXPECT_NEAR(aSurfaceArea, EXPECTED_SURFACE_AREA_ONE_SHAPE, 1.0e-4);

#if (ENABLE_DEBUG_STEP_OUTPUT)
  const bool aExportSuccess = exportToSTEP(aResult, "PolarPattern_1occ_0deg.step");
  EXPECT_TRUE(aExportSuccess);
#endif
}

//==================================================================================================

TEST_F(BFusePolarPatternTest, TwoOccurrences180Degrees)
{
  const TopoDS_Shape aResult = createPatternResult(2);
  
  EXPECT_FALSE(aResult.IsNull());
  
  int aSolidCount = countSolids(aResult);
  EXPECT_EQ(1, aSolidCount);
  
  double aSurfaceArea = getSurfaceArea(aResult);
  // Two occurrences should have a higher surface area than one
  EXPECT_GT(aSurfaceArea, (EXPECTED_SURFACE_AREA_ONE_SHAPE * 1));

#if (ENABLE_DEBUG_STEP_OUTPUT)
  const bool aExportSuccess = exportToSTEP(aResult, "PolarPattern_2occ_180deg.step");
  EXPECT_TRUE(aExportSuccess);
#endif
}

//==================================================================================================

TEST_F(BFusePolarPatternTest, ThreeOccurrences120Degrees)
{
  const TopoDS_Shape aResult = createPatternResult(3);
  
  EXPECT_FALSE(aResult.IsNull());
  
  int aSolidCount = countSolids(aResult);
  EXPECT_EQ(1, aSolidCount);
  
  double aSurfaceArea = getSurfaceArea(aResult);
  // Three occurrences should have a higher surface area than two
  EXPECT_GT(aSurfaceArea, (EXPECTED_SURFACE_AREA_ONE_SHAPE * 2));

#if (ENABLE_DEBUG_STEP_OUTPUT)
  const bool aExportSuccess = exportToSTEP(aResult, "PolarPattern_3occ_120deg.step");
  EXPECT_TRUE(aExportSuccess);
#endif
}

//==================================================================================================

TEST_F(BFusePolarPatternTest, FourOccurrences90Degrees)
{
  const TopoDS_Shape aResult = createPatternResult(4);
  
  EXPECT_FALSE(aResult.IsNull());
  
  int aSolidCount = countSolids(aResult);
  EXPECT_EQ(1, aSolidCount);
  
  double aSurfaceArea = getSurfaceArea(aResult);
  // Four occurrences should have a higher surface area than three including some tolerance for fusion simplification
  EXPECT_GT(aSurfaceArea, (EXPECTED_SURFACE_AREA_ONE_SHAPE * 3) * 0.80 );

#if (ENABLE_DEBUG_STEP_OUTPUT)
  const bool aExportSuccess = exportToSTEP(aResult, "PolarPattern_4occ_90deg.step");
  EXPECT_TRUE(aExportSuccess);
#endif
}

//==================================================================================================

TEST_F(BFusePolarPatternTest, FiveOccurrences72Degrees)
{
  const TopoDS_Shape aResult = createPatternResult(5);
  
  EXPECT_FALSE(aResult.IsNull());
  
  int aSolidCount = countSolids(aResult);
  EXPECT_EQ(1, aSolidCount);
  
  double aSurfaceArea = getSurfaceArea(aResult);
  // Five occurrences should have a higher surface area than four including some tolerance for fusion simplification
  EXPECT_GT(aSurfaceArea, (EXPECTED_SURFACE_AREA_ONE_SHAPE * 4) * 0.80 );

#if (ENABLE_DEBUG_STEP_OUTPUT)
  const bool aExportSuccess = exportToSTEP(aResult, "PolarPattern_5occ_72deg.step");
  EXPECT_TRUE(aExportSuccess);
#endif
}

//==================================================================================================

TEST_F(BFusePolarPatternTest, SixOccurrences60Degrees)
{
  const TopoDS_Shape aResult = createPatternResult(6);
  
  EXPECT_FALSE(aResult.IsNull());
  
  int aSolidCount = countSolids(aResult);
  EXPECT_EQ(1, aSolidCount);
  
  double aSurfaceArea = getSurfaceArea(aResult);
  // Six occurrences should have a higher surface area than five including some tolerance for fusion simplification
  EXPECT_GT(aSurfaceArea, (EXPECTED_SURFACE_AREA_ONE_SHAPE * 5) * 0.80 );

#if (ENABLE_DEBUG_STEP_OUTPUT)
  const bool aExportSuccess = exportToSTEP(aResult, "PolarPattern_6occ_60deg.step");
  EXPECT_TRUE(aExportSuccess);
#endif
}

//==================================================================================================

TEST_F(BFusePolarPatternTest, SevenOccurrences51Degrees)
{
  const TopoDS_Shape aResult = createPatternResult(7);
  
  EXPECT_FALSE(aResult.IsNull());
  
  int aSolidCount = countSolids(aResult);
  EXPECT_EQ(1, aSolidCount);
  
  double aSurfaceArea = getSurfaceArea(aResult);
  // Seven occurrences should have a higher surface area than six including some tolerance for fusion simplification
  EXPECT_GT(aSurfaceArea, (EXPECTED_SURFACE_AREA_ONE_SHAPE * 6) * 0.80);

#if (ENABLE_DEBUG_STEP_OUTPUT)
  const bool aExportSuccess = exportToSTEP(aResult, "PolarPattern_7occ_51deg.step");
  EXPECT_TRUE(aExportSuccess);
#endif
}

