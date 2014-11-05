// Copyright (c) 2013 OPEN CASCADE SAS
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

#ifndef _BRepMesh_SelectorOfDataStructureOfDelaun_HeaderFile
#define _BRepMesh_SelectorOfDataStructureOfDelaun_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Macro.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh.hxx>
#include <Standard_Integer.hxx>
#include <BRepMesh_Triangle.hxx>

class BRepMesh_Vertex;
class BRepMesh_Edge;

//! Describes a selector and an iterator on a 
//! selector of components of a mesh.
class BRepMesh_SelectorOfDataStructureOfDelaun
{
public:

  DEFINE_STANDARD_ALLOC

  //! Default constructor.
  Standard_EXPORT BRepMesh_SelectorOfDataStructureOfDelaun();
  
  //! Constructor.
  //! Initializes selector by the mesh.
  Standard_EXPORT BRepMesh_SelectorOfDataStructureOfDelaun(const Handle(BRepMesh_DataStructureOfDelaun)& theMesh);
  
  //! Initializes selector by the mesh.
  Standard_EXPORT void Initialize(const Handle(BRepMesh_DataStructureOfDelaun)& theMesh);
  
  //! Selects all neighboring elements of the given node.
  Standard_EXPORT void NeighboursOf(const BRepMesh_Vertex& theNode);
  
  //! Selects all neighboring elements of node with the given index.
  Standard_EXPORT void NeighboursOfNode(const Standard_Integer theNodeIndex);
  
  //! Selects all neighboring elements of the given link.
  Standard_EXPORT void NeighboursOf(const BRepMesh_Edge& theLink);
  
  //! Selects all neighboring elements of link with the given index.
  Standard_EXPORT void NeighboursOfLink(const Standard_Integer theLinkIndex);
  
  //! Selects all neighboring elements of the given element.
  Standard_EXPORT void NeighboursOf(const BRepMesh_Triangle& theElement);
  
  //! Selects all neighboring elements by nodes of the given element.
  Standard_EXPORT void NeighboursOfElement(const Standard_Integer theElementIndex);
  
  //! Selects all neighboring elements by links of the given element.
  Standard_EXPORT void NeighboursByEdgeOf(const BRepMesh_Triangle& theElement);
  
  //! Adds a level of neighbours by edge to the selector.
  inline void NeighboursOf(const BRepMesh_SelectorOfDataStructureOfDelaun& /*theSelector*/)
  {
  }
  
  //! Adds a level of neighbours by edge the selector.
  inline void AddNeighbours()
  {
  }
  
  //! Returns selected nodes.
  inline const BRepMesh::MapOfInteger& Nodes() const
  {
    return myNodes;
  }
  
  //! Returns selected links.
  inline const BRepMesh::MapOfInteger& Links() const
  {
    return myLinks;
  }
  
  //! Returns selected elements.
  inline const BRepMesh::MapOfInteger& Elements() const
  {
    return myElements;
  }
  
  //! Gives the list of incices of frontier links.
  inline const BRepMesh::MapOfInteger& FrontierLinks() const
  {
    return myFrontier;
  }

private:

  //! Collects elements connected to link with the given index.
  void elementsOfLink(const Standard_Integer theIndex);

private:
  Handle(NCollection_IncAllocator)       myAllocator;
  Handle(BRepMesh_DataStructureOfDelaun) myMesh;
  BRepMesh::MapOfInteger                 myNodes;
  BRepMesh::MapOfInteger                 myLinks;
  BRepMesh::MapOfInteger                 myElements;
  BRepMesh::MapOfInteger                 myFrontier;
};

#endif
