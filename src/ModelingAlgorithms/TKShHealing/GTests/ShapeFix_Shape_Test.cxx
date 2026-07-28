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

#include <BRep_Builder.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <Precision.hxx>
#include <ShapeExtend_Status.hxx>
#include <ShapeFix_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

TEST(ShapeFix_ShapeTest, FixValidBox)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone());

  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape(aBox);
  aFixer->Perform();
  const TopoDS_Shape aResult = aFixer->Shape();
  ASSERT_FALSE(aResult.IsNull());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

TEST(ShapeFix_ShapeTest, StatusAfterFix)
{
  BRepPrimAPI_MakeBox aMakeBox(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox = aMakeBox.Shape();
  ASSERT_TRUE(aMakeBox.IsDone());

  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape(aBox);
  aFixer->Perform();

  // A valid box should not require any failure-level fixes
  EXPECT_FALSE(aFixer->Status(ShapeExtend_FAIL));
}

TEST(ShapeFix_ShapeTest, SetPrecision)
{
  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape();

  const double aPrecision    = 0.01;
  const double aMinTolerance = 0.001;
  const double aMaxTolerance = 0.1;

  aFixer->SetPrecision(aPrecision);
  aFixer->SetMinTolerance(aMinTolerance);
  aFixer->SetMaxTolerance(aMaxTolerance);

  EXPECT_NEAR(aFixer->Precision(), aPrecision, Precision::Confusion());
  EXPECT_NEAR(aFixer->MinTolerance(), aMinTolerance, Precision::Confusion());
  EXPECT_NEAR(aFixer->MaxTolerance(), aMaxTolerance, Precision::Confusion());
}

TEST(ShapeFix_ShapeTest, FixCompound)
{
  BRepPrimAPI_MakeBox aMakeBox1(10.0, 10.0, 10.0);
  const TopoDS_Shape& aBox1 = aMakeBox1.Shape();
  ASSERT_TRUE(aMakeBox1.IsDone());

  BRepPrimAPI_MakeBox aMakeBox2(gp_Pnt(20.0, 0.0, 0.0), 5.0, 5.0, 5.0);
  const TopoDS_Shape& aBox2 = aMakeBox2.Shape();
  ASSERT_TRUE(aMakeBox2.IsDone());

  BRep_Builder    aBuilder;
  TopoDS_Compound aCompound;
  aBuilder.MakeCompound(aCompound);
  aBuilder.Add(aCompound, aBox1);
  aBuilder.Add(aCompound, aBox2);

  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape(aCompound);
  aFixer->Perform();
  const TopoDS_Shape aResult = aFixer->Shape();
  ASSERT_FALSE(aResult.IsNull());

  BRepCheck_Analyzer anAnalyzer(aResult);
  EXPECT_TRUE(anAnalyzer.IsValid());
}

// Healing a prism built from a self-intersecting ("bowtie") face must not crash:
// the cap is split into a compound and ShapeFix_Face must not cast it to a face.
TEST(ShapeFix_ShapeTest, HealPrismFromSelfIntersectingFace)
{
  // Bowtie quad: vertices ordered so the two diagonals cross -> the wire self-intersects.
  BRepBuilderAPI_MakePolygon aPoly;
  aPoly.Add(gp_Pnt(0.0, 0.0, 0.0));
  aPoly.Add(gp_Pnt(1.0, 1.0, 0.0));
  aPoly.Add(gp_Pnt(1.0, 0.0, 0.0));
  aPoly.Add(gp_Pnt(0.0, 1.0, 0.0));
  aPoly.Close();
  ASSERT_TRUE(aPoly.IsDone());

  BRepBuilderAPI_MakeFace aMakeFace(aPoly.Wire(), true);
  ASSERT_TRUE(aMakeFace.IsDone());

  BRepPrimAPI_MakePrism aMakePrism(aMakeFace.Face(), gp_Vec(0.0, 0.0, 1.0));
  ASSERT_TRUE(aMakePrism.IsDone());

  occ::handle<ShapeFix_Shape> aFixer = new ShapeFix_Shape(aMakePrism.Shape());
  aFixer->Perform();
  ASSERT_FALSE(aFixer->Shape().IsNull());

  BRepCheck_Analyzer anAnalyzer(aFixer->Shape());
  EXPECT_TRUE(anAnalyzer.IsValid());
}
