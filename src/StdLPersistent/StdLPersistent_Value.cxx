// Copyright (c) 2015 OPEN CASCADE SAS
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

#include <StdLPersistent_Value.hxx>

#include <TCollection_HExtendedString.hxx>
#include <Standard_GUID.hxx>


//=======================================================================
//function : ImportAttribute
//purpose  : Import transient attribuite from the persistent data
//=======================================================================
template <class AttribClass>
void StdLPersistent_Value::integer<AttribClass>::ImportAttribute()
{
  this->myTransient->Set (this->myData);
}

//=======================================================================
//function : ImportAttribute
//purpose  : Import transient attribuite from the persistent data
//=======================================================================
template <class AttribClass, class HStringClass>
void StdLPersistent_Value::string<AttribClass, HStringClass>::ImportAttribute()
{
  Handle(HStringClass) anHString =
    Handle(HStringClass)::DownCast (this->myData);
  if (anHString)
  {
    this->myTransient->Set (anHString->Value()->String());
    this->myData.Nullify();
  }
}

//=======================================================================
//function : ImportAttribute
//purpose  : Import transient attribuite from the persistent data
//=======================================================================
template<>
void StdLPersistent_Value::string<TDF_Reference>::ImportAttribute()
{
  if (this->myData)
  {
    TDF_Label aLabel = myData->Label (this->myTransient->Label().Data());
    if (!aLabel.IsNull())
      this->myTransient->Set (aLabel);
    this->myData.Nullify();
  }
}

//=======================================================================
//function : ImportAttribute
//purpose  : Import transient attribuite from the persistent data
//=======================================================================
template<>
void StdLPersistent_Value::string<TDataStd_UAttribute>::ImportAttribute()
{}

//=======================================================================
//function : CreateAttribute
//purpose  : Create an empty transient attribuite
//=======================================================================
Handle(TDF_Attribute) StdLPersistent_Value::UAttribute::CreateAttribute()
{
  string<TDataStd_UAttribute,StdLPersistent_HString::Extended>::CreateAttribute();

  if (this->myData)
  {
    Handle(TCollection_HExtendedString) aString = this->myData->ExtString();
    if (aString)
      this->myTransient->SetID (Standard_GUID (aString->String().ToExtString()));
    this->myData.Nullify();
  }

  return this->myTransient;
}

//=======================================================================
Handle(TDF_Attribute) StdLPersistent_Value::Integer::CreateAttribute()
{
  integer<TDataStd_Integer>::CreateAttribute();

  if (this->myData)
  {
    this->myTransient->SetID (TDataStd_Integer::GetID());
  }

  return this->myTransient;
}

//=======================================================================
Handle(TDF_Attribute) StdLPersistent_Value::Name::CreateAttribute()
{
  string<TDataStd_Name>::CreateAttribute();

  if (this->myData)
  {
    this->myTransient->SetID (TDataStd_Name::GetID());
  }

  return this->myTransient;
}

//=======================================================================
Handle(TDF_Attribute) StdLPersistent_Value::AsciiString::CreateAttribute()
{
  string<TDataStd_AsciiString, StdLPersistent_HString::Ascii>::CreateAttribute();

  if (this->myData)
  {
    this->myTransient->SetID (TDataStd_AsciiString::GetID());
  }

  return this->myTransient;
}

template class StdLPersistent_Value::integer <TDataStd_Integer>;
template class StdLPersistent_Value::integer <TDF_TagSource>;

template class StdLPersistent_Value::string <TDF_Reference>;
template class StdLPersistent_Value::string <TDataStd_UAttribute>;
template class StdLPersistent_Value::string <TDataStd_Name>;
template class StdLPersistent_Value::string <TDataStd_Comment>;
template class StdLPersistent_Value::string <TDataStd_AsciiString,
                                             StdLPersistent_HString::Ascii>;
