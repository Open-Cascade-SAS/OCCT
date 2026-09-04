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

#include <StepShape_Edge.hxx>
#include <StepShape_OrientedEdge.hxx>
#include <StepShape_Vertex.hxx>
#include <Standard_NotImplemented.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

TEST(StepShape_OrientedEdgeTest, ResolvesNestedOrientation)
{
  const occ::handle<TCollection_HAsciiString> aName  = new TCollection_HAsciiString("edge");
  const occ::handle<StepShape_Vertex>         aStart = new StepShape_Vertex();
  const occ::handle<StepShape_Vertex>         anEnd  = new StepShape_Vertex();
  const occ::handle<StepShape_Edge>           anEdge = new StepShape_Edge();
  anEdge->Init(aName, aStart, anEnd);

  const occ::handle<StepShape_OrientedEdge> aReversed = new StepShape_OrientedEdge();
  aReversed->Init(aName, anEdge, false);
  const occ::handle<StepShape_OrientedEdge> aNested = new StepShape_OrientedEdge();
  aNested->Init(aName, aReversed, true);
  EXPECT_EQ(aNested->EdgeStart(), anEnd);
  EXPECT_EQ(aNested->EdgeEnd(), aStart);
}

TEST(StepShape_OrientedEdgeTest, RejectsCycles)
{
  const occ::handle<TCollection_HAsciiString> aName  = new TCollection_HAsciiString("edge");
  const occ::handle<StepShape_OrientedEdge>   anEdge = new StepShape_OrientedEdge();
  anEdge->Init(aName, anEdge, true);
  EXPECT_TRUE(anEdge->EdgeStart().IsNull());
  EXPECT_TRUE(anEdge->EdgeEnd().IsNull());

  const occ::handle<StepShape_OrientedEdge> anOther = new StepShape_OrientedEdge();
  anOther->Init(aName, anEdge, true);
  anEdge->SetEdgeElement(anOther);
  EXPECT_TRUE(anEdge->EdgeStart().IsNull());
  EXPECT_TRUE(anEdge->EdgeEnd().IsNull());
}

TEST(StepShape_OrientedEdgeTest, ResolvesDeepAcyclicNesting)
{
  const occ::handle<TCollection_HAsciiString> aName  = new TCollection_HAsciiString("edge");
  const occ::handle<StepShape_Vertex>         aStart = new StepShape_Vertex();
  const occ::handle<StepShape_Vertex>         anEnd  = new StepShape_Vertex();
  occ::handle<StepShape_Edge>                 anEdge = new StepShape_Edge();
  anEdge->Init(aName, aStart, anEnd);

  bool isReversed = false;
  for (int anEdgeIdx = 0; anEdgeIdx < 256; ++anEdgeIdx)
  {
    const bool                                isForward  = anEdgeIdx % 3 != 0;
    const occ::handle<StepShape_OrientedEdge> anOriented = new StepShape_OrientedEdge();
    anOriented->Init(aName, anEdge, isForward);
    anEdge = anOriented;
    if (!isForward)
    {
      isReversed = !isReversed;
    }
  }

  EXPECT_EQ(anEdge->EdgeStart(), isReversed ? anEnd : aStart);
  EXPECT_EQ(anEdge->EdgeEnd(), isReversed ? aStart : anEnd);
}

TEST(StepShape_OrientedEdgeTest, RejectsSettingRedefinedEndpoints)
{
  const occ::handle<StepShape_OrientedEdge> anEdge  = new StepShape_OrientedEdge();
  const occ::handle<StepShape_Vertex>       aVertex = new StepShape_Vertex();
  EXPECT_THROW(anEdge->SetEdgeStart(aVertex), Standard_NotImplemented);
  EXPECT_THROW(anEdge->SetEdgeEnd(aVertex), Standard_NotImplemented);
}
