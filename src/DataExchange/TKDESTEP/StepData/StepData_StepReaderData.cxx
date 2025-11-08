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

//    abv 09.04.99 S4136: eliminate parameter step.readaccept.void
//    sln 04,10.2001. BUC61003. Prevent exception which may occur during reading of complex entity
//    (if entity's items are not in alphabetical order)

#include <Interface_Check.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_ParamList.hxx>
#include <Message.hxx>
#include <Message_Messenger.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <StepData_ESDescr.hxx>
#include <StepData_FieldList.hxx>
#include <StepData_PDescr.hxx>
#include <StepData_SelectArrReal.hxx>
#include <StepData_SelectInt.hxx>
#include <StepData_SelectMember.hxx>
#include <StepData_SelectNamed.hxx>
#include <StepData_SelectReal.hxx>
#include <StepData_SelectType.hxx>
#include <StepData_StepReaderData.hxx>
#include <NCollection_IncAllocator.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_UtfIterator.hxx>
#include <TCollection_HAsciiString.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_HArray1OfInteger.hxx>
#include <TColStd_HArray1OfReal.hxx>
#include <TColStd_HArray1OfTransient.hxx>
#include <TColStd_HSequenceOfReal.hxx>
#include <TColStd_IndexedMapOfInteger.hxx>
#include <TColStd_SequenceOfInteger.hxx>
#include <StepData_UndefinedEntity.hxx>
#include <Resource_Unicode.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(StepData_StepReaderData, Interface_FileReaderData)

// The Header consists of entities analogous in principle to those
// of the Data, except that they are without identifier, and can neither
// reference, nor be referenced (whether with Header or with Data)
// Thus, in StepReaderData, the Header consists of the first "thenbhead" Entities
// This separation allows STEP files to have metadata and structural information
// separate from the main geometric and semantic data
//  #########################################################################
//  ....   Creation and basic access to atomic file data    ....

#define Maxlst 64

// ----------  Fonctions Utilitaires  ----------

//! Convert unsigned character to hexadecimal system,
//! if character hasn't representation in this system, returns 0.
static Standard_Integer convertCharacterTo16bit(const Standard_ExtCharacter theCharacter)
{
  switch (theCharacter)
  {
    case '0':
      return 0;
    case '1':
      return 1;
    case '2':
      return 2;
    case '3':
      return 3;
    case '4':
      return 4;
    case '5':
      return 5;
    case '6':
      return 6;
    case '7':
      return 7;
    case '8':
      return 8;
    case '9':
      return 9;
    case 'A':
    case 'a':
      return 10;
    case 'B':
    case 'b':
      return 11;
    case 'C':
    case 'c':
      return 12;
    case 'D':
    case 'd':
      return 13;
    case 'E':
    case 'e':
      return 14;
    case 'F':
    case 'f':
      return 15;
    default:
      return 0;
  }
}

//=================================================================================================

void StepData_StepReaderData::cleanText(const Handle(TCollection_HAsciiString)& theVal) const
{
  if (theVal->Length() == 2)
  {
    theVal->Clear();
    return;
  }
  TCollection_ExtendedString aResString;
  const Standard_Boolean     toConversion = mySourceCodePage != Resource_FormatType_NoConversion;
  Resource_Unicode::ConvertFormatToUnicode(mySourceCodePage, theVal->ToCString() + 1, aResString);
  Standard_Integer           aResStringSize = aResString.Length() - 1; // skip the last apostrophe
  TCollection_ExtendedString aTempExtString;  // string for characters within control directives
  Standard_Integer           aSetCharInd = 1; // index to set value to result string
                                              // clang-format off
  Resource_FormatType aLocalFormatType = Resource_FormatType_iso8859_1; // a code page for a "\S\" control directive
                                              // clang-format on
  for (Standard_Integer aStringInd = 1; aStringInd <= aResStringSize; ++aStringInd)
  {
    const Standard_ExtCharacter aChar = aResString.Value(aStringInd);
    aSetCharInd                       = aStringInd;
    if (aChar == '\\' && aStringInd <= aResStringSize - 3) // can contains the control directive
    {
      Standard_Boolean            isConverted  = Standard_False;
      const Standard_ExtCharacter aDirChar     = aResString.Value(aStringInd + 1);
      const Standard_Boolean      isSecSlash   = aResString.Value(aStringInd + 2) == '\\';
      const Standard_Boolean      isThirdSlash = aResString.Value(aStringInd + 3) == '\\';
      // Encoding ISO 8859 characters within a string;
      // ("\P{N}\") control directive;
      // indicates code page for ("\S\") control directive;
      // {N}: "A", "B", "C", "D", "E", "F", "G", "H", "I";
      // "A" identifies ISO 8859-1; "B" identifies ISO 8859-2, etc.
      if (aDirChar == 'P' && isThirdSlash)
      {
        const Standard_Character aPageId =
          UpperCase(static_cast<Standard_Character>(aResString.Value(aStringInd + 2) & 255));
        if (aPageId >= 'A' && aPageId <= 'I')
        {
          aLocalFormatType = (Resource_FormatType)(Resource_FormatType_iso8859_1 + (aPageId - 'A'));
        }
        else
        {
          thecheck->AddWarning(
            "String control directive \\P*\\ with an unsupported symbol in place of *");
        }
        isConverted = Standard_True;
        aStringInd += 3;
      }
      // Encoding ISO 8859 characters within a string;
      // ("\S\") control directive;
      // converts followed a LATIN CODEPOINT character.
      else if (aDirChar == 'S' && isSecSlash)
      {
        Standard_Character aResChar =
          static_cast<Standard_Character>(aResString.Value(aStringInd + 3) | 0x80);
        const char aStrForCovert[2] = {aResChar, '\0'};
        Resource_Unicode::ConvertFormatToUnicode(aLocalFormatType, aStrForCovert, aTempExtString);
        isConverted = Standard_True;
        aStringInd += 3;
      }
      // Encoding U+0000 to U+00FF in a string
      // ("\X\") control directive;
      // converts followed two hexadecimal character.
      else if (aDirChar == 'X' && aStringInd <= aResStringSize - 4 && isSecSlash)
      {
        Standard_Character aResChar =
          (char)convertCharacterTo16bit(aResString.Value(aStringInd + 3));
        aResChar =
          (aResChar << 4) | (char)convertCharacterTo16bit(aResString.Value(aStringInd + 4));
        const char aStrForConvert[2] = {aResChar, '\0'};
        // clang-format off
        aTempExtString = TCollection_ExtendedString(aStrForConvert, Standard_False); // pass through without conversion
        // clang-format on
        isConverted = Standard_True;
        aStringInd += 4;
      }
      // Encoding ISO 10646 characters within a string
      // ("\X{N}\") control directive;
      // {N}: "0", "2", "4";
      // "\X2\" or "\X4\" converts followed a hexadecimal character sequence;
      // "\X0\" indicate the end of the "\X2\" or "\X4\".
      else if (aDirChar == 'X' && isThirdSlash)
      {
        Standard_Integer aFirstInd = aStringInd + 3;
        Standard_Integer aLastInd  = aStringInd;
        Standard_Boolean isClosed  = Standard_False;
        // find the end of the "\X2\" or "\X4\" by an external "aStringInd"
        for (; aStringInd <= aResStringSize && !isClosed; ++aStringInd)
        {
          if (aResStringSize - aStringInd > 2 && aResString.Value(aStringInd) == '\\'
              && aResString.Value(aStringInd + 1) == 'X' && aResString.Value(aStringInd + 2) == '0'
              && aResString.Value(aStringInd + 3) == '\\')
          {
            aLastInd   = aStringInd - 1;
            aStringInd = aStringInd + 2;
            isClosed   = Standard_True;
          }
        }
        if (!isClosed) // "\X0\" not exists
        {
          aLastInd = aStringInd = aResStringSize;
        }
        const Standard_Integer aStrLen = aLastInd - aFirstInd;
        // "\X2\" control directive;
        // followed by multiples of four or three hexadecimal characters.
        // Encoding in UTF-16
        if (aResString.Value(aFirstInd - 1) == '2' && aResStringSize - aFirstInd > 3)
        {
          Standard_Integer anIterStep = (aStrLen % 4 == 0) ? 4 : 3;
          if (aStrLen % anIterStep)
          {
            aTempExtString.AssignCat('?');
            thecheck->AddWarning(
              "String control directive \\X2\\ is followed by number of digits not multiple of 4");
          }
          else
          {
            Standard_Utf16Char aUtfCharacter = '\0';
            for (Standard_Integer aCharInd = 1; aCharInd <= aStrLen; ++aCharInd)
            {
              aUtfCharacter |= convertCharacterTo16bit(aResString.Value(aCharInd + aFirstInd));
              if (aCharInd % anIterStep == 0)
              {
                aTempExtString.AssignCat(aUtfCharacter);
                aUtfCharacter = '\0';
              }
              aUtfCharacter = aUtfCharacter << 4;
            }
          }
        }
        // "\X4\" control directive;
        // followed by multiples of eight hexadecimal characters.
        // Encoding in UTF-32
        else if (aResString.Value(aFirstInd - 1) == '4' && aResStringSize - aFirstInd > 7)
        {
          if (aStrLen % 8)
          {
            aTempExtString.AssignCat('?');
            thecheck->AddWarning(
              "String control directive \\X4\\ is followed by number of digits not multiple of 8");
          }
          else
          {
            Standard_Utf32Char aUtfCharacter[2] = {'\0', '\0'};
            for (Standard_Integer aCharInd = 1; aCharInd <= aStrLen; ++aCharInd)
            {
              aUtfCharacter[0] |= convertCharacterTo16bit(aResString.Value(aCharInd + aFirstInd));
              if (aCharInd % 8 == 0)
              {
                NCollection_Utf32Iter aUtfIter(aUtfCharacter);
                Standard_Utf16Char    aStringBuffer[3];
                Standard_Utf16Char*   aUtfPntr = aUtfIter.GetUtf16(aStringBuffer);
                *aUtfPntr++                    = '\0';
                TCollection_ExtendedString aUtfString(aStringBuffer);
                aTempExtString.AssignCat(aUtfString);
                aUtfCharacter[0] = '\0';
              }
              aUtfCharacter[0] = aUtfCharacter[0] << 4;
            }
          }
        }
        isConverted = Standard_True;
      }
      if (isConverted) // find the control directive
      {
        if (toConversion) // else skip moving
        {
          // clang-format off
          aResStringSize -= aStringInd - aSetCharInd - aTempExtString.Length() + 1; // change the string size to remove unused symbols
          // clang-format on
          aResString.SetValue(aSetCharInd, aTempExtString);
          aSetCharInd += aTempExtString.Length(); // move to the new position
          aResString.SetValue(aSetCharInd, aResString.ToExtString() + aStringInd);
          aStringInd = aSetCharInd - 1;
          aResString.Trunc(aResStringSize);
          ;
        }
        aTempExtString.Clear();
        continue;
      }
    }
    if (aStringInd <= aResStringSize - 1)
    {
      const Standard_ExtCharacter aCharNext = aResString.Value(aStringInd + 1);
      if (aCharNext == aChar && (aChar == '\'' || aChar == '\\'))
      {
        // clang-format off
        aResString.SetValue(aSetCharInd, aResString.ToExtString() + aStringInd); // move the string,removing one symbol
        // clang-format on
        aResStringSize--; // change the string size to remove unused symbol
        aResString.Trunc(aResStringSize);
      }
      else if (aChar == '\\')
      {
        const Standard_Boolean isDirective =
          aStringInd <= aResStringSize - 2 && aResString.Value(aStringInd + 2) == '\\';
        if (isDirective)
        {
          if (aCharNext == 'N')
          {
            aResString.SetValue(aSetCharInd++, '\n');
            // clang-format off
            aResString.SetValue(aSetCharInd, aResString.ToExtString() + aStringInd + 2); // move the string,removing two symbols
            aResStringSize-=2; // change the string size to remove unused symbols
            aResString.Trunc(aResStringSize);
            continue;
          }
          else if (aCharNext == 'T')
          {
            aResString.SetValue(aSetCharInd++, '\t');
            aResString.SetValue(aSetCharInd, aResString.ToExtString() + aStringInd + 2); // move the string,removing two symbols
            // clang-format on
            aResStringSize -= 2; // change the string size to remove unused symbols
            aResString.Trunc(aResStringSize);
            continue;
          }
        }
      }
    }
    if (aChar == '\n' || aChar == '\r')
    {
      aResString.SetValue(aSetCharInd, aResString.ToExtString() + aStringInd);
      aResStringSize--;
      aResString.Trunc(aResStringSize);
      aStringInd--;
    }
  }
  theVal->Clear();
  aResString.Trunc(aResStringSize); // trunc the last apostrophe
  TCollection_AsciiString aTmpString(aResString, 0);
  theVal->AssignCat(aTmpString.ToCString());
}

//  -------------  METHODES  -------------

//=================================================================================================

StepData_StepReaderData::StepData_StepReaderData(const Standard_Integer    nbheader,
                                                 const Standard_Integer    nbtotal,
                                                 const Standard_Integer    nbpar,
                                                 const Resource_FormatType theSourceCodePage)
    : Interface_FileReaderData(nbtotal, nbpar),
      theidents(1, nbtotal),
      thetypes(1, nbtotal),
      mySourceCodePage(theSourceCodePage) //, themults (1,nbtotal)
{
  //  char textnum[10];
  thenbscop = 0;
  thenbents = 0;
  thelastn  = 0;
  thenbhead = nbheader;
  // themults.Init(0);
  thecheck = new Interface_Check;
}

//=================================================================================================

void StepData_StepReaderData::SetRecord(const Standard_Integer num,
                                        const Standard_CString ident,
                                        const Standard_CString type,
                                        const Standard_Integer /* nbpar */)
{
  Standard_Integer numlst;

  if (type[0] != '(')
    thenbents++; // total number of proper file terms

  thetypes.ChangeValue(num) = thenametypes.Add(TCollection_AsciiString(type));

  if (ident[0] == '$')
  {
    if (strlen(ident) > 2)
      numlst = atoi(&ident[1]);
    else
      numlst = ident[1] - 48;
    if (thelastn < numlst)
      thelastn = numlst; // highest sub-list number
    theidents.SetValue(num, -2 - numlst);
  }
  else if (ident[0] == '#')
  {
    numlst = atoi(&ident[1]);
    theidents.SetValue(num, numlst);
    if (numlst == 0 && num > thenbhead)
    {
      //    Header, or Complex Type ...
      //    If Complex Type, find Previous Type (we consider this is rare)
      //    Chain the previous type to the next one
      //    VERIFICATION that types are in alphabetical order
      for (Standard_Integer prev = num - 1; prev > thenbhead; prev--)
      {
        if (theidents(prev) >= 0)
        {

          // themults.SetValue(prev,num);
          themults.Bind(prev, num);
          if (thenametypes.FindKey(thetypes.Value(num))
                .IsLess(thenametypes.FindKey(thetypes.Value(prev))))
          {
            //  Warning: components in complex entity are not in alphabetical order.
            TCollection_AsciiString errm("Complex Type incorrect : ");
            errm.AssignCat(thenametypes.FindKey(thetypes.Value(prev)));
            errm.AssignCat(" / ");
            errm.AssignCat(thenametypes.FindKey(thetypes.Value(num)));
            errm.AssignCat(" ... ");
            while (theidents(prev) <= 0)
            {
              prev--;
              if (prev <= 0)
                break;
            }

            Message_Messenger::StreamBuffer sout = Message::SendTrace();
            sout << "  ***  Incorrect record " << num << " (on " << NbRecords() << " -> "
                 << num * 100 / NbRecords() << " % in File)  ***";
            if (prev > 0)
              sout << "  Ident #" << theidents(prev);
            sout << "\n" << errm << std::endl;
            thecheck->AddWarning(errm.ToCString(), "Complex Type incorrect : ");
          }
          break;
        }
      }
    }
  }
  else if (!strcmp(ident, "SCOPE"))
  {
    theidents.SetValue(num, -1); // SCOPE
    thenbscop++;
  }
  else if (!strcmp(ident, "ENDSCOPE"))
    theidents.SetValue(num, -2); // ENDSCOPE
  //      Reste 0

  // InitParams(num);
}

//=================================================================================================

void StepData_StepReaderData::AddStepParam(const Standard_Integer    num,
                                           const Standard_CString    aval,
                                           const Interface_ParamType atype,
                                           const Standard_Integer    nument)
{
  if (atype == Interface_ParamSub)
  {
    Standard_Integer numid = 0;
    if (aval[2] != '\0')
    {
      numid = atoi(&aval[1]);
      //      if (numid <= Maxlst) Interface_FileReaderData::AddParam
      //	(num,subl[numid-1].ToCString(),atype,numid);
      Interface_FileReaderData::AddParam(num, aval, atype, numid);
    }
    else
    {
      char* numlstchar = (char*)(aval + 1);
      numid            = (*numlstchar) - 48; // -48 ('0') -1 (adresse [] depuis 0)
      //      Interface_FileReaderData::AddParam (num,subl[numid].ToCString(),atype,numid);
      Interface_FileReaderData::AddParam(num, aval, atype, numid);
    }
  }
  else if (atype == Interface_ParamIdent)
  {
    Standard_Integer numid = atoi(&aval[1]);
    Interface_FileReaderData::AddParam(num, aval, atype, numid);
  }
  else
  {
    Interface_FileReaderData::AddParam(num, aval, atype, nument);
  }

  //  Interface_FileReaderData::AddParam (num,parval,atype,numid);
}

//=================================================================================================

const TCollection_AsciiString& StepData_StepReaderData::RecordType(const Standard_Integer num) const
{
  return thenametypes.FindKey(thetypes.Value(num));
}

//=================================================================================================

Standard_CString StepData_StepReaderData::CType(const Standard_Integer num) const
{
  return thenametypes.FindKey(thetypes.Value(num)).ToCString();
}

//=================================================================================================

Standard_Integer StepData_StepReaderData::RecordIdent(const Standard_Integer num) const
{
  return theidents(num);
}

//  ########################################################################
//  ....       Parameter reading aids, adapted for STEP       ....

//=================================================================================================

Standard_Integer StepData_StepReaderData::SubListNumber(const Standard_Integer num,
                                                        const Standard_Integer nump,
                                                        const Standard_Boolean aslast) const
{
  if (nump == 0 || nump > NbParams(num))
    return 0;
  const Interface_FileParameter& FP = Param(num, nump);
  if (FP.ParamType() != Interface_ParamSub)
    return 0;
  if (aslast)
  {
    if (nump != NbParams(num))
      return 0;
  }
  return FP.EntityNumber();
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::IsComplex(const Standard_Integer num) const
{
  // return (themults(num) != 0);
  return themults.IsBound(num);
}

//=================================================================================================

void StepData_StepReaderData::ComplexType(const Standard_Integer         num,
                                          TColStd_SequenceOfAsciiString& types) const
{
  if (theidents(num) < 0)
    return;
  for (Standard_Integer i = num; i > 0; i = NextForComplex(i))
  {
    types.Append(RecordType(i));
  }
}

//=================================================================================================

Standard_Integer StepData_StepReaderData::NextForComplex(const Standard_Integer num) const
{
  Standard_Integer next = 0;
  if (themults.IsBound(num))
    next = themults.Find(num);
  return next;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::NamedForComplex(const Standard_CString   name,
                                                          const Standard_Integer   num0,
                                                          Standard_Integer&        num,
                                                          Handle(Interface_Check)& ach) const
{
  // Standard_Boolean stat = Standard_True;
  Standard_Integer n = (num <= 0 ? num0 : NextForComplex(num));
  // sln 04,10.2001. BUC61003. if(n==0) the next  function is not called in order to avoid exception
  if ((n != 0) && (!strcmp(RecordType(n).ToCString(), name)))
  {
    num = n;
    return Standard_True;
  }

  if (n == 0)                            /*stat =*/
    NamedForComplex(name, num0, n, ach); // on a rembobine
                                         //  Not in alphabetical order: loop
  char             txtmes[200];
  Standard_CString errmess = "Parameter n0.%d (%s) not a LIST";
  Sprintf(txtmes, errmess, num0, name);
  for (n = num0; n > 0; n = NextForComplex(n))
  {
    if (!strcmp(RecordType(n).ToCString(), name))
    {
      num     = n;
      errmess = "Complex Record n0.%d, member type %s not in alphabetic order";
      Sprintf(txtmes, errmess, num0, name);
      ach->AddWarning(txtmes, errmess);
      return Standard_False;
    }
  }
  num     = 0;
  errmess = "Complex Record n0.%d, member type %s not found";
  Sprintf(txtmes, errmess, num0, name);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::NamedForComplex(const Standard_CString   theName,
                                                          const Standard_CString   theShortName,
                                                          const Standard_Integer   num0,
                                                          Standard_Integer&        num,
                                                          Handle(Interface_Check)& ach) const
{
  Standard_Integer n = (num <= 0 ? num0 : NextForComplex(num));

  if ((n != 0)
      && (!strcmp(RecordType(n).ToCString(), theName)
          || !strcmp(RecordType(n).ToCString(), theShortName)))
  {
    num = n;
    return Standard_True;
  }

  // entities are not in alphabetical order
  char             txtmes[200];
  Standard_CString errmess = "Parameter n0.%d (%s) not a LIST";
  Sprintf(txtmes, errmess, num0, theName);
  for (n = num0; n > 0; n = NextForComplex(n))
  {
    if (!strcmp(RecordType(n).ToCString(), theName)
        || !strcmp(RecordType(n).ToCString(), theShortName))
    {
      num     = n;
      errmess = "Complex Record n0.%d, member type %s not in alphabetic order";
      Sprintf(txtmes, errmess, num0, theName);
      ach->AddWarning(txtmes, errmess);
      return Standard_False;
    }
  }
  num     = 0;
  errmess = "Complex Record n0.%d, member type %s not found";
  Sprintf(txtmes, errmess, num0, theName);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##

//=================================================================================================

Standard_Boolean StepData_StepReaderData::CheckNbParams(const Standard_Integer   num,
                                                        const Standard_Integer   nbreq,
                                                        Handle(Interface_Check)& ach,
                                                        const Standard_CString   mess) const
{
  if (NbParams(num) == nbreq)
    return Standard_True;
  char             txtmes[200];
  Standard_CString errmess;
  if (mess[0] == '\0')
    errmess = "Count of Parameters is not %d";
  else
    errmess = "Count of Parameters is not %d for %s";
  Sprintf(txtmes, errmess, nbreq, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadSubList(const Standard_Integer   num,
                                                      const Standard_Integer   nump,
                                                      const Standard_CString   mess,
                                                      Handle(Interface_Check)& ach,
                                                      Standard_Integer&        numsub,
                                                      const Standard_Boolean   optional,
                                                      const Standard_Integer /* lenmin */,
                                                      const Standard_Integer /* lenmax */) const
{
  char txtmes[200];
  numsub = SubListNumber(num, nump, Standard_False);
  if (numsub > 0)
  {
    const Standard_Integer aNbParams = NbParams(numsub);
    if (aNbParams == 0)
    {
      Standard_CString anErrMess = "Parameter n0.%d (%s) is an empty LIST";
      Sprintf(txtmes, anErrMess, nump, mess);
      ach->AddWarning(txtmes, anErrMess);
    }
    return Standard_True;
  }
  //  Si optionel indefini, on passe l eponge
  numsub                  = 0;
  Standard_Boolean isvoid = (Param(num, nump).ParamType() == Interface_ParamVoid);
  if (isvoid && optional)
    return Standard_False;

  Standard_CString errmess = "Parameter n0.%d (%s) not a LIST";
  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//  ...   Utilities for LateBinding

//=================================================================================================

Standard_Integer StepData_StepReaderData::ReadSub(const Standard_Integer         numsub,
                                                  const Standard_CString         mess,
                                                  Handle(Interface_Check)&       ach,
                                                  const Handle(StepData_PDescr)& descr,
                                                  Handle(Standard_Transient)&    val) const
{
  Standard_Integer nbp = NbParams(numsub);
  if (nbp == 0)
    return 0; // empty list = Handle Null
  const TCollection_AsciiString& rectyp = RecordType(numsub);
  if (nbp == 1 && rectyp.ToCString()[0] != '(')
  {
    //  it's a type with one parameter -> SelectNamed
    //  cf ReadSelect but here, we are already on the parameter content
    Handle(StepData_SelectNamed) sn = new StepData_SelectNamed;
    val                             = sn;
    sn->SetName(rectyp.ToCString());
    Handle(Standard_Transient) aSN = sn;
    if (ReadAny(numsub, 1, mess, ach, descr, aSN))
      return sn->Kind();
    else
      return 0;
  }

  //  common case: make an HArray1 of ... of ... of what exactly
  const Interface_FileParameter&          FP0 = Param(numsub, 1);
  Interface_ParamType                     FT, FT0 = FP0.ParamType();
  Standard_CString                        str = FP0.CValue();
  Handle(TColStd_HArray1OfTransient)      htr;
  Handle(TColStd_HArray1OfInteger)        hin;
  Handle(TColStd_HArray1OfReal)           hre;
  Handle(Interface_HArray1OfHAsciiString) hst;
  Standard_Integer                        kod = 0;
  switch (FT0)
  {
    case Interface_ParamMisc:
      return -1;
    case Interface_ParamInteger:
      kod = 1;
      break;
    case Interface_ParamReal:
      kod = 5;
      break;
    case Interface_ParamIdent:
      kod = 7;
      break;
    case Interface_ParamVoid:
      kod = 0;
      break;
    case Interface_ParamText:
      kod = 6;
      break;
    case Interface_ParamEnum:
      kod = 4;
      break; // a confirmer(logical)
      /*      kod = 4;
        if ( str[0] == '.' && str[2] == '.' && str[3] == '\0' &&
        (str[1] == 'T' || str[1] == 'F' || str[1] == 'U') ) kod = 3;
        break; */ // svv #2
    case Interface_ParamLogical:
      return -1;
    case Interface_ParamSub:
      kod = 0;
      break;
    case Interface_ParamHexa:
      return -1;
    case Interface_ParamBinary:
      return -1;
    default:
      return -1;
  }
  if (kod == 1 || kod == 3)
  {
    hin = new TColStd_HArray1OfInteger(1, nbp);
    val = hin;
  }
  else if (kod == 5)
  {
    hre = new TColStd_HArray1OfReal(1, nbp);
    val = hre;
  }
  else if (kod == 6)
  {
    hst = new Interface_HArray1OfHAsciiString(1, nbp);
    val = hst;
  }
  else
  {
    htr = new TColStd_HArray1OfTransient(1, nbp);
    val = htr;
  }
  //  Attention: if variable type, will need to change approach -> htr

  for (Standard_Integer ip = 1; ip <= nbp; ip++)
  {
    const Interface_FileParameter& FP = Param(numsub, ip);
    str                               = FP.CValue();
    FT                                = FP.ParamType();
    switch (kod)
    {
      case 1: {
        if (FT != Interface_ParamInteger)
        {
          kod = 0;
          break;
        }
        hin->SetValue(ip, atoi(str));
        break;
      }
      case 2:
      case 3: {
        if (FT != Interface_ParamEnum)
        {
          kod = 0;
          break;
        }
        if (!strcmp(str, ".F."))
          hin->SetValue(ip, 0);
        else if (!strcmp(str, ".T."))
          hin->SetValue(ip, 1);
        else if (!strcmp(str, ".U."))
          hin->SetValue(ip, 2);
        else
          kod = 0;
        break;
      }
      case 4: {
        if (FT != Interface_ParamEnum)
        {
          kod = 0;
          break;
        }
        Handle(StepData_SelectNamed) sn = new StepData_SelectNamed;
        sn->SetEnum(-1, str);
        htr->SetValue(ip, sn);
        break;
      }
      case 5: {
        if (FT != Interface_ParamReal)
        {
          kod = 0;
          break;
        }
        hre->SetValue(ip, Interface_FileReaderData::Fastof(str));
        break;
      }
      case 6: {
        if (FT != Interface_ParamText)
        {
          kod = 0;
          break;
        }
        Handle(TCollection_HAsciiString) txt = new TCollection_HAsciiString(str);
        cleanText(txt);
        hst->SetValue(ip, txt);
        break;
      }
      case 7: {
        Handle(Standard_Transient) ent = BoundEntity(FP.EntityNumber());
        htr->SetValue(ip, ent);
        break;
      }
      default:
        break;
    }
    //    Remaining other cases ... everything is possible. cf the Param type
    if (kod > 0)
      continue;
    //    Need to pass to transient ...
    if (htr.IsNull())
    {
      htr = new TColStd_HArray1OfTransient(1, nbp);
      val = htr;
      Standard_Integer jp;
      if (!hin.IsNull())
      {
        for (jp = 1; jp < ip; jp++)
        {
          Handle(StepData_SelectInt) sin = new StepData_SelectInt;
          sin->SetInt(hin->Value(jp));
          htr->SetValue(jp, sin);
        }
      }
      if (!hre.IsNull())
      {
        for (jp = 1; jp < ip; jp++)
        {
          Handle(StepData_SelectReal) sre = new StepData_SelectReal;
          sre->SetReal(hre->Value(jp));
          htr->SetValue(jp, sre);
        }
      }
      if (!hst.IsNull())
      {
        for (jp = 1; jp < ip; jp++)
        {
          htr->SetValue(jp, hst->Value(jp));
        }
      }
    }
    //    Now, let's go: read the field and put it in place
    //    What follows strongly resembles ReadAny ...

    switch (FT)
    {
      case Interface_ParamMisc:
        break;
      case Interface_ParamInteger: {
        Handle(StepData_SelectInt) sin = new StepData_SelectInt;
        sin->SetInteger(atoi(str));
        htr->SetValue(ip, sin);
        break;
      }
      case Interface_ParamReal: {
        Handle(StepData_SelectReal) sre = new StepData_SelectReal;
        sre->SetReal(Interface_FileReaderData::Fastof(str));
        break;
        // htr->SetValue (ip,sre); break; svv #2: unreachable
      }
      case Interface_ParamIdent:
        htr->SetValue(ip, BoundEntity(FP.EntityNumber()));
        break;
      case Interface_ParamVoid:
        break;
      case Interface_ParamEnum: {
        Handle(StepData_SelectInt)   sin;
        Handle(StepData_SelectNamed) sna;
        Standard_Integer             logic = -1;
        // PTV 16.09.2000
        // set the default value of StepData_Logical
        StepData_Logical slog = StepData_LUnknown;
        if (str[0] == '.' && str[2] == '.' && str[3] == '\0')
        {
          if (str[1] == 'F')
          {
            slog  = StepData_LFalse;
            logic = 0;
          }
          else if (str[1] == 'T')
          {
            slog  = StepData_LTrue;
            logic = 1;
          }
          else if (str[1] == 'U')
          {
            slog  = StepData_LUnknown;
            logic = 2;
          }
        }
        if (logic >= 0)
        {
          sin = new StepData_SelectInt;
          sin->SetLogical(slog);
          htr->SetValue(ip, sin);
        }
        else
        {
          sna = new StepData_SelectNamed;
          sna->SetEnum(logic, str);
          htr->SetValue(ip, sna);
        }
        break;
      }
      case Interface_ParamLogical:
        break;
      case Interface_ParamText: {
        Handle(TCollection_HAsciiString) txt = new TCollection_HAsciiString(str);
        cleanText(txt);
        htr->SetValue(ip, txt);
        break;
      }
      case Interface_ParamSub: {
        Handle(Standard_Transient) sub;
        Standard_Integer           nent = FP.EntityNumber();
        Standard_Integer           kind = ReadSub(nent, mess, ach, descr, sub);
        if (kind < 0)
          break;
        htr->SetValue(ip, sub);
        break;
      }
      case Interface_ParamHexa:
        break;
      case Interface_ParamBinary:
        break;
      default:
        break;
    }
    return -1;
  }
  return 8; // for Any
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadMember(const Standard_Integer         num,
                                                     const Standard_Integer         nump,
                                                     const Standard_CString         mess,
                                                     Handle(Interface_Check)&       ach,
                                                     Handle(StepData_SelectMember)& val) const
{
  Handle(Standard_Transient) v = val;
  Handle(StepData_PDescr)    nuldescr;
  if (v.IsNull())
  {
    return ReadAny(num, nump, mess, ach, nuldescr, v)
           && !(val = Handle(StepData_SelectMember)::DownCast(v)).IsNull();
  }
  Standard_Boolean res = ReadAny(num, nump, mess, ach, nuldescr, v);
  if (v == val)
    return res;
  //   changement -> refus
  char             txtmes[200];
  Standard_CString errmess = "Parameter n0.%d (%s) : does not match SELECT clause";
  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadField(const Standard_Integer         num,
                                                    const Standard_Integer         nump,
                                                    const Standard_CString         mess,
                                                    Handle(Interface_Check)&       ach,
                                                    const Handle(StepData_PDescr)& descr,
                                                    StepData_Field&                fild) const
{
  const Interface_FileParameter& FP  = Param(num, nump);
  Standard_CString               str = FP.CValue();
  Standard_Boolean               OK  = Standard_True;
  Standard_Integer               nent, kind;
  Handle(Standard_Transient)     sub;
  Interface_ParamType            FT = FP.ParamType();
  switch (FT)
  {
    case Interface_ParamMisc:
      OK = Standard_False;
      break;
    case Interface_ParamInteger:
      fild.SetInteger(atoi(str));
      break;
    case Interface_ParamReal:
      fild.SetReal(Interface_FileReaderData::Fastof(str));
      break;
    case Interface_ParamIdent:
      nent = FP.EntityNumber();
      if (nent > 0)
        fild.SetEntity(BoundEntity(nent));
      break;
    case Interface_ParamVoid:
      break;
    case Interface_ParamText: {
      Handle(TCollection_HAsciiString) txt = new TCollection_HAsciiString(str);
      cleanText(txt);
      fild.Set(txt);
      break;
    }
    case Interface_ParamEnum:
      if (!strcmp(str, ".T."))
        fild.SetLogical(StepData_LTrue);
      else if (!strcmp(str, ".F."))
        fild.SetLogical(StepData_LFalse);
      else if (!strcmp(str, ".U."))
        fild.SetLogical(StepData_LUnknown);
      else
        fild.SetEnum(-1, str);
      break;
    case Interface_ParamLogical:
      OK = Standard_False;
      break;
    case Interface_ParamSub:
      nent = FP.EntityNumber();
      kind = ReadSub(nent, mess, ach, descr, sub);
      if (kind < 0)
        break;
      fild.Clear(kind);
      fild.Set(sub);
      break;
    case Interface_ParamHexa:
      OK = Standard_False;
      break;
    case Interface_ParamBinary:
      OK = Standard_False;
      break;
    default:
      OK = Standard_False;
      break;
  }

  if (!OK)
  {
    if (!strcmp(str, "*"))
      fild.SetDerived();
  }
  return Standard_True;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadList(const Standard_Integer          num,
                                                   Handle(Interface_Check)&        ach,
                                                   const Handle(StepData_ESDescr)& descr,
                                                   StepData_FieldList&             list) const
{
  // controler nbs egaux
  Standard_Integer i, nb = list.NbFields();
  if (!CheckNbParams(num, nb, ach, descr->TypeName()))
    return Standard_False;
  for (i = 1; i <= nb; i++)
  {
    Handle(StepData_PDescr) pde  = descr->Field(i);
    StepData_Field&         fild = list.CField(i);
    ReadField(num, i, pde->Name(), ach, pde, fild);
  }
  return Standard_True;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadAny(const Standard_Integer         num,
                                                  const Standard_Integer         nump,
                                                  const Standard_CString         mess,
                                                  Handle(Interface_Check)&       ach,
                                                  const Handle(StepData_PDescr)& descr,
                                                  Handle(Standard_Transient)&    val) const
{
  const Interface_FileParameter& FP  = Param(num, nump);
  Standard_CString               str = FP.CValue();
  Interface_ParamType            FT  = FP.ParamType();

  //    Now, let's go: read the field and put it in place
  switch (FT)
  {
    case Interface_ParamMisc:
      break;
    case Interface_ParamInteger: {
      if (!val.IsNull())
      {
        DeclareAndCast(StepData_SelectMember, sm, val);
        sm->SetReal(Interface_FileReaderData::Fastof(str));
        return Standard_True;
      }
      Handle(StepData_SelectInt) sin = new StepData_SelectInt;
      sin->SetInteger(atoi(str));
      val = sin;
      return Standard_True;
    }
    case Interface_ParamReal: {
      if (!val.IsNull())
      {
        DeclareAndCast(StepData_SelectMember, sm, val);
        sm->SetReal(Interface_FileReaderData::Fastof(str));
        return Standard_True;
      }
      Handle(StepData_SelectReal) sre = new StepData_SelectReal;
      sre->SetReal(Interface_FileReaderData::Fastof(str));
      val = sre;
      return Standard_True;
    }
    case Interface_ParamIdent: {
      Standard_Integer nent = FP.EntityNumber();
      if (nent > 0)
        val = BoundEntity(nent);
      return (!val.IsNull());
    }
    case Interface_ParamVoid:
      break;
    case Interface_ParamEnum: {
      Handle(StepData_SelectMember) sm;
      if (!val.IsNull())
        sm = GetCasted(StepData_SelectMember, val);
      Handle(StepData_SelectInt)   sin;
      Handle(StepData_SelectNamed) sna;
      Standard_Integer             logic = -1;

      // PTV 16.09.2000
      // set the default value of StepData_Logical
      StepData_Logical slog = StepData_LUnknown;
      if (str[0] == '.' && str[2] == '.' && str[3] == '\0')
      {
        if (str[1] == 'F')
        {
          slog  = StepData_LFalse;
          logic = 0;
        }
        else if (str[1] == 'T')
        {
          slog  = StepData_LTrue;
          logic = 1;
        }
        else if (str[1] == 'U')
        {
          slog  = StepData_LUnknown;
          logic = 2;
        }
      }
      if (logic >= 0)
      {
        if (!sm.IsNull())
          sm->SetLogical(slog);
        else
        {
          sin = new StepData_SelectInt;
          val = sin;
          sin->SetLogical(slog);
        }
      }
      else
      {
        if (!sm.IsNull())
          sm->SetEnum(logic, str);
        else
        {
          sna = new StepData_SelectNamed;
          val = sna; // Named sans nom...
          sna->SetEnum(logic, str);
        }
      } // -> Select general
      return Standard_True;
    }
    case Interface_ParamLogical:
      break;
    case Interface_ParamText: {
      Handle(TCollection_HAsciiString) txt = new TCollection_HAsciiString(str);
      cleanText(txt);

      // PDN May 2000: for reading SOURCE_ITEM (external references)
      if (!val.IsNull())
      {
        DeclareAndCast(StepData_SelectMember, sm, val);
        sm->SetString(txt->ToCString());
        return Standard_True;
      }

      val = txt;
      return Standard_True;
    }
    case Interface_ParamSub: {
      Standard_Integer numsub = SubListNumber(num, nump, Standard_False);
      Standard_Integer nbp    = NbParams(numsub);
      if (nbp == 0)
        return Standard_False; // empty list = Handle Null
      const TCollection_AsciiString& rectyp = RecordType(numsub);
      if (nbp == 1 && rectyp.ToCString()[0] != '(')
      {
        //  SelectNamed because Field !!!
        // skl 15.01.2003 (for members with array of real)
        DeclareAndCast(StepData_SelectArrReal, sma, val);
        if (!sma.IsNull())
        {
          Standard_Integer numsub2 = SubListNumber(numsub, 1, Standard_False);
          Standard_Integer nbp2    = NbParams(numsub2);
          if (nbp2 > 1)
          {
            if (Param(numsub2, 1).ParamType() == Interface_ParamReal)
            {
              if (!sma->SetName(rectyp.ToCString()))
                return Standard_False;
              Handle(TColStd_HSequenceOfReal) aSeq = new TColStd_HSequenceOfReal;
              for (Standard_Integer i = 1; i <= nbp2; i++)
              {
                if (Param(numsub2, i).ParamType() != Interface_ParamReal)
                  continue;
                Handle(Standard_Transient) asr = new StepData_SelectReal;
                if (!ReadAny(numsub2, i, mess, ach, descr, asr))
                  continue;
                Handle(StepData_SelectReal) sm1 = Handle(StepData_SelectReal)::DownCast(asr);
                if (!sm1.IsNull())
                  aSeq->Append(sm1->Real());
              }
              Handle(TColStd_HArray1OfReal) anArr = new TColStd_HArray1OfReal(1, aSeq->Length());
              for (Standard_Integer nr = 1; nr <= aSeq->Length(); nr++)
              {
                anArr->SetValue(nr, aSeq->Value(nr));
              }
              sma->SetArrReal(anArr);
              return Standard_True;
            }
          }
        }
        DeclareAndCast(StepData_SelectMember, sm, val);
        if (sm.IsNull())
        {
          sm  = new StepData_SelectNamed;
          val = sm;
        }
        if (!sm->SetName(rectyp.ToCString()))
          return Standard_False; // loupe
        return ReadAny(numsub, 1, mess, ach, descr, val);
      }
    }
    default:
      break;
  }
  return Standard_False;
}

//  ....

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadXY(const Standard_Integer   num,
                                                 const Standard_Integer   nump,
                                                 const Standard_CString   mess,
                                                 Handle(Interface_Check)& ach,
                                                 Standard_Real&           X,
                                                 Standard_Real&           Y) const
{
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null if no error
  Standard_Integer numsub  = SubListNumber(num, nump, Standard_False);
  if (numsub != 0)
  {
    if (NbParams(numsub) == 2)
    {
      const Interface_FileParameter& FPX = Param(numsub, 1);
      if (FPX.ParamType() == Interface_ParamReal)
        X = Interface_FileReaderData::Fastof(FPX.CValue());
      else
        errmess = "Parameter n0.%d (%s) : (X,Y) X not a Real";

      const Interface_FileParameter& FPY = Param(numsub, 2);
      if (FPY.ParamType() == Interface_ParamReal)
        Y = Interface_FileReaderData::Fastof(FPY.CValue());
      else
        errmess = "Parameter n0.%d (%s) : (X,Y) Y not a Real";
    }
    else
      errmess = "Parameter n0.%d (%s) : (X,Y) has not 2 params";
  }
  else
    errmess = "Parameter n0.%d (%s) : (X,Y) not a SubList";

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadXYZ(const Standard_Integer   num,
                                                  const Standard_Integer   nump,
                                                  const Standard_CString   mess,
                                                  Handle(Interface_Check)& ach,
                                                  Standard_Real&           X,
                                                  Standard_Real&           Y,
                                                  Standard_Real&           Z) const
{
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null if no error
  Standard_Integer numsub  = SubListNumber(num, nump, Standard_False);
  if (numsub != 0)
  {
    if (NbParams(numsub) == 3)
    {
      const Interface_FileParameter& FPX = Param(numsub, 1);
      if (FPX.ParamType() == Interface_ParamReal)
        X = Interface_FileReaderData::Fastof(FPX.CValue());
      else
        errmess = "Parameter n0.%d (%s) : (X,Y,Z) X not a Real";

      const Interface_FileParameter& FPY = Param(numsub, 2);
      if (FPY.ParamType() == Interface_ParamReal)
        Y = Interface_FileReaderData::Fastof(FPY.CValue());
      else
        errmess = "Parameter n0.%d (%s) : (X,Y,Z) Y not a Real";

      const Interface_FileParameter& FPZ = Param(numsub, 3);
      if (FPZ.ParamType() == Interface_ParamReal)
        Z = Interface_FileReaderData::Fastof(FPZ.CValue());
      else
        errmess = "Parameter n0.%d (%s) : (X,Y,Z) Z not a Real";
    }
    else
      errmess = "Parameter n0.%d (%s) : (X,Y,Z) has not 3 params";
  }
  else
    errmess = "Parameter n0.%d (%s) : (X,Y,Z) not a SubList";

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadReal(const Standard_Integer   num,
                                                   const Standard_Integer   nump,
                                                   const Standard_CString   mess,
                                                   Handle(Interface_Check)& ach,
                                                   Standard_Real&           val) const
{
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null si pas d erreur
  if (nump > 0 && nump <= NbParams(num))
  {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamReal || FP.ParamType() == Interface_ParamInteger)
      val = Interface_FileReaderData::Fastof(FP.CValue());
    else
      errmess = "Parameter n0.%d (%s) not a Real";
  }
  else
    errmess = "Parameter n0.%d (%s) absent";

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadEntity(const Standard_Integer       num,
                                                     const Standard_Integer       nump,
                                                     const Standard_CString       mess,
                                                     Handle(Interface_Check)&     ach,
                                                     const Handle(Standard_Type)& atype,
                                                     Handle(Standard_Transient)&  ent) const
{
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null if no error
  if (nump > 0 && nump <= NbParams(num))
  {
    const Interface_FileParameter& FP   = Param(num, nump);
    Standard_Integer               nent = FP.EntityNumber();
    if (FP.ParamType() == Interface_ParamIdent)
    {
      if (nent > 0)
      {
        Handle(Standard_Transient) entent = BoundEntity(nent);
        if (entent.IsNull() || !entent->IsKind(atype))
        {
          errmess = "Parameter n0.%d (%s) : Entity has illegal type";
          if (!entent.IsNull() && entent->IsKind(STANDARD_TYPE(StepData_UndefinedEntity)))
            ent = entent;
        }
        else
          ent = entent;
      }
      else
        errmess = "Parameter n0.%d (%s) : Unresolved reference";
    }
    else
    {
      errmess = "Parameter n0.%d (%s) not an Entity";
    }
  }
  else
  {
    errmess = "Parameter n0.%d (%s) absent";
  }

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadEntity(const Standard_Integer   num,
                                                     const Standard_Integer   nump,
                                                     const Standard_CString   mess,
                                                     Handle(Interface_Check)& ach,
                                                     StepData_SelectType&     sel) const
{
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null if no error
  if (nump > 0 && nump <= NbParams(num))
  {
    const Interface_FileParameter& FP   = Param(num, nump);
    Standard_Integer               nent = FP.EntityNumber();
    if (FP.ParamType() == Interface_ParamIdent)
    {
      if (nent > 0)
      {
        Handle(Standard_Transient) entent = BoundEntity(nent);
        if (!sel.Matches(entent))
        {
          errmess = "Parameter n0.%d (%s) : Entity has illegal type";
          // fot not supported STEP entity
          if (!entent.IsNull() && entent->IsKind(STANDARD_TYPE(StepData_UndefinedEntity)))
            sel.SetValue(entent);
        }
        else
          sel.SetValue(entent);
      }
      else
        errmess = "Parameter n0.%d (%s) : Unresolved reference";
    }
    else if (FP.ParamType() == Interface_ParamVoid)
    {
      errmess = "Parameter n0.%d (%s) not an Entity";
    }
    else
    {
      // Remaining case: we are actually interested in the SelectMember ...
      Handle(Standard_Transient) sm = sel.NewMember();
      // SelectMember which performs this role. Can be specialized
      if (!ReadAny(num, nump, mess, ach, sel.Description(), sm))
        errmess = "Parameter n0.%d (%s) : could not be read";
      if (!sel.Matches(sm))
        errmess = "Parameter n0.%d (%s) : illegal parameter type";
      else
        sel.SetValue(sm);
    }
  }
  else
  {
    errmess = "Parameter n0.%d (%s) absent";
  }

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadInteger(const Standard_Integer   num,
                                                      const Standard_Integer   nump,
                                                      const Standard_CString   mess,
                                                      Handle(Interface_Check)& ach,
                                                      Standard_Integer&        val) const
{
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null if no error
  if (nump > 0 && nump <= NbParams(num))
  {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamInteger)
      val = atoi(FP.CValue());
    else if (FP.ParamType() == Interface_ParamReal)
    {
      val =
        static_cast<Standard_Integer>(std::round(Interface_FileReaderData::Fastof(FP.CValue())));
      errmess = "Parameter n0.%d (%s) was rounded";
    }
    if (FP.ParamType() != Interface_ParamInteger && FP.ParamType() != Interface_ParamReal)
      errmess = "Parameter n0.%d (%s) not an Integer";
  }
  else
    errmess = "Parameter n0.%d (%s) absent";

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadBoolean(const Standard_Integer   num,
                                                      const Standard_Integer   nump,
                                                      const Standard_CString   mess,
                                                      Handle(Interface_Check)& ach,
                                                      Standard_Boolean&        flag) const
{
  char txtmes[200];
  flag                     = Standard_True;
  Standard_CString errmess = nullptr; // Null si pas d erreur
  if (nump > 0 && nump <= NbParams(num))
  {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamEnum)
    {
      Standard_CString txt = FP.CValue();
      if (!strcmp(txt, ".T."))
        flag = Standard_True;
      else if (!strcmp(txt, ".F."))
        flag = Standard_False;
      else
        errmess = "Parameter n0.%d (%s) : Incorrect Boolean Value. It was set to true";
    }
    else
      errmess = "Parameter n0.%d (%s) not a Boolean. It was set to true";
  }
  else
    errmess = "Parameter n0.%d (%s) absent.It was set to true";

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadLogical(const Standard_Integer   num,
                                                      const Standard_Integer   nump,
                                                      const Standard_CString   mess,
                                                      Handle(Interface_Check)& ach,
                                                      StepData_Logical&        flag) const
{
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null si pas d erreur
  if (nump > 0 && nump <= NbParams(num))
  {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamEnum)
    {
      Standard_CString txt = FP.CValue();
      if (!strcmp(txt, ".T."))
        flag = StepData_LTrue;
      else if (!strcmp(txt, ".F."))
        flag = StepData_LFalse;
      else if (!strcmp(txt, ".U."))
        flag = StepData_LUnknown;
      else
        errmess = "Parameter n0.%d (%s) : Incorrect Logical Value";
    }
    else
      errmess = "Parameter n0.%d (%s) not a Logical";
  }
  else
    errmess = "Parameter n0.%d (%s) absent";

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadString(const Standard_Integer            num,
                                                     const Standard_Integer            nump,
                                                     const Standard_CString            mess,
                                                     Handle(Interface_Check)&          ach,
                                                     Handle(TCollection_HAsciiString)& val) const
{
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null if no error
  if (nump > 0 && nump <= NbParams(num))
  {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamText)
    {
      /*Standard_CString anStr = FP.CValue();
      if(strlen(anStr) < 3)
        val = TCollection_AsciiString("");
      else {
        val = TCollection_AsciiString(FP.CValue());
        CleanText (val);
      }*/
      val = new TCollection_HAsciiString(FP.CValue());
      cleanText(val);
    }
    else
    {
      errmess = "Parameter n0.%d (%s) not a quoted String";
    }
  }
  else
    errmess = "Parameter n0.%d (%s) absent";

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadEnumParam(const Standard_Integer   num,
                                                        const Standard_Integer   nump,
                                                        const Standard_CString   mess,
                                                        Handle(Interface_Check)& ach,
                                                        Standard_CString&        text) const
{
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null if no error
  if (nump > 0 && nump <= NbParams(num))
  {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamEnum)
    {
      text = FP.CValue();
    }
    else if (FP.ParamType() == Interface_ParamVoid)
    {
      errmess = "Parameter n0.%d (%s) : Undefined Enumeration not allowed";
    }
    else
      errmess = "Parameter n0.%d (%s) not an Enumeration";
  }
  else
    errmess = "Parameter n0.%d (%s) absent";

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

void StepData_StepReaderData::FailEnumValue(const Standard_Integer /* num */,
                                            const Standard_Integer   nump,
                                            const Standard_CString   mess,
                                            Handle(Interface_Check)& ach) const
{
  char             txtmes[200];
  Standard_CString errmess = "Parameter n0.%d (%s) : Incorrect Enumeration Value";
  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadEnum(const Standard_Integer   num,
                                                   const Standard_Integer   nump,
                                                   const Standard_CString   mess,
                                                   Handle(Interface_Check)& ach,
                                                   const StepData_EnumTool& enumtool,
                                                   Standard_Integer&        val) const
{
  //  resume with ReadEnumParam?
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null if no error
  if (nump > 0 && nump <= NbParams(num))
  {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamEnum)
    {
      val = enumtool.Value(FP.CValue());
      if (val >= 0)
        return Standard_True;
      else
        errmess = "Parameter n0.%d (%s) : Incorrect Enumeration Value";
    }
    else if (FP.ParamType() == Interface_ParamVoid)
    {
      val = enumtool.NullValue();
      if (val < 0)
        errmess = "Parameter n0.%d (%s) : Undefined Enumeration not allowed";
    }
    else
      errmess = "Parameter n0.%d (%s) not an Enumeration";
  }
  else
    errmess = "Parameter n0.%d (%s) absent";

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::ReadTypedParam(const Standard_Integer   num,
                                                         const Standard_Integer   nump,
                                                         const Standard_Boolean   mustbetyped,
                                                         const Standard_CString   mess,
                                                         Handle(Interface_Check)& ach,
                                                         Standard_Integer&        numr,
                                                         Standard_Integer&        numrp,
                                                         TCollection_AsciiString& typ) const
{
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null si pas d erreur
  if (nump > 0 && nump <= NbParams(num))
  {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() != Interface_ParamSub)
    {
      //    Not a sub-list: OK if allowed
      numr  = num;
      numrp = nump;
      typ.Clear();
      if (mustbetyped)
      {
        errmess = "Parameter n0.%d (%s) : single, not typed";
        Sprintf(txtmes, errmess, nump, mess);
        ach->AddFail(txtmes, errmess);
        return Standard_False;
      }
      return Standard_True;
    }
    numr  = FP.EntityNumber();
    numrp = 1;
    if (NbParams(numr) != 1)
      errmess = "Parameter n0.%d (%s) : SubList, not typed";
    typ = RecordType(numr);
  }
  else
    errmess = "Parameter n0.%d (%s) absent";

  if (errmess == nullptr)
    return Standard_True;
  Sprintf(txtmes, errmess, nump, mess);
  ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//=================================================================================================

Standard_Boolean StepData_StepReaderData::CheckDerived(const Standard_Integer   num,
                                                       const Standard_Integer   nump,
                                                       const Standard_CString   mess,
                                                       Handle(Interface_Check)& ach,
                                                       const Standard_Boolean   errstat) const
{
  char             txtmes[200];
  Standard_CString errmess = nullptr; // Null if no error
  Standard_Boolean warn    = !errstat;
  if (nump > 0 && nump <= NbParams(num))
  {
    if (!strcmp(Param(num, nump).CValue(), "*"))
      return Standard_True;
    else
      errmess = "Parameter n0.%d (%s) not Derived";
  }
  else
    errmess = "Parameter n0.%d (%s) absent";

  if (errmess == nullptr)
    return Standard_True;

  Sprintf(txtmes, errmess, nump, mess);
  if (warn)
    ach->AddWarning(txtmes, errmess);
  else
    ach->AddFail(txtmes, errmess);
  return Standard_False;
}

//  #########################################################################
// ....     Specific methods (requested by FileReaderData)     .... //

//=================================================================================================

Standard_Integer StepData_StepReaderData::NbEntities() const // redefined
{
  return thenbents;
}

//=================================================================================================

Standard_Integer StepData_StepReaderData::FindNextRecord(const Standard_Integer num) const
{
  // returns, for a given record number (by num), the next one which
  // defines an entity, or 0 if finished:
  // passes the Header (first nbhend records) and
  // skips SCOPE and ENDSCOPE records and SUB-LISTS

  if (num < 0)
    return 0;
  Standard_Integer num1 = num + 1;
  if (num == 0)
    num1 = thenbhead + 1;
  Standard_Integer max = NbRecords();

  while (num1 <= max)
  {
    if (theidents(num1) > 0)
      return num1;

    // SCOPE,ENDSCOPE and Sub-List have a fictitious identifier: -1,-2 respectively
    // and SUBLIST have a negative one. Only a real entity has a positive Ident
    num1++;
  }
  return 0;
}

//=================================================================================================

Standard_Integer StepData_StepReaderData::FindEntityNumber(const Standard_Integer num,
                                                           const Standard_Integer id) const
{
  //  Given an "Id": search in the Ident type Parameters of <num>,
  //  if one of them designates #Id precisely. If yes, return its EntityNumber
  if (num == 0)
    return 0;
  Standard_Integer nb = NbParams(num);
  for (Standard_Integer i = 1; i <= nb; i++)
  {
    const Interface_FileParameter& FP = Param(num, i);
    if (FP.ParamType() != Interface_ParamIdent)
      continue;
    Standard_Integer ixp = atoi(&FP.CValue()[1]);
    if (ixp == id)
      return FP.EntityNumber();
  }
  return 0; // ici, pas trouve
}

//  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
// ....         The following function deserves special attention        ....

//  This method preloads the EntityNumbers in the Params: they designate
//  the Entities properly said in the list read by BoundEntity
//  Interest: also addresses sub-lists (Num->record number in the Directory)
//  result exploited by ParamEntity and ParamNumber
//
//  This is a critical optimization that resolves entity references during loading
//  rather than during each access, significantly improving performance for large files

//  In the absence of SCOPE, or if the "ident" are strictly ordered, for sure
//  they are not duplicated, we can use an IndexedMap with full
//  confidence. Otherwise, we must scan the file, but with SCOPES
//  this goes much faster (if they are big enough): we find our way around.

// For the search by scanning, We operate in several steps
// Before anything, the loading has already done preparation: the idents
// (Entity, SubList) are already in integers (reading speed), in particular
// in the EntityNumber: thus, we read this ident, we process it, and we put back
// in its place a real Record number
//
// First, we pass the directory to integer table, sub-lists purged
// in parallel, inverse table towards this table, because sub-lists can on the
// contrary designate objects ...

// For sub-lists, we exploit their construction mode: they are
// recorded BEFORE being referenced. A "subn" array thus notes for
// each sub-list number (relative to an entity that follows, and referenced
// by it or another sub-list that also follows), its record number
// NOTE: this also works for the Header, processed on the occasion

//=================================================================================================

void StepData_StepReaderData::SetEntityNumbers(const Standard_Boolean withmap)
{
  Message_Messenger::StreamBuffer sout = Message::SendTrace();
  //   Initial pass: Direct resolution by Map
  //   if everything passes (no collision), OK. Otherwise, other passes to plan
  //   We resolve sub-lists at the same time
  //   The Map approach is O(1) lookup but requires unique identifiers
  //   If identifiers collide (due to SCOPE sections), we fall back to linear search
  Standard_Integer                 nbdirec = NbRecords();
  Handle(NCollection_IncAllocator) anAlloc =
    new NCollection_IncAllocator(NCollection_IncAllocator::THE_MINIMUM_BLOCK_SIZE);
  TColStd_Array1OfInteger subn(0, thelastn);

  Standard_Boolean            pbmap = Standard_False; // at least one conflict
  Standard_Integer            nbmap = 0;
  TColStd_IndexedMapOfInteger imap(thenbents, anAlloc);
  TColStd_Array1OfInteger     indm(0, nbdirec); // Index Map -> Record Number (only if map)

  Standard_Integer num; // svv Jan11 2000 : porting on DEC
  for (num = 1; num <= nbdirec; num++)
  {
    Standard_Integer ident = theidents(num);
    if (ident > 0)
    { // Ident normal -> Map ?
      //  Map: if Overlap, inhibit it. Otherwise, note index
      Standard_Integer indmap = imap.Add(ident);
      if (indmap <= nbmap)
      {
        indmap       = imap.FindIndex(ident); // safer
        indm(indmap) = -1;                    // Map -> pb
        pbmap        = Standard_True;
        //  pbmap means another pass will be necessary ...
      }
      else
      {
        nbmap        = indmap;
        indm(indmap) = num; // Map ->ident
      }
    }
  }

  for (num = 1; num <= nbdirec; num++)
  {
    Standard_Integer ident = theidents(num);
    if (ident < -2)
      subn(-(ident + 2)) = num; // toujours a jour ...

    Standard_Integer nba = NbParams(num);
    Standard_Integer nda = (num == 1 ? 0 : ParamFirstRank(num - 1));

    for (Standard_Integer na = nba; na > 0; na--)
    {
      //    We process: sub-lists (except subn), idents (if Map says OK ...)
      Interface_FileParameter& FP = ChangeParameter(nda + na);
      //      Interface_FileParameter& FP = ChangeParam (num,na);
      Interface_ParamType letype = FP.ParamType();
      if (letype == Interface_ParamSub)
      {
        Standard_Integer numsub = FP.EntityNumber();
        if (numsub > thelastn)
        {
          Message::SendInfo() << "Bad Sub.N0, Record " << num << " Param " << na << ":$" << numsub
                              << std::endl;
          continue;
        }
        FP.SetEntityNumber(subn(numsub));
      }
      else if (letype == Interface_ParamIdent)
      {
        Standard_Integer id     = FP.EntityNumber();
        Standard_Integer indmap = imap.FindIndex(id);
        if (indmap > 0)
        { // the map found it
          Standard_Integer num0 = indm(indmap);
          if (num0 > 0)
            FP.SetEntityNumber(num0); // AND THERE, we have resolved
          else
            FP.SetEntityNumber(-id); // CONFLICT -> will need to resolve ...
        }
        else
        { // NOT RESOLVED, if no pbmap, say it
          if (pbmap)
          {
            FP.SetEntityNumber(-id);
            continue; // pbmap: we will find ourselves again
          }
          char failmess[100];
          //  ...  Build the Check  ...
          Sprintf(failmess, "Unresolved Reference, Ent.Id.#%d Param.n0 %d (Id.#%d)", ident, na, id);
          thecheck->AddFail(failmess, "Unresolved Reference");
          //  ...  And output a more complete message
          sout << "*** ERR StepReaderData *** Entite #" << ident << "\n    Type:" << RecordType(num)
               << "  Param.n0 " << na << ": #" << id << " Not found" << std::endl;
        } // END  Mapping
      } // END  Reference Processing
    } // END  Parameters Loop
  } // END  Directory Loop

  if (!pbmap)
  {
    return;
  }
  sout << " --  2nd pass required --";

  Standard_Integer                 nbseq = thenbents + 2 * thenbscop;
  TColStd_Array1OfInteger          inds(0, nbseq);   // n0 Record/Entite
  TColStd_Array1OfInteger          indi(0, nbseq);   // Idents/scopes
  TColStd_Array1OfInteger          indr(0, nbdirec); // inverse of nds
  Handle(TColStd_HArray1OfInteger) indx;             // for EXPORT (if any)

  imap.Clear();
  anAlloc->Reset();
  Standard_Boolean iamap = withmap; // (default True)
  nbmap                  = 0;

  TColStd_SequenceOfInteger scopile(anAlloc); // scope chaining noted by stack
  Standard_Integer          nr = 0;
  for (num = 1; num <= nbdirec; num++)
  {
    Standard_Integer ident = theidents(num);
    if (ident < -2)
    {                     // SUB-LIST (most common case)
      indr(num) = nr + 1; // search based on nr (following object)
    }
    else if (ident >= 0)
    { // Ident normal
      nr++;
      inds(nr)  = num;
      indi(nr)  = ident;
      indr(num) = nr;
      if (ident > 0)
      { // and not (iamap && ident > 0)
        //  Map: if Overlap, inhibit it. Otherwise, note index
        Standard_Integer indmap = imap.Add(ident);
        if (indmap <= nbmap)
        {
          Standard_Boolean errorscope = Standard_False;
          indmap                      = imap.FindIndex(ident); // plus sur
          pbmap                       = Standard_True;
          if (thenbscop == 0)
            errorscope = Standard_True;
          //  Identical numbers when there is no SCOPE? ERROR!
          //  (Of course, if there are SCOPES, we pass through, but still...)
          else
          {
            //  If there are SCOPES, let's look more closely to report a problem
            //  Error if SAME SCOPE group
            //  ATTENTION, we search, not in all records, but in the records
            //    CHAINED, cf nr and not num (no sub-list, scope-endscope chaining)
            Standard_Integer fromscope = nr;
            Standard_Integer toscope   = indm(indmap);
            if (toscope < 0)
              toscope = -toscope;
            for (;;)
            {
              fromscope--; // basic iteration
              if (fromscope <= toscope)
              {
                errorscope = Standard_True; // BANG, we are on it
                break;
              }
              Standard_Integer idtest = indi(fromscope);
              if (idtest >= 0)
                continue; // the next one (well, the previous one)
              if (idtest == -1)
                break; // not same level, so it's OK
              if (idtest == -3)
              {
                fromscope = inds(fromscope);
                if (fromscope < toscope)
                  break; // we exit, not on same level
              }
            }
          }
          if (errorscope)
          {
            //  We are inside: report it
            char ligne[80];
            Sprintf(ligne, "Ident defined SEVERAL TIMES : #%d", ident);
            thecheck->AddFail(ligne, "Ident defined SEVERAL TIMES : #%d");
            sout << "StepReaderData : SetEntityNumbers, " << ligne << std::endl;
          }
          if (indm(indmap) > 0)
            indm(indmap) = -indm(indmap); // Not for Map
                                          //  Normal case for the Map
        }
        else
        {
          nbmap        = indmap;
          indm(indmap) = nr; // Map ->(indm)->inds
        }
      }
    }
    else if (ident == -1)
    { // SCOPE
      nr++;
      inds(nr)  = num;
      indi(nr)  = -1;
      indr(num) = 0;
      scopile.Append(nr);
    }
    else if (ident == -2)
    {                                          // ENDSCOPE
      Standard_Integer nscop = scopile.Last(); // chainage SCOPE-ENDSCOPE
      scopile.Remove(scopile.Length());
      nr++;
      inds(nr)    = nscop;
      indi(nr)    = -3;
      indr(num)   = 0;
      inds(nscop) = nr;
      if (NbParams(num) > 0)
      {
        //  EXPORT : traitement special greffe sur celui de SCOPE (sans le perturber)
        if (indx.IsNull())
        {
          indx = new TColStd_HArray1OfInteger(0, nbseq);
          for (Standard_Integer ixp = 0; ixp <= nbseq; ixp++)
            indx->ChangeValue(ixp) = 0;
        }
        indx->ChangeValue(nr)    = num;
        indx->ChangeValue(nscop) = num;
      }
    }
    else if (ident == 0)
    { // HEADER
      indr(num) = 0;
    }
  }

  //  ..    EXPORT resolution, if any and if needed    ..
  //  For each EXPORT value that has not been resolved by the MAP,
  //  determine its local position by backward search from ENDSCOPE
  if ((!iamap || pbmap) && !indx.IsNull())
  {
    for (nr = 0; nr <= nbseq; nr++)
    {
      if (indx->Value(nr) == 0 && indi(nr) != -3)
        continue; // ENDSCOPE + EXPORT
      num                  = indx->Value(nr);
      Standard_Integer nba = NbParams(num);
      for (Standard_Integer na = 1; na <= nba; na++)
      {
        Interface_FileParameter& FP = ChangeParam(num, na);
        if (FP.ParamType() != Interface_ParamIdent)
          continue;
        Standard_Integer id = -FP.EntityNumber();
        if (id < 0)
          continue; // already resolved at head
                    /*	if (imap.Contains(id)) {            et voila
                        FP.SetEntityNumber(indm(imap.FindIndex(id)));
                        continue;
                      }    */

        //  Search for the requested Id: if nested EXPORT, already resolved but we must
        //  look! (useless however to go see: it's already done, because
        //  a nested EXPORT has been processed BEFORE the one that nests)
        Standard_Integer n0 = nr - 1;
        if (indi(n0) == -3)
          n0--; // if we just follow an ENDSCOPE
        while (n0 > 0)
        {
          Standard_Integer irec = indi(n0);
          if (irec == id)
          { // found
            FP.SetEntityNumber(inds(n0));
            break;
          }
          if (irec == -1)
            break; // SCOPE: end of this SCOPE/ENDSCOPE
          if (irec == -3)
          {
            //  beware of EXPORT: if an EXPORT holds Id, note its already calculated Number
            //  Attention: Id to be read from CValue because EntityNumber already resolved
            Standard_Integer nok = FindEntityNumber(indx->Value(n0), id);
            if (nok > 0)
            {
              FP.SetEntityNumber(nok);
              break;
            }
            n0 = inds(n0); // ENDSCOPE or unsuccessful EXPORT: skip it
          } // end processing on an ENDSCOPE or EXPORT
          n0--;
        } // end resolution of an EXPORT Parameter
      } // end resolution of an EXPORT list
    } // end looping on EXPORTs
  }

  //  Table exploitation: looping operates on the table

  //  Sub-lists processing: done on the fly, by managing a stack
  //  based on the constitution of sub-lists
  Standard_Integer                 maxsubpil = 30; // simulated stack with an Array: very strong
  Handle(TColStd_HArray1OfInteger) subpile   =     // ... saves memory ...
    new TColStd_HArray1OfInteger(1, maxsubpil);
  Standard_Integer nbsubpil = 0; // ... and so much faster!

  for (num = 1; num <= nbdirec; num++)
  {
    nr = indr(num);
    if (nr == 0)
      continue; //    not an object or a sub-list
    Standard_Integer nba = NbParams(num);
    for (Standard_Integer na = nba; na > 0; na--)
    {
      //  We read from the end: this allows processing sub-lists on the fly
      //  Otherwise, we should note that there were sub-lists and resume afterwards
      //  Reverse processing ensures that nested sub-lists are resolved before their containers
      //  This is critical for maintaining referential integrity in complex STEP structures

      Interface_FileParameter& FP     = ChangeParam(num, na);
      Interface_ParamType      letype = FP.ParamType();
      if (letype == Interface_ParamSub)
      {
        //  sub-list type parameter: sub-list number read by unstacking
        FP.SetEntityNumber(subpile->Value(nbsubpil));
        nbsubpil--; //	subpile->Remove(nbsubpil);
      }
      else if (letype == Interface_ParamIdent)
      {
        //  ident type parameter (references an entity): search for requested ident
        Standard_Integer id = -FP.EntityNumber();
        if (id < 0)
          continue; // already resolved at head

        // Here we go: we will search for id in ndi; scanning algorithm
        // This implements a bidirectional search strategy: first backward from current position
        // to file beginning, then forward to file end. This optimizes for locality of references.
        Standard_Integer pass, sens, nok, n0, irec;
        pass = sens = nok = 0;
        if (!iamap)
          pass = 1; // if map not available
        while (pass < 3)
        {
          pass++;
          //    MAP available
          if (pass == 1)
          { // MAP AVAILABLE
            Standard_Integer indmap = imap.FindIndex(id);
            if (indmap > 0)
            { // the map found it
              nok = indm(indmap);
              if (nok < 0)
                continue; // CONFLICT -> need to resolve ...
              break;
            }
            else
              continue;
          }
          //    1st Pass: BACKWARD -> File beginning
          if (sens == 0 && nr > 1)
          {
            n0 = nr - 1;
            if (indi(n0) == -3)
              n0--; // if we just follow an ENDSCOPE
            while (n0 > 0)
            {
              irec = indi(n0);
              if (irec == id)
              { // found
                nok = n0;
                break;
              }
              //    ENDSCOPE : Attention a EXPORT sinon sauter
              if (irec == -3)
              {
                if (indx.IsNull())
                  n0 = inds(n0);
                else
                {
                  //    EXPORT, we must look
                  nok = FindEntityNumber(indx->Value(n0), id);
                  if (nok > 0)
                    break;
                  n0 = inds(n0); // ENDSCOPE: skip it
                }
              }
              n0--;
            }
            //    2nd Pass: DESCENT -> End of file
          }
          else if (nr < nbseq)
          { // descent -> end of file
            n0 = nr + 1;
            while (n0 <= nbseq)
            {
              irec = indi(n0);
              if (irec == id)
              { // found
                nok = n0;
                break;
              }
              //    SCOPE: Attention to EXPORT otherwise skip
              if (irec == -1)
              {
                if (indx.IsNull())
                  n0 = inds(n0);
                else
                {
                  //    EXPORT, we must look
                  nok = FindEntityNumber(indx->Value(n0), id);
                  if (nok > 0)
                    break;
                  n0 = inds(n0); // SCOPE: skip it
                }
              }
              n0++;
            }
          }
          if (nok > 0)
            break;
          sens = 1 - sens; // next pass
        }
        // here we have nok, number found
        if (nok > 0)
        {
          Standard_Integer num0 = inds(nok);
          FP.SetEntityNumber(num0); // AND THERE, we have resolved

          // not found: report it
        }
        else
        {
          //  Feed the Check ... For this, determine Entity n0 and Ident
          char             failmess[100];
          Standard_Integer nument = 0;
          Standard_Integer n0ent; // svv Jan11 2000 : porting on DEC
          for (n0ent = 1; n0ent <= nr; n0ent++)
          {
            if (indi(n0ent) > 0)
              nument++;
          }
          Standard_Integer ident = RecordIdent(num);
          if (ident < 0)
          {
            for (n0ent = num + 1; n0ent <= nbdirec; n0ent++)
            {
              ident = RecordIdent(n0ent);
              if (ident > 0)
                break;
            }
          }
          //  ...  Build the Check  ...
          Sprintf(failmess,
                  "Unresolved Reference, Ent.n0 %d (Id.#%d) Param.n0 %d (Id.#%d)",
                  nument,
                  ident,
                  na,
                  id);
          thecheck->AddFail(failmess, "Unresolved Reference");

          //  ...  And output a more complete message
          sout << "*** ERR StepReaderData *** Entite " << nument << ", a " << (nr * 100) / nbseq
               << "% de DATA : #" << ident << "\n    Type:" << RecordType(num) << "  Param.n0 "
               << na << ": #" << id << " Not found" << std::endl;

          FP.SetEntityNumber(0); // -> Unresolved reference
        }
      }
    }
    //  If this record is itself a sub-list, stack it!
    if (inds(nr) != num)
    {
      if (nbsubpil >= maxsubpil)
      {
        maxsubpil                                  = maxsubpil + 30;
        Handle(TColStd_HArray1OfInteger) newsubpil = new TColStd_HArray1OfInteger(1, maxsubpil);
        for (Standard_Integer bidpil = 1; bidpil <= maxsubpil - 30; bidpil++)
          newsubpil->SetValue(bidpil, subpile->Value(bidpil));
        subpile = newsubpil;
      }
      nbsubpil++;
      subpile->SetValue(nbsubpil, num); // Append(num);
    }
  }
}

//  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
//  ....             Header Management: Preparation, reading             ....

//=================================================================================================

Standard_Integer StepData_StepReaderData::FindNextHeaderRecord(const Standard_Integer num) const
{
  // returns, for a given record number (by num), the next one which
  // defines an entity, or 0 if finished:
  // Operates like FindNextRecord but only scans the Header

  if (num < 0)
    return 0;
  Standard_Integer num1 = num + 1;
  Standard_Integer max  = thenbhead;

  while (num1 <= max)
  {
    // SCOPE,ENDSCOPE and Sub-List have a negative identifier
    // Only retain positive or null Idents (null: no Ident in Header)
    if (RecordIdent(num1) >= 0)
      return num1;
    num1++;
  }
  return 0;
}

//=================================================================================================

void StepData_StepReaderData::PrepareHeader()
{
  // Reference resolution: only concerns sub-lists
  //  already done by SetEntityNumbers so no need to worry

  /*
  // Algorithm taken and adapted from SetEntityNumbers
  //  Traitement des sous-listes : se fait dans la foulee, par gestion d une pile
  //  basee sur la constitution des sous-listes
    TColStd_SequenceOfInteger subpile;
    Standard_Integer nbsubpil = 0;     // stack depth but faster ...

    for (Standard_Integer num = 1 ; num <= thenbhead ; num ++) {
      Standard_Integer nba = NbParams(num) ;
      for (Standard_Integer na = nba ; na > 0 ; na --) {
  ..  We read from the end: this allows processing sub-lists on the fly
  ..  Otherwise, we should note that there were sub-lists and resume afterwards

        Interface_FileParameter& FP = ChangeParam(num,na);
        Interface_ParamType letype = FP.ParamType();
        if (letype == Interface_ParamSub) {
  ..  sub-list type parameter: sub-list number read by unstacking
          FP.SetEntityNumber(subpile.Last());
  .. ..        SetParam(num,na,FP);
    subpile.Remove(nbsubpil);
          nbsubpil --;
        }
      }
  .. If it's a sub-list, stack
      if (RecordIdent(num) < -2) {
        subpile.Append(num);
        nbsubpil ++;
      }
    }
  */
}

//=================================================================================================

const Handle(Interface_Check) StepData_StepReaderData::GlobalCheck() const
{
  return thecheck;
}
