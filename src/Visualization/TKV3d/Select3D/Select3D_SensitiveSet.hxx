// Created on: 2014-05-29
// Created by: Varvara POSKONINA
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#ifndef _Select3D_SensitiveSet_Header
#define _Select3D_SensitiveSet_Header

#include <BVH_PrimitiveSet3d.hxx>
#include <Select3D_BVHBuilder3d.hxx>
#include <Select3D_SensitiveEntity.hxx>

//! This class is base class for handling overlap detection of complex sensitive
//! entities. It provides an interface for building BVH tree for some set of entities.
//! Thereby, each iteration of overlap detection is a traverse of BVH tree in fact.
//! To use speed-up hierarchical structure in a custom complex sensitive entity, it is
//! necessary to make that custom entity a descendant of this class and organize sub-entities
//! in some container which allows referencing to elements by index. Note that methods taking
//! index as a parameter are used for BVH build and the range of given index is [0; Size() - 1].
//! For example of usage see Select3D_SensitiveTriangulation.
class Select3D_SensitiveSet : public Select3D_SensitiveEntity
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveSet, Select3D_SensitiveEntity)
public:
  //! Return global instance to default BVH builder.
  Standard_EXPORT static const Handle(Select3D_BVHBuilder3d)& DefaultBVHBuilder();

  //! Assign new BVH builder to be used by default for new sensitive sets (assigning is NOT
  //! thread-safe!).
  Standard_EXPORT static void SetDefaultBVHBuilder(const Handle(Select3D_BVHBuilder3d)& theBuilder);

public:
  //! Creates new empty sensitive set and its content
  Standard_EXPORT Select3D_SensitiveSet(const Handle(SelectMgr_EntityOwner)& theOwnerId);

public:
  //! Returns the amount of sub-entities of the complex entity
  virtual Standard_Integer Size() const = 0;

  //! Returns bounding box of sub-entity with index theIdx in sub-entity list
  virtual Select3D_BndBox3d Box(const Standard_Integer theIdx) const = 0;

  //! Returns geometry center of sensitive entity index theIdx along the given axis theAxis
  virtual Standard_Real Center(const Standard_Integer theIdx,
                               const Standard_Integer theAxis) const = 0;

  //! Swaps items with indexes theIdx1 and theIdx2
  virtual void Swap(const Standard_Integer theIdx1, const Standard_Integer theIdx2) = 0;

  //! Checks whether one or more entities of the set overlap current selecting volume.
  //! Implements the traverse of BVH tree built for the set
  virtual Standard_Boolean Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                   SelectBasics_PickResult& thePickResult) Standard_OVERRIDE
  {
    return matches(theMgr, thePickResult, false);
  }

  //! Builds BVH tree for sensitive set.
  //! Must be called manually to build BVH tree for any sensitive set
  //! in case if its content was initialized not in a constructor,
  //! but element by element
  Standard_EXPORT virtual void BVH() Standard_OVERRIDE;

  //! Returns TRUE if BVH tree is in invalidated state
  virtual Standard_Boolean ToBuildBVH() const Standard_OVERRIDE { return myContent.IsDirty(); }

  //! Sets the method (builder) used to construct BVH.
  void SetBuilder(const Handle(Select3D_BVHBuilder3d)& theBuilder)
  {
    myContent.SetBuilder(theBuilder);
  }

  //! Marks BVH tree of the set as outdated. It will be rebuild
  //! at the next call of BVH()
  void MarkDirty() { myContent.MarkDirty(); }

  //! Returns bounding box of the whole set.
  //! This method should be redefined in Select3D_SensitiveSet descendants
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns center of the whole set.
  //! This method should be redefined in Select3D_SensitiveSet descendants
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Destroys cross-reference to avoid memory leak
  Standard_EXPORT virtual void Clear() Standard_OVERRIDE;

  //! Returns a number of nodes in 1 BVH leaf
  Standard_Integer GetLeafNodeSize() const { return myContent.Builder()->LeafNodeSize(); }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        Standard_Integer  theDepth = -1) const Standard_OVERRIDE;

protected:
  //! Checks whether one or more entities of the set overlap current selecting volume.
  //! Implements the traverse of BVH tree built for the set
  //! @param theMgr selection manager
  //! @param thePickResult picking result (for picking by ray)
  //! @param theToCheckAllInside flag indicating that even with
  //! SelectMgr_SelectingVolumeManager::IsOverlapAllowed() returning FALSE
  //!        the method will return TRUE if at least one sub-element is fully inside selection
  //!        volume ::elementIsInside(); this is useful for entities allowing local selection of
  //!        sub-elements using single Owner object.
  Standard_EXPORT Standard_Boolean matches(SelectBasics_SelectingVolumeManager& theMgr,
                                           SelectBasics_PickResult&             thePickResult,
                                           Standard_Boolean theToCheckAllInside);

  //! Checks whether the entity with index theIdx (partially) overlaps the current selecting volume.
  //! @param[out] thePickResult  picking result, should update minimum depth
  //! @param[in] theMgr  selection manager
  //! @param[in] theElemIdx  element index within BVH tree to check
  //! @param[in] theIsFullInside  when TRUE indicates that entire BVH node is already inside
  //! selection volume (in case of rectangle selection);
  //!                             in this case algorithm might skip checking the element and just
  //!                             register it as detected
  virtual Standard_Boolean overlapsElement(SelectBasics_PickResult&             thePickResult,
                                           SelectBasics_SelectingVolumeManager& theMgr,
                                           Standard_Integer                     theElemIdx,
                                           Standard_Boolean theIsFullInside) = 0;

  //! Checks whether the entity with index theIdx is (fully) inside the current selecting volume
  //! @param[in] theMgr  selection manager
  //! @param[in] theElemIdx  element index within BVH tree to check
  //! @param[in] theIsFullInside  when TRUE indicates that entire BVH node is already inside
  //! selection volume (in case of rectangle selection);
  //!                             in this case algorithm might skip checking the element and just
  //!                             register it as detected
  virtual Standard_Boolean elementIsInside(SelectBasics_SelectingVolumeManager& theMgr,
                                           Standard_Integer                     theElemIdx,
                                           Standard_Boolean theIsFullInside) = 0;

  //! Calculates distance from the 3d projection of used-picked screen point to center of the
  //! geometry
  virtual Standard_Real distanceToCOG(SelectBasics_SelectingVolumeManager& theMgr) = 0;

  //! Process elements overlapped by the selection volume
  //! @param theMgr selection manager
  //! @param theFirstElem index of the first element
  //! @param theLastElem index of the last element
  //! @param theIsFullInside when TRUE indicates that entire BVH node is already inside selection
  //! volume
  //! @param[out] thePickResult  picking result (for picking by ray)
  //! @param[out] theMatchesNb  number of processed elements
  //! @return FALSE if some element is outside the selection volume (if IsOverlapAllowed is FALSE);
  //! TRUE otherwise
  Standard_EXPORT Standard_Boolean processElements(SelectBasics_SelectingVolumeManager& theMgr,
                                                   Standard_Integer         theFirstElem,
                                                   Standard_Integer         theLastElem,
                                                   Standard_Boolean         theIsFullInside,
                                                   Standard_Boolean         theToCheckAllInside,
                                                   SelectBasics_PickResult& thePickResult,
                                                   Standard_Integer&        theMatchesNb);

protected:
  //! The purpose of this class is to provide a link between BVH_PrimitiveSet
  //! and Select3D_SensitiveSet instance to build BVH tree for set of sensitives.
  class BvhPrimitiveSet : public BVH_PrimitiveSet3d
  {
  public:
    //! Empty constructor.
    BvhPrimitiveSet()
        : BVH_PrimitiveSet3d(Handle(Select3D_BVHBuilder3d)()),
          mySensitiveSet(NULL)
    {
    }

    //! Destructor.
    ~BvhPrimitiveSet() {}

    //! Setup sensitivity set.
    void SetSensitiveSet(Select3D_SensitiveSet* theSensitiveSet)
    {
      mySensitiveSet = theSensitiveSet;
      MarkDirty();
    }

    //! Returns the length of set of sensitives
    virtual Standard_Integer Size() const Standard_OVERRIDE { return mySensitiveSet->Size(); }

    //! Returns bounding box of sensitive with index theIdx
    virtual Select3D_BndBox3d Box(const Standard_Integer theIdx) const Standard_OVERRIDE
    {
      return mySensitiveSet->Box(theIdx);
    }

    //! Make inherited method Box() visible to avoid CLang warning
    using BVH_PrimitiveSet3d::Box;

    //! Returns center of sensitive with index theIdx in the set along the given axis theAxis
    virtual Standard_Real Center(const Standard_Integer theIdx,
                                 const Standard_Integer theAxis) const Standard_OVERRIDE
    {
      return mySensitiveSet->Center(theIdx, theAxis);
    }

    //! Swaps items with indexes theIdx1 and theIdx2 in the set
    virtual void Swap(const Standard_Integer theIdx1,
                      const Standard_Integer theIdx2) Standard_OVERRIDE
    {
      mySensitiveSet->Swap(theIdx1, theIdx2);
    }

    //! Returns the tree built for set of sensitives
    const opencascade::handle<BVH_Tree<Standard_Real, 3>>& GetBVH() { return BVH(); }

    //! Dumps the content of me into the stream
    void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const
    {
      (void)theOStream;
      (void)theDepth;
    }

  protected:
    Select3D_SensitiveSet* mySensitiveSet; //!< Set of sensitive entities
  };

protected:
  BvhPrimitiveSet  myContent;     //!< A link between sensitive entity and BVH_PrimitiveSet
  Standard_Integer myDetectedIdx; //!< Index of detected primitive in BVH sorted primitive array
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveSet, Select3D_SensitiveEntity)

#endif // _Select3D_SensitiveSet_Header
