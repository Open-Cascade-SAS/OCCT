// Copyright (c) 2021 OPEN CASCADE SAS
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

#include <XCAFDoc_LengthUnit.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <UnitsMethods.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(XCAFDoc_LengthUnit, TDF_Attribute, "xcaf", "LengthUnit")

//=======================================================================
//function : XCAFDoc_LengthUnit
//purpose  :
//=======================================================================
XCAFDoc_LengthUnit::XCAFDoc_LengthUnit() : TDF_Attribute(), myUnitScaleValue(1.)
{}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================
Handle(XCAFDoc_LengthUnit) XCAFDoc_LengthUnit::Set(const TDF_Label& theLabel,
                                                     const TCollection_AsciiString& theUnitName,
                                                     const Standard_Real theUnitValue)
{
  return Set(theLabel, GetID(), theUnitName, theUnitValue);
}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================
Handle(XCAFDoc_LengthUnit) XCAFDoc_LengthUnit::Set(const TDF_Label& theLabel,
                                                     const Standard_Real theUnitValue)
{
  TCollection_AsciiString aUnitName = UnitsMethods::DumpLengthUnit(theUnitValue, UnitsMethods_LengthUnit_Meter);
  return Set(theLabel, GetID(), aUnitName, theUnitValue);
}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================
Handle(XCAFDoc_LengthUnit) XCAFDoc_LengthUnit::Set(const TDF_Label& theLabel,
                                                     const Standard_GUID& theGUID,
                                                     const TCollection_AsciiString& theUnitName,
                                                     const Standard_Real theUnitValue)
{
  Handle(XCAFDoc_LengthUnit) A;
  if (!theLabel.FindAttribute(theGUID, A)) {
    A = new XCAFDoc_LengthUnit();
    A->SetID(theGUID);
    theLabel.AddAttribute(A);
  }
  A->Set(theUnitName, theUnitValue);
  return A;
}

//=======================================================================
//function : Set
//purpose  :
//=======================================================================
void XCAFDoc_LengthUnit::Set(const TCollection_AsciiString& theUnitName,
                              const Standard_Real theUnitValue)
{
  Backup();
  myUnitName = theUnitName;
  myUnitScaleValue = theUnitValue;
}

//=======================================================================
//function : GetID
//purpose  :
//=======================================================================
const Standard_GUID& XCAFDoc_LengthUnit::GetID()
{
  static const Standard_GUID theGUID ("efd212f8-6dfd-11d4-b9c8-0060b0ee281b");
  return theGUID;
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================

const Standard_GUID& XCAFDoc_LengthUnit::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================

void XCAFDoc_LengthUnit::Restore(const Handle(TDF_Attribute)& theWith)
{
  Handle(XCAFDoc_LengthUnit) anAttr = Handle(XCAFDoc_LengthUnit)::DownCast(theWith);
  myUnitName = anAttr->GetUnitName();
  myUnitScaleValue = anAttr->GetUnitValue();
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void XCAFDoc_LengthUnit::Paste(const Handle(TDF_Attribute)& theInto,
  const Handle(TDF_RelocationTable)&  theRT ) const
{
  (void)theRT;
  Handle(XCAFDoc_LengthUnit) anAttr = Handle(XCAFDoc_LengthUnit)::DownCast(theInto);
  anAttr->Set(myUnitName, myUnitScaleValue);
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& XCAFDoc_LengthUnit::Dump(Standard_OStream& theOS) const
{
  Standard_OStream& anOS = TDF_Attribute::Dump(theOS);
  anOS << " UnitName=|" << myUnitName << "|";
  anOS << " UnitScaleValue=|" << myUnitScaleValue << "|";
  Standard_Character aSGUID[Standard_GUID_SIZE_ALLOC];
  ID().ToCString(aSGUID);
  anOS << aSGUID << "|" << std::endl;
  return anOS;
}

//=======================================================================
//function : DumpJson
//purpose  : 
//=======================================================================
void XCAFDoc_LengthUnit::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUES_STRING(theOStream, "UnitName", 1, &myUnitName)

  OCCT_DUMP_FIELD_VALUES_NUMERICAL(theOStream, "UnitScaleValue", 1, &myUnitScaleValue)
}
