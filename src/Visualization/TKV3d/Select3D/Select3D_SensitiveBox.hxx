// Created on: 1995-04-13
// Created by: Robert COUBLANC
// Copyright (c) 1995-1999 Matra Datavision
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

#ifndef _Select3D_SensitiveBox_HeaderFile
#define _Select3D_SensitiveBox_HeaderFile

#include <Select3D_SensitiveEntity.hxx>

//! A framework to define selection by a sensitive box.
class Select3D_SensitiveBox : public Select3D_SensitiveEntity
{
  DEFINE_STANDARD_RTTIEXT(Select3D_SensitiveBox, Select3D_SensitiveEntity)
public:
  //! Constructs a sensitive box object defined by the
  //! owner theOwnerId, and the box theBox.
  Standard_EXPORT Select3D_SensitiveBox(const occ::handle<SelectMgr_EntityOwner>& theOwnerId,
                                        const Bnd_Box&                            theBox);

  //! Constructs a sensitive box object defined by the
  //! owner theOwnerId, and the coordinates theXmin, theYMin, theZMin, theXMax, theYMax, theZMax.
  //! theXmin, theYMin and theZMin define the minimum point in
  //! the front lower left hand corner of the box,
  //! and theXMax, theYMax and theZMax define the maximum
  //! point in the back upper right hand corner of the box.
  Standard_EXPORT Select3D_SensitiveBox(const occ::handle<SelectMgr_EntityOwner>& theOwnerId,
                                        const double                              theXMin,
                                        const double                              theYMin,
                                        const double                              theZMin,
                                        const double                              theXMax,
                                        const double                              theYMax,
                                        const double                              theZMax);

  //! Returns the amount of sub-entities in sensitive
  Standard_EXPORT int NbSubElements() const override;

  Standard_EXPORT occ::handle<Select3D_SensitiveEntity> GetConnected() override;

  //! Checks whether the box overlaps current selecting volume
  Standard_EXPORT bool Matches(SelectBasics_SelectingVolumeManager& theMgr,
                               SelectBasics_PickResult&             thePickResult) override;

  Bnd_Box Box() const
  {
    Bnd_Box aBox;
    aBox.Update(myBox.CornerMin().x(),
                myBox.CornerMin().y(),
                myBox.CornerMin().z(),
                myBox.CornerMax().x(),
                myBox.CornerMax().y(),
                myBox.CornerMax().z());

    return aBox;
  }

  //! Returns center of the box. If location
  //! transformation is set, it will be applied
  Standard_EXPORT gp_Pnt CenterOfGeometry() const override;

  //! Returns coordinates of the box. If location
  //! transformation is set, it will be applied
  Standard_EXPORT Select3D_BndBox3d BoundingBox() override;

  //! Returns TRUE if BVH tree is in invalidated state
  bool ToBuildBVH() const override { return false; }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const override;

private:
  Select3D_BndBox3d myBox;      //!< 3d coordinates of box corners
  gp_Pnt            myCenter3d; //!< 3d coordinate of box's center
};

#endif // _Select3D_SensitiveBox_HeaderFile
