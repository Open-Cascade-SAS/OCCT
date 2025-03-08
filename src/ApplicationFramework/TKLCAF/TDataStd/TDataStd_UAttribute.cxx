// Created on: 1999-06-11
// Created by: Sergey RUIN
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

#include <TDataStd_UAttribute.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_UAttribute, TDF_Attribute)

//=================================================================================================

Handle(TDataStd_UAttribute) TDataStd_UAttribute::Set(const TDF_Label&     label,
                                                     const Standard_GUID& guid)
{
  Handle(TDataStd_UAttribute) A;
  if (!label.FindAttribute(guid, A))
  {
    A = new TDataStd_UAttribute();
    A->SetID(guid);
    label.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataStd_UAttribute::TDataStd_UAttribute() {}

//=================================================================================================

const Standard_GUID& TDataStd_UAttribute::ID() const
{
  return myID;
}

//=================================================================================================

void TDataStd_UAttribute::SetID(const Standard_GUID& guid)
{
  // OCC2932 correction
  if (myID == guid)
    return;

  Backup();
  myID = guid;
}

//=================================================================================================

Handle(TDF_Attribute) TDataStd_UAttribute::NewEmpty() const
{
  Handle(TDataStd_UAttribute) A = new TDataStd_UAttribute();
  A->SetID(myID);
  return A;
}

//=================================================================================================

void TDataStd_UAttribute::Restore(const Handle(TDF_Attribute)& with)
{
  Handle(TDataStd_UAttribute) A = Handle(TDataStd_UAttribute)::DownCast(with);
  SetID(A->ID());
}

//=================================================================================================

void TDataStd_UAttribute::Paste(const Handle(TDF_Attribute)& into,
                                const Handle(TDF_RelocationTable)& /*RT*/) const
{
  Handle(TDataStd_UAttribute) A = Handle(TDataStd_UAttribute)::DownCast(into);
  A->SetID(myID);
}

//=================================================================================================

void TDataStd_UAttribute::References(const Handle(TDF_DataSet)& /*DS*/) const {}

//=================================================================================================

Standard_OStream& TDataStd_UAttribute::Dump(Standard_OStream& anOS) const
{
  anOS << "UAttribute";
  TDF_Attribute::Dump(anOS);
  return anOS;
}

//=================================================================================================

void TDataStd_UAttribute::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_FIELD_VALUE_GUID(theOStream, myID)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)
}
