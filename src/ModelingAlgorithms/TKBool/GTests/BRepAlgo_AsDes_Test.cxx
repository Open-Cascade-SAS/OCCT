// Copyright (c) 2026 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in the OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

#include <gtest/gtest.h>

#include <BRepAlgo_AsDes.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

TEST(BRepAlgo_AsDesTest, ChangeDescendantCreatesIndependentLists)
{
  BRepAlgo_AsDes     aRelations;
  const TopoDS_Shape aFirstShape  = BRepPrimAPI_MakeBox(1.0, 2.0, 3.0).Shape();
  const TopoDS_Shape aSecondShape = BRepPrimAPI_MakeBox(2.0, 3.0, 4.0).Shape();
  const TopoDS_Shape aDescendant  = BRepPrimAPI_MakeBox(3.0, 4.0, 5.0).Shape();

  NCollection_List<TopoDS_Shape>& aFirstDescendants  = aRelations.ChangeDescendant(aFirstShape);
  NCollection_List<TopoDS_Shape>& aSecondDescendants = aRelations.ChangeDescendant(aSecondShape);
  aFirstDescendants.Append(aDescendant);

  EXPECT_EQ(aFirstDescendants.Extent(), 1);
  EXPECT_TRUE(aSecondDescendants.IsEmpty());
  EXPECT_TRUE(aRelations.HasDescendant(aFirstShape));
  EXPECT_TRUE(aRelations.HasDescendant(aSecondShape));
  EXPECT_EQ(aRelations.Descendant(aFirstShape).Extent(), 1);
  EXPECT_TRUE(aRelations.Descendant(aFirstShape).First().IsSame(aDescendant));
  EXPECT_TRUE(aRelations.Descendant(aSecondShape).IsEmpty());
}
