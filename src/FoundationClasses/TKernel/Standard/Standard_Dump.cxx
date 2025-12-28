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

//=================================================================================================

void Standard_Dump::AddValuesSeparator(Standard_OStream& theOStream)
{
  // Check if the stream has any content by checking the current write position
  std::streampos aPos = theOStream.tellp();
  if (aPos <= 0)
  {
    // Stream is empty or at the beginning, no separator needed
    return;
  }

  // Try to cast to Standard_SStream (std::stringstream)
  Standard_SStream* anSStream = dynamic_cast<Standard_SStream*>(&theOStream);
  if (anSStream != nullptr)
  {
    TCollection_AsciiString aStreamStr = Standard_Dump::Text(*anSStream);
    if (!aStreamStr.IsEmpty() && !aStreamStr.EndsWith("{") && !aStreamStr.EndsWith(", "))
    {
      theOStream << ", ";
    }
    return;
  }

  // Try to cast to std::ostringstream (for cases where users use ostringstream directly)
  std::ostringstream* anOStrStream = dynamic_cast<std::ostringstream*>(&theOStream);
  if (anOStrStream != nullptr)
  {
    TCollection_AsciiString aStreamStr = anOStrStream->str().c_str();
    if (!aStreamStr.IsEmpty() && !aStreamStr.EndsWith("{") && !aStreamStr.EndsWith(", "))
    {
      theOStream << ", ";
    }
  }
  else
  {
    // For other stream types where we cannot read the content,
    // we add the separator unconditionally since we know the stream is not empty
    theOStream << ", ";
  }
}

//=================================================================================================

void Standard_Dump::DumpKeyToClass(Standard_OStream&              theOStream,
                                   const TCollection_AsciiString& theKey,
                                   const TCollection_AsciiString& theField)
{
  AddValuesSeparator(theOStream);
  theOStream << "\"" << theKey << "\": {" << theField << "}";
}

//=================================================================================================

void Standard_Dump::DumpCharacterValues(Standard_OStream& theOStream, int theCount, ...)
{
  va_list vl;
  va_start(vl, theCount);
  for (int i = 0; i < theCount; ++i)
  {
    if (i > 0)
      theOStream << ", ";
    theOStream << "\"" << va_arg(vl, char*) << "\"";
  }
  va_end(vl);
}

//=================================================================================================

void Standard_Dump::DumpRealValues(Standard_OStream& theOStream, int theCount, ...)
{
  va_list vl;
  va_start(vl, theCount);
  for (int i = 0; i < theCount; ++i)
  {
    if (i > 0)
      theOStream << ", ";
    theOStream << va_arg(vl, double);
  }
  va_end(vl);
}

//=================================================================================================

bool Standard_Dump::ProcessStreamName(const TCollection_AsciiString& theStreamStr,
                                                  const TCollection_AsciiString& theName,
                                                  int&              theStreamPos)
{
  if (theStreamStr.IsEmpty())
    return false;

  if (theStreamStr.Length() < theStreamPos)
    return false;

  TCollection_AsciiString aSubText = theStreamStr.SubString(theStreamPos, theStreamStr.Length());
  if (aSubText.StartsWith(JsonKeyToString(Standard_JsonKey_SeparatorValueToValue)))
  {
    theStreamPos += JsonKeyLength(Standard_JsonKey_SeparatorValueToValue);
    aSubText = theStreamStr.SubString(theStreamPos, theStreamStr.Length());
  }
  TCollection_AsciiString aKeyName =
    TCollection_AsciiString(JsonKeyToString(Standard_JsonKey_Quote)) + theName
    + TCollection_AsciiString(JsonKeyToString(Standard_JsonKey_Quote))
    + JsonKeyToString(Standard_JsonKey_SeparatorKeyToValue);
  bool aResult = aSubText.StartsWith(aKeyName);
  if (aResult)
    theStreamPos += aKeyName.Length();

  return aResult;
}

//=================================================================================================

bool Standard_Dump::ProcessFieldName(const TCollection_AsciiString& theStreamStr,
                                                 const TCollection_AsciiString& theName,
                                                 int&              theStreamPos)
{
  if (theStreamStr.IsEmpty())
    return false;

  TCollection_AsciiString aSubText = theStreamStr.SubString(theStreamPos, theStreamStr.Length());
  if (aSubText.StartsWith(JsonKeyToString(Standard_JsonKey_SeparatorValueToValue)))
  {
    theStreamPos += JsonKeyLength(Standard_JsonKey_SeparatorValueToValue);
    aSubText = theStreamStr.SubString(theStreamPos, theStreamStr.Length());
  }

  TCollection_AsciiString aName = Standard_Dump::DumpFieldToName(theName.ToCString());
  TCollection_AsciiString aKeyName =
    TCollection_AsciiString(JsonKeyToString(Standard_JsonKey_Quote)) + aName
    + TCollection_AsciiString(JsonKeyToString(Standard_JsonKey_Quote))
    + JsonKeyToString(Standard_JsonKey_SeparatorKeyToValue);

  bool aResult = aSubText.StartsWith(aKeyName);
  if (aResult)
    theStreamPos += aKeyName.Length();

  return aResult;
}

//=================================================================================================

bool Standard_Dump::InitRealValues(const TCollection_AsciiString& theStreamStr,
                                               int&              theStreamPos,
                                               int                            theCount,
                                               ...)
{
  int aStreamPos = theStreamPos + JsonKeyLength(Standard_JsonKey_OpenContainer);

  TCollection_AsciiString aSubText = theStreamStr.SubString(aStreamPos, theStreamStr.Length());

  va_list vl;
  va_start(vl, theCount);
  aStreamPos                 = 1;
  int aClosePos = aSubText.Location(JsonKeyToString(Standard_JsonKey_CloseContainer),
                                                 aStreamPos,
                                                 aSubText.Length());
  for (int i = 0; i < theCount; ++i)
  {
    int aNextPos =
      (i < theCount - 1)
        ? aSubText.Location(JsonKeyToString(Standard_JsonKey_SeparatorValueToValue),
                            aStreamPos,
                            aSubText.Length())
        : aClosePos;

    TCollection_AsciiString aValueText = aSubText.SubString(aStreamPos, aNextPos - 1);

    if (!aValueText.IsRealValue())
    {
      va_end(vl);
      return false;
    }

    double aValue          = aValueText.RealValue();
    *(va_arg(vl, double*)) = aValue;

    aStreamPos = aNextPos + JsonKeyLength(Standard_JsonKey_SeparatorValueToValue);
  }
  va_end(vl);
  aClosePos    = theStreamStr.Location(JsonKeyToString(Standard_JsonKey_CloseContainer),
                                    theStreamPos,
                                    theStreamStr.Length());
  theStreamPos = aClosePos + JsonKeyLength(Standard_JsonKey_CloseContainer);

  return true;
}

//=================================================================================================

bool Standard_Dump::InitValue(const TCollection_AsciiString& theStreamStr,
                                          int&              theStreamPos,
                                          TCollection_AsciiString&       theValue)
{
  int aStreamPos = theStreamPos;

  TCollection_AsciiString aSubText = theStreamStr.SubString(aStreamPos, theStreamStr.Length());

  aStreamPos = 1;
  int aNextPos =
    aSubText.Location(JsonKeyToString(Standard_JsonKey_SeparatorValueToValue),
                      aStreamPos,
                      aSubText.Length());
  Standard_JsonKey aNextKey = Standard_JsonKey_SeparatorValueToValue;

  int aCloseChildPos =
    aSubText.Location(JsonKeyToString(Standard_JsonKey_CloseChild), aStreamPos, aSubText.Length());
  bool isUseClosePos =
    (aNextPos > 0 && aCloseChildPos > 0 && aCloseChildPos < aNextPos) || !aNextPos;
  if (isUseClosePos)
  {
    aNextPos = aCloseChildPos;
    aNextKey = Standard_JsonKey_CloseChild;
  }

  theValue     = aNextPos ? aSubText.SubString(aStreamPos, aNextPos - 1) : aSubText;
  theStreamPos = aNextPos ? (theStreamPos + (aNextPos - aStreamPos) + JsonKeyLength(aNextKey))
                          : theStreamStr.Length();
  return true;
}

//=================================================================================================

TCollection_AsciiString Standard_Dump::GetPointerInfo(const occ::handle<Standard_Transient>& thePointer,
                                                      const bool                        isShortInfo)
{
  if (thePointer.IsNull())
    return TCollection_AsciiString();

  return GetPointerInfo(thePointer.get(), isShortInfo);
}

//=================================================================================================

TCollection_AsciiString Standard_Dump::GetPointerInfo(const void* thePointer,
                                                      const bool  isShortInfo)
{
  if (!thePointer)
    return TCollection_AsciiString();

  std::ostringstream aPtrStr;
  aPtrStr << thePointer;
  if (!isShortInfo)
    return aPtrStr.str().c_str();

  TCollection_AsciiString anInfoPtr(aPtrStr.str().c_str());
  for (int aSymbolId = 1; aSymbolId < anInfoPtr.Length(); aSymbolId++)
  {
    if (anInfoPtr.Value(aSymbolId) != '0')
    {
      anInfoPtr = anInfoPtr.SubString(aSymbolId, anInfoPtr.Length());
      anInfoPtr.Prepend(GetPointerPrefix());
      return anInfoPtr;
    }
  }
  return aPtrStr.str().c_str();
}

// =======================================================================
// DumpFieldToName
// =======================================================================
TCollection_AsciiString Standard_Dump::DumpFieldToName(const TCollection_AsciiString& theField)
{
  TCollection_AsciiString aName = theField;
  if (theField.StartsWith('&'))
  {
    aName.Remove(1, 1);
  }

  if (aName.Length() > 1 && aName.Value(1) == 'a')
  {
    if (aName.Length() > 2 && aName.Value(2) == 'n')
    {
      aName.Remove(1, 2);
    }
    else
      aName.Remove(1, 1);
  }
  else if (aName.Length() > 2 && ::LowerCase(aName.Value(1)) == 'm' && aName.Value(2) == 'y')
  {
    aName.Remove(1, 2);
  }

  if (aName.EndsWith(".get()"))
  {
    aName = aName.SubString(1, aName.Length() - TCollection_AsciiString(".get()").Length());
  }
  else if (aName.EndsWith("()"))
  {
    aName = aName.SubString(1, aName.Length() - TCollection_AsciiString("()").Length());
  }
  return aName;
}

// =======================================================================
// Text
// =======================================================================
TCollection_AsciiString Standard_Dump::Text(const Standard_SStream& theStream)
{
  return TCollection_AsciiString(theStream.str().c_str());
}

// =======================================================================
// FormatJson
// =======================================================================
TCollection_AsciiString Standard_Dump::FormatJson(const Standard_SStream& theStream,
                                                  const int  theIndent)
{
  TCollection_AsciiString aStreamStr = Text(theStream);
  TCollection_AsciiString anIndentStr;
  for (int anIndentId = 0; anIndentId < theIndent; anIndentId++)
    anIndentStr.AssignCat(' ');

  TCollection_AsciiString aText;

  int anIndentCount   = 0;
  bool isMassiveValues = false;
  for (int anIndex = 1; anIndex <= aStreamStr.Length(); anIndex++)
  {
    char aSymbol = aStreamStr.Value(anIndex);
    if (anIndex == 1 && aText.IsEmpty() && aSymbol != '{')
    {
      // append opening brace for json start
      aSymbol = '{';
      anIndex--;
    }
    if (aSymbol == '{')
    {
      anIndentCount++;

      aText += aSymbol;
      aText += '\n';

      for (int anIndent = 0; anIndent < anIndentCount; anIndent++)
      {
        aText += anIndentStr;
      }
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
      isMassiveValues = true;
      aText += aSymbol;
    }
    else if (aSymbol == ']')
    {
      isMassiveValues = false;
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
        if (anIndex + 1 < aStreamStr.Length() && aStreamStr.Value(anIndex + 1) == ' ')
          anIndex++; // skip empty value after comma
      }
      else
        aText += aSymbol;
    }
    else if (aSymbol == '\n')
    {
      aText += ""; // json does not support multi-lined values, skip this symbol
    }
    else
      aText += aSymbol;

    if (anIndex == aStreamStr.Length() && aSymbol != '}')
    {
      // append closing brace for json end
      aSymbol = '}';

      anIndentCount--;
      aText += '\n';
      for (int anIndent = 0; anIndent < anIndentCount; anIndent++)
        aText += anIndentStr;
      aText += aSymbol;
    }
  }
  return aText;
}

// =======================================================================
// SplitJson
// =======================================================================
bool Standard_Dump::SplitJson(
  const TCollection_AsciiString&                                           theStreamStr,
  NCollection_IndexedDataMap<TCollection_AsciiString, Standard_DumpValue>& theKeyToValues)
{
  int aNextIndex = 1;
  while (aNextIndex < theStreamStr.Length())
  {
    Standard_JsonKey aKey = Standard_JsonKey_None;
    if (!jsonKey(theStreamStr, aNextIndex, aNextIndex, aKey))
      return false;

    bool aProcessed = false;
    switch (aKey)
    {
      case Standard_JsonKey_Quote: {
        aProcessed = splitKeyToValue(theStreamStr, aNextIndex, aNextIndex, theKeyToValues);
        break;
      }
      case Standard_JsonKey_OpenChild: {
        int aStartIndex = aNextIndex;
        int aClosePos   = nextClosePosition(theStreamStr,
                                                       aStartIndex,
                                                       Standard_JsonKey_OpenChild,
                                                       Standard_JsonKey_CloseChild);
        if (aClosePos == 0)
          return false;

        TCollection_AsciiString aSubStreamStr =
          theStreamStr.SubString(aStartIndex + JsonKeyLength(aKey), aNextIndex - 2);
        if (!SplitJson(aSubStreamStr, theKeyToValues))
          return false;

        aNextIndex = aClosePos + int(JsonKeyLength(Standard_JsonKey_CloseChild));
        break;
      }
      case Standard_JsonKey_SeparatorValueToValue: {
        continue;
      }
      default:
        break;
    }
    if (!aProcessed)
      return false;
  }
  return true;
}

// =======================================================================
// HierarchicalValueIndices
// =======================================================================
NCollection_List<int> Standard_Dump::HierarchicalValueIndices(
  const NCollection_IndexedDataMap<TCollection_AsciiString, TCollection_AsciiString>& theValues)
{
  NCollection_List<int> anIndices;

  for (int anIndex = 1; anIndex <= theValues.Extent(); anIndex++)
  {
    if (HasChildKey(theValues.FindFromIndex(anIndex)))
      anIndices.Append(anIndex);
  }
  return anIndices;
}

// =======================================================================
// splitKeyToValue
// =======================================================================
bool Standard_Dump::splitKeyToValue(
  const TCollection_AsciiString&                                           theStreamStr,
  int                                                         theStartIndex,
  int&                                                        theNextIndex,
  NCollection_IndexedDataMap<TCollection_AsciiString, Standard_DumpValue>& theValues)
{
  // find key value: "key"
  int aStartIndex = theStartIndex;
  int aCloseIndex =
    nextClosePosition(theStreamStr, aStartIndex + 1, Standard_JsonKey_None, Standard_JsonKey_Quote);
  if (aCloseIndex == 0)
    return false;

  TCollection_AsciiString aSplitKey = theStreamStr.SubString(aStartIndex, aCloseIndex - 1);

  // key to value
  aStartIndex           = aCloseIndex + 1;
  Standard_JsonKey aKey = Standard_JsonKey_None;
  if (!jsonKey(theStreamStr, aStartIndex, aCloseIndex, aKey))
    return false;

  // find value
  aStartIndex = aCloseIndex;
  aKey        = Standard_JsonKey_None;
  jsonKey(theStreamStr, aStartIndex, aCloseIndex, aKey);
  aStartIndex = aCloseIndex;

  TCollection_AsciiString aSplitValue;
  theNextIndex = -1;
  switch (aKey)
  {
    case Standard_JsonKey_OpenChild: {
      aCloseIndex = nextClosePosition(theStreamStr,
                                      aStartIndex,
                                      Standard_JsonKey_OpenChild,
                                      Standard_JsonKey_CloseChild);
      if (aCloseIndex > aStartIndex)
        aSplitValue = theStreamStr.SubString(aStartIndex, aCloseIndex);
      theNextIndex = aCloseIndex + 1;
      break;
    }
    case Standard_JsonKey_OpenContainer: {
      aCloseIndex = nextClosePosition(theStreamStr,
                                      aStartIndex,
                                      Standard_JsonKey_OpenContainer,
                                      Standard_JsonKey_CloseContainer);
      if (aCloseIndex > aStartIndex)
        aSplitValue = theStreamStr.SubString(aStartIndex, aCloseIndex - 1);
      theNextIndex = aCloseIndex + 1;
      break;
    }
    case Standard_JsonKey_Quote: {
      Standard_JsonKey aKeyTmp = Standard_JsonKey_None;
      if (jsonKey(theStreamStr, aStartIndex, aCloseIndex, aKeyTmp)
          && aKeyTmp == Standard_JsonKey_Quote) // emptyValue
      {
        aSplitValue  = "";
        theNextIndex = aCloseIndex;
      }
      else
      {
        aCloseIndex  = nextClosePosition(theStreamStr,
                                        aStartIndex + 1,
                                        Standard_JsonKey_None,
                                        Standard_JsonKey_Quote);
        aSplitValue  = theStreamStr.SubString(aStartIndex, aCloseIndex - 1);
        theNextIndex = aCloseIndex + 1;
      }
      break;
    }
    case Standard_JsonKey_None: {
      if (aStartIndex == theStreamStr.Length())
      {
        aSplitValue =
          aStartIndex <= aCloseIndex ? theStreamStr.SubString(aStartIndex, aCloseIndex) : "";
        aSplitValue = theStreamStr.SubString(aStartIndex, aCloseIndex);
        aCloseIndex = aStartIndex;
      }
      else
      {
        int aCloseIndex1 = nextClosePosition(theStreamStr,
                                                          aStartIndex,
                                                          Standard_JsonKey_None,
                                                          Standard_JsonKey_CloseChild)
                                        - 1;
        int aCloseIndex2 = nextClosePosition(theStreamStr,
                                                          aStartIndex,
                                                          Standard_JsonKey_None,
                                                          Standard_JsonKey_SeparatorValueToValue)
                                        - 1;
        aCloseIndex = aCloseIndex1 < aCloseIndex2 ? aCloseIndex1 : aCloseIndex2;
        aSplitValue =
          aStartIndex <= aCloseIndex ? theStreamStr.SubString(aStartIndex, aCloseIndex) : "";
      }
      theNextIndex = aCloseIndex + 1;
      break;
    }
    default:
      return false;
  }

  Standard_DumpValue aValue;
  if (theValues.FindFromKey(aSplitKey, aValue))
  {
    int anIndex = 1;
    // increment key until the new key does not exist in the container
    TCollection_AsciiString anIndexedSuffix =
      TCollection_AsciiString("_") + TCollection_AsciiString(anIndex);
    while (theValues.FindFromKey(TCollection_AsciiString(aSplitKey + anIndexedSuffix), aValue))
    {
      anIndex++;
      anIndexedSuffix = TCollection_AsciiString("_") + TCollection_AsciiString(anIndex);
    }
    aSplitKey = aSplitKey + anIndexedSuffix;
  }

  theValues.Add(aSplitKey, Standard_DumpValue(aSplitValue, aStartIndex));
  return true;
}

// =======================================================================
// jsonKey
// =======================================================================
bool Standard_Dump::jsonKey(const TCollection_AsciiString& theStreamStr,
                                        int               theStartIndex,
                                        int&              theNextIndex,
                                        Standard_JsonKey&              theKey)
{
  TCollection_AsciiString aSubStreamStr =
    theStreamStr.SubString(theStartIndex, theStreamStr.Length());
  for (int aKeyId = (int)Standard_JsonKey_OpenChild;
       aKeyId <= Standard_JsonKey_SeparatorValueToValue;
       aKeyId++)
  {
    Standard_JsonKey aKey      = (Standard_JsonKey)aKeyId;
    const char* aKeyToStr = JsonKeyToString(aKey);
    if (!aSubStreamStr.StartsWith(aKeyToStr))
      continue;

    theNextIndex = theStartIndex + int(JsonKeyLength(aKey));
    theKey       = aKey;
    return true;
  }
  return false;
}

// =======================================================================
// HasChildKey
// =======================================================================
bool Standard_Dump::HasChildKey(const TCollection_AsciiString& theSourceValue)
{
  return theSourceValue.Search(JsonKeyToString(Standard_JsonKey_SeparatorKeyToValue)) >= 0;
}

// =======================================================================
// JsonKeyToString
// =======================================================================
const char* Standard_Dump::JsonKeyToString(const Standard_JsonKey theKey)
{
  switch (theKey)
  {
    case Standard_JsonKey_None:
      return "";
    case Standard_JsonKey_OpenChild:
      return "{";
    case Standard_JsonKey_CloseChild:
      return "}";
    case Standard_JsonKey_OpenContainer:
      return "[";
    case Standard_JsonKey_CloseContainer:
      return "]";
    case Standard_JsonKey_Quote:
      return "\"";
    case Standard_JsonKey_SeparatorKeyToValue:
      return ": ";
    case Standard_JsonKey_SeparatorValueToValue:
      return ", ";
  }

  return "";
}

// =======================================================================
// JsonKeyLength
// =======================================================================
int Standard_Dump::JsonKeyLength(const Standard_JsonKey theKey)
{
  return (int)strlen(JsonKeyToString(theKey));
}

// =======================================================================
// nextClosePosition
// =======================================================================
int Standard_Dump::nextClosePosition(const TCollection_AsciiString& theSourceValue,
                                                  const int         theStartPosition,
                                                  const Standard_JsonKey         theOpenKey,
                                                  const Standard_JsonKey         theCloseKey)
{
  const char* anOpenKey    = JsonKeyToString(theOpenKey);
  const char* aCloseKeyStr = JsonKeyToString(theCloseKey);

  int aStartPos = theStartPosition;
  int aDepthKey = 0;

  while (aStartPos < theSourceValue.Length())
  {
    int anOpenKeyPos =
      theSourceValue.Location(anOpenKey, aStartPos, theSourceValue.Length());
    int aCloseKeyPos =
      theSourceValue.Location(aCloseKeyStr, aStartPos, theSourceValue.Length());
    if (aCloseKeyPos == 0)
      break;

    if (anOpenKeyPos != 0 && anOpenKeyPos <= aCloseKeyPos)
    {
      aDepthKey++;
      aStartPos = anOpenKeyPos + 1;
    }
    else
    {
      if (aDepthKey == 0)
        return aCloseKeyPos;
      else
      {
        aDepthKey--;
        aStartPos = aCloseKeyPos + 1;
      }
    }
  }
  return theSourceValue.Length();
}
