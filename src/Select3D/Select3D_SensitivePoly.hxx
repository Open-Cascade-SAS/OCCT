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

#ifndef _Select3D_SensitivePoly_HeaderFile
#define _Select3D_SensitivePoly_HeaderFile

#include <gp_Circ.hxx>
#include <Select3D_PointData.hxx>
#include <Select3D_SensitiveSet.hxx>
#include <Select3D_TypeOfSensitivity.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColgp_HArray1OfPnt.hxx>

//! Sensitive Entity to make a face selectable.
//! In some cases this class can raise Standard_ConstructionError and
//! Standard_OutOfRange exceptions from its member Select3D_PointData
//! myPolyg.
class Select3D_SensitivePoly : public Select3D_SensitiveSet
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitivePoly, Select3D_SensitiveSet)
public:

  //! Constructs a sensitive face object defined by the
  //! owner OwnerId, the array of points ThePoints, and
  //! the sensitivity type Sensitivity.
  //! The array of points is the outer polygon of the geometric face.
  Standard_EXPORT Select3D_SensitivePoly (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                          const TColgp_Array1OfPnt& thePoints,
                                          const Standard_Boolean theIsBVHEnabled);

  //! Constructs a sensitive face object defined by the
  //! owner OwnerId, the array of points ThePoints, and
  //! the sensitivity type Sensitivity.
  //! The array of points is the outer polygon of the geometric face.
  Standard_EXPORT Select3D_SensitivePoly (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                          const Handle(TColgp_HArray1OfPnt)& thePoints,
                                          const Standard_Boolean theIsBVHEnabled);

  //! Constructs the sensitive arc object defined by the
  //! owner theOwnerId, the circle theCircle, the parameters theU1
  //! and theU2, the boolean theIsFilled and the number of points theNbPnts.
  //! theU1 and theU2 define the first and last points of the arc on theCircle.
  Standard_EXPORT Select3D_SensitivePoly (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                          const gp_Circ& theCircle,
                                          const Standard_Real theU1,
                                          const Standard_Real theU2,
                                          const Standard_Boolean theIsFilled = Standard_False,
                                          const Standard_Integer theNbPnts = 12);

  //! Constructs a sensitive curve or arc object defined by the
  //! owner theOwnerId, the theIsBVHEnabled flag, and the
  //! maximum number of points on the curve: theNbPnts.
  Standard_EXPORT Select3D_SensitivePoly (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                          const Standard_Boolean theIsBVHEnabled,
                                          const Standard_Integer theNbPnts = 6);

  //! Checks whether the poly overlaps current selecting volume
  Standard_EXPORT  virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                     SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

  //! Returns the amount of segments in poly
  Standard_EXPORT virtual Standard_Integer NbSubElements() const Standard_OVERRIDE;

  //! Returns the 3D points of the array used at construction time.
  void Points3D (Handle(TColgp_HArray1OfPnt)& theHArrayOfPnt)
  {
    Standard_Integer aSize = myPolyg.Size();
    theHArrayOfPnt = new TColgp_HArray1OfPnt (1,aSize);
    for(Standard_Integer anIndex = 1; anIndex <= aSize; anIndex++)
    {
      theHArrayOfPnt->SetValue (anIndex, myPolyg.Pnt (anIndex-1));
    }
  }

  //! Return array bounds.
  void ArrayBounds (Standard_Integer& theLow,
                    Standard_Integer& theUp) const
  {
    theLow = 0;
    theUp = myPolyg.Size() - 1;
  }

  //! Return point.
  gp_Pnt GetPoint3d (const Standard_Integer thePntIdx) const
  {
    return (thePntIdx >= 0 && thePntIdx < myPolyg.Size())
         ? myPolyg.Pnt (thePntIdx)
         : gp_Pnt();
  }

  //! Returns bounding box of a polygon. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns center of the point set. If location transformation
  //! is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Returns the amount of segments of the poly
  Standard_EXPORT virtual Standard_Integer Size() const Standard_OVERRIDE;

  //! Returns bounding box of segment with index theIdx
  Standard_EXPORT virtual Select3D_BndBox3d Box (const Standard_Integer theIdx) const Standard_OVERRIDE;

  //! Returns geometry center of sensitive entity index theIdx in the vector along
  //! the given axis theAxis
  Standard_EXPORT virtual Standard_Real Center (const Standard_Integer theIdx,
                                                const Standard_Integer theAxis) const Standard_OVERRIDE;

  //! Swaps items with indexes theIdx1 and theIdx2 in the vector
  Standard_EXPORT virtual void Swap (const Standard_Integer theIdx1,
                                     const Standard_Integer theIdx2) Standard_OVERRIDE;

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson (Standard_OStream& theOStream, Standard_Integer theDepth = -1) const Standard_OVERRIDE;

protected:

  //! Checks whether the segment with index theIdx overlaps the current selecting volume
  Standard_EXPORT virtual Standard_Boolean overlapsElement (SelectBasics_PickResult& thePickResult,
                                                            SelectBasics_SelectingVolumeManager& theMgr,
                                                            Standard_Integer theElemIdx,
                                                            Standard_Boolean theIsFullInside) Standard_OVERRIDE;

  //! Checks whether the entity with index theIdx is inside the current selecting volume
  Standard_EXPORT virtual Standard_Boolean elementIsInside (SelectBasics_SelectingVolumeManager& theMgr,
                                                            Standard_Integer theElemIdx,
                                                            Standard_Boolean theIsFullInside) Standard_OVERRIDE;

  //! Calculates distance from the 3d projection of used-picked screen point
  //! to center of the geometry
  Standard_EXPORT virtual Standard_Real distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr) Standard_OVERRIDE;

protected:

  Select3D_PointData               myPolyg;          //!< Points of the poly
  mutable gp_Pnt                   myCOG;            //!< Center of the poly
  Handle(TColStd_HArray1OfInteger) mySegmentIndexes; //!< Segment indexes for BVH tree build
  Select3D_BndBox3d                myBndBox;         //!< Bounding box of the poly
  Select3D_TypeOfSensitivity       mySensType;       //!< Type of sensitivity: boundary or interior
  mutable Standard_Boolean         myIsComputed;     //!< Is true if all the points and data structures of polygon are initialized
};

DEFINE_STANDARD_HANDLE(Select3D_SensitivePoly, Select3D_SensitiveSet)

#endif // _Select3D_SensitivePoly_HeaderFile
