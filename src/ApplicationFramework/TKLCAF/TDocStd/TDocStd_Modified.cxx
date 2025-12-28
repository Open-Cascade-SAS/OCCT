// Created on: 1999-07-12
// Created by: Denis PASCAL
// Copyright (c) 1999-1999 Matra Datavision
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

#include <TDocStd_Modified.hxx>

#include <Standard_DomainError.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDocStd_Modified, TDF_Attribute)

//=================================================================================================

bool TDocStd_Modified::IsEmpty(const TDF_Label& access)
{
  occ::handle<TDocStd_Modified> MDF;
  if (!access.Root().FindAttribute(TDocStd_Modified::GetID(), MDF))
  {
    return true;
  }
  else
    return MDF->IsEmpty();
}

//=================================================================================================

bool TDocStd_Modified::Add(const TDF_Label& alabel)
{
  occ::handle<TDocStd_Modified> MDF;
  if (!alabel.Root().FindAttribute(TDocStd_Modified::GetID(), MDF))
  {
    MDF = new TDocStd_Modified();
    alabel.Root().AddAttribute(MDF);
  }
  return MDF->AddLabel(alabel);
}

//=================================================================================================

bool TDocStd_Modified::Remove(const TDF_Label& alabel)
{
  occ::handle<TDocStd_Modified> MDF;
  if (!alabel.Root().FindAttribute(TDocStd_Modified::GetID(), MDF))
  {
    return true;
  }
  else
    return MDF->RemoveLabel(alabel);
}

//=================================================================================================

bool TDocStd_Modified::Contains(const TDF_Label& alabel)
{
  occ::handle<TDocStd_Modified> MDF;
  if (!alabel.Root().FindAttribute(TDocStd_Modified::GetID(), MDF))
  {
    return false;
  }
  return (MDF->Get().Contains(alabel));
}

//=================================================================================================

const NCollection_Map<TDF_Label>& TDocStd_Modified::Get(const TDF_Label& access)
{
  occ::handle<TDocStd_Modified> MDF;
  if (!access.Root().FindAttribute(TDocStd_Modified::GetID(), MDF))
  {
    throw Standard_DomainError("TDocStd_Modified::Get : IsEmpty");
  }
  return MDF->Get();
}

//=================================================================================================

void TDocStd_Modified::Clear(const TDF_Label& access)
{
  occ::handle<TDocStd_Modified> MDF;
  if (!access.Root().FindAttribute(TDocStd_Modified::GetID(), MDF))
  {
    return;
  }
  else
    MDF->Clear();
}

//=================================================================================================

const Standard_GUID& TDocStd_Modified::GetID()
{
  static Standard_GUID TDocStd_ModifiedID("2a96b622-ec8b-11d0-bee7-080009dc3333");
  return TDocStd_ModifiedID;
}

//=================================================================================================

TDocStd_Modified::TDocStd_Modified() {}

//=================================================================================================

bool TDocStd_Modified::AddLabel(const TDF_Label& L)
{
  Backup();
  return myModified.Add(L);
}

//=================================================================================================

bool TDocStd_Modified::RemoveLabel(const TDF_Label& L)
{
  Backup();
  return myModified.Remove(L);
}

//=================================================================================================

bool TDocStd_Modified::IsEmpty() const
{
  return myModified.IsEmpty();
}

//=================================================================================================

void TDocStd_Modified::Clear()
{
  Backup();
  myModified.Clear();
}

//=================================================================================================

const NCollection_Map<TDF_Label>& TDocStd_Modified::Get() const
{
  return myModified;
}

//=================================================================================================

const Standard_GUID& TDocStd_Modified::ID() const
{
  return GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDocStd_Modified::NewEmpty() const
{
  return new TDocStd_Modified();
}

//=================================================================================================

void TDocStd_Modified::Restore(const occ::handle<TDF_Attribute>& With)
{
  occ::handle<TDocStd_Modified> MDF = occ::down_cast<TDocStd_Modified>(With);
  myModified                   = MDF->myModified;
}

//=================================================================================================

void TDocStd_Modified::Paste(const occ::handle<TDF_Attribute>&, const occ::handle<TDF_RelocationTable>&) const
{
}

//=================================================================================================

Standard_OStream& TDocStd_Modified::Dump(Standard_OStream& anOS) const
{
  anOS << "Modified";
  return anOS;
}
