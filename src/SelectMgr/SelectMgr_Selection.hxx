// Created on: 1995-02-16
// Created by: Mister rmi
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

#ifndef _SelectMgr_Selection_HeaderFile
#define _SelectMgr_Selection_HeaderFile

#include <NCollection_Vector.hxx>

#include <Standard.hxx>
#include <Standard_Type.hxx>
#include <Standard_DefineHandle.hxx>

#include <SelectMgr_TypeOfUpdate.hxx>
#include <MMgt_TShared.hxx>
#include <Handle_SelectBasics_SensitiveEntity.hxx>
#include <SelectMgr_SensitiveEntity.hxx>
#include <SelectMgr_StateOfSelection.hxx>
#include <SelectMgr_TypeOfBVHUpdate.hxx>

class Standard_NullObject;
class SelectBasics_SensitiveEntity;

//!  Represents the state of a given selection mode for a
//! Selectable Object. Contains all the sensitive entities available for this mode.
//! An interactive object can have an indefinite number of
//! modes of selection, each representing a
//! "decomposition" into sensitive primitives; each
//! primitive has an Owner (SelectMgr_EntityOwner)
//! which allows us to identify the exact entity which has
//! been detected. Each Selection mode is identified by
//! an index. The set of sensitive primitives which
//! correspond to a given mode is stocked in a
//! SelectMgr_Selection object. By Convention, the
//! default selection mode which allows us to grasp the
//! Interactive object in its entirety will be mode 0.
//! AIS_Trihedron : 4 selection modes
//! -   mode 0 : selection of a trihedron
//! -   mode 1 : selection of the origin of the trihedron
//! -   mode 2 : selection of the axes
//! -   mode 3 : selection of the planes XOY, YOZ, XOZ
//! when you activate one of modes 1 2 3 4 , you pick AIS objects of type:
//! -   AIS_Point
//! -   AIS_Axis (and information on the type of axis)
//! -   AIS_Plane (and information on the type of plane).
//!   AIS_PlaneTrihedron offers 3 selection modes:
//! -   mode 0 : selection of the whole trihedron
//! -   mode 1 : selection of the origin of the trihedron
//! -   mode 2 : selection of the axes - same remarks as for the Trihedron.
//! AIS_Shape : 7 maximum selection modes, depending
//! on the complexity of the shape :
//! -   mode 0 : selection of the AIS_Shape
//! -   mode 1 : selection of the vertices
//! -   mode 2 : selection of the edges
//! -   mode 3 : selection of the wires
//! -   mode 4 : selection of the faces
//! -   mode 5 : selection of the shells
//! -   mode 6 :   selection of the constituent solids.
class SelectMgr_Selection : public MMgt_TShared
{

public:

  //! Constructs a selection object defined by the selection mode IdMode.
  //! The default setting 0 is the selection mode for a shape in its entirety.
  Standard_EXPORT SelectMgr_Selection (const Standard_Integer theModeIdx = 0);

  ~SelectMgr_Selection();

  Standard_EXPORT void Destroy();

  //! Adds the sensitive primitive aprimitive to the list of
  //! stored entities in this object.
  //! Raises NullObject if the primitive is a null handle.
  Standard_EXPORT void Add (const Handle(SelectBasics_SensitiveEntity)& theSensitive);

  //! empties the selection from all the stored entities
  Standard_EXPORT void Clear();

  //! returns true if no sensitive entity is stored.
  Standard_EXPORT Standard_Boolean IsEmpty() const;

  //! returns the selection mode represented by this selection
  Standard_Integer Mode() const;

  //! Begins an iteration scanning for sensitive primitives.
  void Init();

  //! Continues the iteration scanning for sensitive
  //! primitives with the mode defined in this framework.
  Standard_Boolean More() const;

  //! Returns the next sensitive primitive found in the
  //! iteration. This is a scan for entities with the mode
  //! defined in this framework.
  void Next();

  //! Returns any sensitive primitive in this framework.
  const Handle(SelectMgr_SensitiveEntity)& Sensitive() const;

  //! Returns the flag UpdateFlag.
  //! This flage gives the update status of this framework
  //! in a ViewerSelector object:
  //! -   full
  //! -   partial, or
  //! -   none.
  SelectMgr_TypeOfUpdate UpdateStatus() const;

  void UpdateStatus (const SelectMgr_TypeOfUpdate theStatus);

  void UpdateBVHStatus (const SelectMgr_TypeOfBVHUpdate theStatus);

  SelectMgr_TypeOfBVHUpdate BVHUpdateStatus() const;

  //! Returns status of selection
  Standard_EXPORT const SelectMgr_StateOfSelection GetSelectionState() const;

  //! Sets status of selection
  Standard_EXPORT void SetSelectionState (const SelectMgr_StateOfSelection theState) const;

  //! Returns sensitivity of the selection
  Standard_EXPORT Standard_Integer Sensitivity() const;

  //! Changes sensitivity of the selection and all its entities to the given value.
  //! IMPORTANT: This method does not update any outer selection structures, so for
  //! proper updates use SelectMgr_SelectionManager::SetSelectionSensitivity method.
  Standard_EXPORT void SetSensitivity (const Standard_Integer theNewSens);

  DEFINE_STANDARD_RTTI (SelectMgr_Selection)

protected:

  //! Returns sensitive entity stored by index theIdx in entites vector
  Handle(SelectMgr_SensitiveEntity)& GetEntityById (const Standard_Integer theIdx);

private:

  NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>           myEntities;
  NCollection_Vector<Handle(SelectMgr_SensitiveEntity)>::Iterator myEntityIter;
  Standard_Integer                                                myMode;
  SelectMgr_TypeOfUpdate                                          myUpdateStatus;
  mutable SelectMgr_StateOfSelection                              mySelectionState;
  mutable SelectMgr_TypeOfBVHUpdate                               myBVHUpdateStatus;
  Standard_Integer                                                mySensFactor;
  Standard_Boolean                                                myIsCustomSens;
};

DEFINE_STANDARD_HANDLE(SelectMgr_Selection, MMgt_TShared)

#include <SelectMgr_Selection.lxx>

#endif
