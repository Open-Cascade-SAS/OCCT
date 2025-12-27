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
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveCylinder, Select3D_SensitiveEntity)

public:
  //! Constructs a sensitive cylinder object defined by the owner theOwnerId,
  //! @param[in] theBottomRad cylinder bottom radius
  //! @param[in] theTopRad    cylinder top radius
  //! @param[in] theHeight    cylinder height
  Standard_EXPORT Select3D_SensitiveCylinder(const occ::handle<SelectMgr_EntityOwner>& theOwnerId,
                                             const double                  theBottomRad,
                                             const double                  theTopRad,
                                             const double                  theHeight,
                                             const gp_Trsf&                       theTrsf,
                                             const bool theIsHollow = false);

  //! Checks whether the cylinder overlaps current selecting volume
  Standard_EXPORT virtual bool Matches(SelectBasics_SelectingVolumeManager& theMgr,
                                                   SelectBasics_PickResult& thePickResult)
    override;

  //! Returns the copy of this
  Standard_EXPORT virtual occ::handle<Select3D_SensitiveEntity> GetConnected() override;

  //! Returns bounding box of the cylinder.
  //! If location transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() override;

  //! Always returns false
  virtual bool ToBuildBVH() const override { return false; }

  //! Returns the amount of points
  virtual int NbSubElements() const override { return 1; }

  //! Returns center of the cylinder with transformation applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const override;

  //! Returns cylinder transformation
  const gp_Trsf& Transformation() const { return myTrsf; }

  //! Returns cylinder top radius
  double TopRadius() const { return myTopRadius; }

  //! Returns cylinder bottom radius
  double BottomRadius() const { return myBottomRadius; }

  //! Returns cylinder height
  double Height() const { return myHeight; }

  //! Returns true if the cylinder is empty inside
  bool IsHollow() const { return myIsHollow; }

protected:
  gp_Trsf          myTrsf;         //!< cylinder transformation to apply
  double    myBottomRadius; //!< cylinder bottom radius
  double    myTopRadius;    //!< cylinder top radius
  double    myHeight;       //!< cylinder height
  bool myIsHollow;     //!< true if the cylinder is empty inside
};

#endif // _Select3D_SensitiveSphere_HeaderFile
