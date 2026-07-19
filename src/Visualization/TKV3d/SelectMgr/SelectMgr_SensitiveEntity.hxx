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

#ifndef _SelectMgr_SensitiveEntity_HeaderFile
#define _SelectMgr_SensitiveEntity_HeaderFile

#include <Select3D_SensitiveEntity.hxx>
#include <Standard.hxx>
#include <Standard_Type.hxx>

//! The purpose of this class is to mark sensitive entities selectable or not
//! depending on current active selection of parent object for proper BVH traverse
class SelectMgr_SensitiveEntity : public Standard_Transient
{
public:
  //! Creates new inactive for selection object with base entity theEntity
  Standard_EXPORT SelectMgr_SensitiveEntity(const occ::handle<Select3D_SensitiveEntity>& theEntity);

  ~SelectMgr_SensitiveEntity() override = default;

  //! Clears up all resources and memory
  Standard_EXPORT void Clear();

  //! Returns related instance of SelectBasics class
  const occ::handle<Select3D_SensitiveEntity>& BaseSensitive() const { return mySensitive; }

  //! Returns true if this entity belongs to the active selection
  //! mode of parent object
  bool IsActiveForSelection() const { return myIsActiveForSelection; }

  //! Marks entity as inactive for selection
  void ResetSelectionActiveStatus() const { myIsActiveForSelection = false; }

  //! Marks entity as active for selection
  void SetActiveForSelection() const { myIsActiveForSelection = true; }

  //! Dumps the content of me into the stream
  Standard_EXPORT virtual void DumpJson(Standard_OStream& theOStream, int theDepth = -1) const;

  DEFINE_STANDARD_RTTIEXT(SelectMgr_SensitiveEntity, Standard_Transient) // Type definition

private:
  occ::handle<Select3D_SensitiveEntity> mySensitive;            //!< Related SelectBasics entity
  mutable bool                          myIsActiveForSelection; //!< Selection activity status
};

#endif // _SelectMgr_SensitiveEntity_HeaderFile
