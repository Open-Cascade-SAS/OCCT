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

#include <GeomFill_SectionGenerator.hxx>
#include <gtest/gtest.h>
#include <Standard_NullObject.hxx>
#include <Standard_ConstructionError.hxx>
#include <limits>

TEST(GeomFill_SectionGenerator, ParameterArrayOwnershipAndBounds)
{
  for (const int aLower : {-3, 0, 1, 4})
  {
    SCOPED_TRACE(aLower);
    occ::handle<NCollection_HArray1<double>> aParams =
      new NCollection_HArray1<double>(aLower, aLower + 2);
    for (int anIndex = 0; anIndex < 3; ++anIndex)
    {
      aParams->SetValue(aLower + anIndex, 2.0 + anIndex);
    }
    GeomFill_SectionGenerator aGenerator;
    aGenerator.SetParam(aParams);
    for (int anIndex = 0; anIndex < 3; ++anIndex)
    {
      EXPECT_DOUBLE_EQ(aParams->Value(aLower + anIndex), 2.0 + anIndex);
      EXPECT_DOUBLE_EQ(aGenerator.Parameter(anIndex + 1), 2.0 + anIndex);
    }
    aParams->SetValue(aLower, -99.0);
    EXPECT_DOUBLE_EQ(aGenerator.Parameter(1), 2.0);
    aGenerator.SetParam(aParams);
    EXPECT_DOUBLE_EQ(aGenerator.Parameter(1), -99.0);
  }
}

TEST(GeomFill_SectionGenerator, InvalidParametersDoNotReplacePreviousValues)
{
  GeomFill_SectionGenerator                aGenerator;
  occ::handle<NCollection_HArray1<double>> aParams = new NCollection_HArray1<double>(1, 2);
  aParams->SetValue(1, 0.0);
  aParams->SetValue(2, 1.0);
  aGenerator.SetParam(aParams);
  EXPECT_THROW(aGenerator.SetParam(nullptr), Standard_NullObject);
  aParams->SetValue(2, 0.0);
  EXPECT_THROW(aGenerator.SetParam(aParams), Standard_ConstructionError);
  aParams->SetValue(2, std::numeric_limits<double>::infinity());
  EXPECT_THROW(aGenerator.SetParam(aParams), Standard_ConstructionError);
  EXPECT_DOUBLE_EQ(aGenerator.Parameter(1), 0.0);
  EXPECT_DOUBLE_EQ(aGenerator.Parameter(2), 1.0);
}
