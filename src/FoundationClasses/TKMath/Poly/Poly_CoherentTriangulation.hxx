// Created on: 2007-11-24
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#ifndef Poly_CoherentTriangulation_HeaderFile
#define Poly_CoherentTriangulation_HeaderFile

#include <Poly_Triangulation.hxx>
#include <Poly_CoherentNode.hxx>
#include <Poly_CoherentTriangle.hxx>
#include <Poly_CoherentLink.hxx>
#include <NCollection_Vector.hxx>

class Poly_CoherentTriangulation;
template <class A>
class NCollection_List;

typedef NCollection_Vector<Poly_CoherentTriangle>::Iterator Poly_BaseIteratorOfCoherentTriangle;
typedef NCollection_Vector<Poly_CoherentNode>::Iterator     Poly_BaseIteratorOfCoherentNode;
typedef NCollection_Vector<Poly_CoherentLink>::Iterator     Poly_BaseIteratorOfCoherentLink;

//! Definition of HANDLE object using Standard_DefineHandle.hxx
#include <Standard_Type.hxx>
DEFINE_STANDARD_HANDLE(Poly_CoherentTriangulation, Standard_Transient)

/**
 * Triangulation structure that allows to:
 * <ul>
 * <li>Store the connectivity of each triangle with up to 3 neighbouring ones and with the
 * corresponding 3rd nodes on them,</li> <li>Store the connectivity of each node with all triangles
 * that share this node</li> <li>Add nodes and triangles to the structure,</li> <li>Find all
 * triangles sharing a single or a couple of nodes</li> <li>Remove triangles from structure</li>
 * <li>Optionally create Links between pairs of nodes according to the current triangulation.</li>
 * <li>Convert from/to Poly_Triangulation structure.</li>
 * </ul>
 *
 * This class is useful for algorithms that need to analyse and/or edit a triangulated mesh -- for
 * example for mesh refining. The connectivity model follows the idea that all Triangles in a mesh
 * should have coherent orientation like on a surface of a solid body. Connections between more than
 * 2 triangles are not supported.
 *
 * @section Poly_CoherentTriangulation Architecture
 * The data types used in this structure are:
 * <ul>
 * <li><b>Poly_CoherentNode</b>: Inherits go_XYZ therefore provides the full public API of gp_XYZ.
 * Contains references to all incident triangles. You can add new nodes but you cannot remove
 * existing ones. However each node that has no referenced triangle is considered as "free" (use the
 * method IsFreeNode() to check this). Free nodes are not available to further processing,
 * particularly they are not exported in Poly_Triangulation.
 * </li>
 * <li><b>Poly_CoherentTriangle</b>: Main data type. Refers three Nodes, three connected Triangles,
 * three opposite (connected) Nodes and three Links. If there is boundary then 1, 2 or 3 references
 * to Triangles/connected Nodes/Links are assigned to NULL (for pointers) or -1 (for integer node
 * index).
 *
 * You can find a triangle by one node using its triangle iterator or by
 * two nodes - creating a temporary Poly_CoherentLink and calling the method FindTriangle().
 *
 * Triangles can be removed but they are never deleted from the containing array. Removed triangles
 * have all nodes equal to -1. You can use the method IsEmpty() to check that.
 * </li>
 * <li><b>Poly_CoherentLink</b>: Auxiliary data type. Normally the array of Links is empty, because
 * for many algorithms it is sufficient to define only Triangles. You can explicitly create the
 * Links at least once, calling the method ComputeLinks(). Each Link is oriented couple of
 * Poly_CoherentNode (directed to the ascending Node index). It refers two connected triangulated
 * Nodes - on the left and on the right, therefore a Poly_CoherentLink instance refers the full set
 * of nodes that constitute a couple of connected Triangles. A boundary Link has either the first
 * (left) or the second (right) connected node index equal to -1.
 *
 * When the array of Links is created, all subsequent calls to AddTriangle and RemoveTriangle try to
 * preserve the connectivity Triangle-Link in addition to the connectivity Triangle-Triangle.
 * Particularly, new Links are created by method AddTriangle() and existing ones are removed by
 * method RemoveTriangle(), in each case whenever necessary.
 *
 * Similarly to Poly_CoherentTriangle, a Link can be removed but not destroyed separately from
 * others. Removed Link can be recogniosed using the method IsEmpty(). To destroy all Links, call
 * the method ClearLinks(), this method also nullifies Link references in all Triangles.
 * </li>
 * All objects (except for free Nodes and empty Triangles and Links) can be visited by the
 * corresponding Iterator. Direct access is provided only for Nodes (needed to resolve Node indexed
 * commonly used as reference). Triangles and Links can be retrieved by their index only internally,
 * the public API provides only references or pointers to C++ objects. If you need a direct access
 * to Triangles and Links, you can subclass Poly_CoherentTriangulation and use the protected API for
 * your needs.
 *
 * Memory management: All data objects are stored in NCollection_Vector containers that prove to be
 * efficient for the performance. In addition references to triangles are stored in ring lists, with
 * an instance of such list per Poly_CoherentNode. These lists are allocated in a memory allocator
 * that is provided in the constructor of Poly_CoherentTriangulation. By default the standard OCCT
 * allocator (aka NCollection_BaseAllocator) is used. But if you need to increase the performance
 * you can use NCollection_IncAllocator instead.
 * </ul>
 */
class Poly_CoherentTriangulation : public Standard_Transient
{
public:
  /**
   * Subclass Iterator - allows to iterate all triangles skipping those that
   * have been removed.
   */
  class IteratorOfTriangle : public Poly_BaseIteratorOfCoherentTriangle
  {
  public:
    //! Constructor
    Standard_EXPORT IteratorOfTriangle(const Handle(Poly_CoherentTriangulation)& theTri);
    //! Make step
    Standard_EXPORT virtual void Next();
  };

  /**
   * Subclass Iterator - allows to iterate all nodes skipping the free ones.
   */
  class IteratorOfNode : public Poly_BaseIteratorOfCoherentNode
  {
  public:
    //! Constructor
    Standard_EXPORT IteratorOfNode(const Handle(Poly_CoherentTriangulation)& theTri);
    //! Make step
    Standard_EXPORT virtual void Next();
  };

  /**
   * Subclass Iterator - allows to iterate all links skipping invalid ones.
   */
  class IteratorOfLink : public Poly_BaseIteratorOfCoherentLink
  {
  public:
    //! Constructor
    Standard_EXPORT IteratorOfLink(const Handle(Poly_CoherentTriangulation)& theTri);
    //! Make step
    Standard_EXPORT virtual void Next();
  };

  //! Couple of integer indices (used in RemoveDegenerated()).
  struct TwoIntegers
  {
    Standard_Integer myValue[2];

    TwoIntegers() {}

    TwoIntegers(Standard_Integer i0, Standard_Integer i1)
    {
      myValue[0] = i0;
      myValue[1] = i1;
    }
  };

public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor.
   */
  Standard_EXPORT Poly_CoherentTriangulation(
    const Handle(NCollection_BaseAllocator)& theAlloc = 0L);

  /**
   * Constructor. It does not create Links, you should call ComputeLinks
   * following this constructor if you need these links.
   */
  Standard_EXPORT Poly_CoherentTriangulation(
    const Handle(Poly_Triangulation)&        theTriangulation,
    const Handle(NCollection_BaseAllocator)& theAlloc = 0L);

  /**
   * Destructor.
   */
  Standard_EXPORT virtual ~Poly_CoherentTriangulation();

  /**
   * Create an instance of Poly_Triangulation from this object.
   */
  Standard_EXPORT Handle(Poly_Triangulation) GetTriangulation() const;

  /**
   * Find and remove degenerated triangles in Triangulation.
   * @param theTol
   *   Tolerance for the degeneration case. If any two nodes of a triangle have
   *   the distance less than this tolerance, this triangle is considered
   *   degenerated and therefore removed by this method.
   * @param pLstRemovedNode
   *   Optional parameter. If defined, then it will receive the list of arrays
   *   where the first number is the index of removed node and the second -
   *   the index of remaining node to which the mesh was reconnected.
   */
  Standard_EXPORT Standard_Boolean
    RemoveDegenerated(const Standard_Real            theTol,
                      NCollection_List<TwoIntegers>* pLstRemovedNode = 0L);

  /**
   * Create a list of free nodes. These nodes may appear as a result of any
   * custom mesh decimation or RemoveDegenerated() call. This analysis is
   * necessary if you support additional data structures based on the
   * triangulation (e.g., edges on the surface boundary).
   * @param lstNodes
   *   <tt>[out]</tt> List that receives the indices of free nodes.
   */
  Standard_EXPORT Standard_Boolean GetFreeNodes(NCollection_List<Standard_Integer>& lstNodes) const;

  /**
   * Query the index of the last node in the triangulation
   */
  inline Standard_Integer MaxNode() const { return myNodes.Length() - 1; }

  /**
   * Query the index of the last triangle in the triangulation
   */
  inline Standard_Integer MaxTriangle() const { return myTriangles.Length() - 1; }

  /**
   * Set the Deflection value as the parameter of the given triangulation.
   */
  inline void SetDeflection(const Standard_Real theDefl) { myDeflection = theDefl; }

  /**
   * Query the Deflection parameter (default value 0. -- if never initialized)
   */
  inline Standard_Real Deflection() const { return myDeflection; }

  /**
   * Initialize a node
   * @param thePoint
   *   3D Coordinates of the node.
   * @param iN
   *   Index of the node. If negative (default), the node is added to the
   *   end of the current array of nodes.
   * @return
   *   Index of the added node.
   */
  Standard_EXPORT Standard_Integer SetNode(const gp_XYZ& thePnt, const Standard_Integer iN = -1);

  /**
   * Get the node at the given index 'i'.
   */
  inline const Poly_CoherentNode& Node(const Standard_Integer i) const { return myNodes.Value(i); }

  /**
   * Get the node at the given index 'i'.
   */
  inline Poly_CoherentNode& ChangeNode(const Standard_Integer i) { return myNodes.ChangeValue(i); }

  /**
   * Query the total number of active nodes (i.e. nodes used by 1 or more
   * triangles)
   */
  Standard_EXPORT Standard_Integer NNodes() const;

  /**
   * Get the triangle at the given index 'i'.
   */
  inline const Poly_CoherentTriangle& Triangle(const Standard_Integer i) const
  {
    return myTriangles.Value(i);
  }

  /**
   * Query the total number of active triangles (i.e. triangles that refer
   * nodes, non-empty ones)
   */
  Standard_EXPORT Standard_Integer NTriangles() const;

  /**
   * Query the total number of active Links.
   */
  Standard_EXPORT Standard_Integer NLinks() const;

  /**
   * Removal of a single triangle from the triangulation.
   */
  Standard_EXPORT Standard_Boolean RemoveTriangle(Poly_CoherentTriangle& theTr);

  /**
   * Removal of a single link from the triangulation.
   */
  Standard_EXPORT void RemoveLink(Poly_CoherentLink& theLink);

  /**
   * Add a triangle to the triangulation.
   * @return
   *   Pointer to the added triangle instance or NULL if an error occurred.
   */
  Standard_EXPORT Poly_CoherentTriangle* AddTriangle(const Standard_Integer iNode0,
                                                     const Standard_Integer iNode1,
                                                     const Standard_Integer iNode2);

  /**
   * Replace nodes in the given triangle.
   * @return
   *   True if operation succeeded.
   */
  Standard_EXPORT Standard_Boolean ReplaceNodes(Poly_CoherentTriangle& theTriangle,
                                                const Standard_Integer iNode0,
                                                const Standard_Integer iNode1,
                                                const Standard_Integer iNode2);

  /**
   * Add a single link to triangulation, based on a triangle and its side index.
   * This method does not check for coincidence with already present links.
   * @param theTri
   *   Triangle that contains the link to be added.
   * @param theConn
   *   Index of the side (i.e., 0, 1 0r 2) defining the added link.
   */
  Standard_EXPORT Poly_CoherentLink* AddLink(const Poly_CoherentTriangle& theTri,
                                             const Standard_Integer       theConn);

  /**
   * Find one or two triangles that share the given couple of nodes.
   * @param theLink
   *   Link (in fact, just a couple of nodes) on which the triangle is
   *   searched.
   * @param pTri
   *   <tt>[out]</tt> Array of two pointers to triangle. pTri[0] stores the
   *   triangle to the left of the link, while pTri[1] stores the one to the
   *   right of the link.
   * @return
   *   True if at least one triangle is found and output as pTri.
   */
  Standard_EXPORT Standard_Boolean FindTriangle(const Poly_CoherentLink&     theLink,
                                                const Poly_CoherentTriangle* pTri[2]) const;

  /**
   * (Re)Calculate all links in this Triangulation.
   */
  Standard_EXPORT Standard_Integer ComputeLinks();

  /**
   * Clear all Links data from the Triangulation data.
   */
  Standard_EXPORT void ClearLinks();

  /**
   * Query the allocator of elements, this allocator can be used for other
   * objects
   */
  inline const Handle(NCollection_BaseAllocator)& Allocator() const { return myAlloc; }

  /**
   * Create a copy of this Triangulation, using the given allocator.
   */
  Standard_EXPORT Handle(Poly_CoherentTriangulation) Clone(
    const Handle(NCollection_BaseAllocator)& theAlloc) const;

  /**
   * Debugging output.
   */
  Standard_EXPORT void Dump(Standard_OStream&) const;

protected:
  // ---------- PROTECTED METHODS ----------

protected:
  // ---------- PROTECTED FIELDS ----------

  NCollection_Vector<Poly_CoherentTriangle> myTriangles;
  NCollection_Vector<Poly_CoherentNode>     myNodes;
  NCollection_Vector<Poly_CoherentLink>     myLinks;
  Handle(NCollection_BaseAllocator)         myAlloc;
  Standard_Real                             myDeflection;

public:
  // Declaration of CASCADE RTTI
  DEFINE_STANDARD_RTTIEXT(Poly_CoherentTriangulation, Standard_Transient)

  friend class IteratorOfTriangle;
  friend class IteratorOfNode;
  friend class IteratorOfLink;
};

#include <Poly_CoherentTriangulation.hxx>

#endif
