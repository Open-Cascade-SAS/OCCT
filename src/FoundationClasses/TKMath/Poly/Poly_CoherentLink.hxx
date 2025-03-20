// Created on: 2007-12-25
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

#ifndef Poly_CoherentLink_HeaderFile
#define Poly_CoherentLink_HeaderFile

#include <Standard_TypeDef.hxx>

class Poly_CoherentTriangle;

/**
 * Link between two mesh nodes that is created by existing triangle(s).
 * Keeps reference to the opposite node of each incident triangle.
 * The referred node with index "0" is always on the left side of the link,
 * the one with the index "1" is always on the right side.
 * It is possible to find both incident triangles using the method
 * Poly_CoherentTriangulation::FindTriangle().
 * <p>
 * Any Link can store an arbitrary pointer that is called Attribute.
 */

class Poly_CoherentLink
{
public:
  // ---------- PUBLIC METHODS ----------

  /**
   * Empty constructor.
   */
  Standard_EXPORT Poly_CoherentLink();

  /**
   * Constructor. Creates a Link that has no reference to 'opposite nodes'.
   * This constructor is useful to create temporary object that is not
   * inserted into any existing triangulation.
   */
  inline Poly_CoherentLink(const Standard_Integer iNode0, const Standard_Integer iNode1)
      : myAttribute(0L)
  {
    myNode[0]         = iNode0;
    myNode[1]         = iNode1;
    myOppositeNode[0] = -1;
    myOppositeNode[1] = -1;
  }

  /**
   * Constructor, takes a triangle and a side. A link is created always such
   * that myNode[0] < myNode[1]. Unlike the previous constructor, this one
   * assigns the 'opposite node' fields. This constructor is used when a
   * link is inserted into a Poly_CoherentTriangulation structure.
   * @param theTri
   *   Triangle containing the link that is created
   * @param iSide
   *   Can be 0, 1 or 2. Index of the node
   */
  Standard_EXPORT Poly_CoherentLink(const Poly_CoherentTriangle& theTri, Standard_Integer iSide);

  /**
   * Return the node index in the current triangulation.
   * @param ind
   *   0 or 1 making distinction of the two nodes that constitute the Link.
   *   Node(0) always returns a smaller number than Node(1).
   */
  inline Standard_Integer Node(const Standard_Integer ind) const { return myNode[ind & 0x1]; }

  /**
   * Return the opposite node (belonging to the left or right incident triangle)
   * index in the current triangulation.
   * @param ind
   *   0 or 1 making distinction of the two involved triangles: 0 on the left,
   *   1 on the right side of the Link.
   */
  inline Standard_Integer OppositeNode(const Standard_Integer ind) const
  {
    return myOppositeNode[ind & 0x1];
  }

  /**
   * Query the attribute of the Link.
   */
  inline Standard_Address GetAttribute() const { return myAttribute; }

  /**
   * Set the attribute of the Link.
   */
  inline void SetAttribute(const Standard_Address theAtt) { myAttribute = theAtt; }

  /**
   * Query the status of the link - if it is an invalid one.
   * An invalid link has Node members equal to -1.
   */
  inline Standard_Boolean IsEmpty() const { return myNode[0] < 0 || myNode[1] < 0; }

  /**
   * Invalidate this Link.
   */
  inline void Nullify()
  {
    myNode[0]         = -1;
    myNode[1]         = -1;
    myOppositeNode[0] = -1;
    myOppositeNode[1] = -1;
  }

protected:
  // ---------- PROTECTED METHODS ----------

private:
  // ---------- PRIVATE FIELDS ----------
  Standard_Integer myNode[2];
  Standard_Integer myOppositeNode[2];
  Standard_Address myAttribute;

  friend class Poly_CoherentTriangulation;
};

#endif
