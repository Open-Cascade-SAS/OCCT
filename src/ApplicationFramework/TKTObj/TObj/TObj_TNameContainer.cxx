// Created on: 2004-11-23
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

#include <TObj_TNameContainer.hxx>
#include <Standard_GUID.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TObj_TNameContainer, TDF_Attribute)

//=================================================================================================

TObj_TNameContainer::TObj_TNameContainer() {}

//=================================================================================================

const Standard_GUID& TObj_TNameContainer::GetID()
{
  static Standard_GUID theID("3bbefb47-e618-11d4-ba38-0060b0ee18ea");
  return theID;
}

//=================================================================================================

const Standard_GUID& TObj_TNameContainer::ID() const
{
  return GetID();
}

//=================================================================================================

occ::handle<TObj_TNameContainer> TObj_TNameContainer::Set(const TDF_Label& theLabel)
{
  occ::handle<TObj_TNameContainer> A;
  if (!theLabel.FindAttribute(TObj_TNameContainer::GetID(), A))
  {
    A = new TObj_TNameContainer;
    theLabel.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

void TObj_TNameContainer::RecordName(const occ::handle<TCollection_HExtendedString>& theName,
                                     const TDF_Label&                                theLabel)
{
  if (!IsRegistered(theName))
  {
    Backup();
    myMap.Bind(new TCollection_HExtendedString(theName->String()), theLabel);
  }
}

//=================================================================================================

void TObj_TNameContainer::RemoveName(const occ::handle<TCollection_HExtendedString>& theName)
{
  if (IsRegistered(theName))
  {
    Backup();
    myMap.UnBind(theName);
  }
}

void TObj_TNameContainer::Clear()
{
  Backup();
  myMap.Clear();
}

//=================================================================================================

bool TObj_TNameContainer::IsRegistered(
  const occ::handle<TCollection_HExtendedString>& theName) const
{
  return myMap.IsBound(theName);
}

//=================================================================================================

occ::handle<TDF_Attribute> TObj_TNameContainer::NewEmpty() const
{
  return new TObj_TNameContainer();
}

//=================================================================================================

void TObj_TNameContainer::Restore(const occ::handle<TDF_Attribute>& theWith)
{
  occ::handle<TObj_TNameContainer> R = occ::down_cast<TObj_TNameContainer>(theWith);

  myMap = R->Get();
}

//=================================================================================================

void TObj_TNameContainer::Paste(const occ::handle<TDF_Attribute>& theInto,
                                const occ::handle<TDF_RelocationTable>& /* RT */) const
{
  occ::handle<TObj_TNameContainer> R = occ::down_cast<TObj_TNameContainer>(theInto);
  R->Set(myMap);
}

//=================================================================================================

const NCollection_DataMap<occ::handle<TCollection_HExtendedString>, TDF_Label>&
  TObj_TNameContainer::Get() const
{
  return myMap;
}

//=================================================================================================

void TObj_TNameContainer::Set(
  const NCollection_DataMap<occ::handle<TCollection_HExtendedString>, TDF_Label>& theMap)
{
  Backup();
  myMap.Assign(theMap);
}
