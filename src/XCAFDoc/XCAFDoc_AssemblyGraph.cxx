// Created on: 2022-05-11
// Copyright (c) 2022 OPEN CASCADE SAS
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

#include <Standard_NullObject.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <TDataStd_TreeNode.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_Tool.hxx>
#include <TDocStd_Document.hxx>
#include <XCAFDoc.hxx>
#include <XCAFDoc_AssemblyGraph.hxx>
#include <XCAFDoc_DocumentTool.hxx>
#include <XCAFDoc_ShapeTool.hxx>

// =======================================================================
// function : XCAFDoc_AssemblyGraph constructor
// purpose  : Builds an assembly graph from the OCAF document
// =======================================================================

XCAFDoc_AssemblyGraph::XCAFDoc_AssemblyGraph(const Handle(TDocStd_Document)& theDoc)
{
  Standard_NullObject_Raise_if(theDoc.IsNull(), "Null document!");

  myShapeTool = XCAFDoc_DocumentTool::ShapeTool(theDoc->Main());
  Standard_NoSuchObject_Raise_if(myShapeTool.IsNull(), "No XCAFDoc_ShapeTool attribute!");

  TDF_Label aDummy;
  buildGraph(aDummy);
}

// =======================================================================
// function : XCAFDoc_AssemblyGraph constructor
// purpose  : Builds an assembly graph from the OCAF label
// =======================================================================

XCAFDoc_AssemblyGraph::XCAFDoc_AssemblyGraph(const TDF_Label& theLabel)
{
  Standard_NullObject_Raise_if(theLabel.IsNull(), "Null label!");

  myShapeTool = XCAFDoc_DocumentTool::ShapeTool(theLabel);
  Standard_NoSuchObject_Raise_if(myShapeTool.IsNull(), "No XCAFDoc_ShapeTool attribute!");

  buildGraph(theLabel);
}

// =======================================================================
// function : IsDirectLink
// purpose  : Checks if one node is the direct child of other one
// =======================================================================

Standard_Boolean XCAFDoc_AssemblyGraph::IsDirectLink(const Standard_Integer theNode1,
                                                     const Standard_Integer theNode2) const
{
  if (!HasChildren(theNode1))
    return Standard_False;

  return GetChildren(theNode1).Contains(theNode2);
}

// =======================================================================
// function : GetNodeType
// purpose  : Returns node type
// =======================================================================

XCAFDoc_AssemblyGraph::NodeType XCAFDoc_AssemblyGraph::GetNodeType(
  const Standard_Integer theNode) const
{
  const NodeType* typePtr = myNodeTypes.Seek(theNode);
  if (typePtr == NULL)
    return NodeType_UNDEFINED;

  return (*typePtr);
}

// =======================================================================
// function : NbLinks
// purpose  : Calculates and returns the number of links
// =======================================================================

Standard_Integer XCAFDoc_AssemblyGraph::NbLinks() const
{
  Standard_Integer aNumLinks = 0;
  for (AdjacencyMap::Iterator it(myAdjacencyMap); it.More(); it.Next())
  {
    aNumLinks += it.Value().Extent();
  }
  return aNumLinks;
}

// =======================================================================
// function : GetUsageOccurrenceQuantity
// purpose  :
// =======================================================================

Standard_Integer XCAFDoc_AssemblyGraph::NbOccurrences(const Standard_Integer theNode) const
{
  const Standard_Integer* aUsageOQPtr = myUsages.Seek(theNode);
  if (aUsageOQPtr == NULL)
    return 0;

  return (*aUsageOQPtr);
}

// =======================================================================
// function : buildGraph
// purpose  : Builds an assembly graph from the OCAF document
// =======================================================================

void XCAFDoc_AssemblyGraph::buildGraph(const TDF_Label& theLabel)
{
  // We start from those shapes which are "free" in terms of XDE.
  TDF_LabelSequence aRoots;
  if (theLabel.IsNull() || (myShapeTool->Label() == theLabel))
    myShapeTool->GetFreeShapes(aRoots);
  else
    aRoots.Append(theLabel);

  for (TDF_LabelSequence::Iterator it(aRoots); it.More(); it.Next())
  {
    TDF_Label aLabel = it.Value();

    TDF_Label anOriginal;
    if (!myShapeTool->GetReferredShape(aLabel, anOriginal))
      anOriginal = aLabel;

    const Standard_Integer aRootId = addNode(anOriginal, 0);
    if (aRootId == 0)
      continue;

    myRoots.Add(aRootId);

    // Add components (the objects nested into the current one).
    if (myShapeTool->IsAssembly(anOriginal))
      addComponents(anOriginal, aRootId);
  }
}

// =======================================================================
// function : addComponents
// purpose  : Adds components for the given parent to the graph structure
// =======================================================================

void XCAFDoc_AssemblyGraph::addComponents(const TDF_Label&       theParent,
                                          const Standard_Integer theParentId)
{
  if (!myShapeTool->IsShape(theParent))
  {
    return; // We have to return here in order to prevent iterating by
            // sub-labels. For parts, sub-labels are used to encode
            // metadata which is out of interest in conceptual design
            // intent represented by assembly graph.
  }

  // Loop over the children (persistent representation of "part-of" relation).
  for (TDF_ChildIterator anIt(theParent); anIt.More(); anIt.Next())
  {
    TDF_Label aComponent = anIt.Value();

    // Add component
    const Standard_Integer aComponentId = addNode(aComponent, theParentId);
    if (aComponentId == 0)
      continue;

    // Protection against deleted empty labels (after expand compounds, for example).
    Handle(TDataStd_TreeNode) aJumpNode;
    if (!aComponent.FindAttribute(XCAFDoc::ShapeRefGUID(), aJumpNode))
      continue;

    // Jump to the referred object (the original).
    TDF_Label aChildOriginal;
    if (!aJumpNode.IsNull() && aJumpNode->HasFather())
      aChildOriginal = aJumpNode->Father()->Label(); // Declaration-level origin.

    if (aChildOriginal.IsNull())
      continue;

    // Add child
    const Standard_Integer aChildId = addNode(aChildOriginal, aComponentId);
    if (aChildId == 0)
      continue;

    // Process children: add components recursively.
    addComponents(aChildOriginal, aChildId);
  }
}

// =======================================================================
// function : addNode
// purpose  : Adds node into the graph
// =======================================================================

Standard_Integer XCAFDoc_AssemblyGraph::addNode(const TDF_Label&       theLabel,
                                                const Standard_Integer theParentId)
{
  NodeType aNodeType = NodeType_UNDEFINED;
  if (myShapeTool->IsAssembly(theLabel))
  {
    if (myShapeTool->IsFree(theLabel))
      aNodeType = NodeType_AssemblyRoot;
    else
      aNodeType = NodeType_Subassembly;
  }
  else if (myShapeTool->IsComponent(theLabel))
  {
    aNodeType = NodeType_Occurrence;
  }
  else if (myShapeTool->IsSubShape(theLabel))
  {
    aNodeType = NodeType_Subshape;
  }
  else if (myShapeTool->IsSimpleShape(theLabel))
  {
    aNodeType = NodeType_Part;
  }

  if (aNodeType == NodeType_UNDEFINED)
    return 0;

  // Get ID of the insertion-level node in the abstract assembly graph.
  const Standard_Integer aChildId = myNodes.Add(theLabel);
  myNodeTypes.Bind(aChildId, aNodeType);

  if (aNodeType != NodeType_Occurrence)
  {
    // Bind usage occurrences.
    Standard_Integer* aUsageOQPtr = myUsages.ChangeSeek(aChildId);
    if (aUsageOQPtr == NULL)
      aUsageOQPtr = myUsages.Bound(aChildId, 1);
    else
      ++(*aUsageOQPtr);
  }

  if (theParentId > 0)
  {
    // Add link
    TColStd_PackedMapOfInteger* aMapPtr = myAdjacencyMap.ChangeSeek(theParentId);
    if (aMapPtr == NULL)
      aMapPtr = myAdjacencyMap.Bound(theParentId, TColStd_PackedMapOfInteger());

    (*aMapPtr).Add(aChildId);
  }

  return aChildId;
}

// =======================================================================
// function : Iterator constructor
// purpose  : Iteration starts from the specified node.
// =======================================================================

XCAFDoc_AssemblyGraph::Iterator::Iterator(const Handle(XCAFDoc_AssemblyGraph)& theGraph,
                                          const Standard_Integer               theNode)
{
  Standard_NullObject_Raise_if(theGraph.IsNull(), "Null assembly graph!");
  Standard_NullObject_Raise_if(theNode < 1, "Node ID must be positive one-based integer!");

  myGraph        = theGraph;
  myCurrentIndex = theNode;
}
