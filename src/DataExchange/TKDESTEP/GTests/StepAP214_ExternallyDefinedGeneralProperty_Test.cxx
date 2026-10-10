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

#include <StepAP214_ExternallyDefinedGeneralProperty.hxx>
#include <StepBasic_ExternallyDefinedItem.hxx>
#include <StepBasic_ExternalSource.hxx>
#include <StepBasic_SourceItem.hxx>
#include <TCollection_HAsciiString.hxx>
#include <gtest/gtest.h>

//==================================================================================================

//! STEP readers initialize a newly created entity before any setter is called.
TEST(StepAP214_ExternallyDefinedGeneralPropertyTest, InitializesExternalItem)
{
  StepAP214_ExternallyDefinedGeneralProperty  aProperty;
  const occ::handle<TCollection_HAsciiString> anId    = new TCollection_HAsciiString("id");
  const occ::handle<TCollection_HAsciiString> aName   = new TCollection_HAsciiString("name");
  const occ::handle<StepBasic_ExternalSource> aSource = new StepBasic_ExternalSource();
  const StepBasic_SourceItem                  anItem;
  aProperty.Init(anId, aName, false, nullptr, anItem, aSource);
  ASSERT_FALSE(aProperty.ExternallyDefinedItem().IsNull());
  EXPECT_EQ(aProperty.ExternallyDefinedItem()->Source(), aSource);
  EXPECT_EQ(aProperty.Id(), anId);
  EXPECT_FALSE(aProperty.HasDescription());
}
