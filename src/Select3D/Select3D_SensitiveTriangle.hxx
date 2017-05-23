// Created on: 1997-05-14
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

#ifndef _Select3D_SensitiveTriangle_HeaderFile
#define _Select3D_SensitiveTriangle_HeaderFile

#include <Select3D_TypeOfSensitivity.hxx>
#include <Select3D_SensitivePoly.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>

class Standard_ConstructionError;
class Standard_OutOfRange;

//! A framework to define selection of triangles in a view.
//! This comes into play in the detection of meshing and triangulation in surfaces.
//! In some cases this class can raise Standard_ConstructionError and
//! Standard_OutOfRange exceptions. For more details see Select3D_SensitivePoly.
class Select3D_SensitiveTriangle : public Select3D_SensitiveEntity
{
public:

  //! Constructs a sensitive triangle object defined by the
  //! owner theOwnerId, the points P1, P2, P3, and the type of sensitivity Sensitivity.
  Standard_EXPORT Select3D_SensitiveTriangle (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                              const gp_Pnt& thePnt0,
                                              const gp_Pnt& thePnt1,
                                              const gp_Pnt& thePnt2,
                                              const Select3D_TypeOfSensitivity theType = Select3D_TOS_INTERIOR);

  //! Checks whether the triangle overlaps current selecting volume
  Standard_EXPORT virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

  //! Returns the 3D points P1, P2, P3 used at the time of construction.
  void Points3D (gp_Pnt& thePnt0, gp_Pnt& thePnt1, gp_Pnt& thePnt2) const
  {
    thePnt0 = myPoints[0];
    thePnt1 = myPoints[1];
    thePnt2 = myPoints[2];
  }

  //! Returns the center point of the sensitive triangle created at construction time.
  Standard_EXPORT gp_Pnt Center3D() const { return myCentroid; }

  //! Returns the copy of this
  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Returns bounding box of the triangle. If location transformation is set, it
  //! will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Returns the amount of points
  virtual Standard_Integer NbSubElements() Standard_OVERRIDE { return 3; }

  virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE { return myCentroid; }

  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveTriangle,Select3D_SensitiveEntity)

private:

  Select3D_TypeOfSensitivity mySensType;     //!< Type of sensitivity: boundary or interior
  gp_Pnt                     myCentroid;     //!< Center of triangle
  gp_Pnt                     myPoints[3];
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveTriangle, Select3D_SensitiveEntity)

#endif
