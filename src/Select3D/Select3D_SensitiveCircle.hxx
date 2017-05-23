// Created on: 1996-02-06
// Created by: Robert COUBLANC
// Copyright (c) 1996-1999 Matra Datavision
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

#ifndef _Select3D_SensitiveCircle_HeaderFile
#define _Select3D_SensitiveCircle_HeaderFile

#include <Select3D_SensitivePoly.hxx>
#include <TColgp_HArray1OfPnt.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>
#include <Select3D_TypeOfSensitivity.hxx>

class Geom_Circle;
class Standard_ConstructionError;
class Standard_OutOfRange;

//! A framework to define sensitive 3D arcs and circles.
//! In some cases this class can raise Standard_ConstructionError and
//! Standard_OutOfRange exceptions. For more details see Select3D_SensitivePoly.
class Select3D_SensitiveCircle : public Select3D_SensitivePoly
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveCircle, Select3D_SensitivePoly)
public:

  //! Constructs the sensitive circle object defined by the
  //! owner theOwnerId, the circle theCircle, the boolean
  //! theIsFilled and the number of points theNbPnts.
  Standard_EXPORT Select3D_SensitiveCircle (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                            const Handle(Geom_Circle)& theCircle,
                                            const Standard_Boolean theIsFilled = Standard_False,
                                            const Standard_Integer theNbPnts = 12);

  //! Constructs the sensitive arc object defined by the
  //! owner theOwnerId, the circle theCircle, the parameters theU1
  //! and theU2, the boolean theIsFilled and the number of points theNbPnts.
  //! theU1 and theU2 define the first and last points of the arc on theCircle.
  Standard_EXPORT Select3D_SensitiveCircle (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                            const Handle(Geom_Circle)& theCircle,
                                            const Standard_Real theU1,
                                            const Standard_Real theU2,
                                            const Standard_Boolean theIsFilled = Standard_False,
                                            const Standard_Integer theNbPnts = 12);

  //! Constructs the sensitive circle object defined by the
  //! owner theOwnerId, the array of triangles thePnts3d, and the boolean theIsFilled.
  //! thePnts3d is an array of consecutive triangles on the
  //! circle. The triangle i+1 lies on the intersection of the
  //! tangents to the circle of i and i+2. Note, that the first point of thePnts3d
  //! must be equal to the last point of thePnts3d.
  Standard_EXPORT Select3D_SensitiveCircle (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                            const Handle(TColgp_HArray1OfPnt)& thePnts3d,
                                            const Standard_Boolean theIsFilled = Standard_False);

  //! Constructs the sensitive circle object defined by the
  //! owner theOwnerId, the array of points thePnts3d, and the boolean theIsFilled.
  //! If the length of thePnts3d is more then 1, the first point of thePnts3d
  //! must be equal to the last point of thePnts3d.
  Standard_EXPORT Select3D_SensitiveCircle (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                            const TColgp_Array1OfPnt& thePnts3d,
                                            const Standard_Boolean theIsFilled = Standard_False);

  //! Checks whether the circle overlaps current selecting volume
  Standard_EXPORT  virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                     SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;


  Standard_EXPORT void ArrayBounds (Standard_Integer & theLow, Standard_Integer & theUp) const;

  Standard_EXPORT gp_Pnt GetPoint3d (const Standard_Integer thePntIdx) const;

  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Returns center of the circle. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Builds BVH tree for a circle's edge segments if needed
  Standard_EXPORT virtual void BVH() Standard_OVERRIDE;

protected:

  //! Calculates distance from the 3d projection of used-picked screen point
  //! to center of the geometry
  virtual Standard_Real distanceToCOG (SelectBasics_SelectingVolumeManager& theMgr) Standard_OVERRIDE;

private:

  //! Computes myCenter3D as the barycenter of points from mypolyg3d
  void computeCenter3D();

private:

  Select3D_TypeOfSensitivity mySensType; //!< True if type of selection is interior, false otherwise
  gp_Pnt                     myCenter3D; //!< Center of a circle
  Handle(Geom_Circle)        myCircle;   //!< Points of the circle
  Standard_Real              myStart;    //!< Sensitive arc parameter
  Standard_Real              myEnd;      //!< Sensitive arc parameter
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveCircle, Select3D_SensitivePoly)

#endif // _Select3D_SensitiveCircle_HeaderFile
