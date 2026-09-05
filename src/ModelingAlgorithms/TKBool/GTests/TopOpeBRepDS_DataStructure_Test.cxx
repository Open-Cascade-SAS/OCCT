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
// Alternatively, this file may be used under the terms of the Open CASCADE
// commercial license or contractual agreement.

#include <gtest/gtest.h>

#include <Standard_NoSuchObject.hxx>
#include <Standard_ProgramError.hxx>
#include <TopOpeBRepDS_DataStructure.hxx>
#include <TopoDS_Shape.hxx>

TEST(TopOpeBRepDS_DataStructureTest, MissingMutableEntriesAreNotWritable)
{
  TopOpeBRepDS_DataStructure aDataStructure;

  EXPECT_THROW(aDataStructure.ChangeSurfaceInterferences(1), Standard_NoSuchObject);
  EXPECT_THROW(aDataStructure.ChangeCurveInterferences(1), Standard_NoSuchObject);
  EXPECT_THROW(aDataStructure.ChangePointInterferences(1), Standard_NoSuchObject);
  EXPECT_THROW(aDataStructure.ChangeShapeInterferences(TopoDS_Shape()), Standard_NoSuchObject);
  EXPECT_THROW(aDataStructure.ChangeSurface(1), Standard_NoSuchObject);
  EXPECT_THROW(aDataStructure.ChangeCurve(1), Standard_NoSuchObject);
  EXPECT_THROW(aDataStructure.ChangePoint(1), Standard_ProgramError);
}

TEST(TopOpeBRepDS_DataStructureTest, MissingShapeStateMapIsReportedExplicitly)
{
  TopOpeBRepDS_DataStructure aDataStructure;

  EXPECT_EQ(aDataStructure.ChangeMapOfShapeWithState(TopoDS_Shape()), nullptr);
}
