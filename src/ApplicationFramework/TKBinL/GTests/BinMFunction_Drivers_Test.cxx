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

#include <BinMFunction_GraphNodeDriver.hxx>
#include <BinMFunction_ScopeDriver.hxx>
#include <BinObjMgt_Persistent.hxx>
#include <BinObjMgt_RRelocationTable.hxx>
#include <Message_Messenger.hxx>
#include <TFunction_GraphNode.hxx>
#include <TFunction_Scope.hxx>

#include <gtest/gtest.h>

// A function attribute declares how many ids follow it in the file, and the driver must not size
// memory by that before it checks the record holds that many.

TEST(BinMFunction_Drivers_Test, GraphNodeDeclaringMoreIdsThanTheRecordHolds)
{
  BinObjMgt_Persistent       aRecord;
  BinObjMgt_RRelocationTable aTable;
  aRecord.Init();
  const int aStart = aRecord.Position();
  // status, then the number of previous and of next functions
  aRecord << 0 << (1 << 20) << 0;
  aRecord.SetPosition(aStart);

  const occ::handle<TFunction_GraphNode> anAtt = new TFunction_GraphNode();
  EXPECT_FALSE(BinMFunction_GraphNodeDriver(new Message_Messenger()).Paste(aRecord, anAtt, aTable));
  EXPECT_TRUE(anAtt->GetPrevious().IsEmpty());
}

TEST(BinMFunction_Drivers_Test, ScopeDeclaringMoreFunctionsThanTheRecordHolds)
{
  BinObjMgt_Persistent       aRecord;
  BinObjMgt_RRelocationTable aTable;
  aRecord.Init();
  const int aStart = aRecord.Position();
  aRecord << (1 << 26);
  aRecord.SetPosition(aStart);

  const occ::handle<TFunction_Scope> anAtt = new TFunction_Scope();
  EXPECT_FALSE(BinMFunction_ScopeDriver(new Message_Messenger()).Paste(aRecord, anAtt, aTable));
  EXPECT_TRUE(anAtt->GetFunctions().IsEmpty());
}
