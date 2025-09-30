#include <gtest/gtest.h>

#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>
#include <gp_Ax1.hxx>
#include <Graphic3d_BndBox3d.hxx>
#include <Graphic3d_Vec3.hxx>
#include <Graphic3d_Mat4d.hxx>
#include <Precision.hxx>

//==================================================================================================
// Constructor Tests
//==================================================================================================

TEST(Graphic3d_BndBox3dTest, DefaultConstructor)
{
  Graphic3d_BndBox3d aBox;
  EXPECT_FALSE(aBox.IsValid()) << "Default constructed box should be invalid";
}

TEST(Graphic3d_BndBox3dTest, PointConstructor)
{
  Graphic3d_Vec3d    aPnt(1.0, 2.0, 3.0);
  Graphic3d_BndBox3d aBox(aPnt);
  EXPECT_TRUE(aBox.IsValid()) << "Box constructed with 1 point should be valid";
  EXPECT_DOUBLE_EQ(1.0, aBox.CornerMin().x()) << "Xmin should match constructor input";
  EXPECT_DOUBLE_EQ(2.0, aBox.CornerMin().y()) << "Ymin should match constructor input";
  EXPECT_DOUBLE_EQ(3.0, aBox.CornerMin().z()) << "Zmin should match constructor input";
  EXPECT_DOUBLE_EQ(1.0, aBox.CornerMax().x()) << "Xmax should match constructor input";
  EXPECT_DOUBLE_EQ(2.0, aBox.CornerMax().y()) << "Ymax should match constructor input";
  EXPECT_DOUBLE_EQ(3.0, aBox.CornerMax().z()) << "Zmax should match constructor input";
}

TEST(Graphic3d_BndBox3dTest, PointsConstructor)
{
  Graphic3d_Vec3d    aMinPnt(1.0, 2.0, 3.0);
  Graphic3d_Vec3d    aMaxPnt(4.0, 5.0, 6.0);
  Graphic3d_BndBox3d aBox(aMinPnt, aMaxPnt);
  EXPECT_TRUE(aBox.IsValid()) << "Box constructed with 2 points should be valid";
  EXPECT_DOUBLE_EQ(1.0, aBox.CornerMin().x()) << "Xmin should match constructor input";
  EXPECT_DOUBLE_EQ(2.0, aBox.CornerMin().y()) << "Ymin should match constructor input";
  EXPECT_DOUBLE_EQ(3.0, aBox.CornerMin().z()) << "Zmin should match constructor input";
  EXPECT_DOUBLE_EQ(4.0, aBox.CornerMax().x()) << "Xmax should match constructor input";
  EXPECT_DOUBLE_EQ(5.0, aBox.CornerMax().y()) << "Ymax should match constructor input";
  EXPECT_DOUBLE_EQ(6.0, aBox.CornerMax().z()) << "Zmax should match constructor input";
}

//==================================================================================================
// Add and Combine Tests
//==================================================================================================

TEST(Graphic3d_BndBox3dTest, AddPoint)
{
  Graphic3d_BndBox3d aBox;
  Graphic3d_Vec3d    aPnt(1.5, 2.5, 3.5);
  aBox.Add(aPnt);
  EXPECT_TRUE(aBox.IsValid()) << "Box should be valid after Set";
  EXPECT_DOUBLE_EQ(1.5, aBox.CornerMin().x()) << "Point coordinates should set both min and max";
  EXPECT_DOUBLE_EQ(2.5, aBox.CornerMin().y()) << "Point coordinates should set both min and max";
  EXPECT_DOUBLE_EQ(3.5, aBox.CornerMin().z()) << "Point coordinates should set both min and max";
  EXPECT_DOUBLE_EQ(1.5, aBox.CornerMax().x()) << "Point coordinates should set both min and max";
  EXPECT_DOUBLE_EQ(2.5, aBox.CornerMax().y()) << "Point coordinates should set both min and max";
  EXPECT_DOUBLE_EQ(3.5, aBox.CornerMax().z()) << "Point coordinates should set both min and max";
}

TEST(Graphic3d_BndBox3dTest, CombineBoxes)
{
  Graphic3d_BndBox3d aBox1(Graphic3d_Vec3d(1.0, 2.0, 3.0), Graphic3d_Vec3d(4.0, 5.0, 6.0));
  Graphic3d_BndBox3d aBox2(Graphic3d_Vec3d(2.0, 0.0, 0.0), Graphic3d_Vec3d(6.0, 3.0, 6.0));
  aBox1.Combine(aBox2);
  EXPECT_TRUE(aBox1.IsValid()) << "Combined box should be valid";
  EXPECT_DOUBLE_EQ(1.0, aBox1.CornerMin().x()) << "Combined box should adopt min X from first box";
  EXPECT_DOUBLE_EQ(0.0, aBox1.CornerMin().y()) << "Combined box should adopt min Y from second box";
  EXPECT_DOUBLE_EQ(0.0, aBox1.CornerMin().z()) << "Combined box should adopt min Z from second box";
  EXPECT_DOUBLE_EQ(6.0, aBox1.CornerMax().x()) << "Combined box should adopt max X from second box";
  EXPECT_DOUBLE_EQ(5.0, aBox1.CornerMax().y()) << "Combined box should adopt max Y from first box";
  EXPECT_DOUBLE_EQ(6.0, aBox1.CornerMax().z()) << "Combined box should adopt max Z from either box";
}

//==================================================================================================
// Size, Center and Area Tests
//==================================================================================================

TEST(Graphic3d_BndBox3dTest, BoxSize)
{
  Graphic3d_BndBox3d aBox(Graphic3d_Vec3d(0.5, -2.0, -3.0), Graphic3d_Vec3d(3.5, 1.0, 3.0));
  Graphic3d_Vec3d    aSize = aBox.Size();
  EXPECT_DOUBLE_EQ(3.0, aSize.x()) << "Size X should be max X - min X";
  EXPECT_DOUBLE_EQ(3.0, aSize.y()) << "Size Y should be max Y - min Y";
  EXPECT_DOUBLE_EQ(6.0, aSize.z()) << "Size Z should be max Z - min Z";
}

TEST(Graphic3d_BndBox3dTest, BoxCenter)
{
  Graphic3d_BndBox3d aBox(Graphic3d_Vec3d(-4.0, -4.0, -4.0), Graphic3d_Vec3d(4.0, 4.0, 4.0));
  Graphic3d_Vec3d    aCenter = aBox.Center();
  EXPECT_DOUBLE_EQ(0.0, aCenter.x()) << "Center X should be average of min and max X";
  EXPECT_DOUBLE_EQ(0.0, aCenter.y()) << "Center Y should be average of min and max Y";
  EXPECT_DOUBLE_EQ(0.0, aCenter.z()) << "Center Z should be average of min and max Z";
}

TEST(Graphic3d_BndBox3dTest, BoxArea)
{
  Graphic3d_BndBox3d aBox(Graphic3d_Vec3d(0.0, 0.0, 0.0), Graphic3d_Vec3d(2.0, 3.0, 4.0));
  double             anArea = aBox.Area();
  EXPECT_DOUBLE_EQ(52.0, anArea) << "Area should be 2*(XY + YZ + ZX) = 52 for box 2x3x4";
}

//==================================================================================================
// Transformation Tests
//==================================================================================================

TEST(Graphic3d_BndBox3dTest, TransformationIdentity)
{
  Graphic3d_BndBox3d aBox(Graphic3d_Vec3d(0.0, 0.0, 0.0), Graphic3d_Vec3d(4.0, 5.0, 6.0));

  Graphic3d_Mat4d anIdentity;
  aBox.Transform(anIdentity); // Identity transformation applied
  EXPECT_TRUE(aBox.IsValid()) << "Transformed box should remain valid";
  EXPECT_DOUBLE_EQ(0.0, aBox.CornerMin().x()) << "Xmin should remain unchanged";
  EXPECT_DOUBLE_EQ(0.0, aBox.CornerMin().y()) << "Ymin should remain unchanged";
  EXPECT_DOUBLE_EQ(0.0, aBox.CornerMin().z()) << "Zmin should remain unchanged";
  EXPECT_DOUBLE_EQ(4.0, aBox.CornerMax().x()) << "Xmax should remain unchanged";
  EXPECT_DOUBLE_EQ(5.0, aBox.CornerMax().y()) << "Ymax should remain unchanged";
  EXPECT_DOUBLE_EQ(6.0, aBox.CornerMax().z()) << "Zmax should remain unchanged";
}

TEST(Graphic3d_BndBox3dTest, TransformationTranslation)
{
  Graphic3d_BndBox3d aBox(Graphic3d_Vec3d(0.0, 0.0, 0.0), Graphic3d_Vec3d(1.0, 1.0, 1.0));

  gp_Trsf aTranslation;
  aTranslation.SetTranslation(gp_Vec(2.0, 3.0, 4.0));
  Graphic3d_Mat4d aMat;
  aTranslation.GetMat4(aMat);

  aBox.Transform(aMat);
  EXPECT_TRUE(aBox.IsValid()) << "Transformed box should remain valid";
  EXPECT_DOUBLE_EQ(2.0, aBox.CornerMin().x()) << "Xmin should be translated";
  EXPECT_DOUBLE_EQ(3.0, aBox.CornerMin().y()) << "Ymin should be translated";
  EXPECT_DOUBLE_EQ(4.0, aBox.CornerMin().z()) << "Zmin should be translated";
  EXPECT_DOUBLE_EQ(3.0, aBox.CornerMax().x()) << "Xmax should be translated";
  EXPECT_DOUBLE_EQ(4.0, aBox.CornerMax().y()) << "Ymax should be translated";
  EXPECT_DOUBLE_EQ(5.0, aBox.CornerMax().z()) << "Zmax should be translated";
}

TEST(Graphic3d_BndBox3dTest, TransformationScale)
{
  Graphic3d_BndBox3d aBox(Graphic3d_Vec3d(1.0, 1.0, 1.0), Graphic3d_Vec3d(2.0, 2.0, 2.0));

  gp_Trsf aScale;
  aScale.SetScale(gp_Pnt(0.0, 0.0, 0.0), 2.0); // Scale by factor of 2 from origin
  Graphic3d_Mat4d aMat;
  aScale.GetMat4(aMat);

  aBox.Transform(aMat);
  EXPECT_TRUE(aBox.IsValid()) << "Transformed box should remain valid";
  EXPECT_DOUBLE_EQ(2.0, aBox.CornerMin().x()) << "Xmin should be scaled";
  EXPECT_DOUBLE_EQ(2.0, aBox.CornerMin().y()) << "Ymin should be scaled";
  EXPECT_DOUBLE_EQ(2.0, aBox.CornerMin().z()) << "Zmin should be scaled";
  EXPECT_DOUBLE_EQ(4.0, aBox.CornerMax().x()) << "Xmax should be scaled";
  EXPECT_DOUBLE_EQ(4.0, aBox.CornerMax().y()) << "Ymax should be scaled";
  EXPECT_DOUBLE_EQ(4.0, aBox.CornerMax().z()) << "Zmax should be scaled";
}

TEST(Graphic3d_BndBox3dTest, TransformationRotation)
{
  Graphic3d_BndBox3d aBox(Graphic3d_Vec3d(1.0, 0.0, 0.0), Graphic3d_Vec3d(2.0, 1.0, 1.0));

  gp_Trsf aRotation;
  aRotation.SetRotation(gp_Ax1(gp::Origin(), gp::DZ()), M_PI / 2); // 90 degrees around Z
  Graphic3d_Mat4d aMat;
  aRotation.GetMat4(aMat);

  aBox.Transform(aMat);
  EXPECT_TRUE(aBox.IsValid()) << "Transformed box should remain valid";
  Graphic3d_Vec3d aCornerMin = aBox.CornerMin();
  Graphic3d_Vec3d aCornerMax = aBox.CornerMax();

  Standard_Real aPrecision = Precision::Confusion();

  EXPECT_TRUE(Abs(aCornerMin.x() - -1) < aPrecision) << "Xmin should be rotated";
  EXPECT_TRUE(Abs(aCornerMin.y() - 1) < aPrecision) << "Ymin should be rotated";
  EXPECT_TRUE(Abs(aCornerMin.z() - 0) < aPrecision) << "Zmin should remain unchanged";
  EXPECT_TRUE(Abs(aCornerMax.x() - 0) < aPrecision) << "Xmax should be rotated";
  EXPECT_TRUE(Abs(aCornerMax.y() - 2) < aPrecision) << "Ymax should be rotated";
  EXPECT_TRUE(Abs(aCornerMax.z() - 1) < aPrecision) << "Zmax should remain unchanged";
}

TEST(Graphic3d_BndBox3dTest, TransformationComposed)
{
  Graphic3d_Vec3d aMinPnt =
    Graphic3d_Vec3d(-1.062999963760376, -1.062999963760376, -1.1150000095367432);
  Graphic3d_Vec3d aMaxPnt = Graphic3d_Vec3d(1.059000015258789, 1.062999963760376, 0);

  gp_Ax1  aRotAxis(gp_Pnt(), gp_Dir(0.6220217, 0.6836324, -0.3817536));
  gp_Trsf aTrsf;
  aTrsf.SetRotation(aRotAxis, 3.4186892);
  aTrsf.SetScaleFactor(25.4);
  aTrsf.SetTranslationPart(gp_Vec(-88.76312074860142, 51.79953807026674, -65.57836431443693));
  Graphic3d_Mat4d aMat;
  aTrsf.GetMat4(aMat);

  Graphic3d_BndBox3d aBox(aMinPnt, aMaxPnt);
  aBox.Transform(aMat);

  Graphic3d_Vec3d aResultCornerMin = aBox.CornerMin();
  Graphic3d_Vec3d aResultCornerMax = aBox.CornerMax();

  Graphic3d_Vec3d anExpectedCornerMin =
    Graphic3d_Vec3d(-113.92301586642091, 25.240619264201865, -91.49744953097915);
  Graphic3d_Vec3d anExpectedCornerMax =
    Graphic3d_Vec3d(-45.09248805040103, 87.9443412035472, -20.487612688573407);

  Standard_Real aPrecision = 0.00001; // Acceptable error for this test case
  EXPECT_TRUE(Abs(aResultCornerMin.x() - anExpectedCornerMin.x()) < aPrecision)
    << "Xmin should match expected after composed transformation";
  EXPECT_TRUE(Abs(aResultCornerMin.y() - anExpectedCornerMin.y()) < aPrecision)
    << "Ymin should match expected after composed transformation";
  EXPECT_TRUE(Abs(aResultCornerMin.z() - anExpectedCornerMin.z()) < aPrecision)
    << "Zmin should match expected after composed transformation";
  EXPECT_TRUE(Abs(aResultCornerMax.x() - anExpectedCornerMax.x()) < aPrecision)
    << "Xmax should match expected after composed transformation";
  EXPECT_TRUE(Abs(aResultCornerMax.y() - anExpectedCornerMax.y()) < aPrecision)
    << "Ymax should match expected after composed transformation";
  EXPECT_TRUE(Abs(aResultCornerMax.z() - anExpectedCornerMax.z()) < aPrecision)
    << "Zmax should match expected after composed transformation";
}

TEST(Graphic3d_BndBox3dTest, TransformationInvalidBox)
{
  Graphic3d_BndBox3d aBox; // Invalid box

  gp_Trsf aRotation;
  aRotation.SetRotation(gp_Ax1(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), M_PI / 4);
  Graphic3d_Mat4d aMat;
  aRotation.GetMat4(aMat);

  aBox.Transform(aMat);
  EXPECT_FALSE(aBox.IsValid()) << "Transformed invalid box should remain invalid";
}