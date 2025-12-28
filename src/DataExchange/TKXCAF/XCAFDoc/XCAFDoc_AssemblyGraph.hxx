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

#ifndef _XCAFDoc_AssemblyGraph_HeaderFile
#define _XCAFDoc_AssemblyGraph_HeaderFile

#include <NCollection_DataMap.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TColStd_PackedMapOfInteger.hxx>
#include <NCollection_IndexedMap.hxx>

class TDF_Label;
class TDocStd_Document;
class XCAFDoc_ShapeTool;

// Assembly graph.
class XCAFDoc_AssemblyGraph : public Standard_Transient
{
public:
  //! \brief Type of the graph node.
  enum NodeType
  {
    NodeType_UNDEFINED = 0, //!< Undefined node type.
    NodeType_AssemblyRoot,  //!< Root node.
    NodeType_Subassembly,   //!< Intermediate node.
    NodeType_Occurrence,    //!< Assembly/part occurrence node.
    NodeType_Part,          //!< Leaf node to represent parts.
    NodeType_Subshape       //!< Subshape node.
  };

  //! \brief Type definition for graph adjacency matrix.
  //! This is how parent-component links are realized in the assembly graph.
  typedef NCollection_DataMap<int, TColStd_PackedMapOfInteger> AdjacencyMap;

public:
  //! \brief Graph iterator.
  class Iterator
  {
  public:
    //! \brief Accepting the assembly graph and starting node to iterate.
    //! Iteration starts from the specified node.
    //! \param[in]  theGraph - assembly graph to iterate.
    //! \param[in]  theNode  - graph node ID.
    Standard_EXPORT Iterator(const occ::handle<XCAFDoc_AssemblyGraph>& theGraph,
                             const int               theNode = 1);

    //! Checks if there are more graph nodes to iterate.
    //! \return true/false.
    bool More() const { return myCurrentIndex <= myGraph->NbNodes(); }

    //! \return 1-based ID of the current node.
    int Current() const { return myCurrentIndex; }

    //! Moves iterator to the next position.
    void Next() { ++myCurrentIndex; }

  private:
    occ::handle<XCAFDoc_AssemblyGraph> myGraph;        //!< Assembly graph to iterate.
    int              myCurrentIndex; //!< Current 1-based node ID.
  };

public:
  //! \brief Constructs graph from XCAF document.
  //! Construction of a formal graph will be done immediately.
  //! \param[in]  theDoc - document to iterate.
  Standard_EXPORT XCAFDoc_AssemblyGraph(const occ::handle<TDocStd_Document>& theDoc);

  //! \brief Constructs graph from XCAF label.
  //! Construction of a formal graph will be done immediately. The specified
  //! label is used as a starting position.
  //! \param[in]  theDoc   - document to iterate.
  //! \param[in]  theLabel - starting position.
  Standard_EXPORT XCAFDoc_AssemblyGraph(const TDF_Label& theLabel);

  //! \return Document shape tool.
  const occ::handle<XCAFDoc_ShapeTool>& GetShapeTool() const { return myShapeTool; }

  //! \brief Returns IDs of the root nodes.
  //! \return IDs of the root nodes.
  const TColStd_PackedMapOfInteger& GetRoots() const { return myRoots; }

  //! \brief Checks whether the assembly graph contains (n1, n2) directed link.
  //! \param[in]  theNode1 - one-based ID of the first node.
  //! \param[in]  theNode2 - one-based ID of the second node.
  //! \return true/false.
  Standard_EXPORT bool IsDirectLink(const int theNode1,
                                                const int theNode2) const;

  //! \brief Checks whether direct children exist for the given node.
  //! \param[in]  theNode - one-based node ID.
  //! \return true/false.
  bool HasChildren(const int theNode) const
  {
    return myAdjacencyMap.IsBound(theNode);
  }

  //! \brief Returns IDs of child nodes for the given node.
  //! \param[in]  theNode - one-based node ID.
  //! \return set of child IDs.
  const TColStd_PackedMapOfInteger& GetChildren(const int theNode) const
  {
    return myAdjacencyMap(theNode);
  }

  //! \brief Returns the node type from \ref NodeType enum.
  //! \param[in]  theNode - one-based node ID.
  //! \return node type.
  //! \sa NodeType
  Standard_EXPORT NodeType GetNodeType(const int theNode) const;

  //! \brief returns object ID by node ID.
  //! \param[in]  theNode - one-based node ID.
  //! \return persistent ID.
  const TDF_Label& GetNode(const int theNode) const { return myNodes(theNode); }

  //! \brief Returns the unordered set of graph nodes.
  //! \return graph nodes.
  const NCollection_IndexedMap<TDF_Label>& GetNodes() const { return myNodes; }

  //! \brief Returns the number of graph nodes.
  //! \return number of graph nodes.
  int NbNodes() const { return myNodes.Extent(); }

  //! \brief Returns the collection of graph links in the form of adjacency matrix.
  //! \return graph links.
  const AdjacencyMap& GetLinks() const { return myAdjacencyMap; }

  //! \brief Returns the number of graph links.
  //! \return number of graph links.
  Standard_EXPORT int NbLinks() const;

  //! Returns quantity of part usage occurrences.
  //! \param[in]  theNode - one-based part ID.
  //! \return usage occurrence quantity.
  Standard_EXPORT int NbOccurrences(const int theNode) const;

private:
  //! Builds graph out of OCAF XDE structure.
  //! \param[in]  theLabel - optional starting position.
  Standard_EXPORT void buildGraph(const TDF_Label& theLabel);

  //! Adds components for the given parent to the graph structure.
  //! \param[in]  theParent   - OCAF label of the parent object.
  //! \param[in]  theParentId - ID of the already registered node representing
  //!                           the parent object in the assembly graph
  //!                           being populated.
  Standard_EXPORT void addComponents(const TDF_Label&       theParent,
                                     const int theParentId);

  //! Adds node into the graph.
  //! \param[in]  theLabel    - label at insertion level.
  //! \param[in]  theParentId - parent one-based node IDS.
  //! \return one-based internal ID of the node.
  Standard_EXPORT int addNode(const TDF_Label&       theLabel,
                                           const int theParentId);

private:
  occ::handle<XCAFDoc_ShapeTool>  myShapeTool;                         //!< Document shape tool.
  TColStd_PackedMapOfInteger myRoots;                             //!< IDs of the root nodes.
                                                                  // clang-format off
  NCollection_IndexedMap<TDF_Label>                             myNodes;        //!< Maps assembly/part entries to graph node IDs.
                                                                  // clang-format on
  AdjacencyMap                                    myAdjacencyMap; //!< "Part-of" relations.
  NCollection_DataMap<int, NodeType> myNodeTypes;    //!< Node types.
  NCollection_DataMap<int,
                      int>           myUsages; //!< Occurrences usage.
};

#endif // _XCAFDoc_AssemblyGraph_HeaderFile
