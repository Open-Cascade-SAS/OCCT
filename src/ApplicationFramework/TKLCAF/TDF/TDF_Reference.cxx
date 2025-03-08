// Created on: 2000-03-01
// Created by: Denis PASCAL
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <TDF_Reference.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_Tool.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDF_Reference, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDF_Reference::GetID()
{
  static Standard_GUID TDF_ReferenceID("2a96b610-ec8b-11d0-bee7-080009dc3333");
  return TDF_ReferenceID;
}

//=================================================================================================

Handle(TDF_Reference) TDF_Reference::Set(const TDF_Label& L, const TDF_Label& Origin)
{
  Handle(TDF_Reference) A;
  if (!L.FindAttribute(TDF_Reference::GetID(), A))
  {
    A = new TDF_Reference();
    L.AddAttribute(A);
  }
  A->Set(Origin);
  return A;
}

//=======================================================================
// function : TDF_Reference
// purpose  : Empty Constructor
//=======================================================================

TDF_Reference::TDF_Reference() {}

//=================================================================================================

void TDF_Reference::Set(const TDF_Label& Origin)
{
  // OCC2932 correction
  if (myOrigin == Origin)
    return;

  Backup();
  myOrigin = Origin;
}

//=================================================================================================

TDF_Label TDF_Reference::Get() const
{
  return myOrigin;
}

//=================================================================================================

const Standard_GUID& TDF_Reference::ID() const
{
  return GetID();
}

//=================================================================================================

Handle(TDF_Attribute) TDF_Reference::NewEmpty() const
{
  return new TDF_Reference();
}

//=================================================================================================

void TDF_Reference::Restore(const Handle(TDF_Attribute)& With)
{
  myOrigin = Handle(TDF_Reference)::DownCast(With)->Get();
}

//=================================================================================================

void TDF_Reference::Paste(const Handle(TDF_Attribute)&       Into,
                          const Handle(TDF_RelocationTable)& RT) const
{
  TDF_Label tLab;
  if (!myOrigin.IsNull())
  {
    if (!RT->HasRelocation(myOrigin, tLab))
      tLab = myOrigin;
  }
  Handle(TDF_Reference)::DownCast(Into)->Set(tLab);
}

//=======================================================================
// function : References
// purpose  : Adds the referenced attributes or labels.
//=======================================================================

void TDF_Reference::References(const Handle(TDF_DataSet)& aDataSet) const
{
  // clang-format off
  if (!Label().IsImported()) aDataSet->AddLabel( myOrigin); //pour real et entier mais surtout pas les parts ...
  // clang-format on
}

//=================================================================================================

Standard_OStream& TDF_Reference::Dump(Standard_OStream& anOS) const
{
  anOS << "Reference";
  return anOS;
}

//=================================================================================================

void TDF_Reference::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  TCollection_AsciiString aLabel;
  TDF_Tool::Entry(myOrigin, aLabel);
  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, aLabel)
}
