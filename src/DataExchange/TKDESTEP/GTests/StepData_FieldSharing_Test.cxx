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

#include <Interface_EntityIterator.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_HArray2.hxx>
#include <Standard_Transient.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_FieldListN.hxx>
#include <StepData_Simple.hxx>

#include <gtest/gtest.h>

TEST(StepData_FieldSharingTest, FieldListFillSharedTraversesEntityAggregates)
{
  occ::handle<Standard_Transient> anEntity1 = new Standard_Transient;
  occ::handle<Standard_Transient> anEntity2 = new Standard_Transient;
  occ::handle<Standard_Transient> anEntity3 = new Standard_Transient;
  occ::handle<Standard_Transient> anEntity4 = new Standard_Transient;

  StepData_FieldListN aFields(3);
  aFields.CField(1).SetEntity(anEntity1);

  aFields.CField(2).SetEntity();
  aFields.CField(2).SetList(2, -1);
  aFields.CField(2).SetEntity(-1, anEntity2);
  aFields.CField(2).SetEntity(0, anEntity3);

  aFields.CField(3).SetEntity();
  aFields.CField(3).SetList2(1, 1, 2, -3);
  occ::handle<NCollection_HArray2<occ::handle<Standard_Transient>>> anArray =
    occ::down_cast<NCollection_HArray2<occ::handle<Standard_Transient>>>(
      aFields.CField(3).Transient());
  ASSERT_FALSE(anArray.IsNull());
  anArray->SetValue(2, -3, anEntity4);

  Interface_EntityIterator anIterator;
  aFields.FillShared(anIterator);
  ASSERT_EQ(anIterator.NbEntities(), 4);

  anIterator.Start();
  EXPECT_EQ(anIterator.Value(), anEntity1);
  anIterator.Next();
  EXPECT_EQ(anIterator.Value(), anEntity2);
  anIterator.Next();
  EXPECT_EQ(anIterator.Value(), anEntity3);
  anIterator.Next();
  EXPECT_EQ(anIterator.Value(), anEntity4);
}

TEST(StepData_FieldSharingTest, SimpleSharedTraversesFieldsAndSkipsNullEntities)
{
  occ::handle<StepData_ESDescr> aDescription = new StepData_ESDescr("TEST_ENTITY");
  aDescription->SetNbFields(2);
  occ::handle<StepData_Simple> anEntity = new StepData_Simple(aDescription);

  occ::handle<Standard_Transient> aScalarEntity = new Standard_Transient;
  occ::handle<Standard_Transient> anArrayEntity = new Standard_Transient;
  anEntity->CFieldNum(1).SetEntity(aScalarEntity);

  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> anArray =
    new NCollection_HArray1<occ::handle<Standard_Transient>>(-1, 0);
  anArray->SetValue(-1, anArrayEntity);
  anEntity->CFieldNum(2).Set(anArray);

  Interface_EntityIterator anIterator;
  anEntity->Shared(anIterator);
  ASSERT_EQ(anIterator.NbEntities(), 2);

  anIterator.Start();
  EXPECT_EQ(anIterator.Value(), aScalarEntity);
  anIterator.Next();
  EXPECT_EQ(anIterator.Value(), anArrayEntity);
}
