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



#ifndef Poly_CoherentTriPtr_HeaderFile
#define Poly_CoherentTriPtr_HeaderFile

#include <NCollection_BaseAllocator.hxx>
#include <NCollection_DefineAlloc.hxx>

class Poly_CoherentTriangle;

#ifdef WNT
#pragma warning (push)
#pragma warning(disable:4355) //'this' : used in base member initializer list
#endif

/**
 * Implementation of both list node for Poly_CoherentTriangle type and
 * round double-linked list of these nodes. 
 */

class Poly_CoherentTriPtr
{
 public:
  /**
   * Iterator class for this list of triangles. Because the list is round,
   * an iteration can be started from any member and it finishes before taking
   * this member again. The iteration sense is always forward (Next).
   */  
  class Iterator {
  public:
    //! Empty constructor
    inline Iterator ()
      : myFirst         (0L),
        myCurrent       (0L)
    {}
    //! Constructor
    inline Iterator (const Poly_CoherentTriPtr& thePtr)
      : myFirst         (&thePtr),
        myCurrent       (&thePtr)
    {}
    //! Query the triangle that started the current iteration.
    inline const Poly_CoherentTriangle * First  () const
    { return myFirst ? &myFirst->GetTriangle() : 0L; }
    //! Query if there is available triangle pointer on this iteration
    inline Standard_Boolean             More    () const
    { return myCurrent != 0L; }
    //! Go to the next iteration.
    Standard_EXPORT void                Next    ();
    //! Get the current iterated triangle
    inline const Poly_CoherentTriangle& Value   () const
    { return myCurrent->GetTriangle(); }
    //! Get the current iterated triangle (mutable)
    inline Poly_CoherentTriangle&       ChangeValue   () const
    { return const_cast<Poly_CoherentTriangle&>(myCurrent->GetTriangle()); }
    //! Get the current iterated pointer to triangle
    inline const Poly_CoherentTriPtr&   PtrValue() const
    { return * myCurrent; }
  private:
    const Poly_CoherentTriPtr * myFirst;
    const Poly_CoherentTriPtr * myCurrent;
  };

  // ---------- PUBLIC METHODS ----------


  /**
   * Constructor.
   */
  inline Poly_CoherentTriPtr (const Poly_CoherentTriangle& theTri)
    : mypTriangle (&theTri),
      myNext      (this),
      myPrevious  (this)
  {}

  /**
   * Operator new for dynamic allocations
   */
  DEFINE_NCOLLECTION_ALLOC

  /**
   * Query the stored pointer to Triangle.
   */
  inline const Poly_CoherentTriangle&
                        GetTriangle () const
  { return * mypTriangle; }

  /**
   * Initialize this instance with a pointer to triangle.
   */
  inline void          SetTriangle (const Poly_CoherentTriangle * pTri)
  { mypTriangle = pTri; }
  
  /**
   * Query the next pointer in the list.
   */
  inline Poly_CoherentTriPtr&
                       Next     () const
  { return * myNext; }

  /**
   * Query the previous pointer in the list.
   */
  inline Poly_CoherentTriPtr&
                       Previous () const
  { return * myPrevious; }

  /**
   * Append a pointer to triangle into the list after the current instance.
   * @param pTri
   *   Triangle that is to be included in the list after this one.
   * @param theA
   *   Allocator where the new pointer instance is created.
   */
  Standard_EXPORT void Append   (const Poly_CoherentTriangle *           pTri,
                                 const Handle_NCollection_BaseAllocator& theA);

  /**
   * Prepend a pointer to triangle into the list before the current instance.
   * @param pTri
   *   Triangle that is to be included in the list before this one.
   * @param theA
   *   Allocator where the new pointer instance is created.
   */
  Standard_EXPORT void Prepend  (const Poly_CoherentTriangle *           pTri,
                                 const Handle_NCollection_BaseAllocator& theA);

  /**
   * Remove a pointer to triangle from its list.
   * @param thePtr
   *   This class instance that should be removed from its list.
   * @param theA
   *   Allocator where the current pointer instance was created.
   */
  Standard_EXPORT static void
                       Remove   (Poly_CoherentTriPtr *                   thePtr,
                                 const Handle_NCollection_BaseAllocator& theA);

  /**
   * Remove the list containing the given pointer to triangle.
   */
  Standard_EXPORT static void
                       RemoveList (Poly_CoherentTriPtr *                 thePtr,
                                   const Handle_NCollection_BaseAllocator&);

 protected:
  // ---------- PROTECTED METHODS ----------

  /**
   * Constructor.
   */
  inline Poly_CoherentTriPtr (const Poly_CoherentTriangle * pTri)
    : mypTriangle (pTri),
      myNext      (this),
      myPrevious  (this)
  {}

 private:
  // ---------- PRIVATE FIELDS ----------

  const Poly_CoherentTriangle * mypTriangle;
  Poly_CoherentTriPtr         * myNext;
  Poly_CoherentTriPtr         * myPrevious;

  friend class Iterator;
};

#ifdef WNT
#pragma warning (pop)
#endif

#endif
