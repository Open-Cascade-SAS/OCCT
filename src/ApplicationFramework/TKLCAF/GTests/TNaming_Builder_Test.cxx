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

#include <gtest/gtest.h>

#include <BRepPrimAPI_MakeBox.hxx>
#include <TDocStd_Document.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>
#include <gp_Pnt.hxx>

// Test OCC361bug: TNaming_Builder preservation of shape orientation
TEST(TNaming_Builder_Test, OCC361_ShapeOrientationPreservation)
{
  // Create a document
  Handle(TDocStd_Document) aDoc = new TDocStd_Document("BinOcaf");

  // Create a box shape
  BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(0, 0, 0), 100, 100, 100);
  TopoDS_Shape        aBox = aBoxMaker.Shape();
  aBox.Orientation(TopAbs_FORWARD);

  // Get the main label
  TDF_Label aTestLabel = aDoc->Main();

  // Create a TNaming_Builder and add the shape
  TNaming_Builder aBuilder(aTestLabel);
  aBuilder.Generated(aBox);

  // Create a copy with REVERSED orientation
  TopoDS_Shape aBox1 = aBox;
  aBox1.Orientation(TopAbs_REVERSED);

  // Clear all attributes from the label
  aTestLabel.ForgetAllAttributes();

  // Create a new builder with the reversed shape
  TNaming_Builder aBuilder2(aTestLabel);
  aBuilder2.Generated(aBox1);

  // Get the shape from the builder and verify orientation
  aBox = aBuilder2.NamedShape()->Get();

  EXPECT_EQ(TopAbs_REVERSED, aBox.Orientation())
    << "Shape orientation should be preserved as REVERSED after TNaming_Builder operations";
}
