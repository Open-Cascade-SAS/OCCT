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

#include <inspector/VInspector_CallBack.hxx>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>

#include <SelectMgr_EntityOwner.hxx>
#include <Standard_Version.hxx>

#include <inspector/VInspector_CallBackMode.hxx>
#include <inspector/VInspector_Tools.hxx>
#include <inspector/VInspector_ViewModelHistory.hxx>

#include <QStringList>

#if OCC_VERSION_HEX <= 0x060901
IMPLEMENT_STANDARD_HANDLE(VInspector_CallBack, Standard_Transient)
IMPLEMENT_STANDARD_RTTIEXT(VInspector_CallBack, VInspectorAPI_CallBack)
#else
IMPLEMENT_STANDARD_RTTIEXT(VInspector_CallBack, VInspectorAPI_CallBack)
#endif

// =======================================================================
// function : Activate
// purpose :
// =======================================================================
void VInspector_CallBack::Activate (Handle(AIS_InteractiveObject) thePrs, const Standard_Integer theMode)
{
  QList<QVariant> anInfo = VInspector_Tools::GetInfo (thePrs);
  anInfo[0] = QString ("%1: %2").arg (anInfo[0].toString()).arg (theMode);
  myHistoryModel->AddElement (VInspector_CallBackMode_Activate, anInfo);
}

// =======================================================================
// function : AddOrRemoveSelected
// purpose :
// =======================================================================
void VInspector_CallBack::AddOrRemoveSelected (const TopoDS_Shape& theShape)
{
  QList<QVariant> aValues;
  aValues.append (""); // Name
  aValues.append (""); // Pointer
  aValues.append (VInspector_Tools::GetShapeTypeInfo (theShape.ShapeType()).ToCString()); // Shape Type
  aValues.append (VInspector_Tools::GetSelectedInfoPointers (myContext)); // SelectionInfo
  myHistoryModel->AddElement (VInspector_CallBackMode_AddOrRemoveSelectedShape, aValues);
}

// =======================================================================
// function : AddOrRemoveSelected
// purpose :
// =======================================================================
void VInspector_CallBack::AddOrRemoveSelected (Handle(AIS_InteractiveObject) thePrs)
{
  QList<QVariant> aValues = VInspector_Tools::GetInfo (thePrs);
  aValues.append (VInspector_Tools::GetSelectedInfoPointers (myContext)); // SelectionInfo
  myHistoryModel->AddElement (VInspector_CallBackMode_AddOrRemoveSelected, aValues);
}

// =======================================================================
// function : AddOrRemoveSelected
// purpose :
// =======================================================================
void VInspector_CallBack::AddOrRemoveSelected (Handle(SelectMgr_EntityOwner) theOwner)
{
  QList<QVariant> aValues;
  aValues.append (""); // Name
  aValues.append (VInspector_Tools::GetPointerInfo (theOwner, true).ToCString()); // Pointer
  aValues.append (""); // Shape type
  aValues.append (VInspector_Tools::GetSelectedInfoPointers (myContext)); // SelectionInfo

  myHistoryModel->AddElement (VInspector_CallBackMode_AddOrRemoveSelectedOwner, aValues);
}

// =======================================================================
// function : ClearSelected
// purpose :
// =======================================================================
void VInspector_CallBack::ClearSelected()
{
  QList<QVariant> aValues;
  myHistoryModel->AddElement (VInspector_CallBackMode_ClearSelected, aValues);
}

// =======================================================================
// function : MoveTo
// purpose :
// =======================================================================
void VInspector_CallBack::MoveTo (const Standard_Integer/* theXPix*/, const Standard_Integer/* theYPix*/)
{
  QList<QVariant> aValues;
  aValues = VInspector_Tools::GetHighlightInfo (myContext);
  myHistoryModel->AddElement (VInspector_CallBackMode_MoveTo, aValues);
}

// =======================================================================
// function : Select
// purpose :
// =======================================================================
void VInspector_CallBack::Select()
{
  QList<QVariant> aValues;
  aValues = VInspector_Tools::GetSelectedInfo (myContext);
  myHistoryModel->AddElement (VInspector_CallBackMode_Select, aValues);
}

// =======================================================================
// function : ShiftSelect
// purpose :
// =======================================================================
void VInspector_CallBack::ShiftSelect()
{
  QList<QVariant> aValues;
  aValues = VInspector_Tools::GetSelectedInfo (myContext);
  myHistoryModel->AddElement (VInspector_CallBackMode_ShiftSelect, aValues);
}

// =======================================================================
// function : GetInfo
// purpose :
// =======================================================================
QString VInspector_CallBack::GetInfo (const VInspector_CallBackMode& theMode)
{
  switch (theMode)
  {
    case VInspector_CallBackMode_None:                     return "None";
    case VInspector_CallBackMode_Display:                  return "Display";
    case VInspector_CallBackMode_Redisplay:                return "Redisplay";
    case VInspector_CallBackMode_Remove:                   return "Remove";
    case VInspector_CallBackMode_Load:                     return "Load";
    case VInspector_CallBackMode_ActivatedModes:           return "ActivatedModes";
    case VInspector_CallBackMode_Activate:                 return "Activate";
    case VInspector_CallBackMode_Deactivate:               return "Deactivate";
    case VInspector_CallBackMode_AddOrRemoveSelectedShape: return "AddOrRemoveSelectedShape";
    case VInspector_CallBackMode_AddOrRemoveSelected:      return "AddOrRemoveSelected";
    case VInspector_CallBackMode_AddOrRemoveSelectedOwner: return "AddOrRemoveSelectedOwner";
    case VInspector_CallBackMode_ClearSelected:            return "ClearSelected";
    case VInspector_CallBackMode_MoveTo:                   return "MoveTo";
    case VInspector_CallBackMode_SetSelected:              return "SetSelected";
    case VInspector_CallBackMode_Select:                   return "Select";
    case VInspector_CallBackMode_ShiftSelect:              return "ShiftSelect";
    default: break;
  }
  return QString();
}
