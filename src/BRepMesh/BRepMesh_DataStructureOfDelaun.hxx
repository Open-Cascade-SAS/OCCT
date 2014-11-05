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

#ifndef _BRepMesh_DataStructureOfDelaun_HeaderFile
#define _BRepMesh_DataStructureOfDelaun_HeaderFile

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <BRepMesh_VertexTool.hxx>
#include <Standard_Transient.hxx>
#include <BRepMesh_Triangle.hxx>
#include <BRepMesh_PairOfIndex.hxx>
#include <Standard_OStream.hxx>
#include <BRepMesh.hxx>

class BRepMesh_Vertex;
class BRepMesh_VertexTool;
class BRepMesh_Edge;

//! Describes the data structure necessary for the mesh algorithms in 
//! two dimensions plane or on surface by meshing in UV space.
class BRepMesh_DataStructureOfDelaun : public Standard_Transient
{
public:

  //! Constructor.
  //! @param theAllocator memory allocator to be used by internal structures.
  //! @param theReservedNodeSize presumed number of nodes in this mesh.
  Standard_EXPORT BRepMesh_DataStructureOfDelaun(
    const Handle(NCollection_IncAllocator)& theAllocator,
    const Standard_Integer                  theReservedNodeSize = 100);



public: //! @name API for accessing mesh nodes.

  //! Returns number of nodes.
  inline Standard_Integer NbNodes() const
  {
    return myNodes->Extent();
  }


  //! Adds node to the mesh if it is not already in the mesh.
  //! @param theNode node to be added to the mesh.
  //! @param isForceAdd adds the given node to structure without 
  //! checking on coincidence with other nodes.
  //! @return index of the node in the structure.
  Standard_EXPORT Standard_Integer AddNode(
    const BRepMesh_Vertex& theNode,
    const Standard_Boolean isForceAdd = Standard_False);

  //! Finds the index of the given node.
  //! @param theNode node to find.
  //! @return index of the given element of zero if node is not in the mesh.
  Standard_EXPORT Standard_Integer IndexOf(const BRepMesh_Vertex& theNode)
  {
    return myNodes->FindIndex(theNode);
  }

  //! Get node by the index.
  //! @param theIndex index of a node.
  //! @return node with the given index.
  inline const BRepMesh_Vertex& GetNode(const Standard_Integer theIndex)
  {
    return myNodes->FindKey(theIndex);
  }

  //! Alias for GetNode.
  const BRepMesh_Vertex& operator ()(const Standard_Integer theIndex)
  {
    return GetNode(theIndex);
  }

  //! Substitutes the node with the given index by new one.
  //! @param theIndex index of node to be substituted.
  //! @param theNewNode substituting node.
  //! @return FALSE in case if new node is already in the structure, TRUE elsewhere.
  Standard_EXPORT Standard_Boolean SubstituteNode(
    const Standard_Integer theIndex,
    const BRepMesh_Vertex& theNewNode);

  //! Removes node from the mesh in case if it has no connected links 
  //! and its type is Free.
  //! @param theIndex index of node to be removed.
  //! @param isForce if TRUE node will be removed even if movability
  //! is not Free.
  Standard_EXPORT void RemoveNode(const Standard_Integer theIndex,
                                  const Standard_Boolean isForce = Standard_False)
  {
    if (isForce || myNodes->FindKey(theIndex).Movability() == BRepMesh_Free)
    {
      if (LinksConnectedTo(theIndex).Extent()==0)
        myNodes->Delete(theIndex);
    }
  }

  //! Get list of links attached to the node with the given index.
  //! @param theIndex index of node whose links should be retrieved.
  //! @return list of links attached to the node.
  inline const BRepMesh::ListOfInteger& LinksConnectedTo(
    const Standard_Integer theIndex) const
  {
    return linksConnectedTo(theIndex);
  }


public: //! @name API for accessing mesh links.

  //! Returns number of links.
  inline Standard_Integer NbLinks() const
  {
    return myLinks.Extent();
  }

  //! Adds link to the mesh if it is not already in the mesh.
  //! @param theLink link to be added to the mesh.
  //! @return index of the link in the structure.
  Standard_EXPORT Standard_Integer AddLink(const BRepMesh_Edge& theLink);

  //! Finds the index of the given link.
  //! @param theLink link to find.
  //! @return index of the given element of zero if link is not in the mesh.
  Standard_EXPORT Standard_Integer IndexOf(const BRepMesh_Edge& theLink) const
  {
    return myLinks.FindIndex(theLink);
  }

  //! Get link by the index.
  //! @param theIndex index of a link.
  //! @return link with the given index.
  Standard_EXPORT const BRepMesh_Edge& GetLink(const Standard_Integer theIndex)
  {
    return myLinks.FindKey(theIndex);
  }

  //! Returns map of indices of links registered in mesh.
  inline const BRepMesh::MapOfInteger& LinksOfDomain() const
  {
    return myLinksOfDomain;
  }

  //! Substitutes the link with the given index by new one.
  //! @param theIndex index of link to be substituted.
  //! @param theNewLink substituting link.
  //! @return FALSE in case if new link is already in the structure, TRUE elsewhere.
  Standard_EXPORT Standard_Boolean SubstituteLink(const Standard_Integer theIndex,
                                                  const BRepMesh_Edge&   theNewLink);

  //! Removes link from the mesh in case if it has no connected elements 
  //! and its type is Free.
  //! @param theIndex index of link to be removed.
  //! @param isForce if TRUE link will be removed even if movability
  //! is not Free.
  Standard_EXPORT void RemoveLink(const Standard_Integer theIndex,
                                  const Standard_Boolean isForce = Standard_False);

  //! Returns indices of elements conected to the link with the given index.
  //! @param theLinkIndex index of link whose data should be retrieved.
  //! @return indices of elements conected to the link.
  Standard_EXPORT const BRepMesh_PairOfIndex& ElementsConnectedTo(
    const Standard_Integer theLinkIndex) const
  {
    return myLinks.FindFromIndex(theLinkIndex);
  }



public: //! @name API for accessing mesh elements.

  //! Returns number of links.
  inline Standard_Integer NbElements() const
  {
    return myElements.Extent();
  }

  //! Adds element to the mesh if it is not already in the mesh.
  //! @param theElement element to be added to the mesh.
  //! @return index of the element in the structure.
  Standard_EXPORT Standard_Integer AddElement(const BRepMesh_Triangle& theElement);

  //! Finds the index of the given element.
  //! @param theElement element to find.
  //! @return index of the given element of zero if element is not in the mesh.
  Standard_EXPORT Standard_Integer IndexOf(const BRepMesh_Triangle& theElement) const
  {
    return myElements.FindIndex(theElement);
  }

  //! Get element by the index.
  //! @param theIndex index of an element.
  //! @return element with the given index.
  Standard_EXPORT const BRepMesh_Triangle& GetElement(const Standard_Integer theIndex)
  {
    return myElements.FindKey(theIndex);
  }

  //! Returns map of indices of elements registered in mesh.
  inline const BRepMesh::MapOfInteger& ElementsOfDomain() const
  {
    return myElementsOfDomain;
  }

  //! Substitutes the element with the given index by new one.
  //! @param theIndex index of element to be substituted.
  //! @param theNewLink substituting element.
  //! @return FALSE in case if new element is already in the structure, TRUE elsewhere.
  Standard_EXPORT Standard_Boolean SubstituteElement(const Standard_Integer   theIndex,
                                                     const BRepMesh_Triangle& theNewElement);

  //! Removes element from the mesh.
  //! @param theIndex index of element to be removed.
  Standard_EXPORT void RemoveElement(const Standard_Integer theIndex);

  //! Returns indices of nodes forming the given element.
  //! @param theElement element which nodes should be retrieved.
  //! @param[out] theNodes nodes of the given element.
  Standard_EXPORT void ElementNodes(
    const BRepMesh_Triangle& theElement,
    Standard_Integer         (&theNodes)[3]);



public: //! @name Auxilary API

  //! Dumps information about this structure.
  //! @param theStream stream to be used for dump.
  Standard_EXPORT void Statistics(Standard_OStream& theStream) const;
  
  //! Returns memory allocator used by the structure.
  inline const Handle(NCollection_IncAllocator)& Allocator() const
  {
    return myAllocator;
  }

  //! Gives the data structure for initialization of cell size and tolerance.
  inline BRepMesh::HVertexTool& Data()
  {
    return myNodes;
  }

  //! Removes all elements.
  Standard_EXPORT void ClearDomain();

  //! Substitutes deleted items by the last one from corresponding map 
  //! to have only non-deleted elements, links or nodes in the structure.
  Standard_EXPORT void ClearDeleted()
  {
    clearDeletedLinks();
    clearDeletedNodes();
  }

  DEFINE_STANDARD_RTTI(BRepMesh_DataStructureOfDelaun)

private: 

  //! Get list of links attached to the node with the given index.
  //! @param theIndex index of node whose links should be retrieved.
  //! @return list of links attached to the node.
  inline BRepMesh::ListOfInteger& linksConnectedTo(
    const Standard_Integer theIndex) const
  {
    return (BRepMesh::ListOfInteger&)myNodeLinks.Find(theIndex);
  }

  //! Substitutes deleted links by the last one from corresponding map 
  //! to have only non-deleted links in the structure.
  Standard_EXPORT void clearDeletedLinks();

  //! Substitutes deleted nodes by the last one from corresponding map 
  //! to have only non-deleted nodes in the structure.
  Standard_EXPORT void clearDeletedNodes();

  //! Cleans dependent structures from the given link.
  //! @param theIndex index of link in the data structure.
  //! @param theLink reference to the link to avoid double accessing 
  //! to map of links.
  void cleanLink(const Standard_Integer theIndex,
                 const BRepMesh_Edge&   theLink);

  //! Cleans dependent structures from the given element.
  //! @param theIndex index of element in the data structure.
  //! @param theElement reference to the element to avoid double accessing 
  //! to map of elements.
  void cleanElement(const Standard_Integer   theIndex,
                    const BRepMesh_Triangle& theElement);

  //! Removes element index from the given pair. Used by cleanElement.
  //! @param theIndex index of element to be removed.
  //! @param thePair pair of elements to be cleaned.
  void removeElementIndex(const Standard_Integer theIndex,
                          BRepMesh_PairOfIndex&  thePair);


private:

  Handle(NCollection_IncAllocator)      myAllocator;
  BRepMesh::HVertexTool                 myNodes;
  BRepMesh::DMapOfIntegerListOfInteger  myNodeLinks;
  BRepMesh::IDMapOfLink                 myLinks;
  BRepMesh::ListOfInteger               myDelLinks;
  BRepMesh::IMapOfElement               myElements;
  BRepMesh::MapOfInteger                myElementsOfDomain;
  BRepMesh::MapOfInteger                myLinksOfDomain;
};

DEFINE_STANDARD_HANDLE(BRepMesh_DataStructureOfDelaun,Standard_Transient)

#endif
