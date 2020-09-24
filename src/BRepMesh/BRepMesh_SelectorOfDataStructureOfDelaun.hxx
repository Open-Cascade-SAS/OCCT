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

#include <Standard_Transient.hxx>
#include <BRepMesh_DataStructureOfDelaun.hxx>
#include <BRepMesh_Triangle.hxx>
#include <IMeshData_Types.hxx>

class BRepMesh_Vertex;
class BRepMesh_Edge;

//! Describes a selector and an iterator on a 
//! selector of components of a mesh.
class BRepMesh_SelectorOfDataStructureOfDelaun : public Standard_Transient
{
public:

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
  void NeighboursOf(const BRepMesh_SelectorOfDataStructureOfDelaun& /*theSelector*/)
  {
  }
  
  //! Adds a level of neighbours by edge the selector.
  void AddNeighbours()
  {
  }
  
  //! Returns selected nodes.
  const IMeshData::MapOfInteger& Nodes() const
  {
    return myNodes;
  }
  
  //! Returns selected links.
  const IMeshData::MapOfInteger& Links() const
  {
    return myLinks;
  }
  
  //! Returns selected elements.
  const IMeshData::MapOfInteger& Elements() const
  {
    return myElements;
  }
  
  //! Gives the list of incices of frontier links.
  const IMeshData::MapOfInteger& FrontierLinks() const
  {
    return myFrontier;
  }

  DEFINE_STANDARD_RTTIEXT(BRepMesh_SelectorOfDataStructureOfDelaun, Standard_Transient)

private:

  //! Collects elements connected to link with the given index.
  void elementsOfLink(const Standard_Integer theIndex);

private:
  Handle(BRepMesh_DataStructureOfDelaun) myMesh;
  IMeshData::MapOfInteger                myNodes;
  IMeshData::MapOfInteger                myLinks;
  IMeshData::MapOfInteger                myElements;
  IMeshData::MapOfInteger                myFrontier;
};

#endif
