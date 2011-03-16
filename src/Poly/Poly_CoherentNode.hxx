// File:      Poly_CoherentNode.hxx
// Created:   08.12.07 10:45
// Author:    Alexander GRIGORIEV
// Copyright: Open Cascade 2007


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
