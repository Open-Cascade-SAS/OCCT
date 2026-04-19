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

#include <BRepGraph_NodeId.hxx>
#include <BRepGraph_RefId.hxx>
#include <BRepGraph_RepId.hxx>

#include <gtest/gtest.h>

#include <type_traits>

TEST(BRepGraph_TypedIdDispatchTest, VisitNodeId_ConvertsToMatchingTypedId)
{
  const BRepGraph_NodeId aNodeId(BRepGraph_NodeId::Kind::Face, 4);

  bool      isFace = false;
  const int anIdx  = BRepGraph_NodeId::Visit(aNodeId, [&](const auto theTypedId) -> int {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;
    isFace       = std::is_same_v<TypeId, BRepGraph_FaceId>;
    return theTypedId.Index;
  });

  EXPECT_TRUE(isFace);
  EXPECT_EQ(anIdx, 4);
}

TEST(BRepGraph_TypedIdDispatchTest, VisitRefId_ConvertsToMatchingTypedId)
{
  const BRepGraph_RefId aRefId(BRepGraph_RefId::Kind::CoEdge, 6);

  bool      isCoEdge = false;
  const int anIdx    = BRepGraph_RefId::Visit(aRefId, [&](const auto theTypedId) -> int {
    using TypeId = std::remove_cv_t<decltype(theTypedId)>;
    isCoEdge     = std::is_same_v<TypeId, BRepGraph_CoEdgeRefId>;
    return theTypedId.Index;
  });

  EXPECT_TRUE(isCoEdge);
  EXPECT_EQ(anIdx, 6);
}

TEST(BRepGraph_TypedIdDispatchTest, VisitRepId_ConvertsToMatchingTypedId)
{
  const BRepGraph_RepId aRepId(BRepGraph_RepId::Kind::PolygonOnTri, 2);

  bool      isPolygonOnTri = false;
  const int anIdx          = BRepGraph_RepId::Visit(aRepId, [&](const auto theTypedId) -> int {
    using TypeId   = std::remove_cv_t<decltype(theTypedId)>;
    isPolygonOnTri = std::is_same_v<TypeId, BRepGraph_PolygonOnTriRepId>;
    return theTypedId.Index;
  });

  EXPECT_TRUE(isPolygonOnTri);
  EXPECT_EQ(anIdx, 2);
}
