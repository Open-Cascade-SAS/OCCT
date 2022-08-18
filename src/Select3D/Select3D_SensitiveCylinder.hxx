// Created on: 2021-04-19
// Created by: Maria KRYLOVA
// Copyright (c) 2021 OPEN CASCADE SAS
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

#ifndef _Select3D_SensitiveCylinder_HeaderFile
#define _Select3D_SensitiveCylinder_HeaderFile

#include <Select3D_SensitiveEntity.hxx>

//! A framework to define selection by a sensitive cylinder or cone.
class Select3D_SensitiveCylinder : public Select3D_SensitiveEntity
{
  DEFINE_STANDARD_RTTIEXT (Select3D_SensitiveCylinder, Select3D_SensitiveEntity)

public:
  //! Constructs a sensitive cylinder object defined by the owner theOwnerId,
  //! @param[in] theBottomRad cylinder bottom radius
  //! @param[in] theTopRad    cylinder top radius
  //! @param[in] theHeight    cylinder height
  Standard_EXPORT Select3D_SensitiveCylinder (const Handle(SelectMgr_EntityOwner)& theOwnerId,
                                              const Standard_Real theBottomRad,
                                              const Standard_Real theTopRad,
                                              const Standard_Real theHeight,
                                              const gp_Trsf& theTrsf,
                                              const Standard_Boolean theIsHollow = Standard_False);

  //! Checks whether the cylinder overlaps current selecting volume
  Standard_EXPORT virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

  //! Returns the copy of this
  Standard_EXPORT virtual Handle (Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Returns bounding box of the cylinder.
  //! If location transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  //! Always returns Standard_False
  virtual Standard_Boolean ToBuildBVH() const Standard_OVERRIDE { return Standard_False; }

  //! Returns the amount of points
  virtual Standard_Integer NbSubElements() const Standard_OVERRIDE { return 1; }

  //! Returns center of the cylinder with transformation applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Returns cylinder transformation
  const gp_Trsf& Transformation() const { return myTrsf; }

  //! Returns cylinder top radius
  Standard_Real TopRadius() const { return myTopRadius; }

  //! Returns cylinder bottom radius
  Standard_Real BottomRadius() const { return myBottomRadius; }

  //! Returns cylinder height
  Standard_Real Height() const { return myHeight; }

  //! Returns true if the cylinder is empty inside
  Standard_Boolean IsHollow() const { return myIsHollow; }

protected:
  gp_Trsf          myTrsf;         //!< cylinder transformation to apply
  Standard_Real    myBottomRadius; //!< cylinder bottom radius
  Standard_Real    myTopRadius;    //!< cylinder top radius
  Standard_Real    myHeight;       //!< cylinder height
  Standard_Boolean myIsHollow;     //!< true if the cylinder is empty inside
};

#endif // _Select3D_SensitiveSphere_HeaderFile
