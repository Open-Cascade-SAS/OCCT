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

#ifndef VInspector_CallBack_H
#define VInspector_CallBack_H

#include <AIS_InteractiveContext.hxx>
#include <Standard.hxx>
#include <inspector/VInspectorAPI_CallBack.hxx>
#include <inspector/VInspector_CallBackMode.hxx>
#include <inspector/VInspector_ViewModelHistory.hxx>
#include <inspector/VInspector_Tools.hxx>

class AIS_InteractiveObject;
class SelectMgr_EntityOwner;

DEFINE_STANDARD_HANDLE(VInspector_CallBack, VInspectorAPI_CallBack)

//! \class VInspector_CallBack
//! Updates inspector state on actions in context
class VInspector_CallBack : public VInspectorAPI_CallBack
{
public:

  //! Constructor
  VInspector_CallBack() : VInspectorAPI_CallBack() {}

  //! Destructor
  virtual ~VInspector_CallBack() Standard_OVERRIDE {}

  //! Appends displayed presentation into history model
  //! \param thePrs the presentation
  virtual void Display (Handle(AIS_InteractiveObject) thePrs) Standard_OVERRIDE
  { myHistoryModel->AddElement (VInspector_CallBackMode_Display, VInspector_Tools::GetInfo (thePrs)); }

  //! Appends redisplayed presentation into history model
  //! \param thePrs the presentation
  virtual void Redisplay (Handle(AIS_InteractiveObject) thePrs) Standard_OVERRIDE
  { myHistoryModel->AddElement (VInspector_CallBackMode_Redisplay, VInspector_Tools::GetInfo (thePrs)); }

  //! Appends removed presentation into history model
  //! \param thePrs the presentation
  virtual void Remove (Handle(AIS_InteractiveObject) thePrs) Standard_OVERRIDE
  { myHistoryModel->AddElement (VInspector_CallBackMode_Remove, VInspector_Tools::GetInfo (thePrs)); }

  //! Appends loaded presentation into history model
  //! \param thePrs the presentation
  virtual void Load (Handle(AIS_InteractiveObject) thePrs) Standard_OVERRIDE
  { myHistoryModel->AddElement (VInspector_CallBackMode_Load, VInspector_Tools::GetInfo (thePrs)); }

  //! Appends information about modes activation
  //! \param thePrs the presentation
  //! \param theModes the selection modes
  virtual void ActivatedModes (Handle(AIS_InteractiveObject) thePrs, TColStd_ListOfInteger& theModes) Standard_OVERRIDE
  { (void)thePrs; (void)theModes; }

  //! Appends information about mode activation
  //! \param thePrs the presentation
  //! \param theModes the selection mode
  Standard_EXPORT virtual void Activate (Handle(AIS_InteractiveObject) thePrs,
                                         const Standard_Integer theMode) Standard_OVERRIDE;

  //! Appends information about modes deactivation
  //! \param thePrs the presentation
  virtual void Deactivate (Handle(AIS_InteractiveObject) thePrs) Standard_OVERRIDE
  { myHistoryModel->AddElement (VInspector_CallBackMode_Deactivate, VInspector_Tools::GetInfo (thePrs)); }

  //! Appends information about modes deactivation
  //! \param thePrs the presentation
  //! \param theMode the selection mode
  virtual void Deactivate (Handle(AIS_InteractiveObject) thePrs, const Standard_Integer theMode) Standard_OVERRIDE
  { (void)theMode; myHistoryModel->AddElement (VInspector_CallBackMode_Deactivate, VInspector_Tools::GetInfo (thePrs)); }

  //! Appends information about selection change
  //! \param theShape the selected shape
  Standard_EXPORT virtual void AddOrRemoveSelected (const TopoDS_Shape& theShape) Standard_OVERRIDE;

  //! Appends information about selection change
  //! \param thePrs the selected presentation
  Standard_EXPORT virtual void AddOrRemoveSelected (Handle(AIS_InteractiveObject) thePrs) Standard_OVERRIDE;

  //! Appends information about selection change
  //! \param theOwner the selected owner
  Standard_EXPORT virtual void AddOrRemoveSelected (Handle(SelectMgr_EntityOwner) theOwner) Standard_OVERRIDE;

  //! Appends information about selection clear
  Standard_EXPORT virtual void ClearSelected() Standard_OVERRIDE;

  //! Appends information about selection clear
  //! \param thePrs the selected presentation
  virtual void ClearSelected (Handle(AIS_InteractiveObject) thePrs) Standard_OVERRIDE
  { myHistoryModel->AddElement (VInspector_CallBackMode_ClearSelected, VInspector_Tools::GetInfo (thePrs)); }

  //! Appends information about moving to point
  //! \param theXPix a pixels on horizontal
  //! \param theYPix a pixels on vertical
  Standard_EXPORT virtual void MoveTo (const Standard_Integer theXPix, const Standard_Integer theYPix) Standard_OVERRIDE;

  //! Appends information about setting selection
  //! \param thePrs the selected presentation
  virtual void SetSelected (Handle(AIS_InteractiveObject) thePrs) Standard_OVERRIDE
  { myHistoryModel->AddElement (VInspector_CallBackMode_SetSelected, VInspector_Tools::GetInfo (thePrs)); }

  //! Appends information about select
  Standard_EXPORT virtual void Select() Standard_OVERRIDE;

  //! Appends information about shift select
  Standard_EXPORT virtual void ShiftSelect() Standard_OVERRIDE;

  //! Sets history model, it will be filled by actions in context
  //! \param theHistoryModel the view model
  void SetHistoryModel (VInspector_ViewModelHistory* theHistoryModel) { myHistoryModel = theHistoryModel; }

  //! Sets context, where actions happen
  //! \param theContext a context instance
  void SetContext (const Handle(AIS_InteractiveContext)& theContext) { myContext = theContext; }
#if OCC_VERSION_HEX <= 0x060901
  DEFINE_STANDARD_RTTI(VInspector_CallBack)
#else
  DEFINE_STANDARD_RTTIEXT(VInspector_CallBack, VInspectorAPI_CallBack)
#endif

  //! Returns string information of call back mode
  //! \param theMode type of AIS action
  //! \return information text
  static QString GetInfo (const VInspector_CallBackMode& theMode);

private:
  VInspector_ViewModelHistory* myHistoryModel; //!< the current history model, fulfilled by context actions
  Handle(AIS_InteractiveContext) myContext; //!< the listened context
};

#endif 
