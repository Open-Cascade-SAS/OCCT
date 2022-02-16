// Copyright (c) 2022 OPEN CASCADE SAS
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

#include <DE_ConfigurationContext.hxx>

#include <Message.hxx>
#include <OSD_File.hxx>
#include <OSD_StreamBuffer.hxx>
#include <OSD_Path.hxx>
#include <OSD_Protection.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_HAsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(DE_ConfigurationContext, Standard_Transient)

enum DE_ConfigurationContext_KindOfLine
{
  DE_ConfigurationContext_KindOfLine_End,
  DE_ConfigurationContext_KindOfLine_Empty,
  DE_ConfigurationContext_KindOfLine_Comment,
  DE_ConfigurationContext_KindOfLine_Resource,
  DE_ConfigurationContext_KindOfLine_Error
};

namespace
{
  //=======================================================================
  //function : GetLine
  //purpose  :
  //=======================================================================
  static Standard_Boolean GetLine(OSD_File& theFile, TCollection_AsciiString& theLine)
  {
    TCollection_AsciiString aBuffer;
    Standard_Integer aBufSize = 10;
    Standard_Integer aLen;
    theLine.Clear();
    do
    {
      theFile.ReadLine(aBuffer, aBufSize, aLen);
      theLine += aBuffer;
      if (theFile.IsAtEnd())
      {
        if (!theLine.Length())
        {
          return Standard_False;
        }
        else
        {
          theLine += "\n";
        }
      }
    } while (theLine.Value(theLine.Length()) != '\n');
    return Standard_True;
  }

  //=======================================================================
  //function : WhatKindOfLine
  //purpose  :
  //=======================================================================
  static DE_ConfigurationContext_KindOfLine WhatKindOfLine(const TCollection_AsciiString& theLine,
                                                           TCollection_AsciiString& theToken1,
                                                           TCollection_AsciiString& theToken2)
  {
    static const TCollection_AsciiString aWhiteSpace = " \t\r\n";
    Standard_Integer aPos1 = 0, aPos2 = 0, aPos = 0;
    TCollection_AsciiString aLine(theLine);
    aLine.LeftAdjust();
    aLine.RightAdjust();
    if (!aLine.EndsWith(':') && (!aLine.EndsWith(' ') || !aLine.EndsWith('\t') || !aLine.EndsWith('\n')))
    {
      aLine.InsertAfter(aLine.Length(), " ");
    }

    if (aLine.Value(1) == '!')
    {
      return DE_ConfigurationContext_KindOfLine_Comment;
    }
    aPos1 = aLine.FirstLocationNotInSet(aWhiteSpace, 1, aLine.Length());
    if (aLine.Value(aPos1) == '\n')
    {
      return DE_ConfigurationContext_KindOfLine_Empty;
    }

    aPos2 = aLine.Location(1, ':', aPos1, aLine.Length());
    if (aPos2 == 0 || aPos1 == aPos2)
    {
      return DE_ConfigurationContext_KindOfLine_Error;
    }

    for (aPos = aPos2 - 1; aLine.Value(aPos) == '\t' || aLine.Value(aPos) == ' '; aPos--);

    theToken1 = aLine.SubString(aPos1, aPos);
    if(aPos2 != aLine.Length())
    {
      aPos2++;
    }
    aPos = aLine.FirstLocationNotInSet(aWhiteSpace, aPos2, aLine.Length());
    if (aPos != 0)
    {
      if (aLine.Value(aPos) == '\\')
      {
        switch (aLine.Value(aPos + 1))
        {
          case '\\':
          case ' ':
          case '\t':
            aPos++;
            break;
        }
      }
    }
    if (aPos == aLine.Length() || aPos == 0)
    {
      theToken2.Clear();
    }
    else
    {
      aLine.Remove(1, aPos - 1);
      aLine.Remove(aLine.Length());
      theToken2 = aLine;
    }
    return DE_ConfigurationContext_KindOfLine_Resource;
  }

  //=======================================================================
  //function : MakeName
  //purpose  :
  //=======================================================================
  static TCollection_AsciiString MakeName(const TCollection_AsciiString& theScope,
                                          const TCollection_AsciiString& theParam)
  {
    TCollection_AsciiString aStr(theScope);
    if (!aStr.IsEmpty())
    {
      aStr += '.';
    }
    aStr += theParam;
    return aStr;
  }
}

//=======================================================================
//function : DE_ConfigurationContext
//purpose  :
//=======================================================================
DE_ConfigurationContext::DE_ConfigurationContext()
{}

//=======================================================================
//function : Load
//purpose  :
//=======================================================================
Standard_Boolean DE_ConfigurationContext::Load(const TCollection_AsciiString& theConfiguration)
{
  OSD_Path aPath = theConfiguration;
  OSD_File aFile(aPath);
  if (!aFile.Exists())
  {
    if (!LoadStr(theConfiguration))
    {
      return false;
    }
  }
  else
  {
    if (!LoadFile(theConfiguration))
    {
      return false;
    }
  }
  return true;
}

//=======================================================================
//function : LoadFile
//purpose  :
//=======================================================================
Standard_Boolean DE_ConfigurationContext::LoadFile(const TCollection_AsciiString& theFile)
{
  myResource.Clear();
  OSD_Path aPath(theFile);
  OSD_File aFile = aPath;
  TCollection_AsciiString FileName = aPath.Name();
  aFile.Open(OSD_ReadOnly, OSD_Protection());
  if (aFile.Failed())
  {
    Message::SendFail("Error: DE Context loading is stopped. Can't open the file");
    return Standard_True;
  }
  TCollection_AsciiString aLine;
  while (GetLine(aFile, aLine))
  {
    if (!load(aLine))
    {
      Message::SendFail() << "Error: DE Context loading is stopped. Syntax error: " << aLine;
      return Standard_False;
    }
  }
  return Standard_True;
}

//=======================================================================
//function : LoadStr
//purpose  :
//=======================================================================
Standard_Boolean DE_ConfigurationContext::LoadStr(const TCollection_AsciiString& theResource)
{
  myResource.Clear();
  TCollection_AsciiString aLine = "";
  const Standard_Integer aLength = theResource.Length();
  for (Standard_Integer anInd = 1; anInd <= aLength; anInd++)
  {
    const Standard_Character aChar = theResource.Value(anInd);
    if (aChar != '\n')
      aLine += aChar;
    if ((aChar == '\n' || anInd == aLength) && !aLine.IsEmpty())
    {
      if (!load(aLine))
      {
        Message::SendFail() << "Error: DE Context loading is stopped. Syntax error: " << aLine;
        return Standard_False;
      }
      aLine.Clear();
    }
  }
  return Standard_True;
}

//=======================================================================
//function : IsParamSet
//purpose  :
//=======================================================================
Standard_Boolean DE_ConfigurationContext::IsParamSet(const TCollection_AsciiString& theParam,
                                                     const TCollection_AsciiString& theScope) const
{
  TCollection_AsciiString  aResource(MakeName(theScope, theParam));
  return myResource.IsBound(aResource);
}

//=======================================================================
//function : RealVal
//purpose  :
//=======================================================================
Standard_Real DE_ConfigurationContext::RealVal(const TCollection_AsciiString& theParam,
                                               const Standard_Real theDefValue,
                                               const TCollection_AsciiString& theScope) const
{
  Standard_Real aVal = 0.;
  return GetReal(theParam, aVal, theScope) ? aVal : theDefValue;
}

//=======================================================================
//function : IntegerVal
//purpose  :
//=======================================================================
Standard_Integer DE_ConfigurationContext::IntegerVal(const TCollection_AsciiString& theParam,
                                                     const Standard_Integer theDefValue,
                                                     const TCollection_AsciiString& theScope) const
{
  Standard_Integer aVal = 0;
  return GetInteger(theParam, aVal, theScope) ? aVal : theDefValue;
}

//=======================================================================
//function : BooleanVal
//purpose  :
//=======================================================================
Standard_Boolean DE_ConfigurationContext::BooleanVal(const TCollection_AsciiString& theParam,
                                                     const Standard_Boolean theDefValue,
                                                     const TCollection_AsciiString& theScope) const
{
  Standard_Boolean aVal = Standard_False;
  return GetBoolean(theParam, aVal, theScope) ? aVal : theDefValue;
}

//=======================================================================
//function : StringVal
//purpose  :
//=======================================================================
TCollection_AsciiString DE_ConfigurationContext::StringVal(const TCollection_AsciiString& theParam,
                                                           const TCollection_AsciiString& theDefValue,
                                                           const TCollection_AsciiString& theScope) const
{
  TCollection_AsciiString aVal = "";
  return GetString(theParam, aVal, theScope) ? aVal : theDefValue;
}

//=======================================================================
//function : GetReal
//purpose  :
//=======================================================================
Standard_Boolean DE_ConfigurationContext::GetReal(const TCollection_AsciiString& theParam,
                                                  Standard_Real& theValue,
                                                  const TCollection_AsciiString& theScope) const
{
  TCollection_AsciiString aStr;
  if (!GetString(theParam, aStr, theScope))
  {
    return Standard_False;
  }
  if (aStr.IsRealValue())
  {
    theValue = aStr.RealValue();
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : GetInteger
//purpose  :
//=======================================================================
Standard_Boolean DE_ConfigurationContext::GetInteger(const TCollection_AsciiString& theParam,
                                                     Standard_Integer& theValue,
                                                     const TCollection_AsciiString& theScope) const
{
  TCollection_AsciiString aStr;
  if (!GetString(theParam, aStr, theScope))
  {
    return Standard_False;
  }
  if (aStr.IsIntegerValue())
  {
    theValue = aStr.IntegerValue();
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : GetBoolean
//purpose  :
//=======================================================================
Standard_Boolean DE_ConfigurationContext::GetBoolean(const TCollection_AsciiString& theParam,
                                                     Standard_Boolean& theValue,
                                                     const TCollection_AsciiString& theScope) const
{
  TCollection_AsciiString aStr;
  if (!GetString(theParam, aStr, theScope))
  {
    return Standard_False;
  }
  if (aStr.IsIntegerValue())
  {
    theValue = aStr.IntegerValue() != 0;
    return Standard_True;
  }
  return Standard_False;
}

//=======================================================================
//function : GetString
//purpose  :
//=======================================================================
Standard_Boolean DE_ConfigurationContext::GetString(const TCollection_AsciiString& theParam,
                                                    TCollection_AsciiString& theStr,
                                                    const TCollection_AsciiString& theScope) const
{
  TCollection_AsciiString aResource = MakeName(theScope, theParam);
  return myResource.Find(aResource, theStr);
}

//=======================================================================
//function : GetStringSeq
//purpose  :
//=======================================================================
Standard_Boolean DE_ConfigurationContext::GetStringSeq(const TCollection_AsciiString& theParam,
                                                       TColStd_ListOfAsciiString& theValue,
                                                       const TCollection_AsciiString& theScope) const
{
  TCollection_AsciiString aStr;
  if (!GetString(theParam, aStr, theScope))
  {
    return Standard_False;
  }
  theValue.Clear();
  TCollection_AsciiString anElem;
  const Standard_Integer aLength = aStr.Length();
  for (Standard_Integer anInd = 1; anInd <= aLength; anInd++)
  {
    const Standard_Character aChar = aStr.Value(anInd);
    anElem += aChar;
    if ((aChar == ' ' || anInd == aLength) && !anElem.IsEmpty())
    {
      anElem.RightAdjust();
      anElem.LeftAdjust();
      theValue.Append(anElem);
      anElem.Clear();
    }
  }
  return Standard_True;
}

//=======================================================================
//function : load
//purpose  :
//=======================================================================
Standard_Boolean DE_ConfigurationContext::load(const TCollection_AsciiString& theResourceLine)
{
  if (theResourceLine.IsEmpty())
  {
    return Standard_False;
  }
  TCollection_AsciiString aToken1, aToken2;
  DE_ConfigurationContext_KindOfLine aKind = WhatKindOfLine(theResourceLine, aToken1, aToken2);
  switch (aKind)
  {
    case DE_ConfigurationContext_KindOfLine_End:
    case DE_ConfigurationContext_KindOfLine_Comment:
    case DE_ConfigurationContext_KindOfLine_Empty:
      break;
    case DE_ConfigurationContext_KindOfLine_Resource:
      myResource.Bind(aToken1, aToken2);
      break;
    case DE_ConfigurationContext_KindOfLine_Error:
      break;
  }
  return Standard_True;
}
