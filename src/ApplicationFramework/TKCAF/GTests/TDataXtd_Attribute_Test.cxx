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

#include <gtest/gtest.h>

#include <TDataXtd_Axis.hxx>
#include <TDataXtd_Geometry.hxx>
#include <TDataXtd_Plane.hxx>
#include <TDataXtd_Point.hxx>
#include <TDataXtd_Shape.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <TDocStd_Document.hxx>
#include <TDF_Label.hxx>
#include <gp_Ax1.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

namespace
{
static occ::handle<TDocStd_Document> NewDocument()
{
  occ::handle<TDocStd_Document> aDocument = new TDocStd_Document("BinOcaf");
  aDocument->SetUndoLimit(10);
  return aDocument;
}

static TDF_Label NewGeometryLabel(const occ::handle<TDocStd_Document>& theDocument)
{
  return theDocument->Main().FindChild(2, true);
}

static void ExpectPoint(const gp_Pnt& theActual, const gp_Pnt& theExpected)
{
  EXPECT_TRUE(theActual.IsEqual(theExpected, 1.0e-12));
}

static void ExpectAxis(const gp_Ax1& theActual, const gp_Ax1& theExpected)
{
  EXPECT_TRUE(theActual.IsCoaxial(theExpected, 1.0e-12, 1.0e-12));
}

static void ExpectPlane(const gp_Pln& theActual, const gp_Pln& theExpected)
{
  double anActualA   = 0.0;
  double anActualB   = 0.0;
  double anActualC   = 0.0;
  double anActualD   = 0.0;
  double anExpectedA = 0.0;
  double anExpectedB = 0.0;
  double anExpectedC = 0.0;
  double anExpectedD = 0.0;
  theActual.Coefficients(anActualA, anActualB, anActualC, anActualD);
  theExpected.Coefficients(anExpectedA, anExpectedB, anExpectedC, anExpectedD);
  EXPECT_NEAR(anActualA, anExpectedA, 1.0e-12);
  EXPECT_NEAR(anActualB, anExpectedB, 1.0e-12);
  EXPECT_NEAR(anActualC, anExpectedC, 1.0e-12);
  EXPECT_NEAR(anActualD, anExpectedD, 1.0e-12);
}
} // namespace

// caf/basic/G1: point geometry is stored as a construction shape.
TEST(TDataXtd_Attribute_Test, CafBasic_G1_PointSetGet)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewGeometryLabel(aDoc);
  const gp_Pnt                  anExpected(10.0, 20.0, 30.0);
  ASSERT_FALSE(TDataXtd_Point::Set(aLabel, anExpected).IsNull());

  gp_Pnt anActual;
  ASSERT_TRUE(TDataXtd_Geometry::Point(aLabel, anActual));
  ExpectPoint(anActual, anExpected);
}

// caf/basic/G2: setting a new point replaces the construction geometry.
TEST(TDataXtd_Attribute_Test, CafBasic_G2_PointUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewGeometryLabel(aDoc);
  const gp_Pnt                  anExpected(10.0, 20.0, 30.0);
  const gp_Pnt                  aReplacement(-10.0, -20.0, -30.0);
  TDataXtd_Point::Set(aLabel, anExpected);
  aDoc->NewCommand();
  TDataXtd_Point::Set(aLabel, aReplacement);
  aDoc->NewCommand();
  aDoc->Undo();

  gp_Pnt anActual;
  ASSERT_TRUE(TDataXtd_Geometry::Point(aLabel, anActual));
  ExpectPoint(anActual, anExpected);
}

// caf/basic/H1: axis construction geometry is retrievable from its label.
TEST(TDataXtd_Attribute_Test, CafBasic_H1_AxisSetGet)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewGeometryLabel(aDoc);
  const gp_Lin                  anExpected(gp_Pnt(10.0, 20.0, 30.0), gp_Dir(100.0, 200.0, 300.0));
  ASSERT_FALSE(TDataXtd_Axis::Set(aLabel, anExpected).IsNull());

  gp_Ax1 anActual;
  ASSERT_TRUE(TDataXtd_Geometry::Axis(aLabel, anActual));
  ExpectAxis(anActual, anExpected.Position());
}

TEST(TDataXtd_Attribute_Test, CafBasic_H2_AxisUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewGeometryLabel(aDoc);
  const gp_Lin                  anExpected(gp_Pnt(10.0, 20.0, 30.0), gp_Dir(100.0, 200.0, 300.0));
  const gp_Lin aReplacement(gp_Pnt(-10.0, -20.0, -30.0), gp_Dir(-100.0, 200.0, 300.0));
  TDataXtd_Axis::Set(aLabel, anExpected);
  aDoc->NewCommand();
  TDataXtd_Axis::Set(aLabel, aReplacement);
  aDoc->NewCommand();
  aDoc->Undo();

  gp_Ax1 anActual;
  ASSERT_TRUE(TDataXtd_Geometry::Axis(aLabel, anActual));
  ExpectAxis(anActual, anExpected.Position());
}

// caf/basic/I1-I2: plane construction geometry is retrievable and undoable.
TEST(TDataXtd_Attribute_Test, CafBasic_I1_PlaneSetGet)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewGeometryLabel(aDoc);
  const gp_Pln                  anExpected(gp_Pnt(10.0, 20.0, 30.0), gp_Dir(-1.0, 0.0, 0.0));
  ASSERT_FALSE(TDataXtd_Plane::Set(aLabel, anExpected).IsNull());

  gp_Pln anActual;
  ASSERT_TRUE(TDataXtd_Geometry::Plane(aLabel, anActual));
  ExpectPlane(anActual, anExpected);
}

TEST(TDataXtd_Attribute_Test, CafBasic_I2_PlaneUndo)
{
  occ::handle<TDocStd_Document> aDoc   = NewDocument();
  const TDF_Label               aLabel = NewGeometryLabel(aDoc);
  const gp_Pln                  anExpected(gp_Pnt(10.0, 20.0, 30.0), gp_Dir(-1.0, 0.0, 0.0));
  const gp_Pln                  aReplacement(gp_Pnt(-10.0, -20.0, -30.0), gp_Dir(0.0, -1.0, 0.0));
  TDataXtd_Plane::Set(aLabel, anExpected);
  aDoc->NewCommand();
  TDataXtd_Plane::Set(aLabel, aReplacement);
  aDoc->NewCommand();
  aDoc->Undo();

  gp_Pln anActual;
  ASSERT_TRUE(TDataXtd_Geometry::Plane(aLabel, anActual));
  ExpectPlane(anActual, anExpected);
}

// caf/named_shape/B1: NewShape stores and restores an associated shape.
TEST(TDataXtd_Attribute_Test, CafNamedShape_B1_NewShapeUndoRedo)
{
  occ::handle<TDocStd_Document> aDoc = NewDocument();
  TDF_Label                    aLabel = NewGeometryLabel(aDoc);
  const TopoDS_Shape           aBox   = BRepPrimAPI_MakeBox(100.0, 200.0, 300.0).Shape();

  aDoc->NewCommand();
  ASSERT_FALSE(TDataXtd_Shape::Set(aLabel, aBox).IsNull());
  aDoc->NewCommand();
  aDoc->Undo();
  occ::handle<TDataXtd_Shape> anAttribute;
  aLabel = aDoc->Main().FindChild(2, false);
  EXPECT_FALSE(TDataXtd_Shape::Find(aLabel, anAttribute));

  aDoc->Redo();
  aLabel = aDoc->Main().FindChild(2, false);
  ASSERT_TRUE(TDataXtd_Shape::Find(aLabel, anAttribute));
  EXPECT_TRUE(TDataXtd_Shape::Get(aLabel).IsSame(aBox));
  aDoc->ClearUndos();
  aDoc->ClearRedos();
}
