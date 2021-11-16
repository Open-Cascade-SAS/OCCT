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

#ifndef Poly_CoherentTriangle_HeaderFile
#define Poly_CoherentTriangle_HeaderFile

#include <Standard_TypeDef.hxx>

class Poly_CoherentLink;

/**
 * Data class used in Poly_CoherentTriangultion.
 * Implements a triangle with references to its neighbours.
 */

class Poly_CoherentTriangle 
{
 public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor.
   */
  Standard_EXPORT Poly_CoherentTriangle    ();

  /**
   * Constructor.
   */
  Standard_EXPORT Poly_CoherentTriangle    (const Standard_Integer iNode0,
                                            const Standard_Integer iNode1,
                                            const Standard_Integer iNode2);

  /**
   * Query the node index in the position given by the parameter 'ind'
   */
  inline Standard_Integer  Node            (const Standard_Integer ind) const
  { return myNodes[ind]; }

//   /**
//    * Set the Node at the given position 'ind'.
//    */ 
//   inline void              SetNode         (const Standard_Integer ind,
//                                             const Standard_Integer iNode)
//   { myNodes[ind] = iNode; }

  /**
   * Query if this is a valid triangle.
   */
  inline Standard_Boolean  IsEmpty         () const
  { return myNodes[0] < 0 || myNodes[1] < 0 || myNodes[2] < 0; }

  /**
   * Create connection with another triangle theTri.
   * This method creates both connections: in this triangle and in theTri. You
   * do not need to call the same method on triangle theTr.
   * @param iConn
   *   Can be 0, 1 or 2 - index of the node that is opposite to the connection
   *   (shared link).
   * @param theTr
   *   Triangle that is connected on the given link.
   * @return
   *   True if successful, False if the connection is rejected
   *   due to improper topology.
   */
  Standard_EXPORT Standard_Boolean
                           SetConnection   (const Standard_Integer  iConn,
                                            Poly_CoherentTriangle&  theTr);
  
  /**
   * Create connection with another triangle theTri.
   * This method creates both connections: in this triangle and in theTri.
   * This method is slower than the previous one, because it makes analysis
   * what sides of both triangles are connected.
   * @param theTri
   *   Triangle that is connected.
   * @return
   *   True if successful, False if the connection is rejected
   *   due to improper topology.
   */
  Standard_EXPORT Standard_Boolean
                           SetConnection   (Poly_CoherentTriangle& theTri);

  /**
   * Remove the connection with the given index.
   * @param iConn
   *   Can be 0, 1 or 2 - index of the node that is opposite to the connection
   *   (shared link).
   */
  Standard_EXPORT void     RemoveConnection(const Standard_Integer iConn);

  /**
   * Remove the connection with the given Triangle.
   * @return
   *  True if successfuol or False if the connection has not been found.
   */
  Standard_EXPORT Standard_Boolean
                           RemoveConnection(Poly_CoherentTriangle& theTri);

  /**
   * Query the number of connected triangles.
   */
  inline Standard_Integer  NConnections    () const
  { return myNConnections; }

  /**
   * Query the connected node on the given side.
   * Returns -1 if there is no connection on the specified side.
   */
  inline Standard_Integer  GetConnectedNode(const Standard_Integer iConn) const
  { return myNodesOnConnected[iConn]; }

  /**
   * Query the connected triangle on the given side.
   * Returns NULL if there is no connection on the specified side.
   */
  inline const Poly_CoherentTriangle *
                           GetConnectedTri (const Standard_Integer iConn) const
  { return mypConnected[iConn]; }

  /**
   * Query the Link associate with the given side of the Triangle.
   * May return NULL if there are no links in the triangulation.
   */
  inline const Poly_CoherentLink *
                           GetLink         (const Standard_Integer iLink) const
  { return mypLink[iLink]; }

  /**
   * Returns the index of the connection with the given triangle, or -1 if not found.
   */
  Standard_EXPORT Standard_Integer
                           FindConnection  (const Poly_CoherentTriangle&) const;

 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_Integer              myNConnections;
  Standard_Integer              myNodes[3];
  Standard_Integer              myNodesOnConnected[3];
  const Poly_CoherentTriangle * mypConnected[3];
  const Poly_CoherentLink     * mypLink[3];

  friend class Poly_CoherentTriangulation;
};


#endif
