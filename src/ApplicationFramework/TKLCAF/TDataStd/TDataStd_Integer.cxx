// Created on: 1997-03-06
// Created by: Denis PASCAL
// Copyright (c) 1997-1999 Matra Datavision
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

#include <TDataStd_Integer.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_Reference.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_Integer, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDataStd_Integer::GetID()
{
  static Standard_GUID TDataStd_IntegerID("2a96b606-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_IntegerID;
}

//=======================================================================
// function : SetAttr
// purpose  : Implements Set functionality
//=======================================================================
static occ::handle<TDataStd_Integer> SetAttr(const TDF_Label&       label,
                                        const int V,
                                        const Standard_GUID&   theGuid)
{
  occ::handle<TDataStd_Integer> A;
  if (!label.FindAttribute(theGuid, A))
  {
    A = new TDataStd_Integer();
    A->SetID(theGuid);
    label.AddAttribute(A);
  }
  A->Set(V);
  return A;
}

//=================================================================================================

occ::handle<TDataStd_Integer> TDataStd_Integer::Set(const TDF_Label& L, const int V)

{
  return SetAttr(L, V, GetID());
}

//=======================================================================
// function : Set
// purpose  : Set user defined attribute
//=======================================================================

occ::handle<TDataStd_Integer> TDataStd_Integer::Set(const TDF_Label&       L,
                                               const Standard_GUID&   theGuid,
                                               const int V)
{
  return SetAttr(L, V, theGuid);
}

//=================================================================================================

TDataStd_Integer::TDataStd_Integer()
    : myValue(-1),
      myID(GetID())
{
}

//=================================================================================================

void TDataStd_Integer::Set(const int v)
{
  // OCC2932 correction
  if (myValue == v)
    return;

  Backup();
  myValue = v;
}

//=================================================================================================

int TDataStd_Integer::Get() const
{
  return myValue;
}

//=================================================================================================

bool TDataStd_Integer::IsCaptured() const
{
  occ::handle<TDF_Reference> R;
  return (Label().FindAttribute(TDF_Reference::GetID(), R));
}

//=================================================================================================

const Standard_GUID& TDataStd_Integer::ID() const
{
  return myID;
}

//=================================================================================================

void TDataStd_Integer::SetID(const Standard_GUID& theGuid)
{
  if (myID == theGuid)
    return;

  Backup();
  myID = theGuid;
}

//=================================================================================================

void TDataStd_Integer::SetID()
{
  Backup();
  myID = GetID();
}

//=================================================================================================

occ::handle<TDF_Attribute> TDataStd_Integer::NewEmpty() const
{
  return new TDataStd_Integer();
}

//=================================================================================================

void TDataStd_Integer::Restore(const occ::handle<TDF_Attribute>& With)
{
  occ::handle<TDataStd_Integer> anInt = occ::down_cast<TDataStd_Integer>(With);
  myValue                        = anInt->Get();
  myID                           = anInt->ID();
}

//=================================================================================================

void TDataStd_Integer::Paste(const occ::handle<TDF_Attribute>& Into,
                             const occ::handle<TDF_RelocationTable>& /*RT*/) const
{
  occ::handle<TDataStd_Integer> anInt = occ::down_cast<TDataStd_Integer>(Into);
  anInt->Set(myValue);
  anInt->SetID(myID);
}

//=================================================================================================

Standard_OStream& TDataStd_Integer::Dump(Standard_OStream& anOS) const
{
  anOS << "Integer:: " << this << " : ";
  anOS << myValue;
  char sguid[Standard_GUID_SIZE_ALLOC];
  myID.ToCString(sguid);
  anOS << sguid;
  //
  anOS << "\nAttribute fields: ";
  TDF_Attribute::Dump(anOS);
  return anOS;
}

//=================================================================================================

void TDataStd_Integer::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myValue)
}
