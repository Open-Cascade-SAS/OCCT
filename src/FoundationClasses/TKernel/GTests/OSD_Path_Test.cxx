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

#include <OSD_Path.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

// Test fixture for OSD_Path tests
class OSD_PathTest : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(OSD_PathTest, DosAbsolutePaths)
{
  // Test DOS absolute paths
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("c:\\folder\\file.png"));
  EXPECT_TRUE(OSD_Path::IsDosPath("c:\\folder\\file.png"));
  EXPECT_FALSE(OSD_Path::IsRelativePath("c:\\folder\\file.png"));
  EXPECT_FALSE(OSD_Path::IsUnixPath("c:\\folder\\file.png"));
  EXPECT_FALSE(OSD_Path::IsUncPath("c:\\folder\\file.png"));

  // Test DOS root
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("D:\\"));
  EXPECT_TRUE(OSD_Path::IsDosPath("D:\\"));

  // Test single file on DOS drive
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("c:\\file.png"));
  EXPECT_TRUE(OSD_Path::IsDosPath("c:\\file.png"));
}

TEST_F(OSD_PathTest, UncPaths)
{
  // Test UNC paths
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("\\\\share\\file.pdf"));
  EXPECT_TRUE(OSD_Path::IsUncPath("\\\\share\\file.pdf"));
  EXPECT_FALSE(OSD_Path::IsDosPath("\\\\share\\file.pdf"));
  EXPECT_FALSE(OSD_Path::IsUnixPath("\\\\share\\file.pdf"));
}

TEST_F(OSD_PathTest, NtExtendedPaths)
{
  // Test NT Extended paths
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("\\\\?\\C:\\documents\\file.docx"));
  EXPECT_TRUE(OSD_Path::IsNtExtendedPath("\\\\?\\C:\\documents\\file.docx"));
  EXPECT_FALSE(OSD_Path::IsUncPath("\\\\?\\C:\\documents\\file.docx"));
}

TEST_F(OSD_PathTest, UncExtendedPaths)
{
  // Test UNC Extended paths
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("\\\\?\\UNC\\server\\share\\file.zip"));
  EXPECT_TRUE(OSD_Path::IsUncPath("\\\\?\\UNC\\server\\share\\file.zip"));
  EXPECT_TRUE(OSD_Path::IsNtExtendedPath("\\\\?\\UNC\\server\\share\\file.zip"));
  EXPECT_TRUE(OSD_Path::IsUncExtendedPath("\\\\?\\UNC\\server\\share\\file.zip"));
}

TEST_F(OSD_PathTest, RemoteProtocolPaths)
{
  // Test remote protocol paths
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("https://www.server.org/file.gif"));
  EXPECT_TRUE(OSD_Path::IsRemoteProtocolPath("https://www.server.org/file.gif"));
  EXPECT_FALSE(OSD_Path::IsUnixPath("https://www.server.org/file.gif"));
  EXPECT_FALSE(OSD_Path::IsDosPath("https://www.server.org/file.gif"));

  // Test other protocols
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("ftp://ftp.server.com/file.dat"));
  EXPECT_TRUE(OSD_Path::IsRemoteProtocolPath("ftp://ftp.server.com/file.dat"));

  EXPECT_TRUE(OSD_Path::IsAbsolutePath("http://example.com/path"));
  EXPECT_TRUE(OSD_Path::IsRemoteProtocolPath("http://example.com/path"));
}

TEST_F(OSD_PathTest, ContentProtocolPaths)
{
  // Test content protocol paths
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("content://file.jpg"));
  EXPECT_TRUE(OSD_Path::IsRemoteProtocolPath("content://file.jpg"));
  EXPECT_TRUE(OSD_Path::IsContentProtocolPath("content://file.jpg"));
}

TEST_F(OSD_PathTest, UnixAbsolutePaths)
{
  // Test Unix absolute paths
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("/home/username/file.txt"));
  EXPECT_TRUE(OSD_Path::IsUnixPath("/home/username/file.txt"));
  EXPECT_FALSE(OSD_Path::IsRelativePath("/home/username/file.txt"));
  EXPECT_FALSE(OSD_Path::IsDosPath("/home/username/file.txt"));

  // Test Unix root
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("/"));
  EXPECT_TRUE(OSD_Path::IsUnixPath("/"));

  // Test single file on Unix
  EXPECT_TRUE(OSD_Path::IsAbsolutePath("/boot.bin"));
  EXPECT_TRUE(OSD_Path::IsUnixPath("/boot.bin"));
}

TEST_F(OSD_PathTest, RelativePaths)
{
  // Test relative paths with navigation
  EXPECT_TRUE(OSD_Path::IsRelativePath("./subfolder/../file.txt"));
  EXPECT_FALSE(OSD_Path::IsAbsolutePath("./subfolder/../file.txt"));

  EXPECT_TRUE(OSD_Path::IsRelativePath("../file.txt"));
  EXPECT_FALSE(OSD_Path::IsAbsolutePath("../file.txt"));

  // Test current directory
  EXPECT_TRUE(OSD_Path::IsRelativePath("."));
  EXPECT_FALSE(OSD_Path::IsAbsolutePath("."));

  // Test parent directory
  EXPECT_TRUE(OSD_Path::IsRelativePath(".."));
  EXPECT_FALSE(OSD_Path::IsAbsolutePath(".."));

  // Test simple relative path
  EXPECT_TRUE(OSD_Path::IsRelativePath("folder/file.txt"));
  EXPECT_FALSE(OSD_Path::IsAbsolutePath("folder/file.txt"));

  // Test simple filename
  EXPECT_TRUE(OSD_Path::IsRelativePath("file.txt"));
  EXPECT_FALSE(OSD_Path::IsAbsolutePath("file.txt"));
}

TEST_F(OSD_PathTest, FolderAndFileFromPath_UnixPaths)
{
  TCollection_AsciiString aFolder, aFileName;

  // Test Unix absolute path
  OSD_Path::FolderAndFileFromPath("/home/username/file.txt", aFolder, aFileName);
  EXPECT_STREQ("/home/username/", aFolder.ToCString());
  EXPECT_STREQ("file.txt", aFileName.ToCString());

  // Test Unix root file
  OSD_Path::FolderAndFileFromPath("/file.txt", aFolder, aFileName);
  EXPECT_STREQ("/", aFolder.ToCString());
  EXPECT_STREQ("file.txt", aFileName.ToCString());

  // Test Unix folder only (trailing slash)
  OSD_Path::FolderAndFileFromPath("/home/username/", aFolder, aFileName);
  EXPECT_STREQ("/home/username/", aFolder.ToCString());
  EXPECT_STREQ("", aFileName.ToCString());
}

TEST_F(OSD_PathTest, FolderAndFileFromPath_DosPaths)
{
  TCollection_AsciiString aFolder, aFileName;

  // Test DOS absolute path
  OSD_Path::FolderAndFileFromPath("C:\\Users\\John\\document.txt", aFolder, aFileName);
  EXPECT_STREQ("C:\\Users\\John\\", aFolder.ToCString());
  EXPECT_STREQ("document.txt", aFileName.ToCString());

  // Test DOS root file
  OSD_Path::FolderAndFileFromPath("C:\\file.txt", aFolder, aFileName);
  EXPECT_STREQ("C:\\", aFolder.ToCString());
  EXPECT_STREQ("file.txt", aFileName.ToCString());

  // Test DOS folder only (trailing backslash)
  OSD_Path::FolderAndFileFromPath("C:\\Program Files\\", aFolder, aFileName);
  EXPECT_STREQ("C:\\Program Files\\", aFolder.ToCString());
  EXPECT_STREQ("", aFileName.ToCString());
}

TEST_F(OSD_PathTest, FolderAndFileFromPath_RelativePaths)
{
  TCollection_AsciiString aFolder, aFileName;

  // Test relative path
  OSD_Path::FolderAndFileFromPath("folder/subfolder/file.txt", aFolder, aFileName);
  EXPECT_STREQ("folder/subfolder/", aFolder.ToCString());
  EXPECT_STREQ("file.txt", aFileName.ToCString());

  // Test relative with navigation
  OSD_Path::FolderAndFileFromPath("../folder/file.txt", aFolder, aFileName);
  EXPECT_STREQ("../folder/", aFolder.ToCString());
  EXPECT_STREQ("file.txt", aFileName.ToCString());

  // Test just filename
  OSD_Path::FolderAndFileFromPath("file.txt", aFolder, aFileName);
  EXPECT_STREQ("", aFolder.ToCString());
  EXPECT_STREQ("file.txt", aFileName.ToCString());
}

TEST_F(OSD_PathTest, FolderAndFileFromPath_ProtocolPaths)
{
  TCollection_AsciiString aFolder, aFileName;

  // Test HTTPS path
  OSD_Path::FolderAndFileFromPath("https://www.server.org/folder/file.gif", aFolder, aFileName);
  EXPECT_STREQ("https://www.server.org/folder/", aFolder.ToCString());
  EXPECT_STREQ("file.gif", aFileName.ToCString());

  // Test content protocol
  OSD_Path::FolderAndFileFromPath("content://path/file.jpg", aFolder, aFileName);
  EXPECT_STREQ("content://path/", aFolder.ToCString());
  EXPECT_STREQ("file.jpg", aFileName.ToCString());
}

TEST_F(OSD_PathTest, EdgeCases)
{
  TCollection_AsciiString aFolder, aFileName;

  // Test empty path
  OSD_Path::FolderAndFileFromPath("", aFolder, aFileName);
  EXPECT_STREQ("", aFolder.ToCString());
  EXPECT_STREQ("", aFileName.ToCString());

  // Test just separator
  OSD_Path::FolderAndFileFromPath("/", aFolder, aFileName);
  EXPECT_STREQ("/", aFolder.ToCString());
  EXPECT_STREQ("", aFileName.ToCString());

  OSD_Path::FolderAndFileFromPath("\\", aFolder, aFileName);
  EXPECT_STREQ("\\", aFolder.ToCString());
  EXPECT_STREQ("", aFileName.ToCString());

  // Test path with no file extension
  OSD_Path::FolderAndFileFromPath("/home/username/foldername", aFolder, aFileName);
  EXPECT_STREQ("/home/username/", aFolder.ToCString());
  EXPECT_STREQ("foldername", aFileName.ToCString());
}

TEST_F(OSD_PathTest, MixedSeparators)
{
  TCollection_AsciiString aFolder, aFileName;

  // Test mixed separators (this might happen in cross-platform scenarios)
  OSD_Path::FolderAndFileFromPath("C:/Users/John\\Documents/file.txt", aFolder, aFileName);
  // The exact behavior might depend on implementation, but it should handle this gracefully
  EXPECT_FALSE(aFolder.IsEmpty() || aFileName.IsEmpty());
}

//==================================================================================================
// Validation Tests
//==================================================================================================

// Test OCC132: OSD_Path validation for DOS/WindowsNT systems
// Migrated from QABugs_16.cxx
TEST_F(OSD_PathTest, OCC132_PathValidation_InvalidCharacters)
{
  // Test paths with invalid characters for DOS/Windows systems
  // According to Windows file system rules, these characters are invalid: / : * ? " < > |

  // Test path with forward slash (invalid for OS2/DOS paths)
  EXPECT_THROW({
    OSD_Path aPath("C:/test", OSD_OS2);
  }, Standard_ProgramError) << "Forward slash should be invalid for OSD_OS2";

  // Test path with colon in filename (invalid)
  EXPECT_THROW({
    OSD_Path aPath("C:\\file:name", OSD_OS2);
  }, Standard_ProgramError) << "Colon in filename should be invalid for OSD_OS2";

  // Test path with asterisk (invalid)
  EXPECT_THROW({
    OSD_Path aPath("C:\\file*name", OSD_WindowsNT);
  }, Standard_ProgramError) << "Asterisk should be invalid for OSD_WindowsNT";

  // Test path with question mark (invalid)
  EXPECT_THROW({
    OSD_Path aPath("C:\\file?name", OSD_WindowsNT);
  }, Standard_ProgramError) << "Question mark should be invalid for OSD_WindowsNT";

  // Test path with double quote (invalid)
  EXPECT_THROW({
    OSD_Path aPath("C:\\file\"name", OSD_OS2);
  }, Standard_ProgramError) << "Double quote should be invalid for OSD_OS2";

  // Test path with less-than sign (invalid)
  EXPECT_THROW({
    OSD_Path aPath("C:\\file<name", OSD_WindowsNT);
  }, Standard_ProgramError) << "Less-than sign should be invalid for OSD_WindowsNT";

  // Test path with greater-than sign (invalid)
  EXPECT_THROW({
    OSD_Path aPath("C:\\file>name", OSD_OS2);
  }, Standard_ProgramError) << "Greater-than sign should be invalid for OSD_OS2";

  // Test path with pipe (invalid)
  EXPECT_THROW({
    OSD_Path aPath("C:\\file|name", OSD_WindowsNT);
  }, Standard_ProgramError) << "Pipe should be invalid for OSD_WindowsNT";

  // Test valid path (should not throw)
  EXPECT_NO_THROW({
    OSD_Path aPath("C:\\valid\\path\\file.txt", OSD_OS2);
  }) << "Valid path should not throw exception";

  EXPECT_NO_THROW({
    OSD_Path aPath("C:\\valid\\path\\file.txt", OSD_WindowsNT);
  }) << "Valid path should not throw exception";
}