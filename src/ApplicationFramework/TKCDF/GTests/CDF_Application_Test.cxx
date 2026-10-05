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

#include <CDF_Application.hxx>
#include <Resource_Manager.hxx>
#include <TCollection_ExtendedString.hxx>

namespace
{
class CDF_TestApplication : public CDF_Application
{
public:
  CDF_TestApplication()
      : myResources(new Resource_Manager())
  {
  }

  occ::handle<Resource_Manager> Resources() override { return myResources; }

private:
  occ::handle<Resource_Manager> myResources;
};
} // namespace

TEST(CDF_ApplicationTest, FormatFromExtensionResource)
{
  occ::handle<CDF_TestApplication> anApplication = new CDF_TestApplication();
  anApplication->Resources()->SetResource("MiXeD.FileFormat", "Test format");

  TCollection_ExtendedString aFormat;
  EXPECT_TRUE(anApplication->Format("document.MiXeD", aFormat));
  EXPECT_EQ(aFormat, TCollection_ExtendedString("Test format"));
}

TEST(CDF_ApplicationTest, ExtensionlessFileHasNoFormat)
{
  occ::handle<CDF_TestApplication> anApplication = new CDF_TestApplication();
  anApplication->Resources()->SetResource(".FileFormat", "Invalid fallback");

  TCollection_ExtendedString aFormat;
  EXPECT_FALSE(anApplication->Format("document", aFormat));
  EXPECT_TRUE(aFormat.IsEmpty());
}

TEST(CDF_ApplicationTest, EmptyExtensionHasNoFormat)
{
  occ::handle<CDF_TestApplication> anApplication = new CDF_TestApplication();

  TCollection_ExtendedString aFormat;
  EXPECT_FALSE(anApplication->Format("document.", aFormat));
  EXPECT_TRUE(aFormat.IsEmpty());
}
