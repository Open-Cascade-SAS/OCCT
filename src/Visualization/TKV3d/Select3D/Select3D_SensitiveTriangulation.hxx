// Created on: 1997-05-15
// Created by: Robert COUBLANC
// Copyright (c) 1997-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Select3D_SensitiveTriangulation_Header
#define _Select3D_SensitiveTriangulation_Header

#include <Standard_Integer.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <Select3D_SensitiveSet.hxx>

class Poly_Triangle;
class Poly_Triangulation;

//! A framework to define selection of a sensitive entity made of a set of triangles.
class Select3D_SensitiveTriangulation : public Select3D_SensitiveSet
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveTriangulation, Select3D_SensitiveSet)
public:
  //! Constructs a sensitive triangulation object defined by
  //! the owner theOwnerId, the triangulation theTrg,
  //! the location theInitLoc, and the flag theIsInterior.
  Standard_EXPORT Select3D_SensitiveTriangulation(
    const occ::handle<SelectMgr_EntityOwner>& theOwnerId,
    const occ::handle<Poly_Triangulation>&    theTrg,
    const TopLoc_Location&                    theInitLoc,
    const bool                                theIsInterior = true);

  //! Constructs a sensitive triangulation object defined by
  //! the owner theOwnerId, the triangulation theTrg,
  //! the location theInitLoc, the array of free edges
  //! theFreeEdges, the center of gravity theCOG, and the flag theIsInterior.
  //! As free edges and the center of gravity do not have
  //! to be computed later, this syntax reduces computation time.
  Standard_EXPORT Select3D_SensitiveTriangulation(
    const occ::handle<SelectMgr_EntityOwner>&    theOwnerId,
    const occ::handle<Poly_Triangulation>&       theTrg,
    const TopLoc_Location&                       theInitLoc,
    const occ::handle<NCollection_HArray1<int>>& theFreeEdges,
    const gp_Pnt&                                theCOG,
    const bool                                   theIsInterior);

public:
  //! Get last detected triangle.
  //! @param[out] theTriangle  triangle node indexes
  //! @return TRUE if defined
  Standard_EXPORT bool LastDetectedTriangle(Poly_Triangle& theTriangle) const;

  //! Get last detected triangle.
  //! @param[out] theTriangle  triangle node indexes
  //! @param[out] theTriNodes  triangle nodes (with pre-applied transformation)
  //! @return TRUE if defined
  Standard_EXPORT bool LastDetectedTriangle(Poly_Triangle& theTriangle,
                                            gp_Pnt         theTriNodes[3]) const;

  //! Return index of last detected triangle within [1..NbTris] range, or -1 if undefined.
  int LastDetectedTriangleIndex() const
  {
    return (myDetectedIdx != -1 && mySensType == Select3D_TOS_INTERIOR
            && !myBVHPrimIndexes.IsNull())
             ? myBVHPrimIndexes->Value(myDetectedIdx) + 1
             : -1;
  }

public:
  //! Returns the amount of nodes in triangulation
  Standard_EXPORT virtual int NbSubElements() const override;

  Standard_EXPORT occ::handle<Select3D_SensitiveEntity> GetConnected() override;

  const occ::handle<Poly_Triangulation>& Triangulation() const { return myTriangul; }

  //! Returns the length of array of triangles or edges
  Standard_EXPORT virtual int Size() const override;

  //! Returns bounding box of triangle/edge with index theIdx
  Standard_EXPORT virtual Select3D_BndBox3d Box(const int theIdx) const override;

  //! Returns geometry center of triangle/edge with index theIdx
  //! in array along the given axis theAxis
  Standard_EXPORT virtual double Center(const int theIdx, const int theAxis) const override;

  //! Swaps items with indexes theIdx1 and theIdx2 in array
  Standard_EXPORT virtual void Swap(const int theIdx1, const int theIdx2) override;

  //! Returns bounding box of the triangulation. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() override;

  //! Returns center of triangulation. If location transformation
  //! is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const override;

  //! Returns true if the shape corresponding to the entity has init location
  Standard_EXPORT virtual bool HasInitLocation() const override;

  //! Returns inversed location transformation matrix if the shape corresponding
  //! to this entity has init location set. Otherwise, returns identity matrix.
  Standard_EXPORT virtual gp_GTrsf InvInitLocation() const override;

  const TopLoc_Location& GetInitLocation() const { return myInitLocation; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream,
                                        int               theDepth = -1) const override;

  //! Checks whether one or more entities of the set overlap current selecting volume.
  Standard_EXPORT virtual bool Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                       SelectBasics_PickResult&             thePickResult) override;

protected:
  //! Compute bounding box.
  void computeBoundingBox();

  //! Inner function for transformation application to bounding
  //! box of the triangulation
  Select3D_BndBox3d applyTransformation();

private:
  //! Checks whether the element with index theIdx overlaps the current selecting volume
  Standard_EXPORT virtual bool overlapsElement(SelectBasics_PickResult&             thePickResult,
                                               SelectBasics_SelectingVolumeManager& theMgr,
                                               int                                  theElemIdx,
                                               bool theIsFullInside) override;

  //! Calculates distance from the 3d projection of used-picked screen point to center of the
  //! geometry
  Standard_EXPORT virtual double distanceToCOG(
    SelectBasics_SelectingVolumeManager& theMgr) override;

  //! Checks whether the entity with index theIdx is inside the current selecting volume
  Standard_EXPORT virtual bool elementIsInside(SelectBasics_SelectingVolumeManager& theMgr,
                                               int                                  theElemIdx,
                                               bool theIsFullInside) override;

protected:
  occ::handle<Poly_Triangulation>       myTriangul;
  TopLoc_Location                       myInitLocation;
  gp_Pnt                                myCDG3D; //!< Center of the whole triangulation
  occ::handle<NCollection_HArray1<int>> myFreeEdges;
  // clang-format off
  bool                 mySensType;            //!< Type of sensitivity: boundary or interior
  int                 myPrimitivesNb;       //!< Amount of free edges or triangles depending on sensitivity type
  occ::handle<NCollection_HArray1<int>> myBVHPrimIndexes;     //!< Indexes of edges or triangles for BVH build
  mutable Select3D_BndBox3d        myBndBox;             //!< Bounding box of the whole triangulation
  // clang-format on
  gp_GTrsf myInvInitLocation;
};

#endif // _Select3D_SensitiveTriangulation_Header
