// Created on: 1999-08-02
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

#include <TDataStd_Current.hxx>

#include <Standard_DomainError.hxx>
#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Data.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_Current, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_Current::GetID()
{
  static Standard_GUID TDataStd_CurrentID("2a96b623-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_CurrentID;
}

//=================================================================================================

void TDataStd_Current::Set(const TDF_Label& current)
{
  Handle(TDataStd_Current) A;
  Handle(TDF_Data)         D = current.Data();
  if (!D->Root().FindAttribute(TDataStd_Current::GetID(), A))
  {
    A = new TDataStd_Current();
    D->Root().AddAttribute(A);
  }
  A->SetLabel(current);
}

//=================================================================================================

TDF_Label TDataStd_Current::Get(const TDF_Label& access)
{
  //  TDF_Label current;
  Handle(TDataStd_Current) A;
  if (!access.Data()->Root().FindAttribute(TDataStd_Current::GetID(), A))
  {
    throw Standard_DomainError("TDataStd_Current::Get : not set");
  }
  return A->GetLabel();
}

//=================================================================================================

Standard_Boolean TDataStd_Current::Has(const TDF_Label& access)
{
  return (access.Data()->Root().IsAttribute(TDataStd_Current::GetID()));
}

//=================================================================================================

TDataStd_Current::TDataStd_Current() {}

//=================================================================================================

void TDataStd_Current::SetLabel(const TDF_Label& current)
{
  // OCC2932 correction
  if (myLabel == current)
    return;

  Backup();
  myLabel = current;
}

//=================================================================================================

TDF_Label TDataStd_Current::GetLabel() const
{
  return myLabel;
}

//=================================================================================================

const Standard_GUID& TDataStd_Current::ID() const
{
  return GetID();
}

//=================================================================================================

Handle(TDF_Attribute) TDataStd_Current::NewEmpty() const
{
  return new TDataStd_Current();
}

//=================================================================================================

void TDataStd_Current::Restore(const Handle(TDF_Attribute)& With)
{
  myLabel = Handle(TDataStd_Current)::DownCast(With)->GetLabel();
}

//=================================================================================================

void TDataStd_Current::Paste(const Handle(TDF_Attribute)&       Into,
                             const Handle(TDF_RelocationTable)& RT) const
{
  TDF_Label tLab;
  if (!myLabel.IsNull())
  {
    if (!RT->HasRelocation(myLabel, tLab))
      tLab = myLabel;
  }
  Handle(TDataStd_Current)::DownCast(Into)->SetLabel(tLab);
}

//=================================================================================================

Standard_OStream& TDataStd_Current::Dump(Standard_OStream& anOS) const
{
  anOS << "Current";
  return anOS;
}

//=================================================================================================

void TDataStd_Current::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  TCollection_AsciiString aLabel;
  TDF_Tool::Entry(myLabel, aLabel);
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aLabel)
}
