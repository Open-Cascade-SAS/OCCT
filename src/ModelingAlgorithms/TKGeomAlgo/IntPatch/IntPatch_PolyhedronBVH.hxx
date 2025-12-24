// Copyright (c) 2024 OPEN CASCADE SAS
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

#ifndef IntPatch_PolyhedronBVH_HeaderFile
#define IntPatch_PolyhedronBVH_HeaderFile

#include <BVH_PrimitiveSet.hxx>
#include <BVH_LinearBuilder.hxx>
#include <NCollection_Array1.hxx>

class IntPatch_Polyhedron;

//! Wraps IntPatch_Polyhedron as a BVH_PrimitiveSet for efficient spatial queries.
//! This class provides a BVH (Bounding Volume Hierarchy) representation of a polyhedron's
//! triangles, enabling O(log n) spatial queries instead of linear search.
//!
//! The class stores a reference to the polyhedron (no data copy) and maintains
//! an index mapping to track triangle reordering during BVH construction.
class IntPatch_PolyhedronBVH : public BVH_PrimitiveSet<Standard_Real, 3>
{
public:
  //! Creates an empty BVH set.
  Standard_EXPORT IntPatch_PolyhedronBVH();

  //! Creates BVH set from the given polyhedron.
  //! @param[in] thePoly the polyhedron to wrap (must remain valid during BVH lifetime)
  Standard_EXPORT IntPatch_PolyhedronBVH(const IntPatch_Polyhedron& thePoly);

  //! Destructor.
  Standard_EXPORT virtual ~IntPatch_PolyhedronBVH();

  //! Initializes BVH set from the given polyhedron.
  //! @param[in] thePoly the polyhedron to wrap (must remain valid during BVH lifetime)
  Standard_EXPORT void Init(const IntPatch_Polyhedron& thePoly);

  //! Clears the BVH set.
  Standard_EXPORT void Clear();

public: //! @name BVH_Set interface implementation
  // Make inherited Box() method visible
  using BVH_PrimitiveSet<Standard_Real, 3>::Box;

  //! Returns the total number of triangles.
  Standard_EXPORT virtual int Size() const Standard_OVERRIDE;

  //! Returns AABB of the triangle with the given index.
  //! @param[in] theIndex 0-based triangle index (after BVH reordering)
  Standard_EXPORT virtual BVH_Box<Standard_Real, 3> Box(const int theIndex) const Standard_OVERRIDE;

  //! Returns centroid coordinate of the triangle along the given axis.
  //! @param[in] theIndex 0-based triangle index (after BVH reordering)
  //! @param[in] theAxis axis index (0=X, 1=Y, 2=Z)
  Standard_EXPORT virtual double Center(const int theIndex,
                                        const int theAxis) const Standard_OVERRIDE;

  //! Swaps two triangles in the set (used during BVH construction).
  //! @param[in] theIndex1 first triangle index
  //! @param[in] theIndex2 second triangle index
  Standard_EXPORT virtual void Swap(const int theIndex1, const int theIndex2) Standard_OVERRIDE;

public: //! @name Additional methods
  //! Returns the original (1-based) triangle index in the polyhedron
  //! for the given 0-based index after BVH reordering.
  //! @param[in] theIndex 0-based triangle index (after BVH reordering)
  //! @return 1-based original triangle index in the polyhedron
  Standard_EXPORT int OriginalIndex(const int theIndex) const;

  //! Returns true if the BVH set is initialized.
  bool IsInitialized() const { return myPoly != nullptr; }

private:
  const IntPatch_Polyhedron* myPoly;     //!< Reference to the wrapped polyhedron
  NCollection_Array1<int>    myIndexMap; //!< Maps current indices to original 1-based indices
};

#endif // IntPatch_PolyhedronBVH_HeaderFile
