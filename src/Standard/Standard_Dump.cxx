// Copyright (c) 2019 OPEN CASCADE SAS
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

#include <Standard_Dump.hxx>

#include <stdarg.h>

// =======================================================================
// function : Constructor
// purpose :
// =======================================================================
Standard_DumpSentry::Standard_DumpSentry (Standard_OStream& theOStream, const char* theClassName)
: myOStream (&theOStream)
{
  (*myOStream) << "\"" << theClassName << "\": {";
}

// =======================================================================
// function : Destructor
// purpose :
// =======================================================================
Standard_DumpSentry::~Standard_DumpSentry()
{
  (*myOStream) << "}";
}

// =======================================================================
// function : AddValuesSeparator
// purpose :
// =======================================================================
void Standard_Dump::AddValuesSeparator (Standard_OStream& theOStream)
{
  Standard_SStream aStream;
  aStream << theOStream.rdbuf();
  TCollection_AsciiString aStreamStr = Standard_Dump::Text (aStream);
  if (!aStreamStr.EndsWith ("{"))
    theOStream << ", ";
}

//=======================================================================
//function : DumpKeyToClass
//purpose  : 
//=======================================================================
void Standard_Dump::DumpKeyToClass (Standard_OStream& theOStream,
                                    const char* theKey,
                                    const TCollection_AsciiString& theField)
{
  AddValuesSeparator (theOStream);
  theOStream << "\"" << theKey << "\": {" << theField << "}";
}

//=======================================================================
//function : DumpCharacterValues
//purpose  : 
//=======================================================================
void Standard_Dump::DumpCharacterValues (Standard_OStream& theOStream, int theCount, ...)
{
  va_list  vl;
  va_start(vl, theCount);
  for(int i = 0; i < theCount; ++i)
  {
    if (i > 0)
      theOStream << ", ";
    theOStream << "\"" << va_arg(vl, char*) << "\"";
  }
  va_end(vl);
}

//=======================================================================
//function : DumpRealValues
//purpose  : 
//=======================================================================
void Standard_Dump::DumpRealValues (Standard_OStream& theOStream, int theCount, ...)
{
  va_list  vl;
  va_start(vl, theCount);
  for(int i = 0; i < theCount; ++i)
  {
    if (i > 0)
      theOStream << ", ";
    theOStream << va_arg(vl, Standard_Real);
  }
  va_end(vl);
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString Standard_Dump::GetPointerInfo (const Handle(Standard_Transient)& thePointer,
                                                       const bool isShortInfo)
{
  if (thePointer.IsNull())
    return TCollection_AsciiString();

  return GetPointerInfo (thePointer.get(), isShortInfo);
}

// =======================================================================
// function : GetPointerInfo
// purpose :
// =======================================================================
TCollection_AsciiString Standard_Dump::GetPointerInfo (const void* thePointer, const bool isShortInfo)
{
  std::ostringstream aPtrStr;
  aPtrStr << thePointer;
  if (!isShortInfo)
    return aPtrStr.str().c_str();

  TCollection_AsciiString anInfoPtr (aPtrStr.str().c_str());
  for (int aSymbolId = 1; aSymbolId < anInfoPtr.Length(); aSymbolId++)
  {
    if (anInfoPtr.Value(aSymbolId) != '0')
    {
      anInfoPtr = anInfoPtr.SubString (aSymbolId, anInfoPtr.Length());
      anInfoPtr.Prepend (GetPointerPrefix());
      return anInfoPtr;
    }
  }
  return aPtrStr.str().c_str();
}

// =======================================================================
// DumpFieldToName
// =======================================================================
const char* Standard_Dump::DumpFieldToName (const char* theField)
{
  const char* aName = theField;

  if (aName[0] == '&')
  {
    aName = aName + 1;
  }
  if (::LowerCase (aName[0]) == 'm' && aName[1] == 'y')
  {
    aName = aName + 2;
  }
  return aName;
}

// =======================================================================
// Text
// =======================================================================
TCollection_AsciiString Standard_Dump::Text (const Standard_SStream& theStream)
{
  return TCollection_AsciiString (theStream.str().c_str());
}

// =======================================================================
// FormatJson
// =======================================================================
TCollection_AsciiString Standard_Dump::FormatJson (const Standard_SStream& theStream,
                                                   const Standard_Integer theIndent)
{
  TCollection_AsciiString aStreamStr = Text (theStream);
  TCollection_AsciiString anIndentStr;
  for (Standard_Integer anIndentId = 0; anIndentId < theIndent; anIndentId++)
    anIndentStr.AssignCat (' ');

  TCollection_AsciiString aText;

  Standard_Integer anIndentCount = 0;
  Standard_Boolean isMassiveValues = Standard_False;
  for (Standard_Integer anIndex = 1; anIndex < aStreamStr.Length(); anIndex++)
  {
    Standard_Character aSymbol = aStreamStr.Value (anIndex);
    if (aSymbol == '{')
    {
      anIndentCount++;

      aText += aSymbol;
      aText += '\n';

      for (int anIndent = 0; anIndent < anIndentCount; anIndent++)
        aText += anIndentStr;
    }
    else if (aSymbol == '}')
    {
      anIndentCount--;

      aText += '\n';
      for (int anIndent = 0; anIndent < anIndentCount; anIndent++)
        aText += anIndentStr;
      aText += aSymbol;
    }
    else if (aSymbol == '[')
    {
      isMassiveValues = Standard_True;
      aText += aSymbol;
    }
    else if (aSymbol == ']')
    {
      isMassiveValues = Standard_False;
      aText += aSymbol;
    }
    else if (aSymbol == ',')
    {
      if (!isMassiveValues)
      {
        aText += aSymbol;
        aText += '\n';
        for (int anIndent = 0; anIndent < anIndentCount; anIndent++)
          aText += anIndentStr;
        if (anIndex + 1 < aStreamStr.Length() && aStreamStr.Value (anIndex + 1) == ' ')
          anIndex++; // skip empty value after comma
      }
      else
        aText += aSymbol;
    }
    else
      aText += aSymbol;
  }
  return aText;
}
