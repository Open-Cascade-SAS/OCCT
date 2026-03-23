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
#include <BRep_TEdge.hxx>
#include <BRep_TFace.hxx>
#include <BRep_Tool.hxx>
#include <BRepCheck.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepCheck_Result.hxx>
#include <BRepCheck_Status.hxx>
#include <BRepGraph.hxx>
#include <BRepGraph_DefsView.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepTools.hxx>
#include <Geom2d_Curve.hxx>
#include <Geom_Surface.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_Map.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
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

void dumpShapeCheckDetails(const BRepCheck_Analyzer& theAnalyzer,
                            const TopoDS_Shape&       theShape,
                            const char*               theLabel)
{
  // Per-face: check PCurve availability and wire self-intersection
  int aFaceIdx = 0;
  for (TopExp_Explorer aFaceExp(theShape, TopAbs_FACE); aFaceExp.More(); aFaceExp.Next(), ++aFaceIdx)
  {
    const TopoDS_Face& aFace = TopoDS::Face(aFaceExp.Current());
    const occ::handle<BRepCheck_Result>& aFaceResult = theAnalyzer.Result(aFace);
    if (!aFaceResult.IsNull())
    {
      for (auto aIt = aFaceResult->Status().cbegin(); aIt != aFaceResult->Status().cend(); ++aIt)
      {
        if (*aIt != BRepCheck_NoError)
        {
          std::ostringstream aMsg;
          BRepCheck::Print(*aIt, aMsg);
          std::cerr << "[" << theLabel << "] Face " << aFaceIdx << " error: " << aMsg.str();
        }
      }
    }

    // Check PCurve availability for all edges in face
    int aWireIdx = 0;
    for (TopoDS_Iterator aWireIt(aFace); aWireIt.More(); aWireIt.Next())
    {
      if (aWireIt.Value().ShapeType() != TopAbs_WIRE)
        continue;
      const TopoDS_Wire& aWire = TopoDS::Wire(aWireIt.Value());
      const occ::handle<BRepCheck_Result>& aWireResult = theAnalyzer.Result(aWire);
      if (!aWireResult.IsNull())
      {
        for (auto aIt = aWireResult->Status().cbegin(); aIt != aWireResult->Status().cend(); ++aIt)
        {
          if (*aIt != BRepCheck_NoError)
          {
            std::ostringstream aMsg;
            BRepCheck::Print(*aIt, aMsg);
            std::cerr << "[" << theLabel << "] Face " << aFaceIdx << " Wire " << aWireIdx
                      << " error: " << aMsg.str();
          }
        }
      }

      int anEdgeIdx = 0;
      for (TopoDS_Iterator anEdgeIt(aWire); anEdgeIt.More(); anEdgeIt.Next(), ++anEdgeIdx)
      {
        if (anEdgeIt.Value().ShapeType() != TopAbs_EDGE)
          continue;
        const TopoDS_Edge& anEdge = TopoDS::Edge(anEdgeIt.Value());
        double aFirst = 0.0, aLast = 0.0;
        const occ::handle<Geom2d_Curve> aPC = BRep_Tool::CurveOnSurface(anEdge, aFace, aFirst, aLast);
        if (aPC.IsNull() && !BRep_Tool::Degenerated(anEdge))
        {
          std::cerr << "[" << theLabel << "] Face " << aFaceIdx << " Wire " << aWireIdx
                    << " Edge " << anEdgeIdx
                    << " -> NULL PCurve! Ori=" << anEdge.Orientation()
                    << " EdgeLoc.IsId=" << anEdge.Location().IsIdentity() << "\n";

          // Dump edge CRs
          const occ::handle<BRep_TEdge> aTEdge = occ::down_cast<BRep_TEdge>(anEdge.TShape());
          if (!aTEdge.IsNull())
          {
            TopLoc_Location aFaceSurfLoc;
            const occ::handle<Geom_Surface>& aFaceSurf = BRep_Tool::Surface(aFace, aFaceSurfLoc);
            std::cerr << "  FaceSurf=" << aFaceSurf.get()
                      << " FaceSurfLoc.IsId=" << aFaceSurfLoc.IsIdentity() << "\n";
            int aCRIdx = 0;
            for (const auto& aCR : aTEdge->Curves())
            {
              if (aCR->IsCurveOnSurface())
              {
                std::cerr << "  CR[" << aCRIdx << "] surf=" << aCR->Surface().get()
                          << " loc.IsId=" << aCR->Location().IsIdentity()
                          << " match=" << (aCR->Surface() == aFaceSurf) << "\n";
              }
              ++aCRIdx;
            }
          }
        }
      }
      ++aWireIdx;
    }
  }

  // Dump face surfaces and PCurve details for failing faces
  aFaceIdx = 0;
  for (TopExp_Explorer aFaceExp2(theShape, TopAbs_FACE); aFaceExp2.More(); aFaceExp2.Next(), ++aFaceIdx)
  {
    const TopoDS_Face& aFace2 = TopoDS::Face(aFaceExp2.Current());
    TopLoc_Location aSurfLoc;
    const occ::handle<Geom_Surface>& aSurf2 = BRep_Tool::Surface(aFace2, aSurfLoc);
    const auto* aTFace = static_cast<const BRep_TFace*>(aFace2.TShape().get());
    std::cerr << "[" << theLabel << "] Face " << aFaceIdx
              << " surf=" << aSurf2.get()
              << " TFaceSurf=" << aTFace->Surface().get()
              << " TFaceLoc.IsId=" << aTFace->Location().IsIdentity()
              << " faceLoc.IsId=" << aFace2.Location().IsIdentity()
              << " surfLoc.IsId=" << aSurfLoc.IsIdentity()
              << " ori=" << aFace2.Orientation()
              << " type=" << aSurf2->DynamicType()->Name()
              << "\n";

    // For the failing face, dump all edge PCurves
    const occ::handle<BRepCheck_Result>& aFaceResult2 = theAnalyzer.Result(aFace2);
    bool aHasError2 = false;
    if (!aFaceResult2.IsNull())
    {
      for (auto aIt = aFaceResult2->Status().cbegin(); aIt != aFaceResult2->Status().cend(); ++aIt)
      {
        if (*aIt != BRepCheck_NoError)
          aHasError2 = true;
      }
    }
    if (aHasError2)
    {
      int aWireIdx2 = 0;
      for (TopoDS_Iterator aWireIt2(aFace2); aWireIt2.More(); aWireIt2.Next())
      {
        if (aWireIt2.Value().ShapeType() != TopAbs_WIRE)
          continue;
        const TopoDS_Wire& aWire2 = TopoDS::Wire(aWireIt2.Value());
        std::cerr << "  Wire " << aWireIdx2 << " wireLoc.IsId=" << aWire2.Location().IsIdentity()
                  << " ori=" << aWire2.Orientation() << "\n";
        int anEdgeIdx2 = 0;
        for (TopoDS_Iterator anEdgeIt2(aWire2); anEdgeIt2.More(); anEdgeIt2.Next(), ++anEdgeIdx2)
        {
          if (anEdgeIt2.Value().ShapeType() != TopAbs_EDGE)
            continue;
          const TopoDS_Edge& anEdge2 = TopoDS::Edge(anEdgeIt2.Value());
          double aFirst2 = 0.0, aLast2 = 0.0;
          const occ::handle<Geom2d_Curve> aPC2 =
            BRep_Tool::CurveOnSurface(anEdge2, aFace2, aFirst2, aLast2);
          std::cerr << "    Edge " << anEdgeIdx2
                    << " ori=" << anEdge2.Orientation()
                    << " loc.IsId=" << anEdge2.Location().IsIdentity()
                    << " degen=" << BRep_Tool::Degenerated(anEdge2);
          if (!aPC2.IsNull())
          {
            gp_Pnt2d aP0 = aPC2->Value(aFirst2);
            gp_Pnt2d aP1 = aPC2->Value(aLast2);
            std::cerr << " PC=[" << aFirst2 << "," << aLast2 << "]"
                      << " start=(" << aP0.X() << "," << aP0.Y() << ")"
                      << " end=(" << aP1.X() << "," << aP1.Y() << ")";
          }
          else
          {
            std::cerr << " PC=NULL";
          }

          // Dump all CRs on this edge's TShape
          const occ::handle<BRep_TEdge> aTEdge2 = occ::down_cast<BRep_TEdge>(anEdge2.TShape());
          int aCRCount = 0;
          if (!aTEdge2.IsNull())
          {
            for (const auto& aCR : aTEdge2->Curves())
            {
              if (aCR->IsCurveOnSurface())
                ++aCRCount;
            }
          }
          std::cerr << " #CRonSurf=" << aCRCount << "\n";
        }
        ++aWireIdx2;
      }
    }
  }

  // Per-shell: check orientation
  int aShellIdx = 0;
  for (TopExp_Explorer aShellExp(theShape, TopAbs_SHELL); aShellExp.More(); aShellExp.Next(), ++aShellIdx)
  {
    const TopoDS_Shape& aShell = aShellExp.Current();
    const occ::handle<BRepCheck_Result>& aShellResult = theAnalyzer.Result(aShell);
    if (!aShellResult.IsNull())
    {
      for (auto aIt = aShellResult->Status().cbegin(); aIt != aShellResult->Status().cend(); ++aIt)
      {
        if (*aIt != BRepCheck_NoError)
        {
          std::ostringstream aMsg;
          BRepCheck::Print(*aIt, aMsg);
          std::cerr << "[" << theLabel << "] Shell " << aShellIdx << " error: " << aMsg.str();
        }
      }
    }
  }
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

  // Step 10: BRepCheck on reconstructed - expect same validity as original
  BRepCheck_Analyzer aReconChecker(aReconShape);
  const bool         isReconValid = aReconChecker.IsValid();
  const std::string  anOrigInvalidityTypes = invalidityTypesToString(anOrigChecker, anOrigShape);
  const std::string  aReconInvalidityTypes = invalidityTypesToString(aReconChecker, aReconShape);
  EXPECT_EQ(isReconValid, isOrigValid)
    << "Validity mismatch (orig=" << isOrigValid << ", recon=" << isReconValid
    << ", origInvalidityTypes=[" << anOrigInvalidityTypes
    << "], reconInvalidityTypes=[" << aReconInvalidityTypes
    << "): " << aFilePath;

  // Debug: detailed sub-shape diagnostics and BRepTools::Write comparison
  if (isReconValid != isOrigValid)
  {
    std::cerr << "\n=== DETAILED DIAGNOSTICS for " << aFilePath << " ===\n";
    dumpShapeCheckDetails(anOrigChecker, anOrigShape, "ORIG");
    dumpShapeCheckDetails(aReconChecker, aReconShape, "RECON");
    std::cerr << "=== END DIAGNOSTICS ===\n\n";
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
