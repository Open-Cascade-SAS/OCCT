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

#include <OSD_Environment.hxx>
#include <Resource_Manager.hxx>
#include <TCollection_AsciiString.hxx>

#include <filesystem>
#include <fstream>

namespace
{
//! Returns a platform-independent temp base directory for tests.
std::filesystem::path getTempBase()
{
  return std::filesystem::temp_directory_path() / "occt_resource_manager_test";
}

//! Writes a minimal resource file containing one entry:  key : value
void writeResourceFile(const std::filesystem::path& theDir,
                       const std::string&           theFileName,
                       const std::string&           theKey,
                       const std::string&           theValue)
{
  std::filesystem::create_directories(theDir);
  std::ofstream aFile(theDir / theFileName);
  aFile << theKey << " : " << theValue << "\n";
}
} // namespace

class Resource_ManagerTest : public testing::Test
{
protected:
  void SetUp() override { std::filesystem::remove_all(getTempBase()); }

  void TearDown() override { std::filesystem::remove_all(getTempBase()); }
};

//! Sets CSF_<theFileName>UserDefaults environment variable to theDir.
static void setUserDefaultsEnv(const std::string& theFileName, const std::filesystem::path& theDir)
{
  const std::string anEnvName = "CSF_" + theFileName + "UserDefaults";
  OSD_Environment   anEnv(anEnvName.c_str());
  anEnv.SetValue(TCollection_AsciiString(theDir.string().c_str()));
  anEnv.Build();
}

// Test OCC27849: Resource_Manager correctly reads resource files located in
// directories whose names contain dots, spaces, or multiple path segments.
TEST_F(Resource_ManagerTest, OCC27849_PathsWithSpecialChars)
{
  const std::string aResourceName = "TestResource";
  const std::string aKey          = "test.resource";
  const std::string aExpected     = "ok";

  const std::vector<std::string> aPaths = {
    "path",
    "path.with.dots",
    "path with spaces",
    "nested/dirs/path with spaces",
  };

  for (const std::string& aRelPath : aPaths)
  {
    std::filesystem::path aDir = getTempBase() / aRelPath;
    writeResourceFile(aDir, aResourceName, aKey, aExpected);

    // Point the env variable to the directory containing the resource file
    OSD_Environment anEnv("CSF_TestResourceDefaults");
    anEnv.SetValue(TCollection_AsciiString(aDir.string().c_str()));
    anEnv.Build();

    Resource_Manager aManager(aResourceName.c_str());

    EXPECT_TRUE(aManager.Find(aKey.c_str())) << "Resource key not found in path: " << aDir.string();

    if (aManager.Find(aKey.c_str()))
    {
      EXPECT_STREQ(aManager.Value(aKey.c_str()), aExpected.c_str())
        << "Wrong resource value for path: " << aDir.string();
    }
  }
}

// Test OCC181 (case 1): Resource_Manager::Save() succeeds when the target user-defaults
// directory already exists (flat path).
TEST_F(Resource_ManagerTest, OCC181_SaveToExistingDirectory)
{
  const std::string aFileName = "OCC181";

  const std::filesystem::path aSourceDir = getTempBase() / "source";
  const std::filesystem::path aSaveDir   = getTempBase() / "save_flat";

  writeResourceFile(aSourceDir, aFileName, "test.key", "test_value");
  std::filesystem::create_directories(aSaveDir);

  setUserDefaultsEnv(aFileName, aSourceDir);
  Resource_Manager aManager(aFileName.c_str());

  // Redirect Save() to a different directory
  setUserDefaultsEnv(aFileName, aSaveDir);
  EXPECT_TRUE(aManager.Save());
  EXPECT_TRUE(std::filesystem::exists(aSaveDir / aFileName))
    << "Saved resource file not found in " << aSaveDir.string();
}

// Test OCC181 (case 2): Resource_Manager::Save() succeeds and creates intermediate
// directories when the target user-defaults path does not yet exist (nested path).
TEST_F(Resource_ManagerTest, OCC181_SaveToNestedNonExistentDirectory)
{
  const std::string aFileName = "OCC181";

  const std::filesystem::path aSourceDir = getTempBase() / "source2";
  const std::filesystem::path aSaveDir   = getTempBase() / "nested" / "deep" / "dir";

  writeResourceFile(aSourceDir, aFileName, "test.key", "test_value");

  setUserDefaultsEnv(aFileName, aSourceDir);
  Resource_Manager aManager(aFileName.c_str());

  // Redirect Save() to a nested path that does not yet exist
  setUserDefaultsEnv(aFileName, aSaveDir);
  EXPECT_TRUE(aManager.Save());
  EXPECT_TRUE(std::filesystem::exists(aSaveDir / aFileName))
    << "Saved resource file not found in nested path " << aSaveDir.string();
}
