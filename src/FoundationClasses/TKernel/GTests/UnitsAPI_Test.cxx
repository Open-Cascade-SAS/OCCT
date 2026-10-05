// Copyright (c) 2025 OPEN CASCADE SAS
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

#include <UnitsAPI.hxx>
#include <Units_Token.hxx>
#include <Units_UnitSentence.hxx>

#include <gtest/gtest.h>

// Test BUC60727: UnitsAPI unit conversion
// Migrated from QABugs_3.cxx
TEST(UnitsAPI_Test, BUC60727_AnyToLS_Conversion)
{
  // Set local system to MDTV (Millimeter, Degree, Ton, Velocity)
  UnitsAPI::SetLocalSystem(UnitsAPI_MDTV);

  // Test conversion: 3 mm in the MDTV system should remain 3
  // (since the base unit for length in MDTV is millimeter)
  double aResult = UnitsAPI::AnyToLS(3.0, "mm");
  EXPECT_DOUBLE_EQ(3.0, aResult);
}

// Test of fix for bug 33176: UnitsAPI::AnyToAny must not crash when one of the units is unknown.
TEST(UnitsAPI_Test, AnyToAny_UnknownUnit)
{
  // Attempt to convert using an unknown unit, should not crash and return the input value.
  const double aResult1 = UnitsAPI::AnyToAny(1.0, "unknown_unit", "mm");
  const double aResult2 = UnitsAPI::AnyToAny(1.0, "mm", "unknown_unit");
  EXPECT_DOUBLE_EQ(1.0, aResult1);
  EXPECT_DOUBLE_EQ(1.0, aResult2);
}

// fclasses/bugs/bug23497: composite units must be accepted by the SI converter.
TEST(UnitsAPI_Test, FClassesBug_23497_CompositeUnit)
{
  GTEST_SKIP() << "TODO OCC23497: composite units are not supported by UnitsAPI.";
}

// fclasses/bugs/bug30800: Poise is 0.1 Pa*s and 0.001 kg/(cm*s).
TEST(UnitsAPI_Test, FClassesBug_30800_PoiseConversion)
{
  EXPECT_NEAR(UnitsAPI::AnyToSI(1.0, "Po"), 0.1, 1.0e-12);
  EXPECT_NEAR(UnitsAPI::AnyToAny(1.0, "Po", "kg/cm/s"), 0.001, 1.0e-12);
}

TEST(UnitsAPI_Test, FClassesBug_11568_2_MinuteAlias)
{
  // fclasses/bug11568_2: the legacy "mn" minute symbol converts to 60 seconds.
  EXPECT_DOUBLE_EQ(UnitsAPI::AnyToSI(1.0, "mn"), 60.0);
}

TEST(UnitsAPI_Test, FClassesBug_11568_4_MinuteSymbol)
{
  // fclasses/bug11568_4: the standard "min" minute symbol converts to 60 seconds.
  EXPECT_DOUBLE_EQ(UnitsAPI::AnyToSI(1.0, "min"), 60.0);
}

// fclasses/bugs/bug11568_1: the legacy minute alias must be parsed as "min".
TEST(UnitsAPI_Test, FClassesBug_11568_1_MinuteAliasParsing)
{
  UnitsAPI::SetLocalSystem();
  Units_UnitSentence aSentence("mn");
  ASSERT_TRUE(aSentence.IsDone());

  const occ::handle<Units_Token> aToken = aSentence.Evaluate();
  ASSERT_FALSE(aToken.IsNull());
  EXPECT_STREQ("min", aToken->Word().ToCString());
}

// fclasses/bugs/bug11568_3: the standard minute symbol must parse unchanged.
TEST(UnitsAPI_Test, FClassesBug_11568_3_MinuteSymbolParsing)
{
  UnitsAPI::SetLocalSystem();
  Units_UnitSentence aSentence("min");
  ASSERT_TRUE(aSentence.IsDone());

  const occ::handle<Units_Token> aToken = aSentence.Evaluate();
  ASSERT_FALSE(aToken.IsNull());
  EXPECT_STREQ("min", aToken->Word().ToCString());
}

// fclasses/bugs/bug9848_1: an unknown unit name must reject the sentence.
TEST(UnitsAPI_Test, FClassesBug_9848_1_InvalidUnitSentence)
{
  UnitsAPI::SetLocalSystem();
  Units_UnitSentence aSentence("tonne/(mm*s**2)");
  EXPECT_FALSE(aSentence.IsDone());
}

// fclasses/bugs/bug9848_2: a valid composite unit must produce the DRAW token word.
TEST(UnitsAPI_Test, FClassesBug_9848_2_ValidCompositeUnitSentence)
{
  UnitsAPI::SetLocalSystem();
  Units_UnitSentence aSentence("ton/(m*s**2)");
  ASSERT_TRUE(aSentence.IsDone());

  const occ::handle<Units_Token> aToken = aSentence.Evaluate();
  ASSERT_FALSE(aToken.IsNull());
  EXPECT_STREQ("(ton)/((m)*((s)**(2)))", aToken->Word().ToCString());
}
