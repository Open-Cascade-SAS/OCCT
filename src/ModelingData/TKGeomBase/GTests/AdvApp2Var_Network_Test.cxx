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

#include <AdvApp2Var_Network.hxx>
#include <AdvApp2Var_Patch.hxx>
#include <NCollection_Sequence.hxx>

TEST(AdvApp2Var_NetworkTest, UpdateInV_PreservesPatchOrdersPerColumn)
{
  NCollection_Sequence<occ::handle<AdvApp2Var_Patch>> aNet;
  aNet.Append(new AdvApp2Var_Patch(0.0, 1.0, 0.0, 1.0, 0, 1));
  aNet.Append(new AdvApp2Var_Patch(1.0, 2.0, 0.0, 1.0, 2, 3));

  NCollection_Sequence<double> aUParams;
  aUParams.Append(0.0);
  aUParams.Append(1.0);
  aUParams.Append(2.0);

  NCollection_Sequence<double> aVParams;
  aVParams.Append(0.0);
  aVParams.Append(1.0);

  AdvApp2Var_Network aNetwork(aNet, aUParams, aVParams);
  aNetwork.UpdateInV(0.5);

  EXPECT_EQ(aNetwork.NbPatch(), 4);
  EXPECT_EQ(aNetwork.NbPatchInU(), 2);
  EXPECT_EQ(aNetwork.NbPatchInV(), 2);
  EXPECT_DOUBLE_EQ(aNetwork.VParameter(2), 0.5);

  const AdvApp2Var_Patch& aBottomLeft  = aNetwork.Patch(1, 1);
  const AdvApp2Var_Patch& aBottomRight = aNetwork.Patch(2, 1);
  const AdvApp2Var_Patch& aTopLeft     = aNetwork.Patch(1, 2);
  const AdvApp2Var_Patch& aTopRight    = aNetwork.Patch(2, 2);

  EXPECT_EQ(aBottomLeft.UOrder(), 0);
  EXPECT_EQ(aBottomLeft.VOrder(), 1);
  EXPECT_EQ(aBottomRight.UOrder(), 2);
  EXPECT_EQ(aBottomRight.VOrder(), 3);
  EXPECT_EQ(aTopLeft.UOrder(), 0);
  EXPECT_EQ(aTopLeft.VOrder(), 1);
  EXPECT_EQ(aTopRight.UOrder(), 2);
  EXPECT_EQ(aTopRight.VOrder(), 3);

  EXPECT_DOUBLE_EQ(aBottomLeft.V0(), 0.0);
  EXPECT_DOUBLE_EQ(aBottomLeft.V1(), 0.5);
  EXPECT_DOUBLE_EQ(aTopLeft.V0(), 0.5);
  EXPECT_DOUBLE_EQ(aTopLeft.V1(), 1.0);
}

TEST(AdvApp2Var_NetworkTest, UpdateInU_PreservesPatchOrdersPerRow)
{
  NCollection_Sequence<occ::handle<AdvApp2Var_Patch>> aNet;
  aNet.Append(new AdvApp2Var_Patch(0.0, 1.0, 0.0, 1.0, 0, 1));
  aNet.Append(new AdvApp2Var_Patch(1.0, 2.0, 0.0, 1.0, 2, 3));
  aNet.Append(new AdvApp2Var_Patch(0.0, 1.0, 1.0, 2.0, 4, 5));
  aNet.Append(new AdvApp2Var_Patch(1.0, 2.0, 1.0, 2.0, 6, 7));

  NCollection_Sequence<double> aUParams;
  aUParams.Append(0.0);
  aUParams.Append(1.0);
  aUParams.Append(2.0);

  NCollection_Sequence<double> aVParams;
  aVParams.Append(0.0);
  aVParams.Append(1.0);
  aVParams.Append(2.0);

  AdvApp2Var_Network aNetwork(aNet, aUParams, aVParams);
  aNetwork.UpdateInU(0.5);

  EXPECT_EQ(aNetwork.NbPatch(), 6);
  EXPECT_EQ(aNetwork.NbPatchInU(), 3);
  EXPECT_EQ(aNetwork.NbPatchInV(), 2);
  EXPECT_DOUBLE_EQ(aNetwork.UParameter(2), 0.5);

  const AdvApp2Var_Patch& aBottomLeft   = aNetwork.Patch(1, 1);
  const AdvApp2Var_Patch& aBottomMiddle = aNetwork.Patch(2, 1);
  const AdvApp2Var_Patch& aBottomRight  = aNetwork.Patch(3, 1);
  const AdvApp2Var_Patch& aTopLeft      = aNetwork.Patch(1, 2);
  const AdvApp2Var_Patch& aTopMiddle    = aNetwork.Patch(2, 2);
  const AdvApp2Var_Patch& aTopRight     = aNetwork.Patch(3, 2);

  EXPECT_EQ(aBottomLeft.UOrder(), 0);
  EXPECT_EQ(aBottomLeft.VOrder(), 1);
  EXPECT_EQ(aBottomMiddle.UOrder(), 0);
  EXPECT_EQ(aBottomMiddle.VOrder(), 1);
  EXPECT_EQ(aBottomRight.UOrder(), 2);
  EXPECT_EQ(aBottomRight.VOrder(), 3);

  EXPECT_EQ(aTopLeft.UOrder(), 4);
  EXPECT_EQ(aTopLeft.VOrder(), 5);
  EXPECT_EQ(aTopMiddle.UOrder(), 4);
  EXPECT_EQ(aTopMiddle.VOrder(), 5);
  EXPECT_EQ(aTopRight.UOrder(), 6);
  EXPECT_EQ(aTopRight.VOrder(), 7);

  EXPECT_DOUBLE_EQ(aBottomLeft.U0(), 0.0);
  EXPECT_DOUBLE_EQ(aBottomLeft.U1(), 0.5);
  EXPECT_DOUBLE_EQ(aBottomMiddle.U0(), 0.5);
  EXPECT_DOUBLE_EQ(aBottomMiddle.U1(), 1.0);
}
