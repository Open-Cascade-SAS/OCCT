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

#include <AdvApp2Var_Iso.hxx>
#include <GeomAbs_IsoType.hxx>

TEST(AdvApp2Var_IsoTest, ConstructorWithOrders_AssignsOrdersByIsoType)
{
  const AdvApp2Var_Iso anIsoU(GeomAbs_IsoU, 3, 1);
  EXPECT_EQ(anIsoU.Type(), GeomAbs_IsoU);
  EXPECT_EQ(anIsoU.UOrder(), 3);
  EXPECT_EQ(anIsoU.VOrder(), 1);
  EXPECT_DOUBLE_EQ(anIsoU.Constante(), 0.5);
  EXPECT_DOUBLE_EQ(anIsoU.U0(), 0.0);
  EXPECT_DOUBLE_EQ(anIsoU.U1(), 1.0);
  EXPECT_DOUBLE_EQ(anIsoU.V0(), 0.0);
  EXPECT_DOUBLE_EQ(anIsoU.V1(), 1.0);
  EXPECT_DOUBLE_EQ(anIsoU.T0(), 0.0);
  EXPECT_DOUBLE_EQ(anIsoU.T1(), 1.0);
  EXPECT_EQ(anIsoU.Position(), 0);

  const AdvApp2Var_Iso anIsoV(GeomAbs_IsoV, 3, 1);
  EXPECT_EQ(anIsoV.Type(), GeomAbs_IsoV);
  EXPECT_EQ(anIsoV.UOrder(), 3);
  EXPECT_EQ(anIsoV.VOrder(), 1);
}
