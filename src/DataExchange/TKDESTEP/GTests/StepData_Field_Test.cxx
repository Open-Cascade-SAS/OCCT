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

#include <StepData_Field.hxx>

#include <Interface_EntityIterator.hxx>
#include <NCollection_HArray1.hxx>
#include <NCollection_HArray2.hxx>
#include <Standard_RangeError.hxx>
#include <Standard_Transient.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_FieldListN.hxx>
#include <StepData_PDescr.hxx>
#include <StepData_SelectArrReal.hxx>
#include <StepData_SelectInt.hxx>
#include <StepData_SelectNamed.hxx>
#include <StepData_SelectReal.hxx>
#include <StepData_Simple.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

TEST(StepData_FieldTest, ScalarValuesKeepTheirKinds)
{
  StepData_Field aField;

  aField.SetInteger(42);
  EXPECT_EQ(aField.Kind(), StepData_Field::FieldKind::Integer);
  EXPECT_EQ(aField.Integer(), 42);

  aField.SetBoolean(true);
  EXPECT_EQ(aField.Kind(), StepData_Field::FieldKind::Boolean);
  EXPECT_TRUE(aField.Boolean());

  aField.SetLogical(StepData_LUnknown);
  EXPECT_EQ(aField.Kind(), StepData_Field::FieldKind::Logical);
  EXPECT_EQ(aField.Logical(), StepData_LUnknown);

  aField.SetEnum(4, ".FOUR.");
  EXPECT_EQ(aField.Kind(), StepData_Field::FieldKind::Enum);
  EXPECT_EQ(aField.Enum(), 4);
  EXPECT_STREQ(aField.EnumText(), ".FOUR.");

  aField.SetString(".OTHER.");
  EXPECT_EQ(aField.Enum(), 4);
  EXPECT_STREQ(aField.EnumText(), ".OTHER.");
  EXPECT_TRUE(occ::is_kind<TCollection_HAsciiString>(aField.Transient()));

  aField.SetString("text");
  EXPECT_STREQ(aField.String(), "text");
  EXPECT_TRUE(occ::is_kind<TCollection_HAsciiString>(aField.Transient()));
}

TEST(StepData_FieldTest, AggregateBoundsArePreserved)
{
  occ::handle<NCollection_HArray1<int>> anArray = new NCollection_HArray1<int>(-2, 0);
  anArray->SetValue(-2, 10);
  anArray->SetValue(-1, 20);
  anArray->SetValue(0, 30);

  StepData_Field aField;
  aField.Set(anArray);

  EXPECT_EQ(aField.Kind(), StepData_Field::FieldKind::Integer);
  EXPECT_EQ(aField.Arity(), 1);
  EXPECT_EQ(aField.Lower(), -2);
  EXPECT_EQ(aField.Length(), 3);
  EXPECT_EQ(aField.ItemKind(-2), StepData_Field::FieldKind::Integer);
  EXPECT_EQ(aField.Integer(-2), 10);
  EXPECT_EQ(aField.Integer(0), 30);
}

TEST(StepData_FieldTest, ItemKindReadsOneDimensionalAnyList)
{
  StepData_Field aField;
  aField.SetList(4, -2);

  occ::handle<NCollection_HArray1<occ::handle<Standard_Transient>>> anArray =
    occ::down_cast<NCollection_HArray1<occ::handle<Standard_Transient>>>(aField.Transient());
  ASSERT_FALSE(anArray.IsNull());

  occ::handle<StepData_SelectInt> aSelect = new StepData_SelectInt;
  aSelect->SetKind(static_cast<int>(StepData_Field::FieldKind::Integer));
  occ::handle<Standard_Transient> anEntity = new Standard_Transient;
  anArray->SetValue(-1, new TCollection_HAsciiString("text"));
  anArray->SetValue(0, aSelect);
  anArray->SetValue(1, anEntity);

  EXPECT_EQ(aField.ItemKind(-2), StepData_Field::FieldKind::Undefined);
  EXPECT_EQ(aField.ItemKind(-1), StepData_Field::FieldKind::String);
  EXPECT_EQ(aField.ItemKind(0), StepData_Field::FieldKind::Integer);
  EXPECT_EQ(aField.ItemKind(1), StepData_Field::FieldKind::Entity);
}

TEST(StepData_FieldTest, ItemKindReadsTwoDimensionalAnyList)
{
  StepData_Field aField;
  aField.SetList2(1, 3, -1, 2);

  occ::handle<NCollection_HArray2<occ::handle<Standard_Transient>>> anArray =
    occ::down_cast<NCollection_HArray2<occ::handle<Standard_Transient>>>(aField.Transient());
  ASSERT_FALSE(anArray.IsNull());

  occ::handle<Standard_Transient> anEntity = new Standard_Transient;
  anArray->SetValue(-1, 3, new TCollection_HAsciiString("text"));
  anArray->SetValue(-1, 4, anEntity);

  EXPECT_EQ(aField.ItemKind(-1, 2), StepData_Field::FieldKind::Undefined);
  EXPECT_EQ(aField.ItemKind(-1, 3), StepData_Field::FieldKind::String);
  EXPECT_EQ(aField.ItemKind(-1, 4), StepData_Field::FieldKind::Entity);
}

TEST(StepData_FieldTest, TwoDimensionalEntityArrayUsesRowAndColumnBounds)
{
  occ::handle<NCollection_HArray2<occ::handle<Standard_Transient>>> anArray =
    new NCollection_HArray2<occ::handle<Standard_Transient>>(-1, 0, 3, 5);
  occ::handle<Standard_Transient> anEntity = new Standard_Transient;
  anArray->SetValue(0, 5, anEntity);

  StepData_Field aField;
  aField.Set(anArray);

  EXPECT_EQ(aField.Lower(1), -1);
  EXPECT_EQ(aField.Lower(2), 3);
  EXPECT_EQ(aField.Length(1), 2);
  EXPECT_EQ(aField.Length(2), 3);
  EXPECT_EQ(aField.ItemKind(0, 5), StepData_Field::FieldKind::Entity);
  EXPECT_EQ(aField.Entity(0, 5), anEntity);
}

TEST(StepData_FieldTest, ItemKindHandlesAnyKindWithIntegerArrays)
{
  occ::handle<NCollection_HArray1<int>> anArray1 = new NCollection_HArray1<int>(1, 1);
  StepData_Field                        aField1;
  aField1.Clear(StepData_Field::FieldKind::Any);
  aField1.Set(anArray1);
  EXPECT_EQ(aField1.ItemKind(1), StepData_Field::FieldKind::Any);

  occ::handle<NCollection_HArray2<int>> anArray2 = new NCollection_HArray2<int>(1, 1, 1, 1);
  StepData_Field                        aField2;
  aField2.Clear(StepData_Field::FieldKind::Any);
  aField2.Set(anArray2);
  EXPECT_EQ(aField2.ItemKind(1, 1), StepData_Field::FieldKind::Any);
}

TEST(StepData_FieldTest, TwoDimensionalAggregateUsesRowAndColumnBounds)
{
  occ::handle<NCollection_HArray2<double>> anArray = new NCollection_HArray2<double>(-1, 0, 3, 5);
  anArray->SetValue(-1, 3, 1.5);
  anArray->SetValue(0, 5, 2.5);

  StepData_Field aField;
  aField.Set(anArray);

  EXPECT_EQ(aField.Kind(), StepData_Field::FieldKind::Real);
  EXPECT_EQ(aField.Arity(), 2);
  EXPECT_EQ(aField.Lower(1), -1);
  EXPECT_EQ(aField.Lower(2), 3);
  EXPECT_EQ(aField.Length(1), 2);
  EXPECT_EQ(aField.Length(2), 3);
  EXPECT_DOUBLE_EQ(aField.Real(-1, 3), 1.5);
  EXPECT_DOUBLE_EQ(aField.Real(0, 5), 2.5);
}

TEST(StepData_FieldTest, TwoDimensionalIntegerArrayPreservesValues)
{
  occ::handle<NCollection_HArray2<int>> anArray = new NCollection_HArray2<int>(-2, -1, 4, 5);
  anArray->SetValue(-2, 4, 10);
  anArray->SetValue(-1, 5, 20);

  StepData_Field aField;
  aField.Set(anArray);

  EXPECT_EQ(aField.Kind(), StepData_Field::FieldKind::Integer);
  EXPECT_EQ(aField.Lower(1), -2);
  EXPECT_EQ(aField.Lower(2), 4);
  EXPECT_EQ(aField.Length(1), 2);
  EXPECT_EQ(aField.Length(2), 2);
  EXPECT_EQ(aField.Integer(-2, 4), 10);
  EXPECT_EQ(aField.Integer(-1, 5), 20);
}

TEST(StepData_FieldTest, TwoDimensionalStringListKeepsTransientRepresentation)
{
  StepData_Field aField;
  aField.SetString();
  aField.SetList2(2, 3, -1, 4);

  EXPECT_EQ(aField.Kind(), StepData_Field::FieldKind::String);
  EXPECT_EQ(aField.Lower(1), -1);
  EXPECT_EQ(aField.Lower(2), 4);
  EXPECT_TRUE(
    occ::is_kind<NCollection_HArray2<occ::handle<Standard_Transient>>>(aField.Transient()));
}

TEST(StepData_FieldTest, CopyPreservesValueOwnership)
{
  StepData_Field aStringField;
  aStringField.SetString("text");

  StepData_Field aStringCopy;
  aStringCopy.CopyFrom(aStringField);
  EXPECT_STREQ(aStringCopy.String(), "text");
  EXPECT_NE(aStringCopy.Transient(), aStringField.Transient());

  occ::handle<Standard_Transient> anEntity = new Standard_Transient;
  StepData_Field                  anEntityField;
  anEntityField.SetEntity(anEntity);

  StepData_Field anEntityCopy;
  anEntityCopy.CopyFrom(anEntityField);
  EXPECT_EQ(anEntityCopy.Entity(), anEntity);
}

TEST(StepData_FieldTest, CopyPreservesListOwnership)
{
  StepData_Field aSource;
  aSource.SetInteger();
  aSource.SetList(2, -1);
  aSource.SetInteger(-1, 10);
  aSource.SetInteger(0, 20);

  StepData_Field aCopy;
  aCopy.CopyFrom(aSource);

  EXPECT_NE(aCopy.Transient(), aSource.Transient());
  EXPECT_EQ(aCopy.Lower(), -1);
  EXPECT_EQ(aCopy.Integer(-1), 10);
  EXPECT_EQ(aCopy.Integer(0), 20);

  aSource.SetInteger(-1, 30);
  EXPECT_EQ(aCopy.Integer(-1), 10);
}

TEST(StepData_FieldTest, CopyPreservesSparseStringList)
{
  StepData_Field aSource;
  aSource.SetString();
  aSource.SetList(2, -1);
  aSource.SetString(0, "text");

  StepData_Field aCopy;
  aCopy.CopyFrom(aSource);

  EXPECT_FALSE(aCopy.IsSet(-1));
  EXPECT_TRUE(aCopy.IsSet(0));
  EXPECT_STREQ(aCopy.String(0), "text");
  EXPECT_NE(aCopy.Transient(), aSource.Transient());
  aSource.SetString(0, "changed");
  EXPECT_STREQ(aCopy.String(0), "text");
}

TEST(StepData_FieldTest, CopyPreservesSelectValues)
{
  occ::handle<StepData_SelectInt> anIntegerSelect = new StepData_SelectInt;
  anIntegerSelect->SetKind(static_cast<int>(StepData_Field::FieldKind::Logical));
  anIntegerSelect->SetInt(static_cast<int>(StepData_LUnknown));
  StepData_Field anIntegerSource;
  anIntegerSource.SetSelectMember(anIntegerSelect);

  StepData_Field anIntegerCopy;
  anIntegerCopy.CopyFrom(anIntegerSource);
  occ::handle<StepData_SelectInt> aCopiedInteger =
    occ::down_cast<StepData_SelectInt>(anIntegerCopy.Transient());
  ASSERT_FALSE(aCopiedInteger.IsNull());
  EXPECT_NE(aCopiedInteger, anIntegerSelect);
  EXPECT_EQ(aCopiedInteger->Kind(), static_cast<int>(StepData_Field::FieldKind::Logical));
  EXPECT_EQ(aCopiedInteger->Logical(), StepData_LUnknown);

  occ::handle<StepData_SelectReal> aRealSelect = new StepData_SelectReal;
  aRealSelect->SetReal(4.5);
  StepData_Field aRealSource;
  aRealSource.SetSelectMember(aRealSelect);

  StepData_Field aRealCopy;
  aRealCopy.CopyFrom(aRealSource);
  occ::handle<StepData_SelectReal> aCopiedReal =
    occ::down_cast<StepData_SelectReal>(aRealCopy.Transient());
  ASSERT_FALSE(aCopiedReal.IsNull());
  EXPECT_NE(aCopiedReal, aRealSelect);
  EXPECT_DOUBLE_EQ(aCopiedReal->Real(), 4.5);
}

TEST(StepData_FieldTest, ClearItemClearsStringAndEntityLists)
{
  StepData_Field aStringField;
  aStringField.SetString();
  aStringField.SetList(1);
  aStringField.SetString(1, "text");
  ASSERT_TRUE(aStringField.IsSet(1));
  aStringField.ClearItem(1);
  EXPECT_FALSE(aStringField.IsSet(1));

  StepData_Field anEntityField;
  anEntityField.SetEntity();
  anEntityField.SetList(1);
  anEntityField.SetEntity(1, new Standard_Transient);
  ASSERT_TRUE(anEntityField.IsSet(1));
  anEntityField.ClearItem(1);
  EXPECT_FALSE(anEntityField.IsSet(1));
}

TEST(StepData_FieldTest, LogicalSelectPreservesValue)
{
  occ::handle<StepData_SelectInt> aSelect = new StepData_SelectInt;
  StepData_Field                  aField;
  aField.SetSelectMember(aSelect);

  aField.SetLogical(StepData_LFalse);
  EXPECT_EQ(aSelect->Kind(), static_cast<int>(StepData_Field::FieldKind::Logical));
  EXPECT_EQ(aSelect->Logical(), StepData_LFalse);

  aField.SetLogical(StepData_LTrue);
  EXPECT_EQ(aSelect->Logical(), StepData_LTrue);

  aField.SetLogical(StepData_LUnknown);
  EXPECT_EQ(aSelect->Logical(), StepData_LUnknown);
}

TEST(StepData_FieldTest, LogicalListPreservesAllValues)
{
  StepData_Field aField;
  aField.SetLogical();
  aField.SetList(3, -1);
  aField.SetLogical(-1, StepData_LFalse);
  aField.SetLogical(0, StepData_LTrue);
  aField.SetLogical(1, StepData_LUnknown);

  EXPECT_EQ(aField.Logical(-1), StepData_LFalse);
  EXPECT_EQ(aField.Logical(0), StepData_LTrue);
  EXPECT_EQ(aField.Logical(1), StepData_LUnknown);
}

TEST(StepData_FieldTest, FieldListFillSharedTraversesEntityAggregates)
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

TEST(StepData_FieldTest, FieldListNValidatesFieldCount)
{
  EXPECT_EQ(StepData_FieldListN(0).NbFields(), 0);
  EXPECT_EQ(StepData_FieldListN(3).NbFields(), 3);
  EXPECT_THROW(StepData_FieldListN(-1), Standard_RangeError);
}

TEST(StepData_FieldTest, SimpleSharedTraversesFieldsAndSkipsNullEntities)
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

TEST(StepData_FieldTest, PDescrSetFromPreservesFieldDefinition)
{
  occ::handle<StepData_PDescr> aSource = new StepData_PDescr;
  aSource->SetEnum();
  aSource->AddEnumDef(".FIRST.");
  aSource->AddEnumDef(".SECOND.");
  aSource->SetArity(2);
  aSource->SetOptional();
  aSource->SetDerived();
  aSource->SetField("value", 3);

  occ::handle<StepData_PDescr> aCopy = new StepData_PDescr;
  aCopy->SetFrom(aSource);

  EXPECT_TRUE(aCopy->IsEnum());
  EXPECT_EQ(aCopy->EnumMax(), 1);
  EXPECT_STREQ(aCopy->EnumText(0), ".FIRST.");
  EXPECT_STREQ(aCopy->EnumText(1), ".SECOND.");
  EXPECT_EQ(aCopy->Arity(), 2);
  EXPECT_TRUE(aCopy->IsOptional());
  EXPECT_TRUE(aCopy->IsDerived());
  EXPECT_TRUE(aCopy->IsField());
  EXPECT_STREQ(aCopy->FieldName(), "value");
  EXPECT_EQ(aCopy->FieldRank(), 3);
}

TEST(StepData_FieldTest, SelectMemberUnsupportedKindHasMiscParameterType)
{
  occ::handle<StepData_SelectInt> aSelect = new StepData_SelectInt;
  aSelect->SetKind(257);
  EXPECT_EQ(aSelect->ParamType(), Interface_ParamMisc);
}

TEST(StepData_FieldTest, SelectRealStoresRealValue)
{
  occ::handle<StepData_SelectReal> aSelect = new StepData_SelectReal;
  aSelect->SetReal(3.25);
  EXPECT_EQ(aSelect->Kind(), static_cast<int>(StepData_Field::FieldKind::Real));
  EXPECT_DOUBLE_EQ(aSelect->Real(), 3.25);
}

TEST(StepData_FieldTest, SelectNamedDelegatesValueToField)
{
  occ::handle<StepData_SelectNamed> aSelect = new StepData_SelectNamed;
  aSelect->SetName("VALUE");
  aSelect->SetKind(static_cast<int>(StepData_Field::FieldKind::Integer));
  aSelect->SetInt(7);

  EXPECT_TRUE(aSelect->HasName());
  EXPECT_STREQ(aSelect->Name(), "VALUE");
  EXPECT_EQ(aSelect->Kind(), static_cast<int>(StepData_Field::FieldKind::Integer));
  EXPECT_EQ(aSelect->Int(), 7);

  aSelect->SetKind(261);
  EXPECT_EQ(aSelect->Kind(), static_cast<int>(StepData_Field::FieldKind::Undefined));
}

TEST(StepData_FieldTest, SelectArrRealPreservesArrayBounds)
{
  occ::handle<NCollection_HArray1<double>> anArray = new NCollection_HArray1<double>(-2, 0);
  anArray->SetValue(-2, 1.0);
  occ::handle<StepData_SelectArrReal> aSelect = new StepData_SelectArrReal;
  aSelect->SetArrReal(anArray);

  EXPECT_EQ(aSelect->Kind(), static_cast<int>(StepData_Field::FieldKind::Any));
  ASSERT_FALSE(aSelect->ArrReal().IsNull());
  EXPECT_EQ(aSelect->ArrReal()->Lower(), -2);
  EXPECT_DOUBLE_EQ(aSelect->ArrReal()->Value(-2), 1.0);
}
