// Copyright (c) 2015-2021 OPEN CASCADE SAS
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

#ifndef _Poly_MergeNodesTool_HeaderFile
#define _Poly_MergeNodesTool_HeaderFile

#include <NCollection_Map.hxx>
#include <Poly_Triangulation.hxx>
#include <Standard_HashUtils.hxx>

//! Auxiliary tool for merging triangulation nodes for visualization purposes.
//! Tool tries to merge all nodes within input triangulation, but split the ones on sharp corners at specified angle.
class Poly_MergeNodesTool : public Standard_Transient
{
  DEFINE_STANDARD_RTTIEXT(Poly_MergeNodesTool, Standard_Transient)
public:

  //! Merge nodes of existing mesh and return the new mesh.
  //! @param[in] theTris triangulation to add
  //! @param[in] theTrsf transformation to apply
  //! @param[in] theToReverse reverse triangle nodes order
  //! @param[in] theSmoothAngle merge angle in radians
  //! @param[in] theMergeTolerance linear merge tolerance
  //! @param[in] theToForce return merged triangulation even if it's statistics is equal to input one
  //! @return merged triangulation or NULL on no result
  Standard_EXPORT static Handle(Poly_Triangulation) MergeNodes (const Handle(Poly_Triangulation)& theTris,
                                                                const gp_Trsf& theTrsf,
                                                                const Standard_Boolean theToReverse,
                                                                const double theSmoothAngle,
                                                                const double theMergeTolerance = 0.0,
                                                                const bool   theToForce = true);

public:

  //! Constructor
  //! @param[in] theSmoothAngle smooth angle in radians or 0.0 to disable merging by angle
  //! @param[in] theMergeTolerance node merging maximum distance
  //! @param[in] theNbFacets estimated number of facets for map preallocation
  Standard_EXPORT Poly_MergeNodesTool (const double theSmoothAngle,
                                       const double theMergeTolerance = 0.0,
                                       const int    theNbFacets = -1);

  //! Return merge tolerance; 0.0 by default (only 3D points with exactly matching coordinates are merged).
  double MergeTolerance() const { return myNodeIndexMap.MergeTolerance(); }

  //! Set merge tolerance.
  void SetMergeTolerance (double theTolerance) { myNodeIndexMap.SetMergeTolerance (theTolerance); }

  //! Return merge angle in radians; 0.0 by default (normals with non-exact directions are not merged).
  double MergeAngle() const { return myNodeIndexMap.MergeAngle(); }

  //! Set merge angle.
  void SetMergeAngle (double theAngleRad) { myNodeIndexMap.SetMergeAngle (theAngleRad); }

  //! Return TRUE if nodes with opposite normals should be merged; FALSE by default.
  bool ToMergeOpposite() const { return myNodeIndexMap.ToMergeOpposite(); }

  //! Set if nodes with opposite normals should be merged.
  void SetMergeOpposite (bool theToMerge) { myNodeIndexMap.SetMergeOpposite (theToMerge); }

  //! Setup unit factor.
  void SetUnitFactor (double theUnitFactor) { myUnitFactor = theUnitFactor; }

  //! Return TRUE if degenerate elements should be discarded; TRUE by default.
  bool ToDropDegenerative() const { return myToDropDegenerative; }

  //! Set if degenerate elements should be discarded.
  void SetDropDegenerative (bool theToDrop) { myToDropDegenerative = theToDrop; }

  //! Return TRUE if equal elements should be filtered; FALSE by default.
  bool ToMergeElems() const { return myToMergeElems; }

  //! Set if equal elements should be filtered.
  void SetMergeElems (bool theToMerge) { myToMergeElems = theToMerge; }

  //! Compute normal for the mesh element.
  NCollection_Vec3<float> computeTriNormal() const
  {
    const gp_XYZ aVec01 = myPlaces[1] - myPlaces[0];
    const gp_XYZ aVec02 = myPlaces[2] - myPlaces[0];
    const gp_XYZ aCross = aVec01 ^ aVec02;
    NCollection_Vec3<float> aNorm ((float )aCross.X(), (float )aCross.Y(), (float )aCross.Z());
    return aNorm.Normalized();
  }

public:

  //! Add another triangulation to created one.
  //! @param[in] theTris triangulation to add
  //! @param[in] theTrsf transformation to apply
  //! @param[in] theToReverse reverse triangle nodes order
  Standard_EXPORT virtual void AddTriangulation (const Handle(Poly_Triangulation)& theTris,
                                                 const gp_Trsf& theTrsf = gp_Trsf(),
                                                 const Standard_Boolean theToReverse = false);

  //! Prepare and return result triangulation (temporary data will be truncated to result size).
  Standard_EXPORT Handle(Poly_Triangulation) Result();

public:

  //! Add new triangle.
  //! @param[in] theElemNodes 3 element nodes
  void AddTriangle (const gp_XYZ theElemNodes[3])
  {
    AddElement (theElemNodes, 3);
  }

  //! Add new quad.
  //! @param[in] theElemNodes 4 element nodes
  void AddQuad (const gp_XYZ theElemNodes[4])
  {
    AddElement (theElemNodes, 4);
  }

  //! Add new triangle or quad.
  //! @param[in] theElemNodes element nodes
  //! @param[in] theNbNodes number of element nodes, should be 3 or 4
  Standard_EXPORT void AddElement (const gp_XYZ* theElemNodes,
                                   int theNbNodes);

  //! Change node coordinates of element to be pushed.
  //! @param[in] theIndex node index within current element, in 0..3 range
  gp_XYZ& ChangeElementNode (int theIndex) { return myPlaces[theIndex]; }

  //! Add new triangle or quad with nodes specified by ChangeElementNode().
  Standard_EXPORT void PushLastElement (int theNbNodes);

  //! Add new triangle with nodes specified by ChangeElementNode().
  void PushLastTriangle() { PushLastElement (3); }

  //! Add new quad with nodes specified by ChangeElementNode().
  void PushLastQuad() { PushLastElement (4); }

  //! Return current element node index defined by PushLastElement().
  Standard_Integer ElementNodeIndex (int theIndex) const { return myNodeInds[theIndex]; }

  //! Return number of nodes.
  int NbNodes() const { return myNbNodes; }

  //! Return number of elements.
  int NbElements() const { return myNbElems; }

  //! Return number of discarded degenerate elements.
  int NbDegenerativeElems() const { return myNbDegenElems; }

  //! Return number of merged equal elements.
  int NbMergedElems() const { return myNbMergedElems; }

  //! Setup output triangulation for modifications.
  //! When set to NULL, the tool could be used as a merge map for filling in external mesh structure.
  Handle(Poly_Triangulation)& ChangeOutput() { return myPolyData; }

private:

  //! Push triangle node with normal angle comparison.
  void pushNodeCheck (bool& theIsOpposite,
                      const int theTriNode)
  {
    int aNodeIndex = myNbNodes;
    const gp_XYZ& aPlace = myPlaces[theTriNode];
    const NCollection_Vec3<float> aVec3 ((float )aPlace.X(), (float )aPlace.Y(), (float )aPlace.Z());
    if (myNodeIndexMap.Bind (aNodeIndex, theIsOpposite, aVec3, myTriNormal))
    {
      ++myNbNodes;
      if (!myPolyData.IsNull())
      {
        if (myPolyData->NbNodes() < myNbNodes)
        {
          myPolyData->ResizeNodes (myNbNodes * 2, true);
        }
        myPolyData->SetNode (myNbNodes, aPlace * myUnitFactor);
      }
    }
    myNodeInds[theTriNode] = aNodeIndex;
  }

  //! Push triangle node without merging vertices.
  inline void pushNodeNoMerge (const int theTriNode)
  {
    int aNodeIndex = myNbNodes;
    const gp_XYZ aPlace = myPlaces[theTriNode] * myUnitFactor;

    ++myNbNodes;
    if (!myPolyData.IsNull())
    {
      if (myPolyData->NbNodes() < myNbNodes)
      {
        myPolyData->ResizeNodes (myNbNodes * 2, true);
      }
      myPolyData->SetNode (myNbNodes, aPlace);
    }

    myNodeInds[theTriNode] = aNodeIndex;
  }

private:

  //! Pair holding Vec3 and Normal to the triangle
  struct Vec3AndNormal
  {
    NCollection_Vec3<float> Pos;  //!< position
    NCollection_Vec3<float> Norm; //!< normal to the element

    Vec3AndNormal (const NCollection_Vec3<float>& thePos,
                   const NCollection_Vec3<float>& theNorm)
    : Pos (thePos), Norm (theNorm) {}
  };

  //! Custom map class with key as Node + element normal and value as Node index.
  //! NCollection_DataMap is not used, as it requires Hasher to be defined as class template and not class field.
  class MergedNodesMap : public NCollection_BaseMap
  {
  public:
    typedef NCollection_Vec3<int64_t> CellVec3i;
  public:
    //! Main constructor.
    Standard_EXPORT MergedNodesMap (const int theNbBuckets);

    //! Return merge angle in radians;
    double MergeAngle() const { return myAngle; }

    //! Set merge angle.
    void SetMergeAngle (double theAngleRad)
    {
      myAngle    = (float )theAngleRad;
      myAngleCos = (float )Cos (theAngleRad);
    }

    //! Return TRUE if merge angle is non-zero.
    //! 0 angle means angles should much without a tolerance.
    bool HasMergeAngle() const { return myAngle > 0.0f; }

    //! Return TRUE if merge angle comparison can be skipped (angle is close to 90 degrees).
    bool ToMergeAnyAngle() const { return myAngleCos <= 0.01f; }

    //! Return TRUE if nodes with opposite normals should be merged; FALSE by default.
    bool ToMergeOpposite() const { return myToMergeOpposite; }

    //! Set if nodes with opposite normals should be merged.
    void SetMergeOpposite (bool theToMerge) { myToMergeOpposite = theToMerge; }

    //! Return merge tolerance.
    double MergeTolerance() const { return myTolerance; }

    //! Set merge tolerance.
    Standard_EXPORT void SetMergeTolerance (double theTolerance);

    //! Return TRUE if merge tolerance is non-zero.
    bool HasMergeTolerance() const { return myTolerance > 0.0f; }

    //! Bind node to the map or find existing one.
    //! @param theIndex [in,out] index of new key to add, or index of existing key, if already bound
    //! @param theIsOpposite [out] flag indicating that existing (already bound) node has opposite direction
    //! @param thePos   [in] node position to add or find
    //! @param theNorm  [in] element normal for equality check
    //! @return TRUE if node was not bound already
    Standard_EXPORT bool Bind (int&  theIndex,
                               bool& theIsOpposite,
                               const NCollection_Vec3<float>& thePos,
                               const NCollection_Vec3<float>& theNorm);

    //! ReSize the map.
    Standard_EXPORT void ReSize (const int theSize);

  private:

    //! Return cell index for specified 3D point and inverted cell size.
    CellVec3i vec3ToCell (const NCollection_Vec3<float>& thePnt) const
    {
      return CellVec3i (thePnt * myInvTol);
    }

    //! Hash code for integer vec3.
    Standard_EXPORT static size_t vec3iHashCode (const Poly_MergeNodesTool::MergedNodesMap::CellVec3i& theVec,
                                                 const int theUpper);

    //! Compute hash code.
    Standard_EXPORT size_t hashCode (const NCollection_Vec3<float>& thePos,
                                     const NCollection_Vec3<float>& theNorm,
                                     const int theUpper) const;

    //! Compute hash code.
    size_t hashCode (const Vec3AndNormal& theKey, const int theUpper) const
    {
      return hashCode (theKey.Pos, theKey.Norm, theUpper);
    }

    //! Compare two vectors with inversed tolerance.
    Standard_EXPORT bool vec3AreEqual (const NCollection_Vec3<float>& theKey1,
                                       const NCollection_Vec3<float>& theKey2) const;

    //! Compare two nodes.
    Standard_EXPORT bool isEqual (const Vec3AndNormal& theKey1,
                                  const NCollection_Vec3<float>& thePos2,
                                  const NCollection_Vec3<float>& theNorm2,
                                  bool& theIsOpposite) const;
  private:
    //! Map node.
    class DataMapNode;
  private:
    float myTolerance;       //!< linear tolerance for comparison
    float myInvTol;          //!< inversed linear tolerance for comparison
    float myAngle;           //!< angle for comparison
    float myAngleCos;        //!< angle cosine for comparison
    bool  myToMergeOpposite; //!< merge nodes with opposite normals
  };

  //! Hasher for merging equal elements (with pre-sorted indexes).
  struct MergedElemHasher
  {
    size_t operator()(const NCollection_Vec4<int>& theVec) const
    {
      return opencascade::hashBytes(&theVec[0], 4 * sizeof(int));
    }

    bool operator()(const NCollection_Vec4<int>& theKey1, const NCollection_Vec4<int>& theKey2) const
    {
      return theKey1.IsEqual (theKey2);
    }
  };

private:

  Handle(Poly_Triangulation) myPolyData;           //!< output triangulation
  MergedNodesMap             myNodeIndexMap;       //!< map of merged nodes
  NCollection_Map<NCollection_Vec4<int>, MergedElemHasher>
                             myElemMap;            //!< map of elements
  NCollection_Vec4<int>      myNodeInds;           //!< current element indexes
  NCollection_Vec3<float>    myTriNormal;          //!< current triangle normal
  gp_XYZ                     myPlaces[4];          //!< current triangle/quad coordinates to push

  Standard_Real              myUnitFactor;         //!< scale factor to apply
  Standard_Integer           myNbNodes;            //!< number of output nodes
  Standard_Integer           myNbElems;            //!< number of output elements
  Standard_Integer           myNbDegenElems;       //!< number of degenerated elements
  Standard_Integer           myNbMergedElems;      //!< number of merged elements
  Standard_Boolean           myToDropDegenerative; //!< flag to filter our degenerate elements
  Standard_Boolean           myToMergeElems;       //!< flag to merge elements

};

#endif // _Poly_MergeNodesTool_HeaderFile
