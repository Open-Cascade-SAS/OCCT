// Created on: 1997-12-10
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

#include <TDataStd_Relation.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Variable.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_ListIteratorOfAttributeList.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(TDataStd_Relation, TDataStd_Expression)

//=================================================================================================

const Standard_GUID& TDataStd_Relation::GetID()
{
  static Standard_GUID TDataStd_RelationID("ce24146b-8e57-11d1-8953-080009dc4425");
  return TDataStd_RelationID;
}

//=================================================================================================

Handle(TDataStd_Relation) TDataStd_Relation::Set(const TDF_Label& L)
{
  Handle(TDataStd_Relation) A;
  if (!L.FindAttribute(TDataStd_Relation::GetID(), A))
  {
    A = new TDataStd_Relation();
    L.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataStd_Relation::TDataStd_Relation() {}

//=================================================================================================

void TDataStd_Relation::SetRelation(const TCollection_ExtendedString& R)
{
  SetExpression(R);
}

//=================================================================================================

const TCollection_ExtendedString& TDataStd_Relation::GetRelation() const
{
  return GetExpression();
}

//=================================================================================================

const Standard_GUID& TDataStd_Relation::ID() const
{
  return GetID();
}

//=================================================================================================

Standard_OStream& TDataStd_Relation::Dump(Standard_OStream& anOS) const
{
  anOS << "Relation";
  return anOS;
}

//=================================================================================================

void TDataStd_Relation::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUE_STRING(theOStream, GetRelation())

  for (TDF_AttributeList::Iterator aVariableIt(myVariables); aVariableIt.More(); aVariableIt.Next())
  {
    const Handle(TDF_Attribute)& aVariable = aVariableIt.Value();
    OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, aVariable.get())
  }
}