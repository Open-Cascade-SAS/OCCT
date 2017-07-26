// Created on: 2017-06-16
// Created by: Natalia ERMOLAEVA
// Copyright (c) 2017 OPEN CASCADE SAS
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

#ifndef VInspectorAPI_CallBack_H
#define VInspectorAPI_CallBack_H

#include <Standard.hxx>
#include <Standard_Integer.hxx>
#include <Standard_Version.hxx>
#include <Standard_Transient.hxx>
#include <TColStd_ListOfInteger.hxx>
#include <TopoDS_Shape.hxx>

#if OCC_VERSION_HEX > 0x060901
class AIS_InteractiveObject;
#else
#include <AIS_InteractiveObject.hxx>
#endif
class SelectMgr_EntityOwner;

DEFINE_STANDARD_HANDLE(VInspectorAPI_CallBack, Standard_Transient)

//! \class VInspectorAPI_CallBack
//! API of AIS_InteractiveContext methods to provide information about the actions
class VInspectorAPI_CallBack : public Standard_Transient
{

public:

  //! Constructor
  Standard_EXPORT VInspectorAPI_CallBack();

  //! Destructor
  virtual ~VInspectorAPI_CallBack() {}

  //! Processing method of context
  virtual void Display(Handle(AIS_InteractiveObject) thePrs) = 0;

  //! Processing method of context
  virtual void Redisplay(Handle(AIS_InteractiveObject) thePrs) = 0;

  //! Processing method of context
  virtual void Remove(Handle(AIS_InteractiveObject) thePrs) = 0;

  //! Processing method of context
  virtual void Load(Handle(AIS_InteractiveObject) thePrs) = 0;

  //! Processing method of context
  virtual void ActivatedModes (Handle(AIS_InteractiveObject) thePrs, TColStd_ListOfInteger& theList) = 0;

  //! Processing method of context
  virtual void Activate(Handle(AIS_InteractiveObject) thePrs, const Standard_Integer theMode) = 0;

  //! Processing method of context
  virtual void Deactivate(Handle(AIS_InteractiveObject) thePrs) = 0;

  //! Processing method of context
  virtual void Deactivate(Handle(AIS_InteractiveObject) thePrs, const Standard_Integer theMode) = 0;

  //! Processing method of context
  virtual void AddOrRemoveSelected (const TopoDS_Shape& theShape) = 0;

  //! Processing method of context
  virtual void AddOrRemoveSelected (Handle(AIS_InteractiveObject) thePrs) = 0;

  //! Processing method of context
  virtual void AddOrRemoveSelected (Handle(SelectMgr_EntityOwner) theOwner) = 0;

  //! Processing method of context
  virtual void ClearSelected() = 0;

  //! Processing method of context
  virtual void ClearSelected(Handle(AIS_InteractiveObject) thePrs) = 0;

  //! Processing method of context
  virtual void MoveTo (const Standard_Integer theXPix, const Standard_Integer theYPix) = 0;

  //! Processing method of context
  virtual void SetSelected(Handle(AIS_InteractiveObject) thePrs) = 0;

  //! Processing method of context
  virtual void Select() = 0;

  //! Processing method of context
  virtual void ShiftSelect() = 0;

#if OCC_VERSION_HEX <= 0x060901
  DEFINE_STANDARD_RTTI(VInspectorAPI_CallBack)
#else
  DEFINE_STANDARD_RTTIEXT(VInspectorAPI_CallBack, Standard_Transient)
#endif
};

#endif
