// Created on: 2007-12-08
// Created by: Alexander GRIGORIEV
// Copyright (c) 2007-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#ifndef Poly_CoherentNode_HeaderFile
#define Poly_CoherentNode_HeaderFile

#include <gp_XYZ.hxx>
#include <Poly_CoherentTriPtr.hxx>
#include <Precision.hxx>

class NCollection_BaseAllocator;

/**
 * Node of coherent triangulation. Contains:
 * <ul>
 * <li>Coordinates of a 3D point defining the node location</li>
 * <li>2D point coordinates</li>
 * <li>List of triangles that use this Node</li>
 * <li>Integer index, normally the index of the node in the original
 *     triangulation</li>
 * </ul>
 */

class Poly_CoherentNode : public gp_XYZ 
{
 public:
  // ---------- PUBLIC METHODS ----------


  /**
   * Empty constructor.
   */
  inline Poly_CoherentNode ()
    : gp_XYZ            (0., 0., 0.),
      myTriangles       (0L),
      myIndex           (-1)
  { myUV[0] = Precision::Infinite(); myUV[1] = Precision::Infinite(); }

  /**
   * Constructor.
   */
  inline Poly_CoherentNode (const gp_XYZ& thePnt)
    : gp_XYZ            (thePnt),
      myTriangles       (0L),
      myIndex           (-1)
  { myUV[0] = Precision::Infinite(); myUV[1] = Precision::Infinite(); myNormal[0] = 0.f; myNormal[1] = 0.f; myNormal[2] = 0.f; }

  /**
   * Set the UV coordinates of the Node.
   */
  inline void             SetUV         (const Standard_Real theU,
                                         const Standard_Real theV)
  { myUV[0] = theU; myUV[1] = theV; }

  /**
   * Get U coordinate of the Node.
   */
  inline Standard_Real    GetU          () const
  { return myUV[0]; }

  /**
   * Get V coordinate of the Node.
   */
  inline Standard_Real    GetV          () const
  { return myUV[1]; }

  /**
   * Define the normal vector in the Node.
   */
  Standard_EXPORT void    SetNormal     (const gp_XYZ& theVector);

  /**
   * Query if the Node contains a normal vector.
   */
  inline Standard_Boolean HasNormal     () const
  { return ((myNormal[0]*myNormal[0] + myNormal[1]*myNormal[1] +
             myNormal[2]*myNormal[2]) > Precision::Confusion()); }

  /**
   * Get the stored normal in the node.
   */
  inline gp_XYZ           GetNormal    () const
  { return gp_XYZ (myNormal[0], myNormal[1], myNormal[2]); }

  /**
   * Set the value of node Index.
   */
  inline void             SetIndex     (const Standard_Integer theIndex)
  { myIndex = theIndex; }

  /**
   * Get the value of node Index.
   */
  inline Standard_Integer GetIndex      () const
  { return myIndex; }

  /**
   * Check if this is a free node, i.e., a node without a single
   * incident triangle.
   */
  inline Standard_Boolean IsFreeNode    () const
  { return myTriangles == 0L; }

  /**
   * Reset the Node to void.
   */
  Standard_EXPORT void  Clear   (const Handle_NCollection_BaseAllocator&);

  /**
   * Connect a triangle to this Node.
   */
  Standard_EXPORT void  AddTriangle
                                (const Poly_CoherentTriangle&            theTri,
                                 const Handle_NCollection_BaseAllocator& theA);

  /**
   * Disconnect a triangle from this Node.
   */
  Standard_EXPORT Standard_Boolean
                        RemoveTriangle
                                (const Poly_CoherentTriangle&            theTri,
                                 const Handle_NCollection_BaseAllocator& theA);

  /**
   * Create an iterator of incident triangles.
   */
  inline Poly_CoherentTriPtr::Iterator
                        TriangleIterator () const
  { return * myTriangles; }

  Standard_EXPORT void  Dump    (Standard_OStream& theStream) const;

//   /**
//    * Destructor.
//    */
//   Standard_EXPORT virtual ~Poly_CoherentNode ();



 protected:
  // ---------- PROTECTED METHODS ----------



 private:
  // ---------- PRIVATE FIELDS ----------

  Standard_Real         myUV[2];
  Poly_CoherentTriPtr   * myTriangles;
  Standard_Integer      myIndex;
  Standard_ShortReal    myNormal[3];
};

#endif
