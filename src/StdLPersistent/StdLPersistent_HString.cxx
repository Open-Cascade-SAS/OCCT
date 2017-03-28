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

#include <StdLPersistent_HString.hxx>
#include <StdObjMgt_ReadData.hxx>
#include <StdObjMgt_WriteData.hxx>
#include <TDF_Label.hxx>
#include <TDF_Tool.hxx>


//=======================================================================
//function : Read
//purpose  : Read persistent data from a file
//=======================================================================
template <class StringClass, typename CharType>
void StdLPersistent_HString::instance<StringClass, CharType>::Read
  (StdObjMgt_ReadData& theReadData)
{
  StdObjMgt_ReadData::Object anObjectData (theReadData);

  Standard_Integer aSize;
  anObjectData >> aSize;
  myValue = new StringClass (aSize, 0);

  for (Standard_Integer i = 1; i <= aSize; i++)
  {
    CharType aChar;
    anObjectData >> aChar;
    myValue->SetValue (i, aChar);
  }
}

//=======================================================================
//function : Write
//purpose  : Write persistent data to a file
//=======================================================================
template <class StringClass, typename CharType>
void StdLPersistent_HString::instance<StringClass, CharType>::Write
  (StdObjMgt_WriteData& theWriteData) const
{
  StdObjMgt_WriteData::Object anObjectData(theWriteData);

  Standard_Integer aSize = myValue->Length();
  anObjectData << aSize;

  for (Standard_Integer i = 1; i <= aSize; i++)
  {
    CharType aChar (0);
    anObjectData << aChar;
  }
}

//=======================================================================
//function : Label
//purpose  : Get/create a label defined by referenced string
//=======================================================================
template <class StringClass, typename CharType>
TDF_Label StdLPersistent_HString::instance<StringClass, CharType>::Label
  (const Handle(TDF_Data)& theDF) const
{
  TDF_Label aLabel;

  if (!myValue.IsNull())
    TDF_Tool::Label (theDF, myValue->String(), aLabel, Standard_True);

  return aLabel;
}

//=======================================================================
//function : AsciiString
//purpose  : Get referenced ASCII string
//=======================================================================
Handle(TCollection_HAsciiString)
  StdLPersistent_HString::Ascii::AsciiString() const
    { return myValue; }

//=======================================================================
//function : ExtString
//purpose  : Get referenced extended string
//=======================================================================
Handle(TCollection_HExtendedString)
  StdLPersistent_HString::Extended::ExtString() const
    { return myValue; }


template class StdLPersistent_HString::instance
  <TCollection_HAsciiString, Standard_Character>;

template class StdLPersistent_HString::instance
  <TCollection_HExtendedString, Standard_ExtCharacter>;
