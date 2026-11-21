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

#include <NCollection_HArray2.hxx>
#include <StepData_Field.hxx>
#include <StepData_StepModel.hxx>
#include <StepData_StepWriter.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

TEST(StepData_FieldArrayTest, WritesIntegerRowsInDeclaredOrder)
{
  const occ::handle<NCollection_HArray2<int>> anArray =
    new NCollection_HArray2<int>(-2, -1, 4, 6);
  anArray->SetValue(-2, 4, 11);
  anArray->SetValue(-2, 5, 12);
  anArray->SetValue(-2, 6, 13);
  anArray->SetValue(-1, 4, 21);
  anArray->SetValue(-1, 5, 22);
  anArray->SetValue(-1, 6, 23);

  StepData_Field aField;
  aField.Set(anArray);
  EXPECT_EQ(aField.Lower(1), -2);
  EXPECT_EQ(aField.Lower(2), 4);
  EXPECT_EQ(aField.Length(1), 2);
  EXPECT_EQ(aField.Length(2), 3);
  EXPECT_EQ(aField.Integer(-1, 6), 23);

  const occ::handle<StepData_StepModel> aModel = new StepData_StepModel;
  StepData_StepWriter                   aWriter(aModel);
  aWriter.StartEntity("TEST");
  aWriter.SendField(aField, {});
  aWriter.EndEntity();
  EXPECT_STREQ(aWriter.Line(1)->ToCString(), "TEST(((11,12,13),(21,22,23)));");
}

TEST(StepData_FieldArrayTest, KeepsRealValuesAndEntityBounds)
{
  const occ::handle<NCollection_HArray2<double>> aReals =
    new NCollection_HArray2<double>(-1, 0, 3, 5);
  aReals->SetValue(0, 5, 2.5);
  StepData_Field aRealField;
  aRealField.Set(aReals);
  EXPECT_EQ(aRealField.Kind(), 5); // Legacy Real kind.
  EXPECT_EQ(aRealField.Lower(1), -1);
  EXPECT_EQ(aRealField.Lower(2), 3);
  EXPECT_EQ(aRealField.Length(1), 2);
  EXPECT_EQ(aRealField.Length(2), 3);
  EXPECT_DOUBLE_EQ(aRealField.Real(0, 5), 2.5);

  const occ::handle<NCollection_HArray2<occ::handle<Standard_Transient>>> anItems =
    new NCollection_HArray2<occ::handle<Standard_Transient>>(-1, 0, 3, 5);
  const occ::handle<Standard_Transient> anEntity = new Standard_Transient;
  anItems->SetValue(0, 5, anEntity);
  StepData_Field anEntityField;
  anEntityField.Set(anItems);
  EXPECT_EQ(anEntityField.Lower(1), -1);
  EXPECT_EQ(anEntityField.Lower(2), 3);
  EXPECT_EQ(anEntityField.Length(1), 2);
  EXPECT_EQ(anEntityField.Length(2), 3);
  EXPECT_EQ(anEntityField.ItemKind(-1, 3), 0);
  EXPECT_EQ(anEntityField.ItemKind(0, 5), 7); // Legacy Entity kind.
  EXPECT_EQ(anEntityField.Entity(0, 5), anEntity);
}
