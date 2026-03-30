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

#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Sewing.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepGProp.hxx>
#include <BRepGraph_TopoView.hxx>
#include <BRepTools.hxx>
#include <BRepGraph_History.hxx>
#include <BRepGraph_ShapesView.hxx>
#include <BRepGraph_Tool.hxx>
#include <BRepGraphAlgo_Copy.hxx>
#include <BRepGraphAlgo_SameParameter.hxx>
#include <BRepGraphAlgo_Sewing.hxx>
#include <BRepGraphAlgo_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakeCylinder.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>
#include <DE_Wrapper.hxx>
#include <DESTEP_ConfigurationNode.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <NCollection_List.hxx>
#include <OSD_Parallel.hxx>
#include <OSD_ThreadPool.hxx>
#include <Standard_Atomic.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>
#include <gp_Trsf.hxx>
#include <gp_Vec.hxx>

#include <chrono>
#include <cstdlib>
#include <filesystem>
#include <cmath>
#include <functional>
#include <iostream>
#include <random>
#include <set>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

namespace
{

constexpr const char* THE_DATASET_ENV_ENABLE   = "OCCT_SEWING_DATASET_TESTS";
constexpr const char* THE_DATASET_ENV_SAMPLES  = "OCCT_SEWING_DATASET_SAMPLES";
constexpr const char* THE_DATASET_ENV_SEED     = "OCCT_SEWING_DATASET_SEED";
constexpr const char* THE_DATASET_ENV_ROOT     = "OCCT_SEWING_DATASET_ROOT";
constexpr const char* THE_DATASET_ENV_MIN_SECONDS = "OCCT_SEWING_DATASET_MIN_SECONDS";
constexpr const char* THE_DATASET_ENV_MAX_ITERS   = "OCCT_SEWING_DATASET_MAX_ITERS";
constexpr const char* THE_DATASET_DEFAULT_ROOT = "data/opencascade-dataset-7.9.0";

struct SewingShapeRun
{
  double       TimeSeconds = 0.0;
  bool         IsDone      = false;
  int          NbSewnEdges = 0;
  int          Iterations  = 1;
  TopoDS_Shape Shape;
};

struct GraphShapeMetrics
{
  bool IsGraphDone            = false;
  bool IsShapeValid           = false;
  int  NbActiveFaces          = 0;
  int  NbActiveEdges          = 0;
  int  NbActiveVertices       = 0;
  int  NbFreeEdges            = 0;
  int  NbMultipleEdges        = 0;
  int  NbDegeneratedEdges     = 0;
  int  NbInvalidFaceAdjacents = 0;
};

TopoDS_Shape reconstructSewnShape(const BRepGraph& theGraph);

int getEnvInt(const char* theVarName, const int theDefault)
{
  const char* aRaw = std::getenv(theVarName);
  if (aRaw == nullptr || *aRaw == '\0')
  {
    return theDefault;
  }
  try
  {
    return std::stoi(aRaw);
  }
  catch (const std::exception&)
  {
    return theDefault;
  }
}

bool isDatasetTestEnabled()
{
  return getEnvInt(THE_DATASET_ENV_ENABLE, 0) != 0;
}

std::filesystem::path datasetRootPath()
{
  const char* aRoot = std::getenv(THE_DATASET_ENV_ROOT);
  if (aRoot != nullptr && *aRoot != '\0')
  {
    return std::filesystem::path(aRoot);
  }
  return std::filesystem::path(THE_DATASET_DEFAULT_ROOT);
}

double datasetMinSeconds()
{
  return static_cast<double>(std::max(1, getEnvInt(THE_DATASET_ENV_MIN_SECONDS, 1)));
}

int datasetMaxIterations()
{
  return std::max(1, getEnvInt(THE_DATASET_ENV_MAX_ITERS, 10000));
}

NCollection_Sequence<TCollection_AsciiString> selectDatasetBrepFiles()
{
  NCollection_Sequence<TCollection_AsciiString> aResult;
  const std::filesystem::path                   aRoot = datasetRootPath();
  if (!std::filesystem::exists(aRoot))
  {
    return aResult;
  }

  std::vector<std::filesystem::path> aCandidates;
  for (const std::filesystem::directory_entry& anEntry :
       std::filesystem::recursive_directory_iterator(aRoot))
  {
    if (!anEntry.is_regular_file())
    {
      continue;
    }
    const std::filesystem::path& aPath = anEntry.path();
    if (aPath.extension() == ".brep" || aPath.extension() == ".BREP")
    {
      aCandidates.push_back(aPath);
    }
  }

  if (aCandidates.empty())
  {
    return aResult;
  }
  std::sort(aCandidates.begin(), aCandidates.end());

  const int aSeed        = getEnvInt(THE_DATASET_ENV_SEED, 42);
  const int aMaxSamples  = std::max(1, getEnvInt(THE_DATASET_ENV_SAMPLES, 8));
  const int aSampleCount = std::min(static_cast<int>(aCandidates.size()), aMaxSamples);

  std::mt19937 aRng(static_cast<unsigned int>(aSeed));
  std::shuffle(aCandidates.begin(), aCandidates.end(), aRng);
  for (int anIdx = 0; anIdx < aSampleCount; ++anIdx)
  {
    aResult.Append(TCollection_AsciiString(aCandidates[anIdx].string().c_str()));
  }
  return aResult;
}

bool readBrepShape(const TCollection_AsciiString& thePath, TopoDS_Shape& theShape)
{
  BRep_Builder aBuilder;
  return BRepTools::Read(theShape, thePath.ToCString(), aBuilder) && !theShape.IsNull();
}

NCollection_Sequence<TopoDS_Shape> extractCopiedFacesFromShape(const TopoDS_Shape& theShape)
{
  NCollection_Sequence<TopoDS_Shape> aFaces;
  for (TopExp_Explorer anExp(theShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    BRepBuilderAPI_Copy aCopyFace(anExp.Current(), true);
    if (!aCopyFace.Shape().IsNull())
    {
      aFaces.Append(aCopyFace.Shape());
    }
  }
  return aFaces;
}

SewingShapeRun runGraphSewingWithShape(const NCollection_Sequence<TopoDS_Shape>& theFaces,
                                       const bool                                 theParallel)
{
  SewingShapeRun aRun;
  BRep_Builder   aBuilder;
  TopoDS_Compound aInput;
  aBuilder.MakeCompound(aInput);
  for (int anIdx = 1; anIdx <= theFaces.Length(); ++anIdx)
  {
    aBuilder.Add(aInput, theFaces.Value(anIdx));
  }

  const auto aStart = std::chrono::steady_clock::now();
  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;
  aOpts.Parallel  = theParallel;

  BRepGraph aGraph;
  aGraph.Build(aInput, theParallel);
  if (aGraph.IsDone())
  {
    const BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);
    aRun.IsDone                                = aResult.IsDone;
    aRun.NbSewnEdges                           = aResult.NbSewnEdges;
    if (aResult.IsDone)
    {
      aRun.Shape = reconstructSewnShape(aGraph);
    }
  }
  const auto aEnd = std::chrono::steady_clock::now();
  aRun.TimeSeconds = std::chrono::duration<double>(aEnd - aStart).count();
  return aRun;
}

SewingShapeRun runLegacySewingWithShape(const NCollection_Sequence<TopoDS_Shape>& theFaces)
{
  SewingShapeRun          aRun;
  BRepBuilderAPI_Sewing   aSewer(1.0e-04);
  const auto              aStart = std::chrono::steady_clock::now();
  for (int anIdx = 1; anIdx <= theFaces.Length(); ++anIdx)
  {
    aSewer.Add(theFaces.Value(anIdx));
  }
  aSewer.Perform();
  const auto aEnd  = std::chrono::steady_clock::now();
  aRun.TimeSeconds = std::chrono::duration<double>(aEnd - aStart).count();
  aRun.Shape       = aSewer.SewedShape();
  aRun.IsDone      = !aRun.Shape.IsNull();
  aRun.NbSewnEdges = aSewer.NbContigousEdges();
  return aRun;
}

SewingShapeRun runGraphSewingWithShapeTimed(const NCollection_Sequence<TopoDS_Shape>& theFaces,
                                            const bool                                 theParallel,
                                            const double                               theMinSeconds)
{
  const int      aMaxIterations = datasetMaxIterations();
  SewingShapeRun aLastRun;
  double         aTotalSeconds = 0.0;
  int            anIterations  = 0;
  do
  {
    aLastRun = runGraphSewingWithShape(theFaces, theParallel);
    ++anIterations;
    aTotalSeconds += aLastRun.TimeSeconds;
    if (!aLastRun.IsDone)
    {
      aLastRun.Iterations = anIterations;
      return aLastRun;
    }
  } while (aTotalSeconds < theMinSeconds && anIterations < aMaxIterations);

  aLastRun.TimeSeconds = aTotalSeconds / static_cast<double>(anIterations);
  aLastRun.Iterations  = anIterations;
  return aLastRun;
}

SewingShapeRun runLegacySewingWithShapeTimed(const NCollection_Sequence<TopoDS_Shape>& theFaces,
                                             const double                               theMinSeconds)
{
  const int      aMaxIterations = datasetMaxIterations();
  SewingShapeRun aLastRun;
  double         aTotalSeconds = 0.0;
  int            anIterations  = 0;
  do
  {
    aLastRun = runLegacySewingWithShape(theFaces);
    ++anIterations;
    aTotalSeconds += aLastRun.TimeSeconds;
    if (!aLastRun.IsDone)
    {
      aLastRun.Iterations = anIterations;
      return aLastRun;
    }
  } while (aTotalSeconds < theMinSeconds && anIterations < aMaxIterations);

  aLastRun.TimeSeconds = aTotalSeconds / static_cast<double>(anIterations);
  aLastRun.Iterations  = anIterations;
  return aLastRun;
}

GraphShapeMetrics collectGraphMetrics(const TopoDS_Shape& theShape)
{
  GraphShapeMetrics aMetrics;
  if (theShape.IsNull())
  {
    return aMetrics;
  }

  aMetrics.IsShapeValid = BRepCheck_Analyzer(theShape).IsValid();

  BRepGraph aGraph;
  aGraph.Build(theShape, false);
  aMetrics.IsGraphDone = aGraph.IsDone();
  if (!aMetrics.IsGraphDone)
  {
    return aMetrics;
  }

  const BRepGraph::TopoView& aTopo = aGraph.Topo();
  for (int aFaceIdx = 0; aFaceIdx < aTopo.NbFaces(); ++aFaceIdx)
  {
    if (!aTopo.Face(BRepGraph_FaceId(aFaceIdx)).IsRemoved)
    {
      ++aMetrics.NbActiveFaces;
    }
  }
  for (int aVtxIdx = 0; aVtxIdx < aTopo.NbVertices(); ++aVtxIdx)
  {
    if (!aTopo.Vertex(BRepGraph_VertexId(aVtxIdx)).IsRemoved)
    {
      ++aMetrics.NbActiveVertices;
    }
  }
  for (int anEdgeIdx = 0; anEdgeIdx < aTopo.NbEdges(); ++anEdgeIdx)
  {
    const BRepGraph_EdgeId       anEdgeId(anEdgeIdx);
    const BRepGraphInc::EdgeDef& anEdge = aTopo.Edge(anEdgeId);
    if (anEdge.IsRemoved)
    {
      continue;
    }
    ++aMetrics.NbActiveEdges;
    if (BRepGraph_Tool::Edge::Degenerated(aGraph, anEdgeId))
    {
      ++aMetrics.NbDegeneratedEdges;
      continue;
    }
    const int aFaceCount = aTopo.CoEdgesOfEdge(anEdgeId).Length();
    if (aFaceCount == 1)
    {
      ++aMetrics.NbFreeEdges;
    }
    else if (aFaceCount > 2)
    {
      ++aMetrics.NbMultipleEdges;
    }
    else if (aFaceCount < 0)
    {
      ++aMetrics.NbInvalidFaceAdjacents;
    }
  }
  return aMetrics;
}

bool runDatasetCase(const TCollection_AsciiString& thePath,
                    SewingShapeRun&                theGraphRun,
                    SewingShapeRun&                theLegacyRun,
                    GraphShapeMetrics&             theGraphMetrics,
                    GraphShapeMetrics&             theLegacyMetrics,
                    int&                           theFaceCount,
                    TCollection_AsciiString&       theSkipReason)
{
  TopoDS_Shape aSource;
  if (!readBrepShape(thePath, aSource))
  {
    theSkipReason = "read-failed";
    return false;
  }

  const NCollection_Sequence<TopoDS_Shape> aFaces = extractCopiedFacesFromShape(aSource);
  theFaceCount                                     = aFaces.Length();
  if (aFaces.IsEmpty())
  {
    theSkipReason = "no-faces";
    return false;
  }

  const double aMinSeconds = datasetMinSeconds();
  theGraphRun              = runGraphSewingWithShapeTimed(aFaces, true, aMinSeconds);
  theLegacyRun             = runLegacySewingWithShapeTimed(aFaces, aMinSeconds);
  theGraphMetrics = collectGraphMetrics(theGraphRun.Shape);
  theLegacyMetrics = collectGraphMetrics(theLegacyRun.Shape);
  return true;
}

bool loadDatasetFaces(const std::filesystem::path& theDatasetRoot,
                      const char*                  theRelPath,
                      NCollection_Sequence<TopoDS_Shape>& theFaces,
                      TCollection_AsciiString&            theSkipReason)
{
  const std::filesystem::path aFullPath = theDatasetRoot / theRelPath;
  if (!std::filesystem::exists(aFullPath))
  {
    theSkipReason = "missing";
    return false;
  }

  TopoDS_Shape aSource;
  if (!readBrepShape(TCollection_AsciiString(aFullPath.string().c_str()), aSource))
  {
    theSkipReason = "read-failed";
    return false;
  }

  theFaces = extractCopiedFacesFromShape(aSource);
  if (theFaces.IsEmpty())
  {
    theSkipReason = "no-faces";
    return false;
  }
  return true;
}

void runAndCheckCuratedDatasetCase(const std::filesystem::path& theDatasetRoot,
                                   const char*                  theRelPath)
{
  const std::filesystem::path aFullPath = theDatasetRoot / theRelPath;
  if (!std::filesystem::exists(aFullPath))
  {
    GTEST_SKIP() << "Curated dataset file is missing: " << aFullPath.string();
  }

  SewingShapeRun         aGraphRun;
  SewingShapeRun         aLegacyRun;
  GraphShapeMetrics      aGraphMetrics;
  GraphShapeMetrics      aLegacyMetrics;
  int                    aFaceCount = 0;
  TCollection_AsciiString aSkipReason;
  const bool             isDone = runDatasetCase(TCollection_AsciiString(aFullPath.string().c_str()),
                                                 aGraphRun,
                                                 aLegacyRun,
                                                 aGraphMetrics,
                                                 aLegacyMetrics,
                                                 aFaceCount,
                                                 aSkipReason);
  ASSERT_TRUE(isDone) << "Curated dataset case skipped (" << aSkipReason.ToCString()
                      << "): " << aFullPath.string();
  ASSERT_TRUE(aGraphRun.IsDone) << "Graph run failed: " << aFullPath.string();
  ASSERT_TRUE(aLegacyRun.IsDone) << "Legacy run failed: " << aFullPath.string();
  ASSERT_TRUE(aGraphMetrics.IsGraphDone) << "Graph metrics failed: " << aFullPath.string();
  ASSERT_TRUE(aLegacyMetrics.IsGraphDone) << "Legacy metrics failed: " << aFullPath.string();
  EXPECT_TRUE(aGraphMetrics.IsShapeValid) << "Graph invalid: " << aFullPath.string();
  EXPECT_TRUE(aLegacyMetrics.IsShapeValid) << "Legacy invalid: " << aFullPath.string();

  EXPECT_LE(std::abs(aGraphMetrics.NbFreeEdges - aLegacyMetrics.NbFreeEdges), 2)
    << "Free-edge delta too high: " << aFullPath.string();
  EXPECT_LE(std::abs(aGraphMetrics.NbMultipleEdges - aLegacyMetrics.NbMultipleEdges), 2)
    << "Multiple-edge delta too high: " << aFullPath.string();
  EXPECT_LE(std::abs(aGraphMetrics.NbDegeneratedEdges - aLegacyMetrics.NbDegeneratedEdges), 4)
    << "Degenerated-edge delta too high: " << aFullPath.string();
  EXPECT_LE(std::abs(aGraphMetrics.NbActiveFaces - aLegacyMetrics.NbActiveFaces), 2)
    << "Face-count delta too high: " << aFullPath.string();

  std::cout << "[  CURATED] " << aFullPath.string() << " faces=" << aFaceCount
            << " tGraph=" << aGraphRun.TimeSeconds << "s(" << aGraphRun.Iterations << " iters)"
            << " tLegacy=" << aLegacyRun.TimeSeconds << "s(" << aLegacyRun.Iterations << " iters)"
            << "s speedup=" << (aGraphRun.TimeSeconds > 0.0
                                  ? (aLegacyRun.TimeSeconds / aGraphRun.TimeSeconds)
                                  : 0.0)
            << "x"
            << "s free(g/l)=" << aGraphMetrics.NbFreeEdges << "/" << aLegacyMetrics.NbFreeEdges
            << " mult(g/l)=" << aGraphMetrics.NbMultipleEdges << "/" << aLegacyMetrics.NbMultipleEdges
            << " deg(g/l)=" << aGraphMetrics.NbDegeneratedEdges << "/" << aLegacyMetrics.NbDegeneratedEdges
            << std::endl;
}

void runAndReportCuratedDatasetCase(const std::filesystem::path& theDatasetRoot,
                                    const char*                  theRelPath)
{
  const std::filesystem::path aFullPath = theDatasetRoot / theRelPath;
  if (!std::filesystem::exists(aFullPath))
  {
    GTEST_SKIP() << "Curated dataset file is missing: " << aFullPath.string();
  }

  SewingShapeRun          aGraphRun;
  SewingShapeRun          aLegacyRun;
  GraphShapeMetrics       aGraphMetrics;
  GraphShapeMetrics       aLegacyMetrics;
  int                     aFaceCount = 0;
  TCollection_AsciiString aSkipReason;
  const bool              isDone = runDatasetCase(TCollection_AsciiString(aFullPath.string().c_str()),
                                     aGraphRun,
                                     aLegacyRun,
                                     aGraphMetrics,
                                     aLegacyMetrics,
                                     aFaceCount,
                                     aSkipReason);
  ASSERT_TRUE(isDone) << "Curated dataset case skipped (" << aSkipReason.ToCString()
                      << "): " << aFullPath.string();
  ASSERT_TRUE(aGraphRun.IsDone) << "Graph run failed: " << aFullPath.string();
  ASSERT_TRUE(aLegacyRun.IsDone) << "Legacy run failed: " << aFullPath.string();
  ASSERT_TRUE(aGraphMetrics.IsGraphDone) << "Graph metrics failed: " << aFullPath.string();
  ASSERT_TRUE(aLegacyMetrics.IsGraphDone) << "Legacy metrics failed: " << aFullPath.string();

  std::cout << "[  CURATED] " << aFullPath.string() << " faces=" << aFaceCount
            << " tGraph=" << aGraphRun.TimeSeconds << "s(" << aGraphRun.Iterations << " iters)"
            << " tLegacy=" << aLegacyRun.TimeSeconds << "s(" << aLegacyRun.Iterations << " iters)"
            << "s speedup=" << (aGraphRun.TimeSeconds > 0.0
                                  ? (aLegacyRun.TimeSeconds / aGraphRun.TimeSeconds)
                                  : 0.0)
            << "x"
            << " valid(g/l)=" << (aGraphMetrics.IsShapeValid ? "1" : "0") << "/"
            << (aLegacyMetrics.IsShapeValid ? "1" : "0")
            << " free(g/l)=" << aGraphMetrics.NbFreeEdges << "/" << aLegacyMetrics.NbFreeEdges
            << " mult(g/l)=" << aGraphMetrics.NbMultipleEdges << "/" << aLegacyMetrics.NbMultipleEdges
            << " deg(g/l)=" << aGraphMetrics.NbDegeneratedEdges << "/" << aLegacyMetrics.NbDegeneratedEdges
            << std::endl;
}

//! Extract faces from a box shape using BRepBuilderAPI_Copy with copyGeom=true.
//! This produces faces with independent TShape edges that are geometrically
//! identical but topologically distinct - the exact scenario sewing must resolve.
NCollection_Sequence<TopoDS_Face> extractCopiedFaces(const TopoDS_Shape& theBox)
{
  NCollection_Sequence<TopoDS_Face> aFaces;
  for (TopExp_Explorer anExp(theBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    BRepBuilderAPI_Copy aCopier(anExp.Current(), /*copyGeom=*/true);
    aFaces.Append(TopoDS::Face(aCopier.Shape()));
  }
  return aFaces;
}

//! Build a compound from shapes and sew using Perform on a pre-built graph.
BRepGraphAlgo_Sewing::Result sewOnGraph(const NCollection_Sequence<TopoDS_Shape>& theShapes,
                                        const BRepGraphAlgo_Sewing::Options&      theOptions,
                                        BRepGraph&                                theGraph)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int anIdx = 1; anIdx <= theShapes.Length(); ++anIdx)
  {
    aBB.Add(aCompound, theShapes.Value(anIdx));
  }
  theGraph.Build(aCompound, theOptions.Parallel);
  if (!theGraph.IsDone())
  {
    return BRepGraphAlgo_Sewing::Result();
  }
  return BRepGraphAlgo_Sewing::Perform(theGraph, theOptions);
}

} // namespace

// ===================================================================
// Many-Faces Sewing: build a shell from a grid of disconnected faces
// ===================================================================

namespace
{

//! Build a grid of NxM copied box faces arranged in a flat grid.
NCollection_Sequence<TopoDS_Shape> buildFaceGrid(int    theNx,
                                                 int    theNy,
                                                 double theSizeX,
                                                 double theSizeY)
{
  NCollection_Sequence<TopoDS_Shape> aFaces;
  BRepPrimAPI_MakeBox                aBoxMaker(theSizeX, theSizeY, 1.0);
  const TopoDS_Shape&                aBox = aBoxMaker.Shape();

  // Find the bottom face (Z=0 plane).
  TopoDS_Face aTemplate;
  for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face(anExp.Current());
    GProp_GProps       aProps;
    BRepGProp::SurfaceProperties(aFace, aProps);
    gp_Pnt aCentroid = aProps.CentreOfMass();
    if (std::abs(aCentroid.Z()) < 0.01)
    {
      aTemplate = aFace;
      break;
    }
  }

  if (aTemplate.IsNull())
  {
    TopExp_Explorer anExp(aBox, TopAbs_FACE);
    aTemplate = TopoDS::Face(anExp.Current());
  }

  BRepGraph aTemplateGraph;
  aTemplateGraph.Build(aTemplate);

  for (int anIx = 0; anIx < theNx; ++anIx)
  {
    for (int anIy = 0; anIy < theNy; ++anIy)
    {
      gp_Trsf aTrsf;
      aTrsf.SetTranslation(gp_Vec(anIx * theSizeX, anIy * theSizeY, 0.0));

      if (aTemplateGraph.IsDone() && aTemplateGraph.Topo().NbFaces() > 0)
      {
        BRepGraph aTransGraph =
          BRepGraphAlgo_Transform::TransformFace(aTemplateGraph, BRepGraph_FaceId(0), aTrsf, true);
        if (aTransGraph.IsDone())
        {
          aFaces.Append(aTransGraph.Shapes().Reconstruct(BRepGraph_FaceId(0)));
          continue;
        }
      }

      BRepBuilderAPI_Copy      aCopier(aTemplate, true);
      TopoDS_Shape             aCopied = aCopier.Shape();
      BRepBuilderAPI_Transform aTransformer(aCopied, aTrsf, true);
      aFaces.Append(aTransformer.Shape());
    }
  }
  return aFaces;
}

} // namespace

// ===================================================================
// Performance: BRepGraphAlgo_Sewing vs BRepBuilderAPI_Sewing
// ===================================================================

namespace
{

constexpr int THE_NB_PERF_ITERS = 10;

struct GraphSewingProfile
{
  double PopulateBuildTime  = 0.0;
  double SewingTime         = 0.0;
  double ReconstructionTime = 0.0;
  double CoreTotalTime      = 0.0;
  double EndToEndTotalTime  = 0.0;
  bool   IsDone             = false;
  bool   HasResultShape     = false;
  int    NbSewnEdges        = 0;
  int    NbHistoryRecords   = 0;
};

TopoDS_Shape reconstructSewnShape(const BRepGraph& theGraph)
{
  const BRepGraph::TopoView& aTopo = theGraph.Topo();
  if (aTopo.NbCompounds() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_NodeId::Compound(0));
  }
  if (aTopo.NbCompSolids() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_NodeId::CompSolid(0));
  }
  if (aTopo.NbSolids() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_NodeId::Solid(0));
  }
  if (aTopo.NbShells() > 0)
  {
    return theGraph.Shapes().Reconstruct(BRepGraph_NodeId::Shell(0));
  }

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int aFaceIdx = 0; aFaceIdx < aTopo.NbFaces(); ++aFaceIdx)
  {
    aBB.Add(aCompound, theGraph.Shapes().Reconstruct(BRepGraph_FaceId(aFaceIdx)));
  }
  return aCompound;
}

GraphSewingProfile runGraphSewingProfiled(const NCollection_Sequence<TopoDS_Shape>& theFaces,
                                          const BRepGraphAlgo_Sewing::Options&      theOptions)
{
  GraphSewingProfile aProfile;

  const auto aSetupStart = std::chrono::steady_clock::now();

  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int anIdx = 1; anIdx <= theFaces.Length(); ++anIdx)
  {
    aBB.Add(aCompound, theFaces.Value(anIdx));
  }

  BRepGraph aGraph;
  aGraph.Build(aCompound, theOptions.Parallel);

  const auto aSetupEnd       = std::chrono::steady_clock::now();
  aProfile.PopulateBuildTime = std::chrono::duration<double>(aSetupEnd - aSetupStart).count();

  if (!aGraph.IsDone())
  {
    aProfile.CoreTotalTime = aProfile.PopulateBuildTime;
    return aProfile;
  }

  const auto                   aSewingStart = std::chrono::steady_clock::now();
  BRepGraphAlgo_Sewing::Result aResult      = BRepGraphAlgo_Sewing::Perform(aGraph, theOptions);
  const auto                   aSewingEnd   = std::chrono::steady_clock::now();
  aProfile.SewingTime       = std::chrono::duration<double>(aSewingEnd - aSewingStart).count();
  aProfile.CoreTotalTime    = std::chrono::duration<double>(aSewingEnd - aSetupStart).count();
  aProfile.IsDone           = aResult.IsDone;
  aProfile.NbSewnEdges      = aResult.IsDone ? aResult.NbSewnEdges : 0;
  aProfile.NbHistoryRecords = aGraph.History().NbRecords();

  if (!aResult.IsDone)
  {
    aProfile.EndToEndTotalTime = aProfile.CoreTotalTime;
    return aProfile;
  }

  const auto   aReconstructStart = std::chrono::steady_clock::now();
  TopoDS_Shape aSewnShape        = reconstructSewnShape(aGraph);
  const auto   aReconstructEnd   = std::chrono::steady_clock::now();
  aProfile.ReconstructionTime =
    std::chrono::duration<double>(aReconstructEnd - aReconstructStart).count();
  aProfile.EndToEndTotalTime = std::chrono::duration<double>(aReconstructEnd - aSetupStart).count();
  aProfile.HasResultShape    = !aSewnShape.IsNull();

  return aProfile;
}

void printPhaseBreakdown(const char* theLabel,
                         int         theNbIters,
                         double      theBuildMin,
                         double      theBuildSum,
                         double      theSewMin,
                         double      theSewSum,
                         double      theReconstructMin,
                         double      theReconstructSum,
                         double      theEndToEndMin,
                         double      theEndToEndSum)
{
  const double aBuildAvg       = theBuildSum / static_cast<double>(theNbIters);
  const double aSewAvg         = theSewSum / static_cast<double>(theNbIters);
  const double aReconstructAvg = theReconstructSum / static_cast<double>(theNbIters);
  const double aEndToEndAvg    = theEndToEndSum / static_cast<double>(theNbIters);

  std::cout << "[  PERF   ]   " << theLabel << " phase timings:" << std::endl;
  std::cout << "[  PERF   ]     graph/build/populate: min " << theBuildMin << " s, avg "
            << aBuildAvg << " s" << std::endl;
  std::cout << "[  PERF   ]     sewing core:          min " << theSewMin << " s, avg " << aSewAvg
            << " s" << std::endl;
  std::cout << "[  PERF   ]     reconstruction:       min " << theReconstructMin << " s, avg "
            << aReconstructAvg << " s" << std::endl;
  std::cout << "[  PERF   ]     end-to-end total:     min " << theEndToEndMin << " s, avg "
            << aEndToEndAvg << " s" << std::endl;
}

double runGraphSewing(const NCollection_Sequence<TopoDS_Shape>& theFaces,
                      bool                                      theParallel,
                      int&                                      theNbSewn)
{
  BRep_Builder    aBB;
  TopoDS_Compound aCompound;
  aBB.MakeCompound(aCompound);
  for (int anIdx = 1; anIdx <= theFaces.Length(); ++anIdx)
    aBB.Add(aCompound, theFaces.Value(anIdx));

  auto aStart = std::chrono::steady_clock::now();

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance = 1.0e-04;
  aOpts.Parallel  = theParallel;

  BRepGraph aGraph;
  aGraph.Build(aCompound, theParallel);

  BRepGraphAlgo_Sewing::Result aResult = BRepGraphAlgo_Sewing::Perform(aGraph, aOpts);

  auto aEnd = std::chrono::steady_clock::now();
  theNbSewn = aResult.IsDone ? aResult.NbSewnEdges : 0;
  return std::chrono::duration<double>(aEnd - aStart).count();
}

double runLegacySewing(const NCollection_Sequence<TopoDS_Shape>& theFaces)
{
  auto                  aStart = std::chrono::steady_clock::now();
  BRepBuilderAPI_Sewing aSewer(1.0e-04);
  for (int anIdx = 1; anIdx <= theFaces.Length(); ++anIdx)
    aSewer.Add(theFaces.Value(anIdx));
  aSewer.Perform();
  auto aEnd = std::chrono::steady_clock::now();
  return std::chrono::duration<double>(aEnd - aStart).count();
}

using FaceListBuilder = std::function<NCollection_Sequence<TopoDS_Shape>()>;

void runBenchmark(const char*            theLabel,
                  int                    theNbFaces,
                  const FaceListBuilder& theBuildFaces,
                  int                    theExpSewnSeq,
                  int                    theExpSewnPar)
{
  // Warm-up.
  {
    int                                aDummy = 0;
    NCollection_Sequence<TopoDS_Shape> aWarm  = theBuildFaces();
    runGraphSewing(aWarm, false, aDummy);
    aWarm = theBuildFaces();
    runGraphSewing(aWarm, true, aDummy);
    aWarm = theBuildFaces();
    runLegacySewing(aWarm);
  }

  double aSeqMin = RealLast(), aSeqSum = 0.0;
  double aParMin = RealLast(), aParSum = 0.0;
  double aLegMin = RealLast(), aLegSum = 0.0;
  int    aNbSewnSeq = 0, aNbSewnPar = 0;

  for (int anIter = 0; anIter < THE_NB_PERF_ITERS; ++anIter)
  {
    NCollection_Sequence<TopoDS_Shape> aFaces;
    double                             aTime;
    int                                aSewn = 0;

    aFaces     = theBuildFaces();
    aTime      = runGraphSewing(aFaces, false, aSewn);
    aNbSewnSeq = aSewn;
    aSeqMin    = std::min(aSeqMin, aTime);
    aSeqSum += aTime;

    aFaces     = theBuildFaces();
    aTime      = runGraphSewing(aFaces, true, aSewn);
    aNbSewnPar = aSewn;
    aParMin    = std::min(aParMin, aTime);
    aParSum += aTime;

    aFaces  = theBuildFaces();
    aTime   = runLegacySewing(aFaces);
    aLegMin = std::min(aLegMin, aTime);
    aLegSum += aTime;
  }

  const double aSeqAvg = aSeqSum / THE_NB_PERF_ITERS;
  const double aParAvg = aParSum / THE_NB_PERF_ITERS;
  const double aLegAvg = aLegSum / THE_NB_PERF_ITERS;

  std::cout << "[  PERF   ] " << theNbFaces << " faces (" << theLabel << "), " << THE_NB_PERF_ITERS
            << " iterations:" << std::endl;
  std::cout << "[  PERF   ]   Graph sequential:  min " << aSeqMin << " s, avg " << aSeqAvg << " s"
            << std::endl;
  std::cout << "[  PERF   ]   Graph parallel:    min " << aParMin << " s, avg " << aParAvg << " s"
            << std::endl;
  std::cout << "[  PERF   ]   Legacy:            min " << aLegMin << " s, avg " << aLegAvg << " s"
            << std::endl;
  if (aSeqAvg > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Speedup seq vs legacy: min " << aLegMin / aSeqMin << "x, avg "
              << aLegAvg / aSeqAvg << "x" << std::endl;
  }
  if (aParAvg > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Speedup par vs legacy: min " << aLegMin / aParMin << "x, avg "
              << aLegAvg / aParAvg << "x" << std::endl;
  }
  if (aParAvg > 1.0e-9 && aSeqAvg > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Parallel speedup:      min " << aSeqMin / aParMin << "x, avg "
              << aSeqAvg / aParAvg << "x" << std::endl;
  }

  if (theExpSewnSeq > 0)
  {
    EXPECT_EQ(aNbSewnSeq, theExpSewnSeq);
  }
  else if (theExpSewnSeq < 0)
  {
    EXPECT_GT(aNbSewnSeq, -theExpSewnSeq);
  }

  if (theExpSewnPar > 0)
  {
    EXPECT_EQ(aNbSewnPar, theExpSewnPar);
  }
  else if (theExpSewnPar < 0)
  {
    EXPECT_GT(aNbSewnPar, -theExpSewnPar);
  }
}

} // namespace

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_6Faces)
{
  BRepPrimAPI_MakeBox aBoxMaker(10.0, 20.0, 30.0);
  const TopoDS_Shape  aBox = aBoxMaker.Shape();

  runBenchmark(
    "box",
    6,
    [&]() {
      NCollection_Sequence<TopoDS_Shape> aFaces;
      NCollection_Sequence<TopoDS_Face>  aF = extractCopiedFaces(aBox);
      for (int anIdx = 1; anIdx <= aF.Length(); ++anIdx)
        aFaces.Append(aF.Value(anIdx));
      return aFaces;
    },
    12,
    12);
}

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_Grid5x5)
{
  runBenchmark("5x5 grid", 25, []() { return buildFaceGrid(5, 5, 2.0, 2.0); }, 40, 40);
}

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_Grid10x10)
{
  runBenchmark("10x10 grid", 100, []() { return buildFaceGrid(10, 10, 1.0, 1.0); }, 180, 180);
}

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_300Faces)
{
  runBenchmark(
    "50 boxes",
    300,
    []() {
      NCollection_Sequence<TopoDS_Shape> aAllFaces;
      for (int anIdx = 0; anIdx < 50; ++anIdx)
      {
        BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(anIdx * 10.0, 0.0, 0.0), 10.0, 10.0, 10.0);
        NCollection_Sequence<TopoDS_Face> aF = extractCopiedFaces(aBoxMaker.Shape());
        for (int aFIdx = 1; aFIdx <= aF.Length(); ++aFIdx)
          aAllFaces.Append(aF.Value(aFIdx));
      }
      return aAllFaces;
    },
    -300,
    -300);
}

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_Grid20x20)
{
  runBenchmark("20x20 grid", 400, []() { return buildFaceGrid(20, 20, 1.0, 1.0); }, 760, 760);
}

// ===================================================================
// Profiling-oriented: 50 iterations of large workloads.
// ===================================================================

constexpr int THE_NB_PROFILE_ITERS = 50;

TEST(BRepGraphAlgo_SewingTest, Profiling_Grid50x50_Sequential)
{
  int    aNbSewn         = 0;
  double aTotal          = 0.0;
  double aBuildMin       = RealLast();
  double aBuildSum       = 0.0;
  double aSewMin         = RealLast();
  double aSewSum         = 0.0;
  double aReconstructMin = RealLast();
  double aReconstructSum = 0.0;
  double aEndToEndMin    = RealLast();
  double aEndToEndSum    = 0.0;
  for (int anIter = 0; anIter < THE_NB_PROFILE_ITERS; ++anIter)
  {
    NCollection_Sequence<TopoDS_Shape> aFaces = buildFaceGrid(50, 50, 1.0, 1.0);
    BRepGraphAlgo_Sewing::Options      aOpts;
    aOpts.Tolerance = 1.0e-04;
    aOpts.Parallel  = false;

    GraphSewingProfile aProfile = runGraphSewingProfiled(aFaces, aOpts);
    aTotal += aProfile.CoreTotalTime;
    aBuildMin = std::min(aBuildMin, aProfile.PopulateBuildTime);
    aBuildSum += aProfile.PopulateBuildTime;
    aSewMin = std::min(aSewMin, aProfile.SewingTime);
    aSewSum += aProfile.SewingTime;
    aReconstructMin = std::min(aReconstructMin, aProfile.ReconstructionTime);
    aReconstructSum += aProfile.ReconstructionTime;
    aEndToEndMin = std::min(aEndToEndMin, aProfile.EndToEndTotalTime);
    aEndToEndSum += aProfile.EndToEndTotalTime;
    if (anIter == 0)
    {
      ASSERT_TRUE(aProfile.IsDone);
      ASSERT_TRUE(aProfile.HasResultShape);
      aNbSewn = aProfile.NbSewnEdges;
    }
  }
  std::cout << "[  PERF   ] 2500 faces (50x50), sequential, " << THE_NB_PROFILE_ITERS
            << " iters: " << aTotal << " s total, " << aTotal / THE_NB_PROFILE_ITERS
            << " s/iter, sewn: " << aNbSewn << std::endl;
  printPhaseBreakdown("sequential",
                      THE_NB_PROFILE_ITERS,
                      aBuildMin,
                      aBuildSum,
                      aSewMin,
                      aSewSum,
                      aReconstructMin,
                      aReconstructSum,
                      aEndToEndMin,
                      aEndToEndSum);
  EXPECT_GT(aNbSewn, 4800);
}

TEST(BRepGraphAlgo_SewingTest, Profiling_Grid50x50_Parallel)
{
  int    aNbSewn         = 0;
  double aTotal          = 0.0;
  double aBuildMin       = RealLast();
  double aBuildSum       = 0.0;
  double aSewMin         = RealLast();
  double aSewSum         = 0.0;
  double aReconstructMin = RealLast();
  double aReconstructSum = 0.0;
  double aEndToEndMin    = RealLast();
  double aEndToEndSum    = 0.0;
  for (int anIter = 0; anIter < THE_NB_PROFILE_ITERS; ++anIter)
  {
    NCollection_Sequence<TopoDS_Shape> aFaces = buildFaceGrid(50, 50, 1.0, 1.0);
    BRepGraphAlgo_Sewing::Options      aOpts;
    aOpts.Tolerance = 1.0e-04;
    aOpts.Parallel  = true;

    GraphSewingProfile aProfile = runGraphSewingProfiled(aFaces, aOpts);
    aTotal += aProfile.CoreTotalTime;
    aBuildMin = std::min(aBuildMin, aProfile.PopulateBuildTime);
    aBuildSum += aProfile.PopulateBuildTime;
    aSewMin = std::min(aSewMin, aProfile.SewingTime);
    aSewSum += aProfile.SewingTime;
    aReconstructMin = std::min(aReconstructMin, aProfile.ReconstructionTime);
    aReconstructSum += aProfile.ReconstructionTime;
    aEndToEndMin = std::min(aEndToEndMin, aProfile.EndToEndTotalTime);
    aEndToEndSum += aProfile.EndToEndTotalTime;
    if (anIter == 0)
    {
      ASSERT_TRUE(aProfile.IsDone);
      ASSERT_TRUE(aProfile.HasResultShape);
      aNbSewn = aProfile.NbSewnEdges;
    }
  }
  std::cout << "[  PERF   ] 2500 faces (50x50), parallel, " << THE_NB_PROFILE_ITERS
            << " iters: " << aTotal << " s total, " << aTotal / THE_NB_PROFILE_ITERS
            << " s/iter, sewn: " << aNbSewn << std::endl;
  printPhaseBreakdown("parallel",
                      THE_NB_PROFILE_ITERS,
                      aBuildMin,
                      aBuildSum,
                      aSewMin,
                      aSewSum,
                      aReconstructMin,
                      aReconstructSum,
                      aEndToEndMin,
                      aEndToEndSum);
  EXPECT_GT(aNbSewn, 4800);
}

TEST(BRepGraphAlgo_SewingTest, Profiling_Grid50x50_NoHistory)
{
  int    aNbSewn         = 0;
  double aTotal          = 0.0;
  double aBuildMin       = RealLast();
  double aBuildSum       = 0.0;
  double aSewMin         = RealLast();
  double aSewSum         = 0.0;
  double aReconstructMin = RealLast();
  double aReconstructSum = 0.0;
  double aEndToEndMin    = RealLast();
  double aEndToEndSum    = 0.0;
  for (int anIter = 0; anIter < THE_NB_PROFILE_ITERS; ++anIter)
  {
    NCollection_Sequence<TopoDS_Shape> aFaces = buildFaceGrid(50, 50, 1.0, 1.0);
    BRepGraphAlgo_Sewing::Options      aOpts;
    aOpts.Tolerance   = 1.0e-04;
    aOpts.Parallel    = false;
    aOpts.HistoryMode = false;

    GraphSewingProfile aProfile = runGraphSewingProfiled(aFaces, aOpts);
    aTotal += aProfile.CoreTotalTime;
    aBuildMin = std::min(aBuildMin, aProfile.PopulateBuildTime);
    aBuildSum += aProfile.PopulateBuildTime;
    aSewMin = std::min(aSewMin, aProfile.SewingTime);
    aSewSum += aProfile.SewingTime;
    aReconstructMin = std::min(aReconstructMin, aProfile.ReconstructionTime);
    aReconstructSum += aProfile.ReconstructionTime;
    aEndToEndMin = std::min(aEndToEndMin, aProfile.EndToEndTotalTime);
    aEndToEndSum += aProfile.EndToEndTotalTime;
    if (anIter == 0)
    {
      ASSERT_TRUE(aProfile.IsDone);
      ASSERT_TRUE(aProfile.HasResultShape);
      aNbSewn = aProfile.NbSewnEdges;
      EXPECT_EQ(aProfile.NbHistoryRecords, 0);
    }
  }
  std::cout << "[  PERF   ] 2500 faces (50x50), no history, " << THE_NB_PROFILE_ITERS
            << " iters: " << aTotal << " s total, " << aTotal / THE_NB_PROFILE_ITERS
            << " s/iter, sewn: " << aNbSewn << std::endl;
  printPhaseBreakdown("no-history",
                      THE_NB_PROFILE_ITERS,
                      aBuildMin,
                      aBuildSum,
                      aSewMin,
                      aSewSum,
                      aReconstructMin,
                      aReconstructSum,
                      aEndToEndMin,
                      aEndToEndSum);
  EXPECT_GT(aNbSewn, 4800);
}

TEST(BRepGraphAlgo_SewingTest, Profiling_Boxes200_Sequential)
{
  int    aNbSewn         = 0;
  double aTotal          = 0.0;
  double aBuildMin       = RealLast();
  double aBuildSum       = 0.0;
  double aSewMin         = RealLast();
  double aSewSum         = 0.0;
  double aReconstructMin = RealLast();
  double aReconstructSum = 0.0;
  double aEndToEndMin    = RealLast();
  double aEndToEndSum    = 0.0;
  for (int anIter = 0; anIter < THE_NB_PROFILE_ITERS; ++anIter)
  {
    NCollection_Sequence<TopoDS_Shape> aFaces;
    for (int aBIdx = 0; aBIdx < 200; ++aBIdx)
    {
      BRepPrimAPI_MakeBox               aBoxMaker(gp_Pnt(aBIdx * 10.0, 0.0, 0.0), 10.0, 10.0, 10.0);
      NCollection_Sequence<TopoDS_Face> aF = extractCopiedFaces(aBoxMaker.Shape());
      for (int aFIdx = 1; aFIdx <= aF.Length(); ++aFIdx)
        aFaces.Append(aF.Value(aFIdx));
    }

    BRepGraphAlgo_Sewing::Options aOpts;
    aOpts.Tolerance = 1.0e-04;
    aOpts.Parallel  = true;

    GraphSewingProfile aProfile = runGraphSewingProfiled(aFaces, aOpts);
    aTotal += aProfile.CoreTotalTime;
    aBuildMin = std::min(aBuildMin, aProfile.PopulateBuildTime);
    aBuildSum += aProfile.PopulateBuildTime;
    aSewMin = std::min(aSewMin, aProfile.SewingTime);
    aSewSum += aProfile.SewingTime;
    aReconstructMin = std::min(aReconstructMin, aProfile.ReconstructionTime);
    aReconstructSum += aProfile.ReconstructionTime;
    aEndToEndMin = std::min(aEndToEndMin, aProfile.EndToEndTotalTime);
    aEndToEndSum += aProfile.EndToEndTotalTime;
    if (anIter == 0)
    {
      ASSERT_TRUE(aProfile.IsDone);
      ASSERT_TRUE(aProfile.HasResultShape);
      aNbSewn = aProfile.NbSewnEdges;
    }
  }
  std::cout << "[  PERF   ] 1200 faces (200 boxes), parallel, " << THE_NB_PROFILE_ITERS
            << " iters: " << aTotal << " s total, " << aTotal / THE_NB_PROFILE_ITERS
            << " s/iter, sewn: " << aNbSewn << std::endl;
  printPhaseBreakdown("boxes200-parallel",
                      THE_NB_PROFILE_ITERS,
                      aBuildMin,
                      aBuildSum,
                      aSewMin,
                      aSewSum,
                      aReconstructMin,
                      aReconstructSum,
                      aEndToEndMin,
                      aEndToEndSum);
  EXPECT_GT(aNbSewn, 1000);
}

// ===================================================================
// Performance on real-world STEP model
// ===================================================================

TEST(BRepGraphAlgo_SewingTest, Performance_VsLegacy_STEPFile)
{
  TCollection_AsciiString aFilePath = "shape1.stp";

  DE_Wrapper                            aWrapper;
  occ::handle<DESTEP_ConfigurationNode> aNode = new DESTEP_ConfigurationNode();
  aWrapper.Bind(aNode);

  TopoDS_Shape aShape;
  bool         aReadOk = aWrapper.Read(aFilePath, aShape);
  if (!aReadOk || aShape.IsNull())
  {
    std::cout << "[  SKIP   ] Could not read " << aFilePath << std::endl;
    GTEST_SKIP() << "Failed to read STEP file";
  }

  NCollection_Sequence<TopoDS_Shape> aFaces;
  for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
  {
    BRepBuilderAPI_Copy aCopier(anExp.Current(), /*copyGeom=*/true);
    aFaces.Append(aCopier.Shape());
  }

  const int aNbFaces = aFaces.Length();
  if (aNbFaces == 0)
  {
    std::cout << "[  SKIP   ] No faces in " << aFilePath << std::endl;
    GTEST_SKIP() << "STEP file contains no faces";
  }

  std::cout << "[  INFO   ] STEP file: " << aFilePath << ", " << aNbFaces << " faces" << std::endl;

  const int aNbIters = aNbFaces > 500 ? 5 : (aNbFaces > 100 ? 20 : 50);

  // Warm-up.
  {
    int aDummy = 0;
    runGraphSewing(aFaces, false, aDummy);

    NCollection_Sequence<TopoDS_Shape> aFaces2;
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
      aFaces2.Append(aCopier.Shape());
    }
    runGraphSewing(aFaces2, true, aDummy);

    NCollection_Sequence<TopoDS_Shape> aFaces3;
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
      aFaces3.Append(aCopier.Shape());
    }
    runLegacySewing(aFaces3);
  }

  double aSeqMin = RealLast(), aSeqSum = 0.0;
  double aParMin = RealLast(), aParSum = 0.0;
  double aLegMin = RealLast(), aLegSum = 0.0;
  int    aNbSewnSeq = 0, aNbSewnPar = 0;

  for (int anIter = 0; anIter < aNbIters; ++anIter)
  {
    NCollection_Sequence<TopoDS_Shape> aFacesSeq, aFacesPar, aFacesLeg;
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
      aFacesSeq.Append(aCopier.Shape());
    }
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
      aFacesPar.Append(aCopier.Shape());
    }
    for (TopExp_Explorer anExp(aShape, TopAbs_FACE); anExp.More(); anExp.Next())
    {
      BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
      aFacesLeg.Append(aCopier.Shape());
    }

    double aTime;
    int    aSewn = 0;

    aTime      = runGraphSewing(aFacesSeq, false, aSewn);
    aNbSewnSeq = aSewn;
    aSeqMin    = std::min(aSeqMin, aTime);
    aSeqSum += aTime;

    aTime      = runGraphSewing(aFacesPar, true, aSewn);
    aNbSewnPar = aSewn;
    aParMin    = std::min(aParMin, aTime);
    aParSum += aTime;

    aTime   = runLegacySewing(aFacesLeg);
    aLegMin = std::min(aLegMin, aTime);
    aLegSum += aTime;
  }

  const double aSeqAvg = aSeqSum / aNbIters;
  const double aParAvg = aParSum / aNbIters;
  const double aLegAvg = aLegSum / aNbIters;

  std::cout << "[  PERF   ] " << aNbFaces << " faces (STEP file), " << aNbIters
            << " iterations:" << std::endl;
  std::cout << "[  PERF   ]   Graph sequential:  min " << aSeqMin << " s, avg " << aSeqAvg
            << " s, sewn: " << aNbSewnSeq << std::endl;
  std::cout << "[  PERF   ]   Graph parallel:    min " << aParMin << " s, avg " << aParAvg
            << " s, sewn: " << aNbSewnPar << std::endl;
  std::cout << "[  PERF   ]   Legacy:            min " << aLegMin << " s, avg " << aLegAvg << " s"
            << std::endl;
  if (aSeqMin > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Speedup seq vs legacy: min " << aLegMin / aSeqMin << "x, avg "
              << aLegAvg / aSeqAvg << "x" << std::endl;
  }
  if (aParMin > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Speedup par vs legacy: min " << aLegMin / aParMin << "x, avg "
              << aLegAvg / aParAvg << "x" << std::endl;
  }
  if (aParMin > 1.0e-9 && aSeqMin > 1.0e-9)
  {
    std::cout << "[  PERF   ]   Parallel speedup:      min " << aSeqMin / aParMin << "x, avg "
              << aSeqAvg / aParAvg << "x" << std::endl;
  }

  EXPECT_GT(aNbSewnSeq, 0);
  EXPECT_GT(aNbSewnPar, 0);
  EXPECT_EQ(aNbSewnSeq, aNbSewnPar);
}

TEST(BRepGraphAlgo_SewingTest, ParallelThreadPoolDiagnostics)
{
  const occ::handle<OSD_ThreadPool>& aPool              = OSD_ThreadPool::DefaultPool();
  const int                          aNbPoolThreads     = aPool->NbThreads();
  const int                          aNbDefLaunch       = aPool->NbDefaultThreadsToLaunch();
  const int                          aNbLogical         = OSD_Parallel::NbLogicalProcessors();
  const bool                         isUsingOcctThreads = OSD_Parallel::ToUseOcctThreads();

  std::cout << "[  INFO   ] OSD_Parallel diagnostics:" << std::endl;
  std::cout << "[  INFO   ]   Logical processors:        " << aNbLogical << std::endl;
  std::cout << "[  INFO   ]   ThreadPool NbThreads:       " << aNbPoolThreads
            << " (includes main thread)" << std::endl;
  std::cout << "[  INFO   ]   NbDefaultThreadsToLaunch:   " << aNbDefLaunch << std::endl;
  std::cout << "[  INFO   ]   Using OCCT threads:         "
            << (isUsingOcctThreads ? "YES" : "NO (TBB)") << std::endl;
  std::cout << "[  INFO   ]   Pool has worker threads:    " << (aPool->HasThreads() ? "YES" : "NO")
            << std::endl;

  EXPECT_GE(aNbPoolThreads, 2) << "Thread pool has only 1 thread, parallel tests are meaningless";

  constexpr int             THE_WORK_ITEMS = 10000;
  std::mutex                aThreadIdMutex;
  std::set<std::thread::id> aThreadIds;
  volatile int              aDummy = 0;

  OSD_Parallel::For(
    0,
    THE_WORK_ITEMS,
    [&](int theIdx) {
      volatile int aSum = 0;
      for (int j = 0; j < 100; ++j)
      {
        aSum += theIdx + j;
      }
      (void)aSum;

      const std::thread::id       aTid = std::this_thread::get_id();
      std::lock_guard<std::mutex> aLock(aThreadIdMutex);
      aThreadIds.insert(aTid);
    },
    false);

  const int aNbDistinctThreads = static_cast<int>(aThreadIds.size());
  std::cout << "[  INFO   ]   Distinct threads used:      " << aNbDistinctThreads << " / "
            << aNbPoolThreads << " (for " << THE_WORK_ITEMS << " work items)" << std::endl;

  EXPECT_GE(aNbDistinctThreads, 2) << "Parallel execution used only 1 thread";

  (void)aDummy;
}

TEST(BRepGraphAlgo_SewingTest, ParallelThreadUtilization_Sewing)
{
  const occ::handle<OSD_ThreadPool>& aPool          = OSD_ThreadPool::DefaultPool();
  const int                          aNbPoolThreads = aPool->NbThreads();
  if (aNbPoolThreads < 2)
  {
    GTEST_SKIP() << "Thread pool has only 1 thread, skipping utilization test";
  }

  constexpr int                      THE_GRID_SIZE = 10;
  constexpr double                   THE_CELL_SIZE = 1.0;
  NCollection_Sequence<TopoDS_Shape> aFaces;
  for (int aRow = 0; aRow < THE_GRID_SIZE; ++aRow)
  {
    for (int aCol = 0; aCol < THE_GRID_SIZE; ++aCol)
    {
      BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(aCol * THE_CELL_SIZE, aRow * THE_CELL_SIZE, 0.0),
                                    THE_CELL_SIZE,
                                    THE_CELL_SIZE,
                                    THE_CELL_SIZE);
      const TopoDS_Shape& aBox = aBoxMaker.Shape();
      for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
      {
        BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
        aFaces.Append(aCopier.Shape());
      }
    }
  }

  // Sequential.
  BRepGraphAlgo_Sewing::Options aSeqOpts;
  aSeqOpts.Tolerance = 1.0e-04;
  aSeqOpts.Parallel  = false;

  BRepGraph                    aSeqGraph;
  BRepGraphAlgo_Sewing::Result aSeqResult = sewOnGraph(aFaces, aSeqOpts, aSeqGraph);
  ASSERT_TRUE(aSeqResult.IsDone);

  // Parallel.
  BRepGraphAlgo_Sewing::Options aParOpts;
  aParOpts.Tolerance = 1.0e-04;
  aParOpts.Parallel  = true;

  BRepGraph                    aParGraph;
  BRepGraphAlgo_Sewing::Result aParResult = sewOnGraph(aFaces, aParOpts, aParGraph);
  ASSERT_TRUE(aParResult.IsDone);

  EXPECT_EQ(aSeqResult.NbSewnEdges, aParResult.NbSewnEdges);
  EXPECT_GT(aParResult.NbSewnEdges, 0);

  std::cout << "[  INFO   ] Thread pool: " << aNbPoolThreads << " threads, " << aFaces.Length()
            << " faces, " << aParResult.NbSewnEdges << " sewn edges" << std::endl;
}

TEST(BRepGraphAlgo_SewingTest, NoNestedParallel_SequentialInsideParallel)
{
  constexpr int                      THE_GRID_SIZE = 20;
  constexpr double                   THE_CELL_SIZE = 1.0;
  NCollection_Sequence<TopoDS_Shape> aFaces;
  for (int aRow = 0; aRow < THE_GRID_SIZE; ++aRow)
  {
    for (int aCol = 0; aCol < THE_GRID_SIZE; ++aCol)
    {
      BRepPrimAPI_MakeBox aBoxMaker(gp_Pnt(aCol * THE_CELL_SIZE, aRow * THE_CELL_SIZE, 0.0),
                                    THE_CELL_SIZE,
                                    THE_CELL_SIZE,
                                    THE_CELL_SIZE);
      const TopoDS_Shape& aBox = aBoxMaker.Shape();
      for (TopExp_Explorer anExp(aBox, TopAbs_FACE); anExp.More(); anExp.Next())
      {
        BRepBuilderAPI_Copy aCopier(anExp.Current(), true);
        aFaces.Append(aCopier.Shape());
      }
    }
  }

  BRepGraphAlgo_Sewing::Options aOpts;
  aOpts.Tolerance         = 1.0e-04;
  aOpts.Parallel          = true;
  aOpts.Cutting           = true;
  aOpts.SameParameterMode = true;

  BRepGraph                    aGraph;
  BRepGraphAlgo_Sewing::Result aResult = sewOnGraph(aFaces, aOpts, aGraph);
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_GT(aResult.NbSewnEdges, 0);

  std::cout << "[  INFO   ] No-nested-parallel test: " << aFaces.Length() << " faces, "
            << aResult.NbSewnEdges << " sewn (no deadlock)" << std::endl;
}

// ===================================================================
// Dataset-driven temporary A/B comparison (opt-in by env flag)
// ===================================================================

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_ABCompare_GraphMetricsAndTiming)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE
                 << "=1 to run dataset-driven temporary sewing tests";
  }

  const NCollection_Sequence<TCollection_AsciiString> aFiles = selectDatasetBrepFiles();
  if (aFiles.IsEmpty())
  {
    GTEST_SKIP() << "No .brep files found under dataset root: " << datasetRootPath().string();
  }

  std::cout << "[  INFO   ] Dataset root: " << datasetRootPath().string() << std::endl;
  std::cout << "[  INFO   ] Selected " << aFiles.Length()
            << " files (seed=" << getEnvInt(THE_DATASET_ENV_SEED, 42)
            << ", samples=" << getEnvInt(THE_DATASET_ENV_SAMPLES, 8)
            << ", minSeconds=" << datasetMinSeconds()
            << ", maxIters=" << datasetMaxIterations() << ")" << std::endl;

  int    aProcessed      = 0;
  int    aSkippedNoFaces = 0;
  int    aReadFailures   = 0;
  double aGraphTimeSum   = 0.0;
  double aLegacyTimeSum  = 0.0;

  for (int aFileIdx = 1; aFileIdx <= aFiles.Length(); ++aFileIdx)
  {
    const TCollection_AsciiString& aPath = aFiles.Value(aFileIdx);
    TopoDS_Shape                   aSource;
    if (!readBrepShape(aPath, aSource))
    {
      ++aReadFailures;
      std::cout << "[  WARN   ] Failed to read: " << aPath.ToCString() << std::endl;
      continue;
    }

    const NCollection_Sequence<TopoDS_Shape> aFaces = extractCopiedFacesFromShape(aSource);
    if (aFaces.IsEmpty())
    {
      ++aSkippedNoFaces;
      std::cout << "[  INFO   ] Skip (no faces): " << aPath.ToCString() << std::endl;
      continue;
    }
    std::cout << "[  INFO   ] Processing: " << aPath.ToCString()
              << " faces=" << aFaces.Length() << std::endl;
    const double         aMinSeconds = datasetMinSeconds();
    const SewingShapeRun aGraphRun   = runGraphSewingWithShapeTimed(aFaces, true, aMinSeconds);
    const SewingShapeRun aLegacyRun  = runLegacySewingWithShapeTimed(aFaces, aMinSeconds);

    ++aProcessed;
    aGraphTimeSum += aGraphRun.TimeSeconds;
    aLegacyTimeSum += aLegacyRun.TimeSeconds;

    ASSERT_TRUE(aGraphRun.IsDone) << "Graph sewing failed for " << aPath.ToCString();
    ASSERT_TRUE(aLegacyRun.IsDone) << "Legacy sewing failed for " << aPath.ToCString();
    ASSERT_FALSE(aGraphRun.Shape.IsNull()) << "Graph sewn shape is null for " << aPath.ToCString();
    ASSERT_FALSE(aLegacyRun.Shape.IsNull()) << "Legacy sewn shape is null for " << aPath.ToCString();

    const GraphShapeMetrics aGraphMetrics  = collectGraphMetrics(aGraphRun.Shape);
    const GraphShapeMetrics aLegacyMetrics = collectGraphMetrics(aLegacyRun.Shape);

    ASSERT_TRUE(aGraphMetrics.IsGraphDone) << "Graph metrics build failed for graph result: "
                                           << aPath.ToCString();
    ASSERT_TRUE(aLegacyMetrics.IsGraphDone) << "Graph metrics build failed for legacy result: "
                                            << aPath.ToCString();
    EXPECT_TRUE(aGraphMetrics.IsShapeValid) << "Graph result invalid for " << aPath.ToCString();
    EXPECT_TRUE(aLegacyMetrics.IsShapeValid) << "Legacy result invalid for " << aPath.ToCString();

    // Keep parity checks strict but not exact-equality for all models.
    EXPECT_LE(std::abs(aGraphMetrics.NbFreeEdges - aLegacyMetrics.NbFreeEdges), 2)
      << "Free-edge delta too high for " << aPath.ToCString();
    EXPECT_LE(std::abs(aGraphMetrics.NbMultipleEdges - aLegacyMetrics.NbMultipleEdges), 2)
      << "Multiple-edge delta too high for " << aPath.ToCString();
    EXPECT_LE(std::abs(aGraphMetrics.NbDegeneratedEdges - aLegacyMetrics.NbDegeneratedEdges), 4)
      << "Degenerated-edge delta too high for " << aPath.ToCString();
    EXPECT_LE(std::abs(aGraphMetrics.NbActiveFaces - aLegacyMetrics.NbActiveFaces), 2)
      << "Face-count delta too high for " << aPath.ToCString();

    std::cout << "[  DATA   ] " << aPath.ToCString() << " facesIn=" << aFaces.Length()
              << " tGraph=" << aGraphRun.TimeSeconds << "s(" << aGraphRun.Iterations << " iters)"
              << " tLegacy=" << aLegacyRun.TimeSeconds << "s(" << aLegacyRun.Iterations << " iters)"
              << " free(g/l)=" << aGraphMetrics.NbFreeEdges << "/" << aLegacyMetrics.NbFreeEdges
              << " mult(g/l)=" << aGraphMetrics.NbMultipleEdges << "/" << aLegacyMetrics.NbMultipleEdges
              << " deg(g/l)=" << aGraphMetrics.NbDegeneratedEdges << "/"
              << aLegacyMetrics.NbDegeneratedEdges << std::endl;
  }

  std::cout << "[  DATA   ] Processed=" << aProcessed << " skippedNoFaces=" << aSkippedNoFaces
            << " readFailures=" << aReadFailures << std::endl;
  if (aProcessed > 0)
  {
    std::cout << "[  DATA   ] AvgTime graph=" << (aGraphTimeSum / aProcessed)
              << "s legacy=" << (aLegacyTimeSum / aProcessed) << "s speedup="
              << ((aGraphTimeSum > 0.0) ? (aLegacyTimeSum / aGraphTimeSum) : 0.0) << "x"
              << std::endl;
  }

  ASSERT_GT(aProcessed, 0) << "No usable dataset models processed";
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_CuratedCoreSet_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }

  const std::filesystem::path aRoot = datasetRootPath();
  if (!std::filesystem::exists(aRoot))
  {
    GTEST_SKIP() << "Dataset root does not exist: " << aRoot.string();
  }

  // Curated hardcoded set: small + medium + practical sewing cases.
  const std::vector<const char*> aCuratedFiles = {
    "brep/OCC105.brep",
    "brep/bug30990.brep",
    "brep/bug25163_qf_25_39.brep",
    "brep/bug25044_face32.brep",
    "brep/OCC327b.brep",
    "brep/CTO908_topo101-o2.brep",
    "brep/buc60706p.brep",
    "brep/pro13403_face.brep",
    "brep/OCC22786-c.brep",
    "brep/OCC107-1.brep",
    "brep/bug25813_hlr-fillet2-tcl-f.brep",
    "brep/shading_wrongshape_017.brep",
    "brep/bug32767.brep",
    "brep/shading_142.brep",
    "brep/CTO909_tool_2.brep",
    "brep/bug25890_f1.brep",
    "brep/bug28719_display_issue.brep",
    "brep/bug24867_pump.brep",
    "brep/bug27814.brep",
    "brep/bug27614_CC.brep",
    "brep/bug28026_Ball.brep",
    "brep/buc60623b.brep",
    "brep/bug27015.brep",
    "brep/bug27760_leid.brep",
    "brep/bug23849_segment_1.brep",
    "brep/bug23849_segment_2.brep",
  };

  int       aProcessed = 0;

  for (const char* aRelPath : aCuratedFiles)
  {
    const std::filesystem::path aFullPath = aRoot / aRelPath;
    if (!std::filesystem::exists(aFullPath))
    {
      std::cout << "[  INFO   ] Curated skip (missing): " << aFullPath.string() << std::endl;
      continue;
    }

    SewingShapeRun      aGraphRun;
    SewingShapeRun      aLegacyRun;
    GraphShapeMetrics   aGraphMetrics;
    GraphShapeMetrics   aLegacyMetrics;
    int                 aFaceCount = 0;
    TCollection_AsciiString aSkipReason;
    const bool          isDone = runDatasetCase(TCollection_AsciiString(aFullPath.string().c_str()),
                                       aGraphRun,
                                       aLegacyRun,
                                       aGraphMetrics,
                                       aLegacyMetrics,
                                       aFaceCount,
                                       aSkipReason);
    if (!isDone)
    {
      std::cout << "[  INFO   ] Curated skip (" << aSkipReason.ToCString()
                << "): " << aFullPath.string() << std::endl;
      continue;
    }

    ++aProcessed;
    ASSERT_TRUE(aGraphRun.IsDone) << "Graph run failed: " << aFullPath.string();
    ASSERT_TRUE(aLegacyRun.IsDone) << "Legacy run failed: " << aFullPath.string();
    ASSERT_TRUE(aGraphMetrics.IsGraphDone) << "Graph metrics failed: " << aFullPath.string();
    ASSERT_TRUE(aLegacyMetrics.IsGraphDone) << "Legacy metrics failed: " << aFullPath.string();
    EXPECT_TRUE(aGraphMetrics.IsShapeValid) << "Graph invalid: " << aFullPath.string();
    EXPECT_TRUE(aLegacyMetrics.IsShapeValid) << "Legacy invalid: " << aFullPath.string();

    EXPECT_LE(std::abs(aGraphMetrics.NbFreeEdges - aLegacyMetrics.NbFreeEdges), 2)
      << "Free-edge delta too high: " << aFullPath.string();
    EXPECT_LE(std::abs(aGraphMetrics.NbMultipleEdges - aLegacyMetrics.NbMultipleEdges), 2)
      << "Multiple-edge delta too high: " << aFullPath.string();
    EXPECT_LE(std::abs(aGraphMetrics.NbDegeneratedEdges - aLegacyMetrics.NbDegeneratedEdges), 4)
      << "Degenerated-edge delta too high: " << aFullPath.string();

    std::cout << "[  CURATED] " << aFullPath.string() << " faces=" << aFaceCount
              << " tGraph=" << aGraphRun.TimeSeconds << "s tLegacy=" << aLegacyRun.TimeSeconds
              << "s free(g/l)=" << aGraphMetrics.NbFreeEdges << "/" << aLegacyMetrics.NbFreeEdges
              << std::endl;
  }

  ASSERT_GT(aProcessed, 0) << "No curated dataset case was processed";
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug32767_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndCheckCuratedDatasetCase(datasetRootPath(), "brep/bug32767.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_shading_142_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndCheckCuratedDatasetCase(datasetRootPath(), "brep/shading_142.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_CTO909_tool_2_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndCheckCuratedDatasetCase(datasetRootPath(), "brep/CTO909_tool_2.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug25890_f1_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndCheckCuratedDatasetCase(datasetRootPath(), "brep/bug25890_f1.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug25044_face1_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndCheckCuratedDatasetCase(datasetRootPath(), "brep/bug25044_face1.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug25044_face3_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndCheckCuratedDatasetCase(datasetRootPath(), "brep/bug25044_face3.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug25152_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndCheckCuratedDatasetCase(datasetRootPath(), "brep/bug25152.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_buc60782a_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndCheckCuratedDatasetCase(datasetRootPath(), "brep/buc60782a.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug28719_display_issue_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndReportCuratedDatasetCase(datasetRootPath(), "brep/bug28719_display_issue.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug24867_pump_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndReportCuratedDatasetCase(datasetRootPath(), "brep/bug24867_pump.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug27814_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndReportCuratedDatasetCase(datasetRootPath(), "brep/bug27814.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug27614_CC_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndReportCuratedDatasetCase(datasetRootPath(), "brep/bug27614_CC.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug28026_Ball_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndReportCuratedDatasetCase(datasetRootPath(), "brep/bug28026_Ball.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_buc60623b_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndCheckCuratedDatasetCase(datasetRootPath(), "brep/buc60623b.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug27015_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndReportCuratedDatasetCase(datasetRootPath(), "brep/bug27015.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug27760_leid_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndReportCuratedDatasetCase(datasetRootPath(), "brep/bug27760_leid.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug23849_segment_1_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndCheckCuratedDatasetCase(datasetRootPath(), "brep/bug23849_segment_1.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_bug23849_segment_2_ABCompare)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }
  runAndReportCuratedDatasetCase(datasetRootPath(), "brep/bug23849_segment_2.brep");
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_GraphParallelVsSequential)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }

  const std::filesystem::path aRoot = datasetRootPath();
  if (!std::filesystem::exists(aRoot))
  {
    GTEST_SKIP() << "Dataset root does not exist: " << aRoot.string();
  }

  const std::vector<const char*> aCuratedFiles = {
    "brep/bug28719_display_issue.brep",
    "brep/bug24867_pump.brep",
    "brep/bug27814.brep",
    "brep/bug27614_CC.brep",
    "brep/bug28026_Ball.brep",
    "brep/buc60623b.brep",
    "brep/bug27015.brep",
    "brep/bug27760_leid.brep",
    "brep/bug23849_segment_1.brep",
    "brep/bug23849_segment_2.brep",
    "brep/bug32767.brep",
    "brep/shading_142.brep",
    "brep/CTO909_tool_2.brep",
    "brep/bug25890_f1.brep",
    "brep/bug25044_face1.brep",
    "brep/bug25044_face3.brep",
    "brep/bug25152.brep",
    "brep/buc60782a.brep",
  };

  int       aProcessed = 0;
  double    aSeqTimeSum = 0.0;
  double    aParTimeSum = 0.0;

  for (const char* aRelPath : aCuratedFiles)
  {
    NCollection_Sequence<TopoDS_Shape> aFaces;
    TCollection_AsciiString            aSkipReason;
    if (!loadDatasetFaces(aRoot, aRelPath, aFaces, aSkipReason))
    {
      std::cout << "[  INFO   ] Parallel check skip (" << aSkipReason.ToCString()
                << "): " << (aRoot / aRelPath).string() << std::endl;
      continue;
    }

    ++aProcessed;
    const double         aMinSeconds = datasetMinSeconds();
    const SewingShapeRun aSeqRun = runGraphSewingWithShapeTimed(aFaces, false, aMinSeconds);
    const SewingShapeRun aParRun = runGraphSewingWithShapeTimed(aFaces, true, aMinSeconds);
    ASSERT_TRUE(aSeqRun.IsDone) << "Graph sequential failed for " << aRelPath;
    ASSERT_TRUE(aParRun.IsDone) << "Graph parallel failed for " << aRelPath;

    const GraphShapeMetrics aSeqMetrics = collectGraphMetrics(aSeqRun.Shape);
    const GraphShapeMetrics aParMetrics = collectGraphMetrics(aParRun.Shape);
    ASSERT_TRUE(aSeqMetrics.IsGraphDone);
    ASSERT_TRUE(aParMetrics.IsGraphDone);
    EXPECT_EQ(aSeqRun.NbSewnEdges, aParRun.NbSewnEdges) << "Sewn-edge mismatch for " << aRelPath;
    EXPECT_EQ(aSeqMetrics.NbFreeEdges, aParMetrics.NbFreeEdges) << "Free-edge mismatch for " << aRelPath;
    EXPECT_EQ(aSeqMetrics.NbMultipleEdges, aParMetrics.NbMultipleEdges)
      << "Multiple-edge mismatch for " << aRelPath;
    EXPECT_EQ(aSeqMetrics.NbDegeneratedEdges, aParMetrics.NbDegeneratedEdges)
      << "Degenerated-edge mismatch for " << aRelPath;

    aSeqTimeSum += aSeqRun.TimeSeconds;
    aParTimeSum += aParRun.TimeSeconds;
    std::cout << "[  PARCMP ] " << aRelPath << " faces=" << aFaces.Length()
              << " seq=" << aSeqRun.TimeSeconds << "s(" << aSeqRun.Iterations << " iters)"
              << " par=" << aParRun.TimeSeconds << "s(" << aParRun.Iterations << " iters)"
              << " speedup="
              << (aParRun.TimeSeconds > 0.0 ? (aSeqRun.TimeSeconds / aParRun.TimeSeconds) : 0.0)
              << "x" << std::endl;
  }

  ASSERT_GT(aProcessed, 0) << "No curated file processed for parallel comparison";
  std::cout << "[  PARCMP ] processed=" << aProcessed
            << " avgSeq=" << (aSeqTimeSum / aProcessed)
            << "s avgPar=" << (aParTimeSum / aProcessed)
            << "s avgSpeedup=" << (aParTimeSum > 0.0 ? (aSeqTimeSum / aParTimeSum) : 0.0)
            << "x" << std::endl;
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_Curated_buc60623b_GraphOnlyProfile)
{
  if (std::getenv(THE_DATASET_ENV_ENABLE) == nullptr)
  {
    setenv(THE_DATASET_ENV_ENABLE, "1", 0);
  }
  if (std::getenv(THE_DATASET_ENV_ROOT) == nullptr)
  {
    setenv(THE_DATASET_ENV_ROOT, "/Users/dpasukhi/work/OCCT/data/opencascade-dataset-7.9.0", 0);
  }

  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }

  const std::filesystem::path aRoot = datasetRootPath();
  if (!std::filesystem::exists(aRoot))
  {
    GTEST_SKIP() << "Dataset root does not exist: " << aRoot.string();
  }

  NCollection_Sequence<TopoDS_Shape> aFaces;
  TCollection_AsciiString            aSkipReason;
  ASSERT_TRUE(loadDatasetFaces(aRoot, "brep/buc60623b.brep", aFaces, aSkipReason))
    << "Failed to load buc60623b.brep: " << aSkipReason.ToCString();

  constexpr int THE_PROFILE_ITERS = 1600;
  SewingShapeRun aLastRun;
  double         aSumInnerTimes = 0.0;

  const auto aStart = std::chrono::steady_clock::now();
  for (int anIter = 0; anIter < THE_PROFILE_ITERS; ++anIter)
  {
    aLastRun = runGraphSewingWithShape(aFaces, true);
    ASSERT_TRUE(aLastRun.IsDone) << "Graph run failed at iteration " << anIter;
    aSumInnerTimes += aLastRun.TimeSeconds;
  }
  const auto aEnd        = std::chrono::steady_clock::now();
  const double aWallTime = std::chrono::duration<double>(aEnd - aStart).count();

  std::cout << "[ PROFILE ] buc60623b graph-only faces=" << aFaces.Length()
            << " iters=" << THE_PROFILE_ITERS
            << " wall=" << aWallTime << "s"
            << " avgInner=" << (aSumInnerTimes / THE_PROFILE_ITERS) << "s"
            << " sewnEdges(last)=" << aLastRun.NbSewnEdges << std::endl;
}

TEST(BRepGraphAlgo_SewingTest, DatasetBrep_CuratedNoFaceCases_SkippedCleanly)
{
  if (!isDatasetTestEnabled())
  {
    GTEST_SKIP() << "Set " << THE_DATASET_ENV_ENABLE << "=1 to run curated dataset tests";
  }

  const std::filesystem::path aRoot = datasetRootPath();
  if (!std::filesystem::exists(aRoot))
  {
    GTEST_SKIP() << "Dataset root does not exist: " << aRoot.string();
  }

  const std::vector<const char*> aExpectedNoFace = {
    "brep/bug27059.brep",
    "brep/offset_wire_020.brep",
  };

  int aObservedNoFace = 0;
  for (const char* aRelPath : aExpectedNoFace)
  {
    const std::filesystem::path aFullPath = aRoot / aRelPath;
    if (!std::filesystem::exists(aFullPath))
    {
      continue;
    }
    TopoDS_Shape aShape;
    ASSERT_TRUE(readBrepShape(TCollection_AsciiString(aFullPath.string().c_str()), aShape));
    const NCollection_Sequence<TopoDS_Shape> aFaces = extractCopiedFacesFromShape(aShape);
    if (aFaces.IsEmpty())
    {
      ++aObservedNoFace;
    }
  }

  EXPECT_GE(aObservedNoFace, 1) << "Expected at least one curated no-face case";
}
