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

#include <Standard.hxx>
#include <Standard_DefineHandle.hxx>
#include <Standard_Type.hxx>

#include <Bnd_Box.hxx>
#include <gp_Pnt.hxx>
#include <Select3D_SensitiveEntity.hxx>
#include <Handle_SelectBasics_EntityOwner.hxx>
#include <SelectMgr_SelectingVolumeManager.hxx>
#include <Standard_OStream.hxx>

class SelectBasics_EntityOwner;
class Bnd_Box;
class TopLoc_Location;


//! A framework to define selection by a sensitive box.
class Select3D_SensitiveBox : public Select3D_SensitiveEntity
{
public:

  //! Constructs a sensitive box object defined by the
  //! owner theOwnerId, and the box theBox.
  Standard_EXPORT Select3D_SensitiveBox (const Handle(SelectBasics_EntityOwner)& theOwnerId, const Bnd_Box& theBox);

  //! Constructs a sensitive box object defined by the
  //! owner theOwnerId, and the coordinates theXmin, theYMin, theZMin, theXMax, theYMax, theZMax.
  //! theXmin, theYMin and theZMin define the minimum point in
  //! the front lower left hand corner of the box,
  //! and theXMax, theYMax and theZMax define the maximum
  //! point in the back upper right hand corner of the box.
  Standard_EXPORT Select3D_SensitiveBox (const Handle(SelectBasics_EntityOwner)& theOwnerId,
                                         const Standard_Real theXMin,
                                         const Standard_Real theYMin,
                                         const Standard_Real theZMin,
                                         const Standard_Real theXMax,
                                         const Standard_Real theYMax,
                                         const Standard_Real theZMax);

  //! Returns the amount of sub-entities in sensitive
  Standard_EXPORT virtual Standard_Integer NbSubElements() Standard_OVERRIDE;

  Standard_EXPORT virtual Handle(Select3D_SensitiveEntity) GetConnected() Standard_OVERRIDE;

  //! Checks whether the box overlaps current selecting volume
  Standard_EXPORT virtual Standard_Boolean Matches (SelectBasics_SelectingVolumeManager& theMgr,
                                                    SelectBasics_PickResult& thePickResult) Standard_OVERRIDE;

  Bnd_Box Box() const;

  //! Returns center of the box. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual gp_Pnt CenterOfGeometry() const Standard_OVERRIDE;

  //! Returns coordinates of the box. If location
  //! transformation is set, it will be applied
  Standard_EXPORT virtual Select3D_BndBox3d BoundingBox() Standard_OVERRIDE;

  DEFINE_STANDARD_RTTI(Select3D_SensitiveBox)

private:

  Select3D_BndBox3d myBox;     //!< 3d coordinates of box corners
  gp_Pnt myCenter3d;           //!< 3d coordinate of box's center
};

DEFINE_STANDARD_HANDLE(Select3D_SensitiveBox, Select3D_SensitiveEntity)

#endif // _Select3D_SensitiveBox_HeaderFile
