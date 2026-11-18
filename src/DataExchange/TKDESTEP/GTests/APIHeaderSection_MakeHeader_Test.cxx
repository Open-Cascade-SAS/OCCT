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

#include <APIHeaderSection_MakeHeader.hxx>
#include <HeaderSection_FileDescription.hxx>
#include <NCollection_HArray1.hxx>
#include <StepData_StepModel.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

TEST(APIHeaderSection_MakeHeader, ReturnNullHandlesForMissingEntities)
{
  occ::handle<StepData_StepModel> aModel = new StepData_StepModel;
  APIHeaderSection_MakeHeader     aHeader(aModel);

  EXPECT_TRUE(aHeader.Name().IsNull());
  EXPECT_TRUE(aHeader.TimeStamp().IsNull());
  EXPECT_TRUE(aHeader.Author().IsNull());
  EXPECT_TRUE(aHeader.AuthorValue(1).IsNull());
  EXPECT_TRUE(aHeader.Organization().IsNull());
  EXPECT_TRUE(aHeader.OrganizationValue(1).IsNull());
  EXPECT_TRUE(aHeader.PreprocessorVersion().IsNull());
  EXPECT_TRUE(aHeader.OriginatingSystem().IsNull());
  EXPECT_TRUE(aHeader.Authorisation().IsNull());
  EXPECT_TRUE(aHeader.SchemaIdentifiers().IsNull());
  EXPECT_TRUE(aHeader.SchemaIdentifiersValue(1).IsNull());
  EXPECT_TRUE(aHeader.Description().IsNull());
  EXPECT_TRUE(aHeader.DescriptionValue(1).IsNull());
  EXPECT_TRUE(aHeader.ImplementationLevel().IsNull());
}

TEST(APIHeaderSection_MakeHeader, SetDescriptionUsesFileDescriptionPresence)
{
  occ::handle<StepData_StepModel>            aModel           = new StepData_StepModel;
  occ::handle<HeaderSection_FileDescription> aFileDescription = new HeaderSection_FileDescription;
  aModel->AddHeaderEntity(aFileDescription);
  APIHeaderSection_MakeHeader aHeader(aModel);

  occ::handle<NCollection_HArray1<occ::handle<TCollection_HAsciiString>>> aDescription =
    new NCollection_HArray1<occ::handle<TCollection_HAsciiString>>(1, 1);
  aDescription->SetValue(1, new TCollection_HAsciiString("Description"));
  aHeader.SetDescription(aDescription);

  EXPECT_EQ(aHeader.Description(), aDescription);
}
