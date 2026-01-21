// Created on: 2004-11-22
// Created by: Pavel TELKOV
// Copyright (c) 2004-2014 OPEN CASCADE SAS
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

// The original implementation Copyright: (C) RINA S.p.A

#include <TObj_Assistant.hxx>

#include <TObj_Model.hxx>
#include <Standard_Transient.hxx>
#include <NCollection_Sequence.hxx>
#include <TCollection_AsciiString.hxx>
#include <NCollection_IndexedMap.hxx>
#include <Standard_Type.hxx>

//=================================================================================================

NCollection_Sequence<occ::handle<Standard_Transient>>& TObj_Assistant::getModels()
{
  static NCollection_Sequence<occ::handle<Standard_Transient>> sModels;
  return sModels;
}

//=================================================================================================

NCollection_IndexedMap<occ::handle<Standard_Transient>>& TObj_Assistant::getTypes()
{
  static NCollection_IndexedMap<occ::handle<Standard_Transient>> sTypes;
  return sTypes;
}

//=================================================================================================

occ::handle<TObj_Model>& TObj_Assistant::getCurrentModel()
{
  static occ::handle<TObj_Model> sCurrentModel;
  return sCurrentModel;
}

//=================================================================================================

int& TObj_Assistant::getVersion()
{
  static int sVersion = 0;
  return sVersion;
}

//=================================================================================================

occ::handle<TObj_Model> TObj_Assistant::FindModel(const char* const theName)
{
  TCollection_ExtendedString aName(theName, true);
  int                        i = getModels().Length();
  occ::handle<TObj_Model>    aModel;
  for (; i > 0; i--)
  {
    aModel = occ::down_cast<TObj_Model>(getModels().Value(i));
    if (aName == aModel->GetModelName()->String())
      break;
  }
  if (i == 0)
    aModel.Nullify();

  return aModel;
}

//=================================================================================================

void TObj_Assistant::BindModel(const occ::handle<TObj_Model>& theModel)
{
  getModels().Append(theModel);
}

//=================================================================================================

void TObj_Assistant::ClearModelMap()
{
  getModels().Clear();
}

//=================================================================================================

occ::handle<Standard_Type> TObj_Assistant::FindType(const int theTypeIndex)
{
  if (theTypeIndex > 0 && theTypeIndex <= getTypes().Extent())
    return occ::down_cast<Standard_Type>(getTypes().FindKey(theTypeIndex));

  return nullptr;
}

//=================================================================================================

int TObj_Assistant::FindTypeIndex(const occ::handle<Standard_Type>& theType)
{
  if (!getTypes().Contains(theType))
    return 0;

  return getTypes().FindIndex(theType);
}

//=================================================================================================

class TObj_Assistant_UnknownType : public Standard_Transient
{
public:
  TObj_Assistant_UnknownType() = default;
  // Empty constructor

  // CASCADE RTTI
  DEFINE_STANDARD_RTTI_INLINE(TObj_Assistant_UnknownType, Standard_Transient)
};

// Define handle class for TObj_Assistant_UnknownType
//=================================================================================================

int TObj_Assistant::BindType(const occ::handle<Standard_Type>& theType)
{
  if (theType.IsNull())
  {
    occ::handle<Standard_Transient> anUnknownType;
    anUnknownType = new TObj_Assistant_UnknownType;
    return getTypes().Add(anUnknownType);
  }

  return getTypes().Add(theType);
}

//=================================================================================================

void TObj_Assistant::ClearTypeMap()
{
  getTypes().Clear();
}

//=================================================================================================

void TObj_Assistant::SetCurrentModel(const occ::handle<TObj_Model>& theModel)
{
  getCurrentModel() = theModel;
  getVersion()      = -1;
}

//=================================================================================================

occ::handle<TObj_Model> TObj_Assistant::GetCurrentModel()
{
  return getCurrentModel();
}

//=================================================================================================

void TObj_Assistant::UnSetCurrentModel()
{
  getCurrentModel().Nullify();
}

//=================================================================================================

int TObj_Assistant::GetAppVersion()
{
  int& aVersion = getVersion();
  if (aVersion < 0)
  {
    occ::handle<TObj_Model>& aModel = getCurrentModel();
    if (!aModel.IsNull())
      aVersion = aModel->GetFormatVersion();
  }
  return aVersion;
}
