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

#include <Select3D_SensitiveEntity.hxx>

#include <gp_Circ.hxx>

//! A framework to define sensitive 3D circles.
class Select3D_SensitiveCircle : public Select3D_SensitiveEntity
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveCircle, Select3D_SensitiveEntity)
public:

  //! Constructs the sensitive circle object defined by the
  //! owner theOwnerId, the circle theCircle and the boolean theIsFilled.
  Standard_EXPORT Select3D_SensitiveCircle (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                            const gp_Circ& theCircle,
                                            const Standard_Boolean theIsFilled = Standard_False);

  //! Constructs the sensitive circle object defined by the
  //! owner theOwnerId, the circle theCircle, the boolean
  //! theIsFilled and the number of points theNbPnts.
  Standard_DEPRECATED("Deprecated constructor, theNbPnts parameter will be ignored")
  Select3D_SensitiveCircle (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                            const gp_Circ& theCircle,
                            const Standard_Boolean theIsFilled,
                            const Standard_Integer /*theNbPnts*/)
  : Select3D_SensitiveCircle (theOwnerId, theCircle, theIsFilled)
  { }

  //! Checks whether the circle overlaps current selecting volume
  Standard_EXPORT  virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                     SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

  //! Returns a copy of this sensitive circle
  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Returns bounding box of the circle.
  //! If location transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Always returns Standard_False
  virtual Standard_Boolean ToBuildBVH() const Standard_OVERRIDE { return Standard_False; }

  //! Returns the amount of points
  virtual Standard_Integer NbSubElements() const Standard_OVERRIDE { return 1; }

  //! Returns center of the circle with transformation applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! The transformation for gp::XOY() with center in gp::Origin(),
  //! it specifies the position and orientation of the circle.
  const gp_Trsf& Transformation() const { return myTrsf; }

  //! Returns circle
  gp_Circ Circle() const { return gp_Circ (gp::XOY().Transformed (myTrsf), myRadius); }

  //! Returns circle radius
  Standard_Real Radius() const { return myRadius; }

private:

  Select3D_TypeOfSensitivity mySensType; //!< Type of sensitivity: boundary or interior
  gp_Trsf                    myTrsf;     //!< Circle transformation to apply
  Standard_Real              myRadius;   //!< Circle radius
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveCircle, Select3D_SensitiveEntity)

#endif // _Select3D_SensitiveCircle_HeaderFile
