// Copyright (c) 2020 OPEN CASCADE SAS
//
// This file is part of the examples of the Open CASCADE Technology software library.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE

#include "BaseSample.h"

#include <iostream>
#include <regex>
#include <exception>
#include <stack>

#include <AIS_ViewCube.hxx>
#include <Message.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>

#include <Standard_WarningsDisable.hxx>
#include <QDir>
#include <Standard_WarningsRestore.hxx>

const TCollection_AsciiString BaseSample::FILE_EXTENSION = "cxx";

void BaseSample::Clear()
{
  myObject3d.Clear();
  myObject2d.Clear();
  myCode.Clear();
  myResult.str("");
}

TCollection_AsciiString BaseSample::GetResult()
{
  TCollection_AsciiString aResult(myResult.str().c_str());
  myResult.str("");
  return aResult;
}

void BaseSample::AppendCube()
{
  Handle(AIS_ViewCube) aViewCube = new AIS_ViewCube();
  myObject3d.Append(aViewCube);
}

void BaseSample::Process (const TCollection_AsciiString& theSampleName)
{
  myObject3d.Clear();
  myObject2d.Clear();
  myCode.Clear();
  myIsProcessed = Standard_False;
  try
  {
    ExecuteSample(theSampleName);
    if (!myObject3d.IsEmpty())
    {
      Handle(AIS_ViewCube) aViewCube = new AIS_ViewCube();
      myObject3d.Append(aViewCube);
    }
  }
  catch (...)
  {
    TraceError(TCollection_AsciiString("Error in sample: ") + theSampleName);
  }
}

void BaseSample::TraceError (const TCollection_AsciiString& theErrorMessage)
{
  Message::SendFail() << "\nERROR: " << theErrorMessage.ToCString() << std::endl;
  myResult << "\nERROR: " << theErrorMessage << std::endl;
}

void BaseSample::FindSourceCode (const TCollection_AsciiString& theSampleName)
{
  TCollection_AsciiString aClassName = DynamicType()->Name();
  char aSeparator = QDir::separator().toLatin1();
  TCollection_AsciiString aCxxFilePach = myCodePath + aSeparator + aClassName + '.' + FILE_EXTENSION;
  OSD_File aCxxFile(aCxxFilePach);
  try
  {
    const Standard_Integer aFileBufferSize = 100 * 1024;
    TCollection_AsciiString aReadedText(aFileBufferSize);
    aCxxFile.Open(OSD_ReadOnly, OSD_Protection());
    aCxxFile.Read(aReadedText, aFileBufferSize);
    TCollection_AsciiString aRegexpTemplate = aClassName + "::" + theSampleName + "[\\n\\s]*\\([\\n\\s]*\\)[\\n\\s]*\\{";
    Standard_Integer aOpeningBracketPosition = findEndOfPhrase (aReadedText, aRegexpTemplate);
    Standard_Integer aClosingBracketPosition = findClosingBracket (aReadedText, aOpeningBracketPosition, '}');
    myCode = aReadedText.SubString(aOpeningBracketPosition + 1, aClosingBracketPosition - 1);
  }
  catch (...)
  {
    TraceError(TCollection_AsciiString("Cannot open file: ") + aCxxFilePach);
  }
}

Standard_Integer BaseSample::findEndOfPhrase (const TCollection_AsciiString& theText,
                                              const TCollection_AsciiString& theRegexpTemplate)
{
  Standard_Integer aIndexOfLastFoundSymbol = -1;
  std::string aStdText = theText.ToCString();
  std::string aRegexpTemplate = theRegexpTemplate.ToCString();

  try
  {
    std::regex aRegex(theRegexpTemplate.ToCString());

    std::sregex_iterator aDetectIterator = std::sregex_iterator(aStdText.begin(), aStdText.end(), aRegex);
    if (aDetectIterator != std::sregex_iterator())
    {
      std::smatch aMatch = *aDetectIterator;
      std::string aFoundString = aMatch.str();
      aIndexOfLastFoundSymbol = static_cast<Standard_Integer>(aStdText.find(aFoundString) + aFoundString.length());
    }
    else
    {
      TraceError(TCollection_AsciiString("No code found for template: ") + theRegexpTemplate);
    }
  }
  catch (const std::regex_error& aRegError)
  {
    TraceError(TCollection_AsciiString("regex_error: ") + aRegError.what());
  }
  catch (const std::exception& aEx)
  {
    TraceError(TCollection_AsciiString("common error: ") + aEx.what());
  }
  catch (...)
  {
    TraceError("unknown error!");
  }
  return aIndexOfLastFoundSymbol;
}

Standard_Integer BaseSample::findClosingBracket (const TCollection_AsciiString& theText,
                                                 const Standard_Integer theOpeningBracketIndex,
                                                 Standard_Character theClosingBracketSymbol)
{
  // TODO this function not implemented at least 2 cases:
  // - brackets in strings & chars
  // - brackets in comments
  Standard_Integer aClosingBracketIndex = -1;
  Standard_Character anOpeningBracketSymbol = theText.Value(theOpeningBracketIndex);
  TCollection_AsciiString aBracketsSet(theClosingBracketSymbol);
  aBracketsSet += anOpeningBracketSymbol;
  Standard_Integer aBracketDepth = 1;
  Standard_Integer aStartFindIndex = theOpeningBracketIndex + 1;
  //Standard_Character aStartFindChar = theText.Value(aStartFindIndex-1);
  while (aBracketDepth)
  {
    aStartFindIndex = theText.FirstLocationInSet(aBracketsSet, aStartFindIndex, theText.Length());
    if (!aStartFindIndex)
    {
      TraceError("No closing bracket found!");
      break;
    }
    TCollection_AsciiString aRSubstr = theText.SubString(aStartFindIndex, theText.Length());
    if (theText.Value(aStartFindIndex) == anOpeningBracketSymbol)
      aBracketDepth++;
    else if (theText.Value(aStartFindIndex) == theClosingBracketSymbol)
      aBracketDepth--;
    if (!aBracketDepth)
    {
      aClosingBracketIndex = aStartFindIndex;
      break;
    }
    aStartFindIndex++;
  }
  return aClosingBracketIndex;
}
