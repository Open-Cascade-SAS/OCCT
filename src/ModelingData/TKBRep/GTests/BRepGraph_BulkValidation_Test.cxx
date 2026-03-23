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

#include <BRep_Builder.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Status.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepTools.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <string>
#include <vector>

namespace
{

const char* getDatasetDir()
{
  const char* anEnvDir = std::getenv("BREP_DATASET_DIR");
  return anEnvDir != nullptr ? anEnvDir
                             : "/Users/dpasukhi/work/OCCT/data/opencascade-dataset-7.9.0/brep/";
}

std::vector<std::string> collectBrepFiles(const char* theDir)
{
  std::vector<std::string> aFiles;
  std::error_code          anEC;
  for (const auto& anEntry : std::filesystem::recursive_directory_iterator(theDir, anEC))
  {
    if (anEntry.is_regular_file() && anEntry.path().extension() == ".brep")
    {
      aFiles.push_back(anEntry.path().string());
    }
  }
  std::sort(aFiles.begin(), aFiles.end());
  return aFiles;
}

// Count unique shapes by TShape pointer, matching BRepGraph's definition-level
// deduplication semantics. Uses recursive traversal for Compound/CompSolid types
// since TopExp_Explorer doesn't recurse into these container types to find
// nested sub-containers.
int countUnique(const TopoDS_Shape& theShape, TopAbs_ShapeEnum theType)
{
  NCollection_Map<const TopoDS_TShape*> aTShapeSet;
  if (theType == TopAbs_COMPOUND || theType == TopAbs_COMPSOLID)
  {
    // Recursive traversal for container types.
    std::function<void(const TopoDS_Shape&)> collect = [&](const TopoDS_Shape& theS) {
      if (theS.ShapeType() == theType)
        aTShapeSet.Add(theS.TShape().get());
      for (TopoDS_Iterator anIt(theS); anIt.More(); anIt.Next())
        collect(anIt.Value());
    };
    collect(theShape);
  }
  else
  {
    // TopExp_Explorer works correctly for non-container types.
    for (TopExp_Explorer anExp(theShape, theType); anExp.More(); anExp.Next())
      aTShapeSet.Add(anExp.Current().TShape().get());
  }
  return aTShapeSet.Extent();
}

struct TopoCounts
{
  int NbCompounds;
  int NbCompSolids;
  int NbSolids;
  int NbShells;
  int NbFaces;
  int NbWires;
  int NbEdges;
  int NbVertices;
};

TopoCounts countAll(const TopoDS_Shape& theShape)
{
  TopoCounts aCounts{};
  aCounts.NbCompounds  = countUnique(theShape, TopAbs_COMPOUND);
  aCounts.NbCompSolids = countUnique(theShape, TopAbs_COMPSOLID);
  aCounts.NbSolids     = countUnique(theShape, TopAbs_SOLID);
  aCounts.NbShells     = countUnique(theShape, TopAbs_SHELL);
  aCounts.NbFaces      = countUnique(theShape, TopAbs_FACE);
  aCounts.NbWires      = countUnique(theShape, TopAbs_WIRE);
  aCounts.NbEdges      = countUnique(theShape, TopAbs_EDGE);
  aCounts.NbVertices   = countUnique(theShape, TopAbs_VERTEX);
  return aCounts;
}

void collectInvalidityStatuses(const BRepCheck_Analyzer& theAnalyzer,
                              const TopoDS_Shape&       theShape,
                              NCollection_IndexedMap<int>& theStatuses)
{
  const occ::handle<BRepCheck_Result>& aResult = theAnalyzer.Result(theShape);
  if (!aResult.IsNull())
  {
    NCollection_List<BRepCheck_Status>::Iterator aStatusIt;
    for (aStatusIt.Initialize(aResult->Status()); aStatusIt.More(); aStatusIt.Next())
    {
      if (aStatusIt.Value() != BRepCheck_NoError)
      {
        theStatuses.Add(static_cast<int>(aStatusIt.Value()));
      }
    }
  }

  for (TopoDS_Iterator anIt(theShape); anIt.More(); anIt.Next())
  {
    collectInvalidityStatuses(theAnalyzer, anIt.Value(), theStatuses);
  }
}

std::string invalidityTypesToString(const BRepCheck_Analyzer& theAnalyzer,
                                    const TopoDS_Shape&       theShape)
{
  NCollection_IndexedMap<int> aStatuses;
  collectInvalidityStatuses(theAnalyzer, theShape, aStatuses);
  if (aStatuses.IsEmpty())
  {
    return "none";
  }

  std::ostringstream aStream;
  for (int anIdx = 1; anIdx <= aStatuses.Extent(); ++anIdx)
  {
    if (anIdx > 1)
    {
      aStream << "; ";
    }
    BRepCheck::Print(static_cast<BRepCheck_Status>(aStatuses.FindKey(anIdx)), aStream);
  }
  return aStream.str();
}

} // namespace

class BRepGraphBulkValidation : public testing::TestWithParam<std::string>
{
};

TEST_P(BRepGraphBulkValidation, RoundTrip)
{
  const std::string& aFilePath = GetParam();

  // Step 1: Read .brep file
  TopoDS_Shape anOrigShape;
  BRep_Builder aBuilder;
  if (!BRepTools::Read(anOrigShape, aFilePath.c_str(), aBuilder))
  {
    GTEST_SKIP() << "Cannot read: " << aFilePath;
  }
  if (anOrigShape.IsNull())
  {
    GTEST_SKIP() << "Null shape: " << aFilePath;
  }

  // Step 2: Count unique elements on the original
  const TopoCounts anOrigCounts = countAll(anOrigShape);

  // Step 3: BRepCheck on the original
  BRepCheck_Analyzer anOrigChecker(anOrigShape);
  const bool         isOrigValid = anOrigChecker.IsValid();

  // Step 4: Build the graph
  BRepGraph aGraph;
  aGraph.Build(anOrigShape);
  ASSERT_TRUE(aGraph.IsDone()) << "Build failed: " << aFilePath;

  // Step 5: Verify graph entity counts match TopExp counts
  const auto aDefs = aGraph.Defs();
  EXPECT_EQ(aDefs.NbVertices(), anOrigCounts.NbVertices) << aFilePath;
  EXPECT_EQ(aDefs.NbEdges(), anOrigCounts.NbEdges) << aFilePath;
  EXPECT_EQ(aDefs.NbWires(), anOrigCounts.NbWires) << aFilePath;
  EXPECT_EQ(aDefs.NbFaces(), anOrigCounts.NbFaces) << aFilePath;
  EXPECT_EQ(aDefs.NbShells(), anOrigCounts.NbShells) << aFilePath;
  EXPECT_EQ(aDefs.NbSolids(), anOrigCounts.NbSolids) << aFilePath;
  EXPECT_EQ(aDefs.NbCompSolids(), anOrigCounts.NbCompSolids) << aFilePath;
  EXPECT_EQ(aDefs.NbCompounds(), anOrigCounts.NbCompounds) << aFilePath;

  // Step 6: Get root NodeId from Product(0)
  ASSERT_GE(aDefs.NbProducts(), 1) << "No products: " << aFilePath;
  const BRepGraph_NodeId aRootId = aDefs.Product(0).ShapeRootId;

  // Step 7: Reconstruct and apply root orientation/location from Product
  TopoDS_Shape aReconShape = aGraph.Shapes().Reconstruct(aRootId);
  ASSERT_FALSE(aReconShape.IsNull()) << "Reconstruct returned null: " << aFilePath;
  {
    const auto& aProduct = aDefs.Product(0);
    aReconShape.Orientation(aProduct.RootOrientation);
    if (!aProduct.RootLocation.IsIdentity())
      aReconShape.Location(aProduct.RootLocation);
  }

  // Step 8: Count unique elements on the reconstructed shape
  const TopoCounts aReconCounts = countAll(aReconShape);

  // Step 9: Compare counts
  EXPECT_EQ(aReconCounts.NbCompounds, anOrigCounts.NbCompounds)
    << "Compound count mismatch: " << aFilePath;
  EXPECT_EQ(aReconCounts.NbCompSolids, anOrigCounts.NbCompSolids)
    << "CompSolid count mismatch: " << aFilePath;
  EXPECT_EQ(aReconCounts.NbSolids, anOrigCounts.NbSolids) << "Solid count mismatch: " << aFilePath;
  EXPECT_EQ(aReconCounts.NbShells, anOrigCounts.NbShells) << "Shell count mismatch: " << aFilePath;
  EXPECT_EQ(aReconCounts.NbFaces, anOrigCounts.NbFaces) << "Face count mismatch: " << aFilePath;
  EXPECT_EQ(aReconCounts.NbWires, anOrigCounts.NbWires) << "Wire count mismatch: " << aFilePath;
  EXPECT_EQ(aReconCounts.NbEdges, anOrigCounts.NbEdges) << "Edge count mismatch: " << aFilePath;
  EXPECT_EQ(aReconCounts.NbVertices, anOrigCounts.NbVertices)
    << "Vertex count mismatch: " << aFilePath;

  // Step 10: BRepCheck on reconstructed — expect same validity as original
  BRepCheck_Analyzer aReconChecker(aReconShape);
  const bool         isReconValid = aReconChecker.IsValid();
  const std::string  anOrigInvalidityTypes = invalidityTypesToString(anOrigChecker, anOrigShape);
  const std::string  aReconInvalidityTypes = invalidityTypesToString(aReconChecker, aReconShape);
  EXPECT_EQ(isReconValid, isOrigValid)
    << "Validity mismatch (orig=" << isOrigValid << ", recon=" << isReconValid
    << ", origInvalidityTypes=[" << anOrigInvalidityTypes
    << "], reconInvalidityTypes=[" << aReconInvalidityTypes
    << "): " << aFilePath;

  // Debug: dump BRepTools::Write comparison
  if (isReconValid != isOrigValid)
  {
    std::ostringstream anOrigStream, aReconStream;
    BRepTools::Write(anOrigShape, anOrigStream);
    BRepTools::Write(aReconShape, aReconStream);
    const std::string anOrigDump  = anOrigStream.str();
    const std::string aReconDump = aReconStream.str();
    if (anOrigDump == aReconDump)
    {
      std::cerr << "\n[DEBUG] BRepTools dumps IDENTICAL\n";
    }
    else
    {
      // Find first difference
      size_t aDiffPos = 0;
      while (aDiffPos < anOrigDump.size() && aDiffPos < aReconDump.size()
             && anOrigDump[aDiffPos] == aReconDump[aDiffPos])
        ++aDiffPos;
      const size_t aCtxStart = aDiffPos > 300 ? aDiffPos - 300 : 0;
      const size_t aCtxEnd   = std::min(aDiffPos + 300, std::min(anOrigDump.size(), aReconDump.size()));
      std::cerr << "\n[DEBUG] First diff at byte " << aDiffPos
                << " (orig=" << anOrigDump.size() << ", recon=" << aReconDump.size() << ")\n"
                << "[ORIG ] ..." << anOrigDump.substr(aCtxStart, aCtxEnd - aCtxStart) << "...\n"
                << "[RECON] ..." << aReconDump.substr(aCtxStart, aCtxEnd - aCtxStart) << "...\n";
    }
  }
}

// Generate a short test name from the file path (just the filename without extension)
struct BrepFileNameGenerator
{
  template <class ParamType>
  std::string operator()(const testing::TestParamInfo<ParamType>& theInfo) const
  {
    std::filesystem::path aPath(theInfo.param);
    std::string           aName = aPath.stem().string();
    // Replace characters invalid for GTest names with underscores
    for (char& aChar : aName)
    {
      if (!std::isalnum(static_cast<unsigned char>(aChar)))
      {
        aChar = '_';
      }
    }
    return aName;
  }
};

INSTANTIATE_TEST_SUITE_P(Dataset,
                         BRepGraphBulkValidation,
                         testing::ValuesIn(collectBrepFiles(getDatasetDir())),
                         BrepFileNameGenerator());
