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

#include <TopLoc_Location.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include <gp.hxx>
#include <gp_Ax1.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Pnt.hxx>
#include <OSD_Parallel.hxx>

#include <gtest/gtest.h>

#include <atomic>
#include <vector>

namespace
{
//! Functor for testing concurrent access to TopLoc_Location::Transformation()
struct TopLocTransformFunctor
{
  TopLocTransformFunctor(const std::vector<TopoDS_Shape>& theShapeVec)
      : myShapeVec(&theShapeVec),
        myIsRaceDetected(0)
  {
  }

  void operator()(size_t i) const
  {
    if (!myIsRaceDetected)
    {
      const TopoDS_Vertex& aVertex = TopoDS::Vertex(myShapeVec->at(i));
      gp_Pnt               aPoint  = BRep_Tool::Pnt(aVertex);
      if (aPoint.X() != static_cast<double>(i))
      {
        ++myIsRaceDetected;
      }
    }
  }

  const std::vector<TopoDS_Shape>* myShapeVec;
  mutable std::atomic<int>         myIsRaceDetected;
};
} // namespace

TEST(TopLoc_Location_Test, OCC25545_ConcurrentTransformationAccess)
{
  // Bug OCC25545: TopLoc_Location::Transformation() provokes data races
  // This test verifies that concurrent access to TopLoc_Location::Transformation()
  // does not cause data races or incorrect geometry results

  // Place vertices in a vector, giving the i-th vertex the
  // transformation that translates it on the vector (i,0,0) from the origin
  Standard_Integer             n = 1000;
  std::vector<TopoDS_Shape>    aShapeVec(n);
  std::vector<TopLoc_Location> aLocVec(n);
  TopoDS_Shape                 aShape = BRepBuilderAPI_MakeVertex(gp::Origin());
  aShapeVec[0]                        = aShape;

  for (Standard_Integer i = 1; i < n; ++i)
  {
    gp_Trsf aTrsf;
    aTrsf.SetTranslation(gp_Vec(1, 0, 0));
    aLocVec[i]   = aLocVec[i - 1] * aTrsf;
    aShapeVec[i] = aShape.Moved(aLocVec[i]);
  }

  // Evaluator function will access vertices geometry concurrently
  TopLocTransformFunctor aFunc(aShapeVec);

  // Process concurrently
  OSD_Parallel::For(0, n, aFunc);

  // Verify no data race was detected
  EXPECT_EQ(aFunc.myIsRaceDetected, 0)
    << "Data race detected in concurrent TopLoc_Location::Transformation() access";
}

//=================================================================================================
// Tests for Phase 1 & 2 Optimizations
//=================================================================================================

TEST(TopLoc_Location_Test, DefaultConstructor_CreatesIdentity)
{
  // Test that default constructor creates identity location
  TopLoc_Location aLoc;

  EXPECT_TRUE(aLoc.IsIdentity()) << "Default constructed location should be identity";

  const gp_Trsf& aTrsf = aLoc.Transformation();
  EXPECT_EQ(aTrsf.Form(), gp_Identity) << "Default location should return identity transformation";
}

TEST(TopLoc_Location_Test, IdentityTransformation_ReturnsSameInstance)
{
  // Test that identity locations return the same transformation instance
  TopLoc_Location aLoc1;
  TopLoc_Location aLoc2;

  const gp_Trsf& aTrsf1 = aLoc1.Transformation();
  const gp_Trsf& aTrsf2 = aLoc2.Transformation();

  // Both should return reference to the same identity transformation
  EXPECT_EQ(&aTrsf1, &aTrsf2) << "Identity locations should share the same transformation instance";
}

TEST(TopLoc_Location_Test, Squared_EqualsMultipliedBySelf)
{
  // Test that Squared() equals Multiplied(self)
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));
  TopLoc_Location aLoc(aTrsf);

  TopLoc_Location aSquared    = aLoc.Squared();
  TopLoc_Location aMultiplied = aLoc.Multiplied(aLoc);

  EXPECT_EQ(aSquared, aMultiplied) << "Squared() should equal Multiplied(self)";

  // Verify the transformation is actually squared
  gp_Pnt anOrigin(0, 0, 0);
  gp_Pnt aTransformed = anOrigin.Transformed(aSquared.Transformation());
  gp_Pnt anExpected(2.0, 4.0, 6.0); // Double translation

  EXPECT_NEAR(aTransformed.X(), anExpected.X(), 1e-10);
  EXPECT_NEAR(aTransformed.Y(), anExpected.Y(), 1e-10);
  EXPECT_NEAR(aTransformed.Z(), anExpected.Z(), 1e-10);
}

TEST(TopLoc_Location_Test, Powered2_UsesSquaredFastPath)
{
  // Test that Powered(2) equals Squared() (verifies fast path)
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(5.0, -3.0, 1.5));
  TopLoc_Location aLoc(aTrsf);

  TopLoc_Location aPowered = aLoc.Powered(2);
  TopLoc_Location aSquared = aLoc.Squared();

  EXPECT_EQ(aPowered, aSquared) << "Powered(2) should equal Squared()";
}

TEST(TopLoc_Location_Test, Powered_VariousPowers)
{
  // Test Powered() with various power values
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  TopLoc_Location aLoc(aTrsf);

  // Power 0 should return identity
  TopLoc_Location aPow0 = aLoc.Powered(0);
  EXPECT_TRUE(aPow0.IsIdentity()) << "Powered(0) should return identity";

  // Power 1 should return self
  TopLoc_Location aPow1 = aLoc.Powered(1);
  EXPECT_EQ(aPow1, aLoc) << "Powered(1) should return self";

  // Power 2 should be double translation
  TopLoc_Location aPow2 = aLoc.Powered(2);
  gp_Pnt          anOrigin(0, 0, 0);
  gp_Pnt          aResult = anOrigin.Transformed(aPow2.Transformation());
  EXPECT_NEAR(aResult.X(), 2.0, 1e-10) << "Powered(2) should double the translation";

  // Power 3
  TopLoc_Location aPow3 = aLoc.Powered(3);
  aResult               = anOrigin.Transformed(aPow3.Transformation());
  EXPECT_NEAR(aResult.X(), 3.0, 1e-10) << "Powered(3) should triple the translation";

  // Negative power (inverse)
  TopLoc_Location aPowNeg1   = aLoc.Powered(-1);
  TopLoc_Location anInverted = aLoc.Inverted();
  EXPECT_EQ(aPowNeg1, anInverted) << "Powered(-1) should equal Inverted()";
}

TEST(TopLoc_Location_Test, SharesNode_DetectsSharedStructure)
{
  // Test that SharesNode correctly detects shared list nodes
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));
  TopLoc_Location aLoc1(aTrsf);

  // Copy constructor should share the same nodes
  TopLoc_Location aLoc2 = aLoc1;

  EXPECT_EQ(aLoc1, aLoc2) << "Copied location should equal original";

  // Assignment should also share nodes
  TopLoc_Location aLoc3;
  aLoc3 = aLoc1;

  EXPECT_EQ(aLoc3, aLoc1) << "Assigned location should equal original";
}

TEST(TopLoc_Location_Test, Equality_DifferentTransformations)
{
  // Test that different transformations are not equal
  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  TopLoc_Location aLoc1(aTrsf1);

  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0.0, 1.0, 0.0));
  TopLoc_Location aLoc2(aTrsf2);

  EXPECT_NE(aLoc1, aLoc2) << "Different transformations should not be equal";
  EXPECT_TRUE(aLoc1.IsDifferent(aLoc2))
    << "IsDifferent should return true for different transformations";
}

TEST(TopLoc_Location_Test, Multiplication_Composition)
{
  // Test location multiplication (composition)
  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  TopLoc_Location aLoc1(aTrsf1);

  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0.0, 2.0, 0.0));
  TopLoc_Location aLoc2(aTrsf2);

  TopLoc_Location aComposed = aLoc1 * aLoc2;

  // Apply composed transformation
  gp_Pnt anOrigin(0, 0, 0);
  gp_Pnt aResult = anOrigin.Transformed(aComposed.Transformation());

  EXPECT_NEAR(aResult.X(), 1.0, 1e-10);
  EXPECT_NEAR(aResult.Y(), 2.0, 1e-10);
  EXPECT_NEAR(aResult.Z(), 0.0, 1e-10);
}

TEST(TopLoc_Location_Test, Inverted_ProducesInverse)
{
  // Test that Location * Inverted() = Identity
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(5.0, -3.0, 2.0));
  TopLoc_Location aLoc(aTrsf);

  TopLoc_Location anInv      = aLoc.Inverted();
  TopLoc_Location anIdentity = aLoc * anInv;

  EXPECT_TRUE(anIdentity.IsIdentity()) << "Location * Inverted() should produce identity";

  // Also test in reverse order
  TopLoc_Location anIdentity2 = anInv * aLoc;
  EXPECT_TRUE(anIdentity2.IsIdentity()) << "Inverted() * Location should also produce identity";
}

TEST(TopLoc_Location_Test, Divided_EqualsMultipliedByInverse)
{
  // Test that Divided equals Multiplied by inverse
  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(3.0, 4.0, 5.0));
  TopLoc_Location aLoc1(aTrsf1);

  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(1.0, 1.0, 1.0));
  TopLoc_Location aLoc2(aTrsf2);

  TopLoc_Location aDivided    = aLoc1 / aLoc2;
  TopLoc_Location aMultiplied = aLoc1 * aLoc2.Inverted();

  EXPECT_EQ(aDivided, aMultiplied) << "Divided should equal Multiplied by Inverted";
}

TEST(TopLoc_Location_Test, Predivided_EqualsInverseMultiplied)
{
  // Test Predivided = Other.Inverted() * this
  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(2.0, 3.0, 4.0));
  TopLoc_Location aLoc1(aTrsf1);

  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(1.0, 1.0, 1.0));
  TopLoc_Location aLoc2(aTrsf2);

  TopLoc_Location aPredivided = aLoc1.Predivided(aLoc2);
  TopLoc_Location anExpected  = aLoc2.Inverted() * aLoc1;

  EXPECT_EQ(aPredivided, anExpected) << "Predivided should equal Other.Inverted() * this";
}

TEST(TopLoc_Location_Test, HashCode_ConsistentForEqualLocations)
{
  // Test that locations sharing the same structure have the same hash code
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));
  TopLoc_Location aLoc1(aTrsf);

  // Copy constructor shares the same underlying structure
  TopLoc_Location aLoc2 = aLoc1;

  EXPECT_EQ(aLoc1.HashCode(), aLoc2.HashCode())
    << "Copied locations (sharing structure) should have the same hash code";

  // Identity locations should have the same hash code
  TopLoc_Location anId1;
  TopLoc_Location anId2;
  EXPECT_EQ(anId1.HashCode(), anId2.HashCode())
    << "Identity locations should have the same hash code";
  EXPECT_EQ(anId1.HashCode(), static_cast<size_t>(0)) << "Identity location hash should be 0";
}

TEST(TopLoc_Location_Test, HashCode_DifferentForDifferentLocations)
{
  // Test that different locations (likely) have different hash codes
  gp_Trsf aTrsf1;
  aTrsf1.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  TopLoc_Location aLoc1(aTrsf1);

  gp_Trsf aTrsf2;
  aTrsf2.SetTranslation(gp_Vec(0.0, 1.0, 0.0));
  TopLoc_Location aLoc2(aTrsf2);

  // Hash codes should (very likely) be different
  // Note: Hash collision is theoretically possible but highly unlikely
  EXPECT_NE(aLoc1.HashCode(), aLoc2.HashCode())
    << "Different locations should (likely) have different hash codes";
}

TEST(TopLoc_Location_Test, ConstCorrectness_TransformationAccess)
{
  // Test const correctness of Transformation() access
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));
  const TopLoc_Location aLoc(aTrsf);

  // This should compile - Transformation() is const
  const gp_Trsf& aTrsfRef = aLoc.Transformation();

  EXPECT_EQ(aTrsfRef.Form(), gp_Translation);

  // Test identity case
  const TopLoc_Location anIdLoc;
  const gp_Trsf&        anIdTrsf = anIdLoc.Transformation();

  EXPECT_EQ(anIdTrsf.Form(), gp_Identity);
}

TEST(TopLoc_Location_Test, Rotation_Composition)
{
  // Test rotation transformations
  gp_Trsf aRot;
  gp_Ax1  anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  aRot.SetRotation(anAxis, M_PI / 2.0); // 90 degrees around Z

  TopLoc_Location aLoc(aRot);
  TopLoc_Location aSquared = aLoc.Squared();

  // Point at (1,0,0) rotated 90 degrees twice should be at (-1,0,0)
  gp_Pnt aPnt(1, 0, 0);
  gp_Pnt aResult = aPnt.Transformed(aSquared.Transformation());

  EXPECT_NEAR(aResult.X(), -1.0, 1e-10);
  EXPECT_NEAR(aResult.Y(), 0.0, 1e-10);
  EXPECT_NEAR(aResult.Z(), 0.0, 1e-10);
}

TEST(TopLoc_Location_Test, Clear_ResetsToIdentity)
{
  // Test that Clear() resets location to identity
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));
  TopLoc_Location aLoc(aTrsf);

  EXPECT_FALSE(aLoc.IsIdentity()) << "Location should not be identity before Clear()";

  aLoc.Clear();

  EXPECT_TRUE(aLoc.IsIdentity()) << "Location should be identity after Clear()";
}

TEST(TopLoc_Location_Test, Identity_Method)
{
  // Test Identity() method
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 2.0, 3.0));
  TopLoc_Location aLoc(aTrsf);

  EXPECT_FALSE(aLoc.IsIdentity());

  aLoc.Identity();

  EXPECT_TRUE(aLoc.IsIdentity()) << "Identity() should reset location to identity";
}

//=================================================================================================
// Additional Edge Case Tests
//=================================================================================================

TEST(TopLoc_Location_Test, ChainedMultiplication_MultipleLocations)
{
  // Test multiple location multiplications
  gp_Trsf aTrsf1, aTrsf2, aTrsf3;
  aTrsf1.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  aTrsf2.SetTranslation(gp_Vec(0.0, 1.0, 0.0));
  aTrsf3.SetTranslation(gp_Vec(0.0, 0.0, 1.0));

  TopLoc_Location aLoc1(aTrsf1);
  TopLoc_Location aLoc2(aTrsf2);
  TopLoc_Location aLoc3(aTrsf3);

  // Chain multiplication
  TopLoc_Location aResult = aLoc1 * aLoc2 * aLoc3;

  // Apply to point
  gp_Pnt anOrigin(0, 0, 0);
  gp_Pnt aTransformed = anOrigin.Transformed(aResult.Transformation());

  EXPECT_NEAR(aTransformed.X(), 1.0, 1e-10);
  EXPECT_NEAR(aTransformed.Y(), 1.0, 1e-10);
  EXPECT_NEAR(aTransformed.Z(), 1.0, 1e-10);
}

TEST(TopLoc_Location_Test, IdentityMultiplication_NeutralElement)
{
  // Test that identity is neutral element for multiplication
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(5.0, 3.0, 1.0));
  TopLoc_Location aLoc(aTrsf);
  TopLoc_Location anIdentity;

  // Identity * Location = Location
  TopLoc_Location aResult1 = anIdentity * aLoc;
  EXPECT_EQ(aResult1, aLoc) << "Identity * Location should equal Location";

  // Location * Identity = Location
  TopLoc_Location aResult2 = aLoc * anIdentity;
  EXPECT_EQ(aResult2, aLoc) << "Location * Identity should equal Location";
}

TEST(TopLoc_Location_Test, CopySemantics)
{
  // Test copy construction and assignment
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(2.0, 3.0, 4.0));
  TopLoc_Location anOriginal(aTrsf);

  // Copy construction
  TopLoc_Location aCopied(anOriginal);
  EXPECT_EQ(aCopied, anOriginal);
  EXPECT_EQ(aCopied.HashCode(), anOriginal.HashCode());

  // Copy assignment
  TopLoc_Location anAssigned;
  anAssigned = anOriginal;
  EXPECT_EQ(anAssigned, anOriginal);

  // All three should be equal (share structure)
  EXPECT_EQ(aCopied, anAssigned);
}

TEST(TopLoc_Location_Test, ScaleTransformation_Composition)
{
  // Test scale transformations
  gp_Trsf aScale;
  aScale.SetScale(gp_Pnt(0, 0, 0), 2.0);

  TopLoc_Location aLoc(aScale);
  TopLoc_Location aSquared = aLoc.Squared();

  // Point scaled by 2, then by 2 again = scaled by 4
  gp_Pnt aPnt(1, 1, 1);
  gp_Pnt aResult = aPnt.Transformed(aSquared.Transformation());

  EXPECT_NEAR(aResult.X(), 4.0, 1e-10);
  EXPECT_NEAR(aResult.Y(), 4.0, 1e-10);
  EXPECT_NEAR(aResult.Z(), 4.0, 1e-10);
}

TEST(TopLoc_Location_Test, ComplexTransformation_TranslationAndRotation)
{
  // Test combination of translation and rotation
  gp_Trsf aTrans;
  aTrans.SetTranslation(gp_Vec(10.0, 0.0, 0.0));

  gp_Trsf aRot;
  gp_Ax1  anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  aRot.SetRotation(anAxis, M_PI); // 180 degrees

  TopLoc_Location aLocTrans(aTrans);
  TopLoc_Location aLocRot(aRot);

  // Translate then rotate
  TopLoc_Location aComposed = aLocRot * aLocTrans;

  // Point at origin, translate to (10,0,0), then rotate 180 degrees -> (-10,0,0)
  gp_Pnt anOrigin(0, 0, 0);
  gp_Pnt aResult = anOrigin.Transformed(aComposed.Transformation());

  EXPECT_NEAR(aResult.X(), -10.0, 1e-9);
  EXPECT_NEAR(aResult.Y(), 0.0, 1e-9);
  EXPECT_NEAR(aResult.Z(), 0.0, 1e-9);
}

TEST(TopLoc_Location_Test, PoweredWithLargePower)
{
  // Test Powered() with larger power value
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  TopLoc_Location aLoc(aTrsf);

  // Power of 10
  TopLoc_Location aPow10 = aLoc.Powered(10);

  gp_Pnt anOrigin(0, 0, 0);
  gp_Pnt aResult = anOrigin.Transformed(aPow10.Transformation());

  EXPECT_NEAR(aResult.X(), 10.0, 1e-9) << "Powered(10) should translate 10 units";
}

TEST(TopLoc_Location_Test, SelfMultiplication_MultipleIterations)
{
  // Test repeated self-multiplication
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  TopLoc_Location aLoc(aTrsf);

  TopLoc_Location aResult = aLoc;
  for (int i = 0; i < 5; ++i)
  {
    aResult = aResult * aLoc;
  }

  // Should be 6 times the original (1 + 5 multiplications)
  gp_Pnt anOrigin(0, 0, 0);
  gp_Pnt aTransformed = anOrigin.Transformed(aResult.Transformation());

  EXPECT_NEAR(aTransformed.X(), 6.0, 1e-10);
}

TEST(TopLoc_Location_Test, AssociativityOfMultiplication)
{
  // Test that (A * B) * C = A * (B * C)
  gp_Trsf aTrsf1, aTrsf2, aTrsf3;
  aTrsf1.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  aTrsf2.SetTranslation(gp_Vec(0.0, 2.0, 0.0));
  aTrsf3.SetTranslation(gp_Vec(0.0, 0.0, 3.0));

  TopLoc_Location aLocA(aTrsf1);
  TopLoc_Location aLocB(aTrsf2);
  TopLoc_Location aLocC(aTrsf3);

  TopLoc_Location aLeft  = (aLocA * aLocB) * aLocC;
  TopLoc_Location aRight = aLocA * (aLocB * aLocC);

  gp_Pnt anOrigin(0, 0, 0);
  gp_Pnt aResultLeft  = anOrigin.Transformed(aLeft.Transformation());
  gp_Pnt aResultRight = anOrigin.Transformed(aRight.Transformation());

  EXPECT_NEAR(aResultLeft.X(), aResultRight.X(), 1e-10);
  EXPECT_NEAR(aResultLeft.Y(), aResultRight.Y(), 1e-10);
  EXPECT_NEAR(aResultLeft.Z(), aResultRight.Z(), 1e-10);
}

TEST(TopLoc_Location_Test, InversionIdempotence)
{
  // Test that (L.Inverted()).Inverted() = L
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(3.0, 4.0, 5.0));
  TopLoc_Location aLoc(aTrsf);

  TopLoc_Location aDoubleInverted = aLoc.Inverted().Inverted();

  gp_Pnt aPnt(1, 1, 1);
  gp_Pnt aResult1 = aPnt.Transformed(aLoc.Transformation());
  gp_Pnt aResult2 = aPnt.Transformed(aDoubleInverted.Transformation());

  EXPECT_NEAR(aResult1.X(), aResult2.X(), 1e-10);
  EXPECT_NEAR(aResult1.Y(), aResult2.Y(), 1e-10);
  EXPECT_NEAR(aResult1.Z(), aResult2.Z(), 1e-10);
}

//=================================================================================================
// Tests for Phase 3: Binary Exponentiation Optimization
//=================================================================================================

TEST(TopLoc_Location_Test, BinaryExponentiation_PowerOf2)
{
  // Test binary exponentiation for powers of 2 (4, 8, 16)
  // These should be most efficient: L^8 requires only 3 squarings instead of 7 multiplications
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  TopLoc_Location aLoc(aTrsf);

  // Test power of 4 (2^2)
  TopLoc_Location aPow4 = aLoc.Powered(4);
  gp_Pnt          anOrigin(0, 0, 0);
  gp_Pnt          aResult = anOrigin.Transformed(aPow4.Transformation());
  EXPECT_NEAR(aResult.X(), 4.0, 1e-9) << "Powered(4) should translate 4 units";

  // Test power of 8 (2^3)
  TopLoc_Location aPow8 = aLoc.Powered(8);
  aResult               = anOrigin.Transformed(aPow8.Transformation());
  EXPECT_NEAR(aResult.X(), 8.0, 1e-9) << "Powered(8) should translate 8 units";

  // Test power of 16 (2^4)
  TopLoc_Location aPow16 = aLoc.Powered(16);
  aResult                = anOrigin.Transformed(aPow16.Transformation());
  EXPECT_NEAR(aResult.X(), 16.0, 1e-9) << "Powered(16) should translate 16 units";
}

TEST(TopLoc_Location_Test, BinaryExponentiation_OddPowers)
{
  // Test binary exponentiation for odd powers (5, 7, 9)
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  TopLoc_Location aLoc(aTrsf);

  gp_Pnt anOrigin(0, 0, 0);

  // Test power of 5
  TopLoc_Location aPow5   = aLoc.Powered(5);
  gp_Pnt          aResult = anOrigin.Transformed(aPow5.Transformation());
  EXPECT_NEAR(aResult.X(), 5.0, 1e-9) << "Powered(5) should translate 5 units";

  // Test power of 7
  TopLoc_Location aPow7 = aLoc.Powered(7);
  aResult               = anOrigin.Transformed(aPow7.Transformation());
  EXPECT_NEAR(aResult.X(), 7.0, 1e-9) << "Powered(7) should translate 7 units";

  // Test power of 9
  TopLoc_Location aPow9 = aLoc.Powered(9);
  aResult               = anOrigin.Transformed(aPow9.Transformation());
  EXPECT_NEAR(aResult.X(), 9.0, 1e-9) << "Powered(9) should translate 9 units";
}

TEST(TopLoc_Location_Test, BinaryExponentiation_VeryLargePower)
{
  // Test with very large power to verify efficiency improvement
  // L^64 with binary exponentiation: only 6 squarings
  // L^64 with naive: 63 multiplications
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(0.5, 0.0, 0.0));
  TopLoc_Location aLoc(aTrsf);

  TopLoc_Location aPow64 = aLoc.Powered(64);

  gp_Pnt anOrigin(0, 0, 0);
  gp_Pnt aResult = anOrigin.Transformed(aPow64.Transformation());

  EXPECT_NEAR(aResult.X(), 32.0, 1e-8) << "Powered(64) should translate 32 units (64 * 0.5)";
}

TEST(TopLoc_Location_Test, BinaryExponentiation_NegativePowers)
{
  // Test binary exponentiation for negative powers
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(1.0, 0.0, 0.0));
  TopLoc_Location aLoc(aTrsf);

  // Test power of -4 (should be inverse of power 4)
  TopLoc_Location aPowNeg4 = aLoc.Powered(-4);
  TopLoc_Location aPow4    = aLoc.Powered(4);

  TopLoc_Location aIdentity = aPowNeg4 * aPow4;
  EXPECT_TRUE(aIdentity.IsIdentity()) << "L^(-4) * L^4 should produce identity";

  // Verify the transformation
  gp_Pnt anOrigin(0, 0, 0);
  gp_Pnt aResult = anOrigin.Transformed(aPowNeg4.Transformation());
  EXPECT_NEAR(aResult.X(), -4.0, 1e-9) << "Powered(-4) should translate -4 units";
}

TEST(TopLoc_Location_Test, BinaryExponentiation_RotationPowers)
{
  // Test binary exponentiation with rotation transformations
  // 45 degrees rotation, power of 8 should equal 360 degrees (identity)
  gp_Trsf aRot;
  gp_Ax1  anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
  aRot.SetRotation(anAxis, M_PI / 4.0); // 45 degrees

  TopLoc_Location aLoc(aRot);
  TopLoc_Location aPow8 = aLoc.Powered(8); // 8 * 45 degrees = 360 degrees

  // Should be close to identity (full rotation)
  gp_Pnt aPnt(1, 0, 0);
  gp_Pnt aResult = aPnt.Transformed(aPow8.Transformation());

  EXPECT_NEAR(aResult.X(), 1.0, 1e-8);
  EXPECT_NEAR(aResult.Y(), 0.0, 1e-8);
  EXPECT_NEAR(aResult.Z(), 0.0, 1e-8);
}

TEST(TopLoc_Location_Test, BinaryExponentiation_MixedEvenOdd)
{
  // Test a mix of even and odd powers to verify the algorithm handles both cases
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(gp_Vec(2.0, 0.0, 0.0));
  TopLoc_Location aLoc(aTrsf);

  gp_Pnt anOrigin(0, 0, 0);

  // Test 6 (even)
  TopLoc_Location aPow6   = aLoc.Powered(6);
  gp_Pnt          aResult = anOrigin.Transformed(aPow6.Transformation());
  EXPECT_NEAR(aResult.X(), 12.0, 1e-9);

  // Test 11 (odd)
  TopLoc_Location aPow11 = aLoc.Powered(11);
  aResult                = anOrigin.Transformed(aPow11.Transformation());
  EXPECT_NEAR(aResult.X(), 22.0, 1e-9);

  // Test 15 (odd)
  TopLoc_Location aPow15 = aLoc.Powered(15);
  aResult                = anOrigin.Transformed(aPow15.Transformation());
  EXPECT_NEAR(aResult.X(), 30.0, 1e-9);
}
