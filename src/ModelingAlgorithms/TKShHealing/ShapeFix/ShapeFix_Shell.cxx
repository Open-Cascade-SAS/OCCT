// Created on: 1998-08-12
// Created by: Galina KULIKOVA
// Copyright (c) 1998-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

// pdn 17.12.98 ie_exhaust-A.stp

#include <Bnd_Box.hxx>
#include <NCollection_Array1.hxx>
#include <BRep_Builder.hxx>
#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <Message_Msg.hxx>
#include <Message_ProgressScope.hxx>
#include <NCollection_DataMap.hxx>
#include <NCollection_IncAllocator.hxx>
#include <NCollection_IndexedMap.hxx>
#include <ShapeAnalysis_Shell.hxx>
#include <ShapeBuild_ReShape.hxx>
#include <ShapeFix_Face.hxx>
#include <ShapeFix_Shell.hxx>
#include <Standard_Type.hxx>
#include <Standard_Integer.hxx>
#include <NCollection_List.hxx>
#include <NCollection_Map.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Compound.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopTools_ShapeMapHasher.hxx>
#include <NCollection_IndexedDataMap.hxx>
#include <NCollection_Sequence.hxx>

#include <unordered_set>
#include <unordered_map>
#include <stack>
#include <algorithm>

IMPLEMENT_STANDARD_RTTIEXT(ShapeFix_Shell, ShapeFix_Root)

namespace
{
// Type aliases for unordered maps with custom allocators
using FaceEdgesMap = NCollection_IndexedDataMap<TopoDS_Face, NCollection_Array1<TopoDS_Edge>>;
using EdgeFacesAllocator =
  NCollection_Allocator<std::pair<const TopoDS_Edge, NCollection_DynamicArray<TopoDS_Face>>>;
using EdgeFacesMap = std::unordered_map<TopoDS_Edge,
                                        NCollection_DynamicArray<TopoDS_Face>,
                                        TopTools_ShapeMapHasher,
                                        TopTools_ShapeMapHasher,
                                        EdgeFacesAllocator>;

// Default increment for dynamic array of faces per edge
constexpr int DEFAULT_EDGE_FACES_INCREMENT = 5;
} // namespace

//=================================================================================================

ShapeFix_Shell::ShapeFix_Shell()
{
  myStatus             = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  myFixFaceMode        = -1;
  myFixOrientationMode = -1;
  myFixFace            = new ShapeFix_Face;
  myNbShells           = 0;
  myNonManifold        = false;
}

//=================================================================================================

ShapeFix_Shell::ShapeFix_Shell(const TopoDS_Shell& shape)
{
  myStatus             = ShapeExtend::EncodeStatus(ShapeExtend_OK);
  myFixFaceMode        = -1;
  myFixOrientationMode = -1;
  myFixFace            = new ShapeFix_Face;
  Init(shape);
  myNonManifold = false;
}

//=================================================================================================

void ShapeFix_Shell::Init(const TopoDS_Shell& shell)
{
  myShape    = shell;
  myShell    = shell;
  myNbShells = 0;
}

//=================================================================================================

bool ShapeFix_Shell::Perform(const Message_ProgressRange& theProgress)
{
  bool status = false;
  if (Context().IsNull())
    SetContext(new ShapeBuild_ReShape);
  myFixFace->SetContext(Context());

  if (NeedFix(myFixFaceMode))
  {
    TopoDS_Shape S = Context()->Apply(myShell);

    // Get the number of faces for progress indication
    int aNbFaces = S.NbChildren();

    // Start progress scope (no need to check if progress exists -- it is safe)
    Message_ProgressScope aPS(theProgress, "Fixing face", aNbFaces);

    for (TopoDS_Iterator iter(S); iter.More() && aPS.More(); iter.Next(), aPS.Next())
    {
      TopoDS_Shape sh      = iter.Value();
      TopoDS_Face  tmpFace = TopoDS::Face(sh);
      myFixFace->Init(tmpFace);
      if (myFixFace->Perform())
      {
        status = true;
        myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
      }
    }

    // Halt algorithm in case of user's abort
    if (!aPS.More())
      return false;
  }

  TopoDS_Shape newsh = Context()->Apply(myShell);
  if (NeedFix(myFixOrientationMode))
    FixFaceOrientation(TopoDS::Shell(newsh), true, myNonManifold);

  TopoDS_Shape        aNewsh = Context()->Apply(newsh);
  ShapeAnalysis_Shell aSas;
  for (TopExp_Explorer aShellExp(aNewsh, TopAbs_SHELL); aShellExp.More(); aShellExp.Next())
  {
    TopoDS_Shell aCurShell = TopoDS::Shell(aShellExp.Current());
    if (aCurShell.Closed())
    {
      aSas.LoadShells(aCurShell);
      aSas.CheckOrientedShells(aCurShell, true);
      if (aSas.HasFreeEdges())
      {
        aCurShell.Closed(false);
        // clang-format off
        SendWarning (Message_Msg ("FixAdvShell.FixClosedFlag.MSG0"));//Shell has incorrect flag isClosed
        // clang-format on
      }
      aSas.Clear();
    }
  }

  if (status)
    myStatus |= ShapeExtend::EncodeStatus(ShapeExtend_DONE1);
  if (Status(ShapeExtend_DONE2))
    status = true;
  return status;
}

//=================================================================================================

static bool GetFreeEdges(const TopoDS_Shape&                                     aShape,
                         NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& MapEdges)
{
  for (TopExp_Explorer aExpF(aShape, TopAbs_FACE); aExpF.More(); aExpF.Next())
  {
    for (TopExp_Explorer aExpE(aExpF.Current(), TopAbs_EDGE); aExpE.More(); aExpE.Next())
    {
      TopoDS_Edge edge = TopoDS::Edge(aExpE.Current());
      if (!MapEdges.Contains(edge))
        MapEdges.Add(edge);
      else
        MapEdges.Remove(edge);
    }
  }
  return !MapEdges.IsEmpty();
}

/// Groups connected faces into separate sequences using existing connectivity data.
/// Uses depth-first search to find connected components through shared edges.
/// Each face appears in exactly one group, ensuring no duplicates across groups.
/// Groups are sorted by size with the largest group first.
/// @param theFaceEdges Map from faces to their constituent edges
/// @param theEdgeFaces Map from edges to faces that contain them
/// @return List of face sequences, each representing one connected component
static NCollection_List<NCollection_Sequence<TopoDS_Shape>> GetConnectedFaceGroups(
  const FaceEdgesMap& theFaceEdges,
  const EdgeFacesMap& theEdgeFaces)
{
  NCollection_List<NCollection_Sequence<TopoDS_Shape>> aConnectedGroups;

  if (theFaceEdges.IsEmpty())
  {
    return aConnectedGroups;
  }

  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aVisitedFaces(
    static_cast<int>(theFaceEdges.Size()));

  for (auto aFaceIter = theFaceEdges.begin(); aFaceIter != theFaceEdges.end(); ++aFaceIter)
  {
    const TopoDS_Face& aStartFace = aFaceIter.ChangeIterator().Key();

    if (aVisitedFaces.Contains(aStartFace))
    {
      continue;
    }

    // Start new connected group
    NCollection_Sequence<TopoDS_Shape> aConnectedGroup;

    // DFS traversal using STL stack with NCollection_Allocator
    using StackAllocator = NCollection_Allocator<TopoDS_Face>;
    std::stack<TopoDS_Face, std::deque<TopoDS_Face, StackAllocator>> aStack;
    aStack.push(aStartFace);
    aVisitedFaces.Add(aStartFace);

    while (!aStack.empty())
    {
      const TopoDS_Face aCurrentFace = aStack.top();
      aStack.pop();
      aConnectedGroup.Append(aCurrentFace);

      // Find connected faces through shared edges
      auto aFaceEdgesIter = theFaceEdges.Seek(aCurrentFace);
      if (aFaceEdgesIter)
      {
        const NCollection_Array1<TopoDS_Edge>& aFaceEdgesArray = *aFaceEdgesIter;

        for (int anEdgeIdx = aFaceEdgesArray.Lower(); anEdgeIdx <= aFaceEdgesArray.Upper();
             ++anEdgeIdx)
        {
          const TopoDS_Edge& anEdge = aFaceEdgesArray.Value(anEdgeIdx);

          auto anEdgeFacesIter = theEdgeFaces.find(anEdge);
          if (anEdgeFacesIter != theEdgeFaces.end())
          {
            const NCollection_DynamicArray<TopoDS_Face>& aConnectedFaces = anEdgeFacesIter->second;

            for (int aFaceIdx = 0; aFaceIdx < aConnectedFaces.Length(); ++aFaceIdx)
            {
              const TopoDS_Face& aNeighborFace = aConnectedFaces.Value(aFaceIdx);

              if (aVisitedFaces.Add(aNeighborFace))
              {
                aStack.push(aNeighborFace);
              }
            }
          }
        }
      }
    }

    // Insert in sorted order (largest groups first)
    bool anIsInserted = false;

    for (NCollection_List<NCollection_Sequence<TopoDS_Shape>>::Iterator anIter(aConnectedGroups);
         anIter.More();
         anIter.Next())
    {
      if (aConnectedGroup.Length() > anIter.Value().Length())
      {
        aConnectedGroups.InsertBefore(aConnectedGroup, anIter);
        anIsInserted = true;
        break;
      }
    }

    if (!anIsInserted)
    {
      aConnectedGroups.Append(aConnectedGroup);
    }
  }

  return aConnectedGroups;
}

/// Creates shells from connected face groups using connectivity analysis.
/// Processes only the largest connected group for shell construction, improving efficiency
/// by focusing on faces that are actually topologically connected.
/// @param theLfaces Input sequence of faces to process; returns unprocessed faces
/// @param theMapMultiConnectEdges Map of edges shared by more than 2 faces (multiconnectivity mode)
/// @param theSeqShells Output sequence of created shells
/// @param theMapFaceShells Output map linking faces to their containing shells
/// @param theErrFaces Output sequence of faces that could not be processed (e.g., Mobius-like)
/// @return true if shell construction was successful, false otherwise
static bool GetShells(
  NCollection_Sequence<TopoDS_Shape>&                                       theLfaces,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>&             theMapMultiConnectEdges,
  NCollection_Sequence<TopoDS_Shape>&                                       theSeqShells,
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& theMapFaceShells,
  NCollection_Sequence<TopoDS_Shape>&                                       theErrFaces)
{
  bool aDone = false;
  if (!theLfaces.Length())
  {
    return false;
  }
  TopoDS_Shell nshell;
  BRep_Builder B;
  B.MakeShell(nshell);
  bool                               anIsMultiConnex = !theMapMultiConnectEdges.IsEmpty();
  int                                aFaceIdx = 1, aFacesInShellCount = 1;
  NCollection_Sequence<TopoDS_Shape> aSeqUnconnectFaces;

  // Using STL containers because number of faces or edges can be too high
  // to keep them on flat basket OCCT map
  using EdgeMapAllocator =
    NCollection_Allocator<std::pair<const TopoDS_Edge, std::pair<bool, bool>>>;
  using EdgeOrientedMap = std::unordered_map<TopoDS_Edge,
                                             std::pair<bool, bool>,
                                             TopTools_ShapeMapHasher,
                                             TopTools_ShapeMapHasher,
                                             EdgeMapAllocator>;
  using TempProcessedEdges =
    NCollection_DataMap<TopoDS_Edge, std::pair<bool, bool>, TopTools_ShapeMapHasher>;

  FaceEdgesMap aFaceEdges;
  aFaceEdges.ReSize(theLfaces.Length());
  size_t                                aNumberOfEdges = 0;
  NCollection_DynamicArray<TopoDS_Edge> aTempEdges;
  for (NCollection_Sequence<TopoDS_Shape>::Iterator anFaceIter(theLfaces); anFaceIter.More();
       anFaceIter.Next())
  {
    aTempEdges.Clear();
    TopoDS_Face aFace = TopoDS::Face(anFaceIter.Value());
    for (TopExp_Explorer anEdgeExp(aFace, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next())
    {
      aTempEdges.Append(TopoDS::Edge(anEdgeExp.Current()));
      aNumberOfEdges++;
    }
    NCollection_Array1<TopoDS_Edge> aFaceEdgesArray(1, static_cast<int>(aTempEdges.Length()));
    for (int idx = 0; idx < aTempEdges.Length(); ++idx)
    {
      aFaceEdgesArray.SetValue(idx + 1, aTempEdges.Value(idx));
    }
    aFaceEdges.Add(aFace, std::move(aFaceEdgesArray));
  }

  EdgeFacesMap aEdgeFaces;
  aEdgeFaces.reserve(aNumberOfEdges);

  for (int aFaceInd = 1; aFaceInd <= aFaceEdges.Size(); ++aFaceInd)
  {
    const TopoDS_Face&                     aFace           = aFaceEdges.FindKey(aFaceInd);
    const NCollection_Array1<TopoDS_Edge>& aFaceEdgesArray = aFaceEdges.FindFromIndex(aFaceInd);

    for (int anEdgeInd = aFaceEdgesArray.Lower(); anEdgeInd <= aFaceEdgesArray.Upper(); ++anEdgeInd)
    {
      const TopoDS_Edge& anEdge = aFaceEdgesArray.Value(anEdgeInd);

      auto& aFacesArray = aEdgeFaces[anEdge];

      // Check if face already exists in the array
      bool aFaceExists = false;
      for (int aFaceCheckIdx = 0; aFaceCheckIdx < aFacesArray.Length(); ++aFaceCheckIdx)
      {
        if (aFacesArray.Value(aFaceCheckIdx).IsSame(aFace))
        {
          aFaceExists = true;
          break;
        }
      }

      if (aFacesArray.IsEmpty())
      {
        aFacesArray.SetIncrement(DEFAULT_EDGE_FACES_INCREMENT);
      }

      if (!aFaceExists)
      {
        aFacesArray.Append(aFace);
      }
    }
  }

  // Get connected groups of faces using existing connectivity data
  NCollection_List<NCollection_Sequence<TopoDS_Shape>> aConnectedGroups =
    GetConnectedFaceGroups(aFaceEdges, aEdgeFaces);

  // Process only the largest connected group for shell construction
  if (aConnectedGroups.IsEmpty())
  {
    return false;
  }

  // Some assumption that each edge can be in two orientations
  aNumberOfEdges = static_cast<size_t>((aNumberOfEdges / 2) + 1);

  EdgeOrientedMap aProcessedEdges;
  aProcessedEdges.reserve(aNumberOfEdges);

  NCollection_Sequence<TopoDS_Shape> aProcessingFaces = std::move(aConnectedGroups.First());

  TempProcessedEdges aTempProcessedEdges(static_cast<int>(aNumberOfEdges));
  for (; aFaceIdx <= aProcessingFaces.Length(); aFaceIdx++)
  {
    aTempProcessedEdges.Clear();

    int         aBadOrientationCount = 0, aGoodOrientationCount = 0;
    TopoDS_Face F1 = TopoDS::Face(aProcessingFaces.Value(aFaceIdx));
    // Get edges of the face
    const NCollection_Array1<TopoDS_Edge>& aFaceEdgesArray = aFaceEdges.FindFromKey(F1);

    for (int anEdgeInd = aFaceEdgesArray.Lower(); anEdgeInd <= aFaceEdgesArray.Upper(); ++anEdgeInd)
    {
      const TopoDS_Edge& edge = aFaceEdgesArray.Value(anEdgeInd);

      // if multiconnexity mode is equal to true faces contains
      // the same multiconnexity edges are not added to one shell.
      if (anIsMultiConnex && theMapMultiConnectEdges.Contains(edge))
        continue;

      auto aProcessedEdgeIt = aProcessedEdges.find(edge);

      if (aProcessedEdgeIt == aProcessedEdges.end())
      {
        std::pair<bool, bool>* aTempProcessedEdgeIt = aTempProcessedEdges.ChangeSeek(edge);
        if (!aTempProcessedEdgeIt)
        {
          std::pair<bool, bool> anEdgeOrientationPair{(edge.Orientation() == TopAbs_FORWARD),
                                                      (edge.Orientation() == TopAbs_REVERSED)};

          aTempProcessedEdges.Bind(edge, anEdgeOrientationPair);
        }
        else
        {
          aTempProcessedEdgeIt->first =
            aTempProcessedEdgeIt->first || (edge.Orientation() == TopAbs_FORWARD);
          aTempProcessedEdgeIt->second =
            aTempProcessedEdgeIt->second || (edge.Orientation() == TopAbs_REVERSED);
        }
        continue;
      }

      auto& aPair = aProcessedEdgeIt->second;

      const bool isDirect   = aPair.first;
      const bool isReversed = aPair.second;

      if ((edge.Orientation() == TopAbs_FORWARD && isDirect)
          || (edge.Orientation() == TopAbs_REVERSED && isReversed))
      {
        aBadOrientationCount++;
      }
      else if ((edge.Orientation() == TopAbs_FORWARD && isReversed)
               || (edge.Orientation() == TopAbs_REVERSED && isDirect))
      {
        aGoodOrientationCount++;
      }

      if (isDirect)
      {
        aPair.first = false;
      }
      else if (isReversed)
      {
        aPair.second = false;
      }

      if (!aPair.first && !aPair.second)
      {
        // if edge is processed in this face it is removed from map of processed edges
        aProcessedEdges.erase(aProcessedEdgeIt);
      }
    }

    if (!aBadOrientationCount && !aGoodOrientationCount && aTempProcessedEdges.IsEmpty())
      continue;

    // if face can not be added to shell it added to sequence of error faces.

    if (aGoodOrientationCount != 0 && aBadOrientationCount != 0)
    {
      theErrFaces.Append(F1);
      aProcessingFaces.Remove(aFaceIdx);
      aFacesInShellCount++;
      continue;
    }

    // Addition of face to shell. In the dependance of orientation faces in the shell
    //  added face can be reversed.

    if ((aGoodOrientationCount != 0 || aBadOrientationCount != 0) || aFacesInShellCount == 1)
    {
      if (aBadOrientationCount != 0)
      {
        F1.Reverse();

        for (TempProcessedEdges::Iterator aTempEdgeIter(aTempProcessedEdges); aTempEdgeIter.More();
             aTempEdgeIter.Next())
        {
          const TopoDS_Edge& edge                  = aTempEdgeIter.Key();
          const auto&        anEdgeOrientationPair = aTempEdgeIter.Value();

          std::pair<bool, bool> aRevertedPair{!anEdgeOrientationPair.first,
                                              !anEdgeOrientationPair.second};

          auto aProcessedEdgeIt = aProcessedEdges.find(edge);
          if (aProcessedEdgeIt == aProcessedEdges.end())
          {
            aProcessedEdges.emplace(edge, aRevertedPair);
          }
          else
          {
            auto& aPair = aProcessedEdgeIt->second;
            aPair       = aRevertedPair;
          }
        }
        aDone = true;
      }
      else
      {
        for (TempProcessedEdges::Iterator aTempEdgeIter(aTempProcessedEdges); aTempEdgeIter.More();
             aTempEdgeIter.Next())
        {
          const TopoDS_Edge& edge                  = aTempEdgeIter.Key();
          const auto&        anEdgeOrientationPair = aTempEdgeIter.Value();

          auto aProcessedEdgeIt = aProcessedEdges.find(edge);
          if (aProcessedEdgeIt == aProcessedEdges.end())
          {
            aProcessedEdges.emplace(edge, anEdgeOrientationPair);
          }
          else
          {
            auto& aPair  = aProcessedEdgeIt->second;
            aPair.first  = anEdgeOrientationPair.first;
            aPair.second = anEdgeOrientationPair.second;
          }
        }
      }
      aFacesInShellCount++;
      B.Add(nshell, F1);
      theMapFaceShells.Bind(F1, nshell);
      aProcessingFaces.Remove(aFaceIdx);

      // check if closed shell is obtained in multi connex mode and add to sequence of
      // shells and new shell begin to construct.
      // (check is n*2)
      if (anIsMultiConnex && BRep_Tool::IsClosed(nshell))
      {
        nshell.Closed(true);
        theSeqShells.Append(nshell);
        TopoDS_Shell nshellnext;
        B.MakeShell(nshellnext);
        nshell             = nshellnext;
        aFacesInShellCount = 1;
      }

      aFaceIdx = 0;
    }
    // if shell contains of one face. This face is added to sequence of faces.
    //  This shell is removed.
    if (aProcessingFaces.Length() && aFaceIdx == aProcessingFaces.Length()
        && aFacesInShellCount <= 2)
    {
      TopoDS_Iterator aItf(nshell, false);
      if (aItf.More())
      {
        aSeqUnconnectFaces.Append(aItf.Value());
        theMapFaceShells.UnBind(aItf.Value());
      }
      TopoDS_Shell nshellnext;
      B.MakeShell(nshellnext);
      nshell             = nshellnext;
      aFaceIdx           = 0;
      aFacesInShellCount = 1;
    }
  }
  bool isContains = false;
  for (int k = 1; k <= theSeqShells.Length() && !isContains; k++)
    isContains = nshell.IsSame(theSeqShells.Value(k));
  if (!isContains)
  {
    int          numFace = 0;
    TopoDS_Shape aFace;
    for (TopoDS_Iterator aItf(nshell, false); aItf.More(); aItf.Next())
    {
      aFace = aItf.Value();
      numFace++;
    }
    if (numFace > 1)
    {
      // close all closed shells in no multi connex mode
      if (!anIsMultiConnex)
        nshell.Closed(BRep_Tool::IsClosed(nshell));
      theSeqShells.Append(nshell);
    }
    else if (numFace == 1)
    {
      if (theMapFaceShells.IsBound(aFace))
        theMapFaceShells.UnBind(aFace);
      aProcessingFaces.Append(aFace);
    }
  }

  // Add all unprocessed connected groups (second group and after) to unconnected faces
  int aGroupIndex = 1;
  for (NCollection_List<NCollection_Sequence<TopoDS_Shape>>::Iterator aGroupIter(aConnectedGroups);
       aGroupIter.More();
       aGroupIter.Next(), ++aGroupIndex)
  {
    if (aGroupIndex == 1)
      continue; // Skip first group (already processed)

    const NCollection_Sequence<TopoDS_Shape>& aUnprocessedGroup = aGroupIter.Value();
    for (int anUnprocFaceIdx = 1; anUnprocFaceIdx <= aUnprocessedGroup.Length(); ++anUnprocFaceIdx)
    {
      aSeqUnconnectFaces.Append(aUnprocessedGroup.Value(anUnprocFaceIdx));
    }
  }

  theLfaces = std::move(aProcessingFaces);

  // Add unconnected faces from the largest group that couldn't be added to shells
  for (int j1 = 1; j1 <= aSeqUnconnectFaces.Length(); j1++)
  {
    theLfaces.Append(aSeqUnconnectFaces.Value(j1));
  }

  return aDone;
}

//=======================================================================
// function : AddMultiConexityFaces
// purpose  : In this function faces have only of multiconnexity boundary
//            are added to shells having free boundary contains the same
//            multiconnexity edges as faces.
//=======================================================================
static bool AddMultiConexityFaces(
  NCollection_Sequence<TopoDS_Shape>&                           Lface,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapMultiConnectEdges,
  NCollection_Sequence<TopoDS_Shape>&                           SeqShells,
  const NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher>& aMapFaceShells,
  const NCollection_IndexedDataMap<TopoDS_Shape,
                                   NCollection_List<TopoDS_Shape>,
                                   TopTools_ShapeMapHasher>&                      aMapEdgeFaces,
  NCollection_Sequence<TopoDS_Shape>&                                             ErrFaces,
  const bool                                                                      NonManifold)
{
  bool done = false;
  //  BRep_Builder aB;
  NCollection_Sequence<TopoDS_Shape> llPosibleShells;
  NCollection_Sequence<TopoDS_Shape> AddShapes;
  for (int i1 = 1; i1 <= Lface.Length(); i1++)
  {

    const TopoDS_Shape& aShape = Lface.Value(i1);

    int aNbMultEdges = 0;

    // Finds faces having only multiconnexity boundary.
    for (TopoDS_Iterator aItWires(aShape, false); aItWires.More(); aItWires.Next())
    {
      int aNbEdges = 0;
      for (TopoDS_Iterator aItEdges(aItWires.Value(), false); aItEdges.More();
           aItEdges.Next(), aNbEdges++)
      {
        const TopoDS_Shape& edge = aItEdges.Value();
        if (!aMapMultiConnectEdges.Contains(edge))
          continue;
        aNbMultEdges++;
      }
      if (!aNbMultEdges)
        continue;

      if (aNbMultEdges == aNbEdges)
        AddShapes.Append(aShape);
      else
        llPosibleShells.Append(aShape);
    }
  }

  // Attempt to create shell from unconnected which have not only multiconnexity boundary.
  NCollection_Sequence<TopoDS_Shape> aTmpShells;
  if (!llPosibleShells.IsEmpty())
  {
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>                   aMap;
    NCollection_Sequence<TopoDS_Shape>                                       aTmp;
    NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aTmpFaceShell;
    if (GetShells(llPosibleShells, aMap, aTmpShells, aTmpFaceShell, aTmp))
    {
      for (int kk = 1; kk <= aTmpShells.Length(); kk++)
      {
        const TopoDS_Shape&                                    aSh = aTmpShells.Value(kk);
        NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapEdges;
        if (GetFreeEdges(aSh, mapEdges))
        {
          int nbedge = 0;
          for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator amapIter(mapEdges);
               amapIter.More();
               amapIter.Next())
          {
            if (aMapMultiConnectEdges.Contains(amapIter.Key()))
              nbedge++;
          }
          if (nbedge && nbedge == mapEdges.Extent())
            AddShapes.Append(aSh);
        }
      }
    }
  }

  // Add chosen faces to shells.
  for (int k1 = 1; k1 <= AddShapes.Length(); k1++)
  {
    NCollection_DataMap<TopoDS_Shape, int, TopTools_ShapeMapHasher> MapOtherShells;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>          dire, reve;
    const TopoDS_Shape&                                             aSh = AddShapes.Value(k1);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>          mapEdges;
    if (!GetFreeEdges(aSh, mapEdges))
      continue;
    NCollection_List<TopoDS_Shape> lfaces;

    // Fill MapOtherShells which will contain shells with orientation in which selected shape aSh
    // will be add.

    for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator amapIter(mapEdges);
         amapIter.More();
         amapIter.Next())
    {
      if (!aMapMultiConnectEdges.Contains(amapIter.Key()))
        continue;
      TopoDS_Edge edge = TopoDS::Edge(amapIter.Key());
      if (edge.Orientation() == TopAbs_FORWARD)
        dire.Add(edge);
      else
        reve.Add(edge);
      NCollection_List<TopoDS_Shape> lf;
      lf = aMapEdgeFaces.FindFromKey(edge);
      lfaces.Append(lf);
    }
    for (NCollection_List<TopoDS_Shape>::Iterator aItl(lfaces); aItl.More(); aItl.Next())
    {
      TopoDS_Shape        aF          = aItl.Value();
      const TopoDS_Shape* pOthershell = aMapFaceShells.Seek(aF);
      if (!pOthershell)
        continue;

      TopoDS_Shape aOthershell = *pOthershell;
      if (MapOtherShells.IsBound(aOthershell))
        continue;
      if (!NonManifold && BRep_Tool::IsClosed(aOthershell))
        continue;

      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapShellEdges;
      GetFreeEdges(aOthershell, mapShellEdges);
      bool isAdd = true;
      for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator amapIter1(mapEdges);
           amapIter1.More() && isAdd;
           amapIter1.Next())
        isAdd = mapShellEdges.Contains(amapIter1.Key());

      if (!isAdd)
        continue;
      int nbdir = 0, nbrev = 0;

      // add only free face whome all edges contains in the shell as open boundary.
      for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aIte(mapShellEdges);
           aIte.More();
           aIte.Next())
      {
        TopoDS_Edge edgeS = TopoDS::Edge(aIte.Key());
        if (!aMapMultiConnectEdges.Contains(edgeS))
          continue;
        if ((edgeS.Orientation() == TopAbs_FORWARD && dire.Contains(edgeS))
            || (edgeS.Orientation() == TopAbs_REVERSED && reve.Contains(edgeS)))
          nbrev++;
        else if ((edgeS.Orientation() == TopAbs_FORWARD && reve.Contains(edgeS))
                 || (edgeS.Orientation() == TopAbs_REVERSED && dire.Contains(edgeS)))
          nbdir++;
      }
      if (nbdir && nbrev)
      {
        ErrFaces.Append(aSh);
        continue;
      }
      if (nbdir || nbrev)
      {
        int isReverse = (nbrev ? 1 : 0);
        MapOtherShells.Bind(aOthershell, isReverse);
      }
    }
    if (MapOtherShells.IsEmpty())
    {
      //      i1++;
      continue;
    }

    // Adds face to open shells containing the same multishared edges.
    // For nonmanifold mode creation ine shell from face and shells containing the same multishared
    // edges.
    //  If one face can be added to a few shells (case of compsolid) face will be added to each
    //  shell.
    done         = true;
    int FirstRev = 0, FirstInd = 0;
    int ind = 0;
    for (int l = 1; l <= SeqShells.Length(); l++)
    {
      const int* pIsRev = MapOtherShells.Seek(SeqShells.Value(l));
      if (!pIsRev)
        continue;
      ind++;
      int          isRev     = *pIsRev;
      TopoDS_Shape anewShape = (isRev ? aSh.Reversed() : aSh);

      BRep_Builder aB1;
      TopoDS_Shape aShell = SeqShells.Value(l);
      if (ind == 1 || !NonManifold)
      {
        if (ind == 1)
        {
          FirstRev = isRev;
          FirstInd = l;
        }
        for (TopExp_Explorer aE(anewShape, TopAbs_FACE); aE.More(); aE.Next())
          aB1.Add(aShell, aE.Current());
        SeqShells.ChangeValue(l) = aShell;
      }
      else if (NonManifold)
      {
        bool isReversed = ((isRev) || FirstRev) && (!(isRev) || !FirstRev);
        aShell          = SeqShells.Value(FirstInd);
        for (TopoDS_Iterator aItF(SeqShells.Value(l), false); aItF.More(); aItF.Next())
        {
          TopoDS_Shape nF = (isReversed ? aItF.Value().Reversed() : aItF.Value());
          aB1.Add(aShell, nF);
        }
        SeqShells.ChangeValue(FirstInd) = aShell;
        SeqShells.Remove(l--);
      }
    }

    dire.Clear();
    reve.Clear();
    for (TopExp_Explorer aEt(aSh, TopAbs_FACE); aEt.More(); aEt.Next())
    {
      for (int kk = 1; kk <= Lface.Length(); kk++)
      {
        if (aEt.Current().IsSame(Lface.Value(kk)))
          Lface.Remove(kk--);
      }
    }
  }
  return done;
}

//=======================================================================
// function : BoxIn
// purpose  : Check if one face contains inside other.
//=======================================================================
static int BoxIn(const Bnd_Box& theBox1, const Bnd_Box& theBox2)
{
  int    aNumIn = 0;
  double aXmin1, aYmin1, aXmax1, aYmax1, aXmin2, aYmin2, aXmax2, aYmax2, aZmin1, aZmax1, aZmin2,
    aZmax2;
  theBox1.Get(aXmin1, aYmin1, aZmin1, aXmax1, aYmax1, aZmax1);
  theBox2.Get(aXmin2, aYmin2, aZmin2, aXmax2, aYmax2, aZmax2);
  if (aXmin1 == aXmin2 && aXmax1 == aXmax2 && aYmin1 == aYmin2 && aYmax1 == aYmax2
      && aZmin1 == aZmin2 && aZmax1 == aZmax2)
    aNumIn = 0;
  else if (aXmin1 >= aXmin2 && aXmax1 <= aXmax2 && aYmin1 >= aYmin2 && aYmax1 <= aYmax2
           && aZmin1 >= aZmin2 && aZmax1 <= aZmax2)
    aNumIn = 1;
  else if (aXmin1 <= aXmin2 && aXmax1 >= aXmax2 && aYmin1 <= aYmin2 && aYmax1 >= aYmax2
           && aZmin1 <= aZmin2 && aZmax1 >= aZmax2)
    aNumIn = 2;
  return aNumIn;
}

//=======================================================================
// function : GetClosedShells
// purpose  : Check if one shell is a part from other shell.
//            For case of compsolid when a few shells are created from
//            the same set of faces.
//=======================================================================
static void GetClosedShells(NCollection_Sequence<TopoDS_Shape>& Shells,
                            NCollection_Sequence<TopoDS_Shape>& aRemainShells)
{
  NCollection_Array1<Bnd_Box> aBoxes(1, Shells.Length());
  for (int i = 1; i <= Shells.Length(); i++)
  {
    Bnd_Box Box;
    BRepBndLib::AddClose(Shells.Value(i), Box);
    aBoxes.SetValue(i, Box);
  }
  NCollection_Map<int> aMapNum;
  for (int j = 1; j <= aBoxes.Length(); j++)
  {
    for (int k = j + 1; k <= aBoxes.Length(); k++)
    {
      int NumIn = BoxIn(aBoxes.Value(j), aBoxes.Value(k));
      switch (NumIn)
      {
        case 1:
          aMapNum.Add(k);
          break;
        case 2:
          aMapNum.Add(j);
          break;
        default:
          break;
      }
    }
  }
  for (int i1 = 1; i1 <= Shells.Length(); i1++)
  {
    if (!aMapNum.Contains(i1))
      aRemainShells.Append(Shells.Value(i1));
  }
}

//=======================================================================
// function : GlueClosedCandidate
// purpose  : First, attempt to create closed shells from sequence of open shells.
//=======================================================================
static void GlueClosedCandidate(
  NCollection_Sequence<TopoDS_Shape>&                           OpenShells,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapMultiConnectEdges,
  NCollection_Sequence<TopoDS_Shape>&                           aSeqNewShells)
{
  // Creating new shells if some open shells contain the same free boundary.
  for (int i = 1; i < OpenShells.Length(); i++)
  {
    TopoDS_Shape                                           aShell = OpenShells.Value(i);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapEdges1;
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> dire, reve;
    if (!GetFreeEdges(aShell, mapEdges1))
      continue;

    for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aIte(mapEdges1);
         aIte.More();
         aIte.Next())
    {
      TopoDS_Edge edge1 = TopoDS::Edge(aIte.Key());
      if (!aMapMultiConnectEdges.Contains(edge1))
        break;
      if (edge1.Orientation() == TopAbs_FORWARD)
        dire.Add(edge1);
      else if (edge1.Orientation() == TopAbs_REVERSED)
        reve.Add(edge1);
    }
    if (mapEdges1.Extent() > (dire.Extent() + reve.Extent()))
      continue;

    // Filling map MapOtherShells which contains candidate to creation of closed shell
    //  with aShell.
    NCollection_DataMap<TopoDS_Shape, bool, TopTools_ShapeMapHasher> MapOtherShells;
    for (int j = i + 1; j <= OpenShells.Length(); j++)
    {
      bool                                                   isAddShell = true;
      bool                                                   isReversed = false;
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapEdges2;
      const TopoDS_Shape&                                    aShell2 = OpenShells.Value(j);
      if (!GetFreeEdges(aShell2, mapEdges2))
        continue;
      for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aIte2(mapEdges2);
           aIte2.More() && isAddShell;
           aIte2.Next())
      {
        TopoDS_Edge edge2 = TopoDS::Edge(aIte2.Key());
        if (!aMapMultiConnectEdges.Contains(edge2))
        {
          isAddShell = false;
          break;
          // continue;
        }
        isAddShell = (dire.Contains(edge2) || reve.Contains(edge2));
        if ((edge2.Orientation() == TopAbs_FORWARD && dire.Contains(edge2))
            || (edge2.Orientation() == TopAbs_REVERSED && reve.Contains(edge2)))
          isReversed = true;
      }

      if (!isAddShell)
        continue;
      MapOtherShells.Bind(OpenShells.Value(j), isReversed);
    }
    if (MapOtherShells.IsEmpty())
      continue;

    if (!MapOtherShells.IsEmpty())
    {
      // Case of compsolid when more than two shells have the same free boundary.
      NCollection_Sequence<TopoDS_Shape> aSeqCandidate;
      aSeqCandidate.Append(OpenShells.Value(i));

      for (NCollection_DataMap<TopoDS_Shape, bool, TopTools_ShapeMapHasher>::Iterator aIt(
             MapOtherShells);
           aIt.More();
           aIt.Next())
      {
        aSeqCandidate.Append(aIt.Key());
      }

      // Creation of all possible shells from chosen candidate.
      //  And the addition of them to temporary sequence.

      NCollection_Sequence<TopoDS_Shape> aTmpSeq;
      for (int k = 1; k <= aSeqCandidate.Length(); k++)
      {

        for (int l = k + 1; l <= aSeqCandidate.Length(); l++)
        {
          TopoDS_Shell aNewSh;
          BRep_Builder aB;
          aB.MakeShell(aNewSh);
          for (TopoDS_Iterator aIt1(aSeqCandidate.Value(k), false); aIt1.More(); aIt1.Next())
            aB.Add(aNewSh, aIt1.Value());
          bool isRev = MapOtherShells.Find(aSeqCandidate.Value(l));
          if (k != 1)
          {
            isRev = (isRev == MapOtherShells.Find(aSeqCandidate.Value(k)));
          }
          for (TopExp_Explorer aExp(aSeqCandidate.Value(l), TopAbs_FACE); aExp.More(); aExp.Next())
          {
            TopoDS_Shape aFace = (isRev ? aExp.Current().Reversed() : aExp.Current());
            aB.Add(aNewSh, aFace);
          }
          aTmpSeq.Append(aNewSh);
        }
      }

      // Choice from temporary sequence shells contains different set of faces (case of compsolid)
      NCollection_Sequence<TopoDS_Shape> aRemainShells;
      GetClosedShells(aTmpSeq, aRemainShells);
      aSeqNewShells.Append(aRemainShells);

      for (int j1 = i + 1; j1 <= OpenShells.Length(); j1++)
      {
        if (!MapOtherShells.IsBound(OpenShells.Value(j1)))
          continue;
        OpenShells.Remove(j1--);
      }
    }
    else
    {
      BRep_Builder aB;
      TopoDS_Shape aNewShell = aShell;
      TopoDS_Shape addShell;
      bool         isReversed = false;
      for (int j1 = i + 1; j1 <= OpenShells.Length(); j1++)
      {
        if (!MapOtherShells.Find(OpenShells.Value(j1), isReversed))
          continue;
        addShell = OpenShells.Value(j1);
        OpenShells.Remove(j1);
        break;
      }

      for (TopExp_Explorer aExpF(addShell, TopAbs_FACE); aExpF.More(); aExpF.Next())
      {
        TopoDS_Shape aFace = aExpF.Current();
        if (isReversed)
          aFace.Reverse();
        aB.Add(aNewShell, aFace);
      }
      aSeqNewShells.Append(aNewShell);
    }

    // OpenShells.ChangeValue(i) = aShell;
    OpenShells.Remove(i--);
  }
}

//=======================================================================
// function : CreateNonManifoldShells
// purpose  : Attempt to create max possible shells from open shells.
//=======================================================================

static void CreateNonManifoldShells(
  NCollection_Sequence<TopoDS_Shape>&                           SeqShells,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapMultiConnectEdges)
{
  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aMap;
  for (int i = 1; i <= SeqShells.Length(); i++)
  {
    const TopoDS_Shape&                                           aShell = SeqShells.Value(i);
    NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> medeg;
    TopExp::MapShapes(aShell, TopAbs_EDGE, medeg);
    for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator mit(
           aMapMultiConnectEdges);
         mit.More();
         mit.Next())
    {
      // for(TopExp_Explorer aExp(aShell,TopAbs_EDGE); aExp.More(); aExp.Next(),nbe++) {
      // TopoDS_Shape ae = aExp.Current();
      const TopoDS_Shape& ae = mit.Key();
      // if( aMapMultiConnectEdges.Contains(aExp.Current())) {
      if (medeg.Contains(ae))
      {
        if (aMap.Contains(ae))
          aMap.ChangeFromKey(ae).Append(aShell);
        else
        {
          NCollection_List<TopoDS_Shape> al;
          al.Append(aShell);
          aMap.Add(ae, al);
        }
      }
    }
  }
  NCollection_IndexedDataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aMapShells;
  for (int j = 1; j <= aMap.Extent(); j++)
  {
    const NCollection_List<TopoDS_Shape>& LShells = aMap.FindFromIndex(j);
    TopoDS_Shell                          aNewShell;
    BRep_Builder                          aB;
    aB.MakeShell(aNewShell);
    NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapmerge;
    bool                                                   ismerged = false;
    int                                                    num      = 1;
    for (NCollection_List<TopoDS_Shape>::Iterator alit(LShells); alit.More(); alit.Next(), num++)
    {
      if (!aMapShells.Contains(alit.Value()))
      {
        for (TopExp_Explorer aEf(alit.Value(), TopAbs_FACE); aEf.More(); aEf.Next())
        {
          aB.Add(aNewShell, aEf.Current());
        }
        ismerged = true;
        mapmerge.Add(alit.Value());
      }
      else if (ismerged)
      {
        TopoDS_Shape arshell = aMapShells.FindFromKey(alit.Value());
        while (aMapShells.Contains(arshell))
        {
          const TopoDS_Shape& ss = aMapShells.FindFromKey(arshell);
          if (ss.IsSame(arshell))
            break;
          arshell = ss;
        }

        if (mapmerge.Add(arshell))
        {
          for (TopExp_Explorer aEf(arshell, TopAbs_FACE); aEf.More(); aEf.Next())
          {
            aB.Add(aNewShell, aEf.Current());
          }
        }
      }
      else
      {
        TopoDS_Shape arshell = aMapShells.FindFromKey(alit.Value());
        while (aMapShells.Contains(arshell))
        {
          const TopoDS_Shape& ss = aMapShells.FindFromKey(arshell);
          if (ss.IsSame(arshell))
            break;
          arshell = ss;
        }
        if (num == 1)
        {
          for (TopExp_Explorer aEf(arshell, TopAbs_FACE); aEf.More(); aEf.Next())
            aB.Add(aNewShell, aEf.Current());

          mapmerge.Add(arshell);
        }
        else if (mapmerge.Add(arshell))
        {
          for (TopExp_Explorer aEf(arshell, TopAbs_FACE); aEf.More(); aEf.Next())
          {
            aB.Add(aNewShell, aEf.Current());
          }
        }
      }
    }
    if (mapmerge.Extent() > 1 || ismerged)
    {
      for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator alit1(mapmerge);
           alit1.More();
           alit1.Next())
      {
        const TopoDS_Shape& oldShell = alit1.Key();
        // while(aMapShells.Contains(oldShell)) {
        //   TopoDS_Shape ss = aMapShells.FindFromKey(oldShell);
        //   if(ss.IsSame(oldShell)) break;
        //   oldShell = ss;
        // }
        aMapShells.Add(oldShell, aNewShell);
      }
    }
  }
  NCollection_IndexedMap<TopoDS_Shape, TopTools_ShapeMapHasher> MapNewShells;
  for (int nn = 1; nn <= SeqShells.Length(); nn++)
  {
    if (aMapShells.Contains(SeqShells.Value(nn)))
    {
      TopoDS_Shape aNewShell = aMapShells.FindFromKey(SeqShells.Value(nn));
      while (aMapShells.Contains(aNewShell))
      {
        const TopoDS_Shape& ss = aMapShells.FindFromKey(aNewShell);
        if (ss.IsSame(aNewShell))
          break;
        aNewShell = ss;
      }
      MapNewShells.Add(aNewShell);

      SeqShells.Remove(nn--);
    }
  }
  for (int ii = 1; ii <= MapNewShells.Extent(); ii++)
    SeqShells.Append(MapNewShells.FindKey(ii));
}

//=======================================================================
// function : CreateClosedShell
// purpose  : Attempt to create max possible shells from open shells.
//=======================================================================

static void CreateClosedShell(
  NCollection_Sequence<TopoDS_Shape>&                           OpenShells,
  const NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>& aMapMultiConnectEdges)
{
  NCollection_Sequence<TopoDS_Shape> aNewShells;
  // First, attempt to create closed shells.
  GlueClosedCandidate(OpenShells, aMapMultiConnectEdges, aNewShells);

  // Creating new shells if some open shells contain the multishared same edges.
  for (int i = 1; i < OpenShells.Length(); i++)
  {
    bool         isAddShell = false;
    TopoDS_Shape aShell     = OpenShells.Value(i);
    bool         isReversed = false;
    for (int j = i + 1; j <= OpenShells.Length(); j++)
    {
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapEdges1;
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> dire, reve;
      if (!GetFreeEdges(aShell, mapEdges1))
        break;
      for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aIte(mapEdges1);
           aIte.More();
           aIte.Next())
      {
        TopoDS_Edge edge1 = TopoDS::Edge(aIte.Key());
        if (!aMapMultiConnectEdges.Contains(edge1))
          continue;
        if (edge1.Orientation() == TopAbs_FORWARD)
          dire.Add(edge1);
        else if (edge1.Orientation() == TopAbs_REVERSED)
          reve.Add(edge1);
      }
      if (dire.IsEmpty() && reve.IsEmpty())
        break;
      NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> mapEdges2;
      TopoDS_Shape                                           aShell2 = OpenShells.Value(j);
      if (!GetFreeEdges(aShell2, mapEdges2))
        continue;
      for (NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher>::Iterator aIte2(mapEdges2);
           aIte2.More();
           aIte2.Next())
      {
        TopoDS_Edge edge2 = TopoDS::Edge(aIte2.Key());
        if (!aMapMultiConnectEdges.Contains(edge2))
          continue;
        if (!dire.Contains(edge2) && !reve.Contains(edge2))
          continue;
        isAddShell = true;
        if ((edge2.Orientation() == TopAbs_FORWARD && dire.Contains(edge2))
            || (edge2.Orientation() == TopAbs_REVERSED && reve.Contains(edge2)))
          isReversed = true;
      }

      if (!isAddShell)
        continue;
      BRep_Builder aB;

      for (TopExp_Explorer aExpF21(OpenShells.Value(j), TopAbs_FACE); aExpF21.More();
           aExpF21.Next())
      {
        TopoDS_Shape aFace = aExpF21.Current();
        if (isReversed)
          aFace.Reverse();
        aB.Add(aShell, aFace);
      }

      OpenShells.ChangeValue(i) = aShell;
      OpenShells.Remove(j--);
    }
  }

  OpenShells.Append(aNewShells);
}

//=================================================================================================

bool ShapeFix_Shell::FixFaceOrientation(const TopoDS_Shell& shell,
                                        const bool          isAccountMultiConex,
                                        const bool          NonManifold)
{
  // myStatus = ShapeExtend::EncodeStatus (ShapeExtend_OK);
  bool                               done = false;
  NCollection_Sequence<TopoDS_Shape> aSeqShells;
  NCollection_Sequence<TopoDS_Shape> aErrFaces; // Compound of faces like to Mebiuce leaf.
  NCollection_Sequence<TopoDS_Shape> Lface;
  NCollection_DataMap<TopoDS_Shape, TopoDS_Shape, TopTools_ShapeMapHasher> aMapFaceShells;
  myShell                                                              = shell;
  myShape                                                              = shell;
  int                                                    aNumMultShell = 0;
  int                                                    nbF           = 0;
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapAdded;
  for (TopoDS_Iterator iter(shell); iter.More(); iter.Next(), nbF++)
  {
    if (aMapAdded.Add(iter.Value()))
      Lface.Append(iter.Value());
  }
  if (Lface.Length() < nbF)
    done = true;

  NCollection_IndexedDataMap<TopoDS_Shape, NCollection_List<TopoDS_Shape>, TopTools_ShapeMapHasher>
    aMapEdgeFaces;
  TopExp::MapShapesAndAncestors(myShell, TopAbs_EDGE, TopAbs_FACE, aMapEdgeFaces);
  NCollection_Map<TopoDS_Shape, TopTools_ShapeMapHasher> aMapMultiConnectEdges;
  bool                                                   isFreeBoundaries = false;
  for (int k = 1; k <= aMapEdgeFaces.Extent(); k++)
  {
    const int aFaceCount = aMapEdgeFaces.FindFromIndex(k).Extent();
    if (!isFreeBoundaries && aFaceCount == 1)
    {
      TopoDS_Edge E = TopoDS::Edge(aMapEdgeFaces.FindKey(k));
      if (!BRep_Tool::Degenerated(E))
        isFreeBoundaries = true;
    }
    // Finds multishared edges
    else if (isAccountMultiConex && aFaceCount > 2)
      aMapMultiConnectEdges.Add(aMapEdgeFaces.FindKey(k));
  }
  if (BRep_Tool::IsClosed(myShell) ? isFreeBoundaries : !isFreeBoundaries)
  {
    myShell.Closed(!isFreeBoundaries);
    // clang-format off
    SendWarning (Message_Msg ("FixAdvShell.FixClosedFlag.MSG0"));//Shell has incorrect flag isClosed
    // clang-format on
  }
  bool isGetShells = true;
  // Gets possible shells with taking in account of multiconnexity.
  while (isGetShells && Lface.Length())
  {
    NCollection_Sequence<TopoDS_Shape> aTmpSeqShells;
    if (GetShells(Lface, aMapMultiConnectEdges, aTmpSeqShells, aMapFaceShells, aErrFaces))
    {
      done = true;
    }
    isGetShells = !aTmpSeqShells.IsEmpty();
    if (isGetShells)
      aSeqShells.Append(aTmpSeqShells);
  }
  if (!done)
    done = (aSeqShells.Length() > 1);
  bool aIsDone = false;
  if (Lface.Length() > 0 && aSeqShells.Length())
  {
    for (int jj = 1; jj <= Lface.Length(); jj++)
    {
      if (aMapFaceShells.IsBound(Lface.Value(jj)))
        aMapFaceShells.UnBind(Lface.Value(jj));
    }

    // Addition of faces having only multiconnexity boundary to shells having holes
    //  containing only the multiconnexity edges
    aIsDone = AddMultiConexityFaces(Lface,
                                    aMapMultiConnectEdges,
                                    aSeqShells,
                                    aMapFaceShells,
                                    aMapEdgeFaces,
                                    aErrFaces,
                                    NonManifold);
  }
  aNumMultShell = aSeqShells.Length();
  if (!aErrFaces.IsEmpty())
  {

    // if Shell contains of Mebius faces one shell will be created from each those face.
    BRep_Builder B;
    B.MakeCompound(myErrFaces);
    TopoDS_Compound aCompShells;
    B.MakeCompound(aCompShells);
    for (int n = 1; n <= aErrFaces.Length(); n++)
      B.Add(myErrFaces, aErrFaces.Value(n));
    if (aNumMultShell)
    {
      if (aNumMultShell == 1)
      {
        B.Add(aCompShells, aSeqShells.Value(1));
        for (int n1 = 1; n1 <= aErrFaces.Length(); n1++)
        {
          TopoDS_Shell aSh;
          B.MakeShell(aSh);
          B.Add(aSh, aErrFaces.Value(n1));
          B.Add(aCompShells, aSh);
        }
        myShape = aCompShells;
      }
      else
      {
        for (int i = 1; i <= aSeqShells.Length(); i++)
          B.Add(aCompShells, aSeqShells.Value(i));
        for (int n1 = 1; n1 <= aErrFaces.Length(); n1++)
        {
          TopoDS_Shell aSh;
          B.MakeShell(aSh);
          B.Add(aSh, aErrFaces.Value(n1));
          B.Add(aCompShells, aSh);
        }
        myShape = aCompShells;
      }
    }

    done     = true;
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_FAIL);
    // clang-format off
    SendWarning ( Message_Msg ( "FixAdvShell.FixOrientation.MSG20" ) );// Impossible to orient faces in shell, several shells created
    // clang-format on
    return true;
  }
  if (aNumMultShell > 1)
  {
    NCollection_Sequence<TopoDS_Shape> OpenShells;
    for (int i1 = 1; i1 <= aSeqShells.Length(); i1++)
    {
      TopoDS_Shape aShell = aSeqShells.Value(i1);
      if (!BRep_Tool::IsClosed(aShell))
      {
        OpenShells.Append(aShell);
        aSeqShells.Remove(i1--);
      }
    }
    if (OpenShells.Length() > 1)
      // Attempt of creation closed shell from open shells with taking into account multiconnexity.
      CreateClosedShell(OpenShells, aMapMultiConnectEdges);
    aSeqShells.Append(OpenShells);
  }

  // In the case if NonManifold is equal to true one non-manifold shell will be created.
  // Else compound from shells will be created if length of sequence of shape >1.
  if (Lface.Length())
  {

    for (int i = 1; i <= Lface.Length(); i++)
    {
      BRep_Builder aB;
      TopoDS_Shell OneShell;
      aB.MakeShell(OneShell);
      aB.Add(OneShell, Lface.Value(i));
      aSeqShells.Append(OneShell);
    }
  }
  if (NonManifold && aSeqShells.Length() > 1)
  {
    CreateNonManifoldShells(aSeqShells, aMapMultiConnectEdges);
  }
  if (!done)
    done = (aSeqShells.Length() > 1 || aIsDone);
  if (aSeqShells.Length() == 1)
  {
    myShell    = TopoDS::Shell(aSeqShells.Value(1));
    myShape    = myShell;
    myNbShells = 1;
  }
  else
  {
    BRep_Builder    B;
    TopoDS_Compound aCompShells;
    B.MakeCompound(aCompShells);
    for (int i = 1; i <= aSeqShells.Length(); i++)
      B.Add(aCompShells, aSeqShells.Value(i));
    myShape    = aCompShells;
    myNbShells = aSeqShells.Length();
  }
  if (done)
  {
    myStatus = ShapeExtend::EncodeStatus(ShapeExtend_DONE2);
    if (!Context().IsNull())
      Context()->Replace(shell, myShape);
    if (myNbShells == 1)
      // clang-format off
      SendWarning ( Message_Msg ( "FixAdvShell.FixOrientation.MSG0" ) );// Faces were incorrectly oriented in the shell, corrected
    else
      SendWarning ( Message_Msg ( "FixAdvShell.FixOrientation.MSG30" ) );// Improperly connected shell split into parts
    // clang-format on
    return true;
  }
  else
    return false;
}

//=================================================================================================

bool ShapeFix_Shell::Status(const ShapeExtend_Status status) const
{
  return ShapeExtend::DecodeStatus(myStatus, status);
}

//=================================================================================================

TopoDS_Shell ShapeFix_Shell::Shell()
{
  return myShell;
}

//=================================================================================================

TopoDS_Shape ShapeFix_Shell::Shape()
{
  return myShape;
}

//=================================================================================================

TopoDS_Compound ShapeFix_Shell::ErrorFaces() const
{
  return myErrFaces;
}

//=================================================================================================

void ShapeFix_Shell::SetMsgRegistrator(const occ::handle<ShapeExtend_BasicMsgRegistrator>& msgreg)
{
  ShapeFix_Root::SetMsgRegistrator(msgreg);
  myFixFace->SetMsgRegistrator(msgreg);
}

//=================================================================================================

void ShapeFix_Shell::SetPrecision(const double preci)
{
  ShapeFix_Root::SetPrecision(preci);
  myFixFace->SetPrecision(preci);
}

//=================================================================================================

void ShapeFix_Shell::SetMinTolerance(const double mintol)
{
  ShapeFix_Root::SetMinTolerance(mintol);
  myFixFace->SetMinTolerance(mintol);
}

//=================================================================================================

void ShapeFix_Shell::SetMaxTolerance(const double maxtol)
{
  ShapeFix_Root::SetMaxTolerance(maxtol);
  myFixFace->SetMaxTolerance(maxtol);
}

//=================================================================================================

int ShapeFix_Shell::NbShells() const
{
  return myNbShells;
}

//=================================================================================================

void ShapeFix_Shell::SetNonManifoldFlag(const bool isNonManifold)
{
  myNonManifold = isNonManifold;
}
