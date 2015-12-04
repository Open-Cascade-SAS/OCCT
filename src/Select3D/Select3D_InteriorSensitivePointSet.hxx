// Created on: 2014-08-15
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

#ifndef _Select3D_InteriorSensitivePointSet_HeaderFile
#define _Select3D_InteriorSensitivePointSet_HeaderFile

#include <NCollection_Vector.hxx>

#include <TColgp_HArray1OfPnt.hxx>
#include <TColStd_HArray1OfInteger.hxx>

#include <Select3D_SensitivePoly.hxx>
#include <Select3D_SensitiveSet.hxx>

class gp_Pnt;
class SelectBasics_EntityOwner;

typedef NCollection_Vector<Handle(Select3D_SensitivePoly)> Select3D_VectorOfHPoly;

//! This class handles the selection of arbitrary point set with internal type of sensitivity.
//! The main principle is to split the point set given onto planar convex polygons and search
//! for the overlap with one or more of them through traverse of BVH tree.
class Select3D_InteriorSensitivePointSet : public Select3D_SensitiveSet
{
public:

  //! Splits the given point set thePoints onto planar convex polygons
  Standard_EXPORT Select3D_InteriorSensitivePointSet (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                                      const TColgp_Array1OfPnt& thePoints);

  //! Initializes the given array theHArrayOfPnt by 3d coordinates of vertices of the
  //! whole point set
  Standard_EXPORT virtual void GetPoints (Handle(TColgp_HArray1OfPnt)& theHArrayOfPnt);

  //! Returns the length of vector of planar convex polygons
  Standard_EXPORT virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Returns bounding box of planar convex polygon with index theIdx
  Standard_EXPORT virtual Select3D_BndBox3d Box (const Standard_Integer theIdx) const Standard_OVERRIDE;

  //! Returns geometry center of planar convex polygon with index
  //! theIdx in the vector along the given axis theAxis
  Standard_EXPORT virtual Standard_Real Center (const Standard_Integer theIdx,
                                                const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Swaps items with indexes theIdx1 and theIdx2 in the vector
  Standard_EXPORT virtual void Swap (const Standard_Integer theIdx1,
                                     const Standard_Integer theIdx2) Standard_OVERRIDE;

  //! Returns bounding box of the point set. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns center of the point set. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Returns the amount of points in set
  Standard_EXPORT virtual Standard_Integer NbSubElements() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTIEXT(Select3D_InteriorSensitivePointSet,Select3D_SensitiveSet)

protected:

  //! Checks whether the planar convex polygon with index theIdx
  //! in myPlanarPolygons overlaps the current selecting volume
  Standard_EXPORT virtual Standard_Boolean overlapsElement (SelectBasics_SelectingVolumeManager& theMgr,
                                                            Standard_Integer theElemIdx,
                                                            Standard_Real& theMatchDepth) Standard_OVERRIDE;

  //! Checks whether the entity with index theIdx is inside the current selecting volume
  Standard_EXPORT virtual Standard_Boolean elementIsInside (SelectBasics_SelectingVolumeManager& theMgr,
                                                            const Standard_Integer               theElemIdx) Standard_OVERRIDE;

  //! Calculates distance from the 3d projection of used-picked
  //! screen point to center of the geometry
  Standard_EXPORT virtual Standard_Real distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr) Standard_OVERRIDE;

protected:

  Select3D_VectorOfHPoly          myPlanarPolygons;     //!< Vector of planar polygons
  Handle(TColStd_HArray1OfInteger) myPolygonsIdxs;       //!< Indexes array for BVH calculation
  gp_Pnt                          myCOG;                //!< Center of the point set
  Select3D_BndBox3d               myBndBox;             //!< Bounding box of the point set
};

DEFINE_STANDARD_HANDLE(Select3D_InteriorSensitivePointSet, Select3D_SensitiveSet)

#endif // _Select3D_InteriorSensitivePointSet_HeaderFile
