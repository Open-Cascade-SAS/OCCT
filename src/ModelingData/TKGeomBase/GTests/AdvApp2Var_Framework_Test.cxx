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

#include <AdvApp2Var_Framework.hxx>
#include <AdvApp2Var_Iso.hxx>
#include <AdvApp2Var_Node.hxx>
#include <GeomAbs_IsoType.hxx>
#include <NCollection_Sequence.hxx>
#include <gp_XY.hxx>

namespace
{
using AdvApp2VarIsoHandle = occ::handle<AdvApp2Var_Iso>;
using AdvApp2VarIsoStrip  = NCollection_Sequence<AdvApp2VarIsoHandle>;
using AdvApp2VarIsoGrid   = NCollection_Sequence<AdvApp2VarIsoStrip>;

AdvApp2VarIsoGrid makeUFrontier()
{
  AdvApp2VarIsoGrid aUFrontier;

  AdvApp2VarIsoStrip aUStrip1;
  aUStrip1.Append(new AdvApp2Var_Iso(GeomAbs_IsoV, 30.0, 0.0, 1.0, 0.0, 1.0, 0, 0, 0));
  aUStrip1.Append(new AdvApp2Var_Iso(GeomAbs_IsoV, 40.0, 0.0, 1.0, 0.0, 1.0, 0, 0, 0));
  aUFrontier.Append(aUStrip1);

  AdvApp2VarIsoStrip aUStrip2;
  aUStrip2.Append(new AdvApp2Var_Iso(GeomAbs_IsoV, 30.0, 1.0, 2.0, 0.0, 1.0, 0, 0, 0));
  aUStrip2.Append(new AdvApp2Var_Iso(GeomAbs_IsoV, 40.0, 1.0, 2.0, 0.0, 1.0, 0, 0, 0));
  aUFrontier.Append(aUStrip2);

  return aUFrontier;
}

AdvApp2VarIsoGrid makeVFrontier()
{
  AdvApp2VarIsoGrid aVFrontier;

  AdvApp2VarIsoStrip aVStrip1;
  aVStrip1.Append(new AdvApp2Var_Iso(GeomAbs_IsoU, 10.0, 0.0, 1.0, 0.0, 1.0, 0, 0, 0));
  aVStrip1.Append(new AdvApp2Var_Iso(GeomAbs_IsoU, 20.0, 0.0, 1.0, 0.0, 1.0, 0, 0, 0));
  aVFrontier.Append(aVStrip1);

  AdvApp2VarIsoStrip aVStrip2;
  aVStrip2.Append(new AdvApp2Var_Iso(GeomAbs_IsoU, 10.0, 0.0, 1.0, 1.0, 2.0, 0, 0, 0));
  aVStrip2.Append(new AdvApp2Var_Iso(GeomAbs_IsoU, 20.0, 0.0, 1.0, 1.0, 2.0, 0, 0, 0));
  aVFrontier.Append(aVStrip2);

  return aVFrontier;
}

NCollection_Sequence<occ::handle<AdvApp2Var_Node>> makeNodeGrid()
{
  NCollection_Sequence<occ::handle<AdvApp2Var_Node>> aNodes;
  aNodes.Append(new AdvApp2Var_Node(gp_XY(0.0, 0.0), 0, 0));
  aNodes.Append(new AdvApp2Var_Node(gp_XY(1.0, 0.0), 0, 0));
  aNodes.Append(new AdvApp2Var_Node(gp_XY(0.0, 1.0), 0, 0));
  aNodes.Append(new AdvApp2Var_Node(gp_XY(1.0, 1.0), 0, 0));
  return aNodes;
}
} // namespace

TEST(AdvApp2Var_FrameworkTest, IsoLookup_ReturnsMatchingIso)
{
  const NCollection_Sequence<occ::handle<AdvApp2Var_Node>> aNodes = makeNodeGrid();
  const AdvApp2Var_Framework aFramework(aNodes, makeUFrontier(), makeVFrontier());

  const AdvApp2Var_Iso& anIsoU = aFramework.IsoU(20.0, 0.0, 1.0);
  EXPECT_EQ(anIsoU.Type(), GeomAbs_IsoU);
  EXPECT_DOUBLE_EQ(anIsoU.Constante(), 20.0);
  EXPECT_DOUBLE_EQ(anIsoU.T0(), 0.0);
  EXPECT_DOUBLE_EQ(anIsoU.T1(), 1.0);

  const AdvApp2Var_Iso& anIsoV = aFramework.IsoV(1.0, 2.0, 40.0);
  EXPECT_EQ(anIsoV.Type(), GeomAbs_IsoV);
  EXPECT_DOUBLE_EQ(anIsoV.Constante(), 40.0);
  EXPECT_DOUBLE_EQ(anIsoV.T0(), 1.0);
  EXPECT_DOUBLE_EQ(anIsoV.T1(), 2.0);
}

TEST(AdvApp2Var_FrameworkTest, NodeIndexMapping_UsesGridLayout)
{
  const NCollection_Sequence<occ::handle<AdvApp2Var_Node>> aNodes = makeNodeGrid();
  const AdvApp2Var_Framework aFramework(aNodes, makeUFrontier(), makeVFrontier());

  EXPECT_EQ(aFramework.FirstNode(GeomAbs_IsoU, 2, 1), 2);
  EXPECT_EQ(aFramework.LastNode(GeomAbs_IsoU, 2, 1), 5);

  EXPECT_EQ(aFramework.FirstNode(GeomAbs_IsoV, 2, 1), 4);
  EXPECT_EQ(aFramework.LastNode(GeomAbs_IsoV, 2, 1), 5);
}

TEST(AdvApp2Var_FrameworkTest, UpdateInU_InsertsCutIsoAndNodes)
{
  const NCollection_Sequence<occ::handle<AdvApp2Var_Node>> aNodes = makeNodeGrid();
  AdvApp2Var_Framework aFramework(aNodes, makeUFrontier(), makeVFrontier());
  aFramework.UpdateInU(0.5);

  const AdvApp2Var_Iso& aCutIso = aFramework.IsoU(0.5, 0.0, 1.0);
  EXPECT_EQ(aCutIso.Type(), GeomAbs_IsoU);
  EXPECT_DOUBLE_EQ(aCutIso.Constante(), 0.5);
  EXPECT_DOUBLE_EQ(aCutIso.T0(), 0.0);
  EXPECT_DOUBLE_EQ(aCutIso.T1(), 1.0);

  const occ::handle<AdvApp2Var_Node>& aBottomCutNode = aFramework.Node(0.5, 0.0);
  const occ::handle<AdvApp2Var_Node>& aTopCutNode    = aFramework.Node(0.5, 1.0);
  ASSERT_FALSE(aBottomCutNode.IsNull());
  ASSERT_FALSE(aTopCutNode.IsNull());
  EXPECT_DOUBLE_EQ(aBottomCutNode->Coord().X(), 0.5);
  EXPECT_DOUBLE_EQ(aBottomCutNode->Coord().Y(), 0.0);
  EXPECT_DOUBLE_EQ(aTopCutNode->Coord().X(), 0.5);
  EXPECT_DOUBLE_EQ(aTopCutNode->Coord().Y(), 1.0);

  EXPECT_EQ(aFramework.FirstNode(GeomAbs_IsoU, 2, 1), 2);
  EXPECT_EQ(aFramework.LastNode(GeomAbs_IsoU, 2, 1), 6);
}
