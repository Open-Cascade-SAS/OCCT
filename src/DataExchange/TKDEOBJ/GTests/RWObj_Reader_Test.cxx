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

#include <RWObj_TriangulationReader.hxx>
#include <NCollection_LinearVector.hxx>

#include <gtest/gtest.h>

#include <sstream>

namespace
{
class TrackingReader : public RWObj_TriangulationReader
{
public:
  const NCollection_LinearVector<TCollection_AsciiString>& Groups() const { return myGroups; }

protected:
  bool addMesh(const RWObj_SubMesh& theMesh, const RWObj_SubMeshReason theReason) override
  {
    if (theReason == RWObj_SubMeshReason_NewGroup)
    {
      myGroups.Append(theMesh.Group);
    }
    return RWObj_TriangulationReader::addMesh(theMesh, theReason);
  }

private:
  NCollection_LinearVector<TCollection_AsciiString> myGroups;
};
} // namespace

TEST(RWObj_ReaderTest, IgnoresIncompleteCommands)
{
  for (const char* aContent :
       {"f", "f ", "v", "v ", "vn", "vt", "g", "s", "o", "mtllib", "mtllib ", "usemtl"})
  {
    std::istringstream        anInput(aContent);
    RWObj_TriangulationReader aReader;
    EXPECT_TRUE(aReader.Read(anInput, "minimal.obj", Message_ProgressRange()));
    EXPECT_TRUE(aReader.ResultShape().IsNull());
  }
}

TEST(RWObj_ReaderTest, ParsesCompleteCommands)
{
  std::istringstream        anInput("v 0 0 0\n"
                                    "v 1 0 0\n"
                                    "v 0 1 0\n"
                                    "f 1 2 3\n");
  RWObj_TriangulationReader aReader;
  ASSERT_TRUE(aReader.Read(anInput, "triangle.obj", Message_ProgressRange()));
  EXPECT_FALSE(aReader.ResultShape().IsNull());
}

TEST(RWObj_ReaderTest, ProcessesEmptyGroupName)
{
  std::istringstream anInput("g named\ng \n");
  TrackingReader     aReader;
  ASSERT_TRUE(aReader.Read(anInput, "groups.obj", Message_ProgressRange()));
  ASSERT_EQ(aReader.Groups().Size(), 2u);
  EXPECT_TRUE(aReader.Groups().Value(0).IsEmpty());
  EXPECT_EQ(aReader.Groups().Value(1), "named");
}

TEST(RWObj_ReaderTest, ParsesFinalLineWithoutNewline)
{
  std::istringstream        anInput("v 0 0 0\n"
                                    "v 1 0 0\n"
                                    "v 0 1 0\n"
                                    "f 1 2 3");
  RWObj_TriangulationReader aReader;
  ASSERT_TRUE(aReader.Read(anInput, "triangle.obj", Message_ProgressRange()));
  EXPECT_FALSE(aReader.ResultShape().IsNull());
}
