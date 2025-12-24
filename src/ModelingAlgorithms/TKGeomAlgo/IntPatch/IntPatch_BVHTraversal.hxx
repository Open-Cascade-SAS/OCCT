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

#ifndef IntPatch_BVHTraversal_HeaderFile
#define IntPatch_BVHTraversal_HeaderFile

#include <BVH_Traverse.hxx>
#include <NCollection_Vector.hxx>

class IntPatch_PolyhedronBVH;

//! Performs BVH tree traversal of two polyhedra to find candidate triangle pairs
//! for intersection testing. This class implements the BVH_PairTraverse interface
//! to efficiently find potentially intersecting triangles using bounding box tests.
//!
//! The traversal collects pairs of original (1-based) triangle indices that have
//! overlapping bounding boxes, which should then be tested for actual geometric
//! intersection using IntPatch_InterferencePolyhedron::Intersect().
class IntPatch_BVHTraversal : public BVH_PairTraverse<Standard_Real, 3>
{
public:
  //! Pair of triangle indices (both 1-based, original indices in polyhedra).
  struct TrianglePair
  {
    int First;  //!< Triangle index in first polyhedron (1-based)
    int Second; //!< Triangle index in second polyhedron (1-based)

    TrianglePair(int theFirst = 0, int theSecond = 0)
        : First(theFirst),
          Second(theSecond)
    {
    }
  };

public:
  //! Creates an empty traversal object.
  Standard_EXPORT IntPatch_BVHTraversal();

  //! Destructor.
  Standard_EXPORT virtual ~IntPatch_BVHTraversal();

  //! Performs BVH traversal and collects candidate triangle pairs.
  //! @param[in] theSet1 BVH set for the first polyhedron
  //! @param[in] theSet2 BVH set for the second polyhedron
  //! @param[in] theSelfInterference if true, skip pairs where first index >= second index
  //!            (used for self-intersection where we don't want to test same pair twice)
  //! @return number of collected pairs
  Standard_EXPORT int Perform(IntPatch_PolyhedronBVH& theSet1,
                              IntPatch_PolyhedronBVH& theSet2,
                              bool                    theSelfInterference = false);

  //! Returns the collected triangle pairs.
  const NCollection_Vector<TrianglePair>& Pairs() const { return myPairs; }

  //! Clears the collected pairs.
  void Clear() { myPairs.Clear(); }

public: //! @name BVH_PairTraverse interface implementation
  //! Rejects pair of nodes if their bounding boxes don't overlap.
  //! @param[in] theCMin1 minimum corner of the first node's bounding box
  //! @param[in] theCMax1 maximum corner of the first node's bounding box
  //! @param[in] theCMin2 minimum corner of the second node's bounding box
  //! @param[in] theCMax2 maximum corner of the second node's bounding box
  //! @param[out] theMetric unused metric parameter
  //! @return true if the pair should be rejected (no overlap), false otherwise
  Standard_EXPORT virtual Standard_Boolean RejectNode(const BVH_Vec3d& theCMin1,
                                                      const BVH_Vec3d& theCMax1,
                                                      const BVH_Vec3d& theCMin2,
                                                      const BVH_Vec3d& theCMax2,
                                                      double& theMetric) const Standard_OVERRIDE;

  //! Accepts a pair of leaf elements and stores their original indices.
  //! @param[in] theIndex1 0-based index in the first BVH set
  //! @param[in] theIndex2 0-based index in the second BVH set
  //! @return true (always accepts the pair)
  Standard_EXPORT virtual Standard_Boolean Accept(const int theIndex1,
                                                  const int theIndex2) Standard_OVERRIDE;

private:
  IntPatch_PolyhedronBVH*          mySet1;             //!< First BVH set
  IntPatch_PolyhedronBVH*          mySet2;             //!< Second BVH set
  bool                             mySelfInterference; //!< Self-interference mode flag
  NCollection_Vector<TrianglePair> myPairs;            //!< Collected triangle pairs
};

#endif // IntPatch_BVHTraversal_HeaderFile
