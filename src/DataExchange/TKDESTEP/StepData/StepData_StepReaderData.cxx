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
//    sln 04,10.2001. BUC61003. Prevent exception which may occur during reading of complex entity (if entity's items are not in alphabetical order)

#include <Interface_Check.hxx>
#include <Interface_HArray1OfHAsciiString.hxx>
#include <Interface_Macros.hxx>
#include <Interface_Static.hxx>
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

// Le Header est constitue d entites analogues dans leur principe a celles
// du Data, a ceci pres qu elles sont sans identifieur, et ne peuvent ni
// referencer, ni etre referencees (que ce soit avec Header ou avec Data)
// Ainsi, dans StepReaderData, le Header est constitue des "thenbhead" 1res Entites
//  #########################################################################
//  ....   Creation et Acces de base aux donnees atomiques du fichier    ....
typedef TCollection_HAsciiString String;
static char txtmes[200];  // plus commode que redeclarer partout

static bool initstr = false;
#define Maxlst 64
//static TCollection_AsciiString subl[Maxlst];          // Maxlst : minimum 10

static int acceptvoid = 0;

// ----------  Fonctions Utilitaires  ----------

//! Convert unsigned character to hexadecimal system, 
//! if character hasn't representation in this system, returns 0.
static int convertCharacterTo16bit(const char16_t theCharacter)
{
  switch (theCharacter)
  {
    case '0': return 0;
    case '1': return 1;
    case '2': return 2;
    case '3': return 3;
    case '4': return 4;
    case '5': return 5;
    case '6': return 6;
    case '7': return 7;
    case '8': return 8;
    case '9': return 9;
    case 'A': case 'a': return 10;
    case 'B': case 'b': return 11;
    case 'C': case 'c': return 12;
    case 'D': case 'd': return 13;
    case 'E': case 'e': return 14;
    case 'F': case 'f': return 15;
    default : return 0;
  }
}

//=======================================================================
//function : cleanText
//purpose  : 
//=======================================================================
void StepData_StepReaderData::cleanText(const occ::handle<TCollection_HAsciiString>& theVal) const
{
  if (theVal->Length() == 2)
  {
    theVal->Clear();
    return;
  }
  TCollection_ExtendedString aResString;
  const bool toConversion = mySourceCodePage != Resource_FormatType_NoConversion;
  Resource_Unicode::ConvertFormatToUnicode(mySourceCodePage, theVal->ToCString() + 1, aResString);
  int aResStringSize = aResString.Length() - 1; // skip the last apostrophe
  TCollection_ExtendedString aTempExtString; // string for characters within control directives
  int aSetCharInd = 1; // index to set value to result string
  Resource_FormatType aLocalFormatType = Resource_FormatType_iso8859_1; // a code page for a "\S\" control directive
  for (int aStringInd = 1; aStringInd <= aResStringSize; ++aStringInd)
  {
    const char16_t aChar = aResString.Value(aStringInd);
    aSetCharInd = aStringInd;
    if (aChar == '\\' && aStringInd <= aResStringSize - 3) // can contains the control directive
    {
      bool isConverted = false;
      const char16_t aDirChar = aResString.Value(aStringInd + 1);
      const bool isSecSlash = aResString.Value(aStringInd + 2) == '\\';
      const bool isThirdSlash = aResString.Value(aStringInd + 3) == '\\';
      // Encoding ISO 8859 characters within a string;
      // ("\P{N}\") control directive;
      // indicates code page for ("\S\") control directive;
      // {N}: "A", "B", "C", "D", "E", "F", "G", "H", "I";
      // "A" identifies ISO 8859-1; "B" identifies ISO 8859-2, etc.
      if (aDirChar == 'P' && isThirdSlash)
      {
        const char aPageId =
          UpperCase(static_cast<char>(aResString.Value(aStringInd + 2) & 255));
        if (aPageId >= 'A' && aPageId <= 'I')
        {
          aLocalFormatType = (Resource_FormatType)(Resource_FormatType_iso8859_1 + (aPageId - 'A'));
        }
        else
        {
          thecheck->AddWarning("String control directive \\P*\\ with an unsupported symbol in place of *");
        }
        isConverted = true;
        aStringInd += 3;
      }
      // Encoding ISO 8859 characters within a string;
      // ("\S\") control directive;
      // converts followed a LATIN CODEPOINT character.
      else if (aDirChar == 'S' && isSecSlash)
      {
        char aResChar = static_cast<char>(aResString.Value(aStringInd + 3) | 0x80);
        const char aStrForCovert[2] = { aResChar, '\0' };
        Resource_Unicode::ConvertFormatToUnicode(aLocalFormatType, aStrForCovert, aTempExtString);
        isConverted = true;
        aStringInd += 3;
      }
      // Encoding U+0000 to U+00FF in a string
      // ("\X\") control directive;
      // converts followed two hexadecimal character.
      else if (aDirChar == 'X' && aStringInd <= aResStringSize - 4 && isSecSlash)
      {
        char aResChar = (char)convertCharacterTo16bit(aResString.Value(aStringInd + 3));
        aResChar = (aResChar << 4) | (char)convertCharacterTo16bit(aResString.Value(aStringInd + 4));
        const char aStrForConvert[2] = { aResChar, '\0' };
        aTempExtString = TCollection_ExtendedString(aStrForConvert, false); // pass through without conversion
        isConverted = true;
        aStringInd += 4;
      }
      // Encoding ISO 10646 characters within a string
      // ("\X{N}\") control directive;
      // {N}: "0", "2", "4";
      // "\X2\" or "\X4\" converts followed a hexadecimal character sequence;
      // "\X0\" indicate the end of the "\X2\" or "\X4\".
      else if (aDirChar == 'X' && isThirdSlash)
      {
        int aFirstInd = aStringInd + 3;
        int aLastInd = aStringInd;
        bool isClosed = false;
        // find the end of the "\X2\" or "\X4\" by an external "aStringInd"
        for (; aStringInd <= aResStringSize && !isClosed; ++aStringInd)
        {
          if (aResStringSize - aStringInd > 2 && aResString.Value(aStringInd) == '\\' &&
            aResString.Value(aStringInd + 1) == 'X' && aResString.Value(aStringInd + 2) == '0' &&
            aResString.Value(aStringInd + 3) == '\\')
          {
            aLastInd = aStringInd - 1;
            aStringInd = aStringInd + 2;
            isClosed = true;
          }
        }
        if (!isClosed) // "\X0\" not exists
        {
          aLastInd = aStringInd = aResStringSize;
        }
        const int aStrLen = aLastInd - aFirstInd;
        // "\X2\" control directive;
        // followed by multiples of four or three hexadecimal characters. 
        // Encoding in UTF-16
        if (aResString.Value(aFirstInd - 1) == '2' && aResStringSize - aFirstInd > 3)
        {
          int anIterStep = (aStrLen % 4 == 0) ? 4 : 3;
          if (aStrLen % anIterStep)
          {
            aTempExtString.AssignCat('?');
            thecheck->AddWarning("String control directive \\X2\\ is followed by number of digits not multiple of 4");
          }
          else
          {
            char16_t aUtfCharacter = '\0';
            for (int aCharInd = 1; aCharInd <= aStrLen; ++aCharInd)
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
            thecheck->AddWarning("String control directive \\X4\\ is followed by number of digits not multiple of 8");
          }
          else
          {
            char32_t aUtfCharacter[2] = { '\0', '\0' };
            for (int aCharInd = 1; aCharInd <= aStrLen; ++aCharInd)
            {
              aUtfCharacter[0] |= convertCharacterTo16bit(aResString.Value(aCharInd + aFirstInd));
              if (aCharInd % 8 == 0)
              {
                NCollection_Utf32Iter aUtfIter(aUtfCharacter);
                char16_t aStringBuffer[3];
                char16_t* aUtfPntr = aUtfIter.GetUtf16(aStringBuffer);
                *aUtfPntr++ = '\0';
                TCollection_ExtendedString aUtfString(aStringBuffer);
                aTempExtString.AssignCat(aUtfString);
                aUtfCharacter[0] = '\0';
              }
              aUtfCharacter[0] = aUtfCharacter[0] << 4;
            }
          }
        }
        isConverted = true;
      }
      if (isConverted) // find the control directive
      {
        if (toConversion) // else skip moving
        {
          aResStringSize -= aStringInd - aSetCharInd - aTempExtString.Length() + 1; // change the string size to remove unused symbols
          aResString.SetValue(aSetCharInd, aTempExtString);
          aSetCharInd += aTempExtString.Length(); // move to the new position
          aResString.SetValue(aSetCharInd, aResString.ToExtString() + aStringInd);
          aStringInd = aSetCharInd - 1;
          aResString.Trunc(aResStringSize);;
        }
        aTempExtString.Clear();
        continue;
      }
    }
    if (aStringInd <= aResStringSize - 1)
    {
      const char16_t aCharNext = aResString.Value(aStringInd + 1);
      if (aCharNext == aChar && (aChar == '\'' || aChar == '\\'))
      {
        aResString.SetValue(aSetCharInd, aResString.ToExtString() + aStringInd); // move the string,removing one symbol
        aResStringSize--; // change the string size to remove unused symbol
        aResString.Trunc(aResStringSize);
      }
      else if (aChar == '\\')
      {
        const bool isDirective =
          aStringInd <= aResStringSize - 2 && aResString.Value(aStringInd + 2) == '\\';
        if (isDirective)
        {
          if (aCharNext == 'N')
          {
            aResString.SetValue(aSetCharInd++, '\n');
            aResString.SetValue(aSetCharInd, aResString.ToExtString() + aStringInd + 2); // move the string,removing two symbols
            aResStringSize-=2; // change the string size to remove unused symbols
            aResString.Trunc(aResStringSize);
            continue;
          }
          else if (aCharNext == 'T')
          {
            aResString.SetValue(aSetCharInd++, '\t');
            aResString.SetValue(aSetCharInd, aResString.ToExtString() + aStringInd + 2); // move the string,removing two symbols
            aResStringSize-=2; // change the string size to remove unused symbols
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

//=======================================================================
//function : StepData_StepReaderData
//purpose  : 
//=======================================================================

StepData_StepReaderData::StepData_StepReaderData
(const int nbheader, const int nbtotal,
  const int nbpar, const Resource_FormatType theSourceCodePage)
  : Interface_FileReaderData(nbtotal, nbpar), theidents(1, nbtotal),
  thetypes(1, nbtotal), mySourceCodePage(theSourceCodePage) //, themults (1,nbtotal)
{
  //  char textnum[10];
  thenbscop = 0;  thenbents = 0;  thelastn = 0;  thenbhead = nbheader;
  //themults.Init(0);
  thecheck = new Interface_Check;
  if (initstr) return;
  //for (int i = 0; i < Maxlst; i ++) {
  //  sprintf(textnum,"$%d",i+1);
  //  subl[i].AssignCat(textnum);
  //}
  initstr = true;
}

//=======================================================================
//function : SetRecord
//purpose  : 
//=======================================================================

void StepData_StepReaderData::SetRecord(const int num,
  const char* const ident,
  const char* const type,
  const int /* nbpar */)
{
  int numlst;
  /*
    if (strcmp(type,"/ * (SUB) * /") == 0) {    // defini dans recfile.pc
      thetypes.SetValue (num,sublist);
    } else {
      thenbents ++;   // total de termes propres du fichier
      thetypes.SetValue(num,TCollection_AsciiString(type));
  //    if (strcmp(ident,"SCOPE") != 0) thenbscop ++;  // ?? a verifier
    }
  */
  if (type[0] != '(') thenbents++;   // total de termes propres du fichier

  //thetypes.ChangeValue(num).SetValue(1,type); gka memory
  //============================================
  int index = 0;
  TCollection_AsciiString strtype(type);
  if (thenametypes.Contains(type))
    index = thenametypes.FindIndex(strtype);
  else index = thenametypes.Add(strtype);
  thetypes.ChangeValue(num) = index;
  //===========================================

  if (ident[0] == '$') {
    if (strlen(ident) > 2) numlst = atoi(&ident[1]);
    else numlst = ident[1] - 48;
    if (thelastn < numlst) thelastn = numlst;    // plus fort n0 de sous-liste
    theidents.SetValue(num, -2 - numlst);
  } else if (ident[0] == '#') {
    numlst = atoi(&ident[1]);
    theidents.SetValue(num, numlst);
    if (numlst == 0 && num > thenbhead) {
      //    Header, ou bien Type Complexe ...
      //    Si Type Complexe, retrouver Type Precedent (on considere que c est rare)
      //    On chaine le type precedent sur le suivant
      //    VERIFICATION que les types sont en ordre alphabetique
      for (int prev = num - 1; prev > thenbhead; prev--) {
        if (theidents(prev) >= 0) {

          //themults.SetValue(prev,num);
          themults.Bind(prev, num);
          if (thenametypes.FindKey(thetypes.Value(num)).IsLess(thenametypes.FindKey(thetypes.Value(prev)))) {
            //  Warning: components in complex entity are not in alphabetical order.
            TCollection_AsciiString errm("Complex Type incorrect : ");
            errm.AssignCat(thenametypes.FindKey(thetypes.Value(prev)));
            errm.AssignCat(" / ");
            errm.AssignCat(thenametypes.FindKey(thetypes.Value(num)));
            errm.AssignCat(" ... ");
            while (theidents(prev) <= 0) {
              prev--;  if (prev <= 0) break;
            }

            Message_Messenger::StreamBuffer sout = Message::SendTrace();
            sout << "  ***  Incorrect record " << num << " (on " << NbRecords()
                 << " -> " << num * 100 / NbRecords() << " % in File)  ***";
            if (prev > 0) sout << "  Ident #" << theidents(prev);
            sout << "\n" << errm << std::endl;
            thecheck->AddWarning(errm.ToCString(), "Complex Type incorrect : ");
          }
          break;
        }
      }
    }
  }
  else if (!strcmp(ident, "SCOPE")) {
    theidents.SetValue(num, -1); // SCOPE
    thenbscop++;
  }
  else if (!strcmp(ident, "ENDSCOPE")) theidents.SetValue(num, -2);  // ENDSCOPE
//      Reste 0

 // InitParams(num);
}

//=======================================================================
//function : AddStepParam
//purpose  : 
//=======================================================================

void StepData_StepReaderData::AddStepParam(const int num,
  const char* const aval,
  const Interface_ParamType atype,
  const int nument)
{
  if (atype == Interface_ParamSub) {
    int numid = 0;
    if (aval[2] != '\0') {
      numid = atoi(&aval[1]);
      //      if (numid <= Maxlst) Interface_FileReaderData::AddParam
      //	(num,subl[numid-1].ToCString(),atype,numid);
      Interface_FileReaderData::AddParam(num, aval, atype, numid);
    } else {
      char *numlstchar = (char *)(aval + 1);
      numid = (*numlstchar) - 48;  // -48 ('0') -1 (adresse [] depuis 0)
//      Interface_FileReaderData::AddParam (num,subl[numid].ToCString(),atype,numid);
      Interface_FileReaderData::AddParam(num, aval, atype, numid);
    }
  } else if (atype == Interface_ParamIdent) {
    int numid = atoi(&aval[1]);
    Interface_FileReaderData::AddParam(num, aval, atype, numid);
  } else {
    Interface_FileReaderData::AddParam(num, aval, atype, nument);
  }

  //  Interface_FileReaderData::AddParam (num,parval,atype,numid);
}

//=======================================================================
//function : RecordType
//purpose  : 
//=======================================================================

const TCollection_AsciiString& StepData_StepReaderData::RecordType
(const int num) const
{
  return thenametypes.FindKey(thetypes.Value(num));
}

//=======================================================================
//function : CType
//purpose  : 
//=======================================================================

const char* StepData_StepReaderData::CType(const int num) const
{
  return thenametypes.FindKey(thetypes.Value(num)).ToCString();
}

//=======================================================================
//function : RecordIdent
//purpose  : 
//=======================================================================

int StepData_StepReaderData::RecordIdent(const int num) const
{
  return theidents(num);
}

//  ########################################################################
//  ....       Aides a la lecture des parametres, adaptees a STEP       ....

//=======================================================================
//function : SubListNumber
//purpose  : 
//=======================================================================

int StepData_StepReaderData::SubListNumber(const int num,
  const int nump,
  const bool aslast) const
{
  if (nump == 0 || nump > NbParams(num)) return 0;
  const Interface_FileParameter& FP = Param(num, nump);
  if (FP.ParamType() != Interface_ParamSub) return 0;
  if (aslast) { if (nump != NbParams(num)) return 0; }
  return FP.EntityNumber();
}

//=======================================================================
//function : IsComplex
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::IsComplex(const int num) const
{
  //return (themults(num) != 0);
  return themults.IsBound(num);
}

//=======================================================================
//function : ComplexType
//purpose  : 
//=======================================================================

void  StepData_StepReaderData::ComplexType(const int num,
  TColStd_SequenceOfAsciiString& types) const
{
  if (theidents(num) < 0) return;
  for (int i = num; i > 0; i = NextForComplex(i)) {
    types.Append(RecordType(i));
  }
}

//=======================================================================
//function : NextForComplex
//purpose  : 
//=======================================================================

int StepData_StepReaderData::NextForComplex
(const int num) const
{
  int next = 0;
  if (themults.IsBound(num))
    next = themults.Find(num);
  return next;
}

//=======================================================================
//function : NamedForComplex
//purpose  : 
//=======================================================================

bool  StepData_StepReaderData::NamedForComplex
(const char* const name, const int num0,
  int& num, occ::handle<Interface_Check>& ach) const
{
  //bool stat = true;
  int n = (num <= 0 ? num0 : NextForComplex(num));
  // sln 04,10.2001. BUC61003. if(n==0) the next  function is not called in order to avoid exception
  if ((n != 0) && (!strcmp(RecordType(n).ToCString(), name)))
    {  num = n;  return true;  }

  if (n == 0) /*stat =*/ NamedForComplex(name, num0, n, ach);  // on a rembobine
//  Pas dans l ordre alphabetique : boucler
  occ::handle<String> errmess = new String("Parameter n0.%d (%s) not a LIST");
  sprintf(txtmes, errmess->ToCString(), num0, name);
  for (n = num0; n > 0; n = NextForComplex(n)) {
    if (!strcmp(RecordType(n).ToCString(), name)) {
      num = n;
      errmess = new String("Complex Record n0.%d, member type %s not in alphabetic order");
      sprintf(txtmes, errmess->ToCString(), num0, name);
      ach->AddWarning(txtmes, errmess->ToCString());
      return false;
    }
  }
  num = 0;
  errmess = new String("Complex Record n0.%d, member type %s not found");
  sprintf(txtmes, errmess->ToCString(), num0, name);
  ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : NamedForComplex
//purpose  : 
//=======================================================================

bool  StepData_StepReaderData::NamedForComplex
(const char* const theName, const char* const theShortName,
  const int num0, int& num,
  occ::handle<Interface_Check>& ach) const
{
  int n = (num <= 0 ? num0 : NextForComplex(num));

  if ((n != 0) && (!strcmp(RecordType(n).ToCString(), theName) ||
    !strcmp(RecordType(n).ToCString(), theShortName)))
  {
    num = n;
    return true;
  }

  //entities are not in alphabetical order
  occ::handle<String> errmess = new String("Parameter n0.%d (%s) not a LIST");
  sprintf(txtmes, errmess->ToCString(), num0, theName);
  for (n = num0; n > 0; n = NextForComplex(n))
  {
    if (!strcmp(RecordType(n).ToCString(), theName) ||
      !strcmp(RecordType(n).ToCString(), theShortName))
    {
      num = n;
      errmess = new String("Complex Record n0.%d, member type %s not in alphabetic order");
      sprintf(txtmes, errmess->ToCString(), num0, theName);
      ach->AddWarning(txtmes, errmess->ToCString());
      return false;
    }
  }
  num = 0;
  errmess = new String("Complex Record n0.%d, member type %s not found");
  sprintf(txtmes, errmess->ToCString(), num0, theName);
  ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##

//=======================================================================
//function : CheckNbParams
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::CheckNbParams(const int num,
  const int nbreq,
  occ::handle<Interface_Check>& ach,
  const char* const mess) const
{
  if (NbParams(num) == nbreq) return true;
  occ::handle<String> errmess;
  if (mess[0] == '\0') errmess = new String("Count of Parameters is not %d");
  else errmess = new String("Count of Parameters is not %d for %s");
  sprintf(txtmes, errmess->ToCString(), nbreq, mess);
  ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : ReadSubList
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadSubList(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  int& numsub,
  const bool optional,
  const int /* lenmin */,
  const int /* lenmax */) const
{
  numsub = SubListNumber(num, nump, false);
  if (numsub > 0)
  {
    return (NbParams(numsub) > 0);
  }
  //  Si optionel indefini, on passe l eponge
  numsub = 0;
  bool isvoid = (Param(num, nump).ParamType() == Interface_ParamVoid);
  if (isvoid && optional) return false;

  occ::handle<String> errmess = new String("Parameter n0.%d (%s) not a LIST");
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  if (acceptvoid && isvoid)  ach->AddWarning(txtmes, errmess->ToCString());
  else { ach->AddFail(txtmes, errmess->ToCString()); return false; }
  return true;
}

//  ...   Facilites pour LateBinding

//=======================================================================
//function : ReadSub
//purpose  : 
//=======================================================================

int StepData_StepReaderData::ReadSub(const int numsub,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  const occ::handle<StepData_PDescr>& descr,
  occ::handle<Standard_Transient>& val) const
{
  int nbp = NbParams(numsub);
  if (nbp == 0) return 0;    // liste vide = Handle Null
  const TCollection_AsciiString& rectyp = RecordType(numsub);
  if (nbp == 1 && rectyp.ToCString()[0] != '(') {
    //  c est un type avec un parametre -> SelectNamed
    //  cf ReadSelect mais ici, on est deja sur le contenu du parametre
    occ::handle<StepData_SelectNamed> sn = new StepData_SelectNamed;
    val = sn;
    sn->SetName(rectyp.ToCString());
    occ::handle<Standard_Transient> aSN = sn;
    if (ReadAny(numsub, 1, mess, ach, descr, aSN)) return sn->Kind();
    else return 0;
  }

  //  cas courant : faire un HArray1 de ... de ... de quoi au fait
  const Interface_FileParameter& FP0 = Param(numsub, 1);
  Interface_ParamType FT, FT0 = FP0.ParamType();
  const char* str = FP0.CValue();
  occ::handle<TColStd_HArray1OfTransient> htr;
  occ::handle<TColStd_HArray1OfInteger>   hin;
  occ::handle<TColStd_HArray1OfReal>      hre;
  occ::handle<Interface_HArray1OfHAsciiString>  hst;
  int kod = 0;
  switch (FT0) {
  case Interface_ParamMisc: return -1;
  case Interface_ParamInteger: kod = 1;  break;
  case Interface_ParamReal: kod = 5;  break;
  case Interface_ParamIdent: kod = 7;  break;
  case Interface_ParamVoid: kod = 0;  break;
  case Interface_ParamText: kod = 6;  break;
  case Interface_ParamEnum: kod = 4;  break;  // a confirmer(logical)
    /*      kod = 4;
      if ( str[0] == '.' && str[2] == '.' && str[3] == '\0' &&
      (str[1] == 'T' || str[1] == 'F' || str[1] == 'U') ) kod = 3;
      break; */ // svv #2
  case Interface_ParamLogical: return -1;
  case Interface_ParamSub: kod = 0;  break;
  case Interface_ParamHexa: return -1;
  case Interface_ParamBinary: return -1;
  default:  return -1;
  }
  if (kod == 1 || kod == 3) { hin = new TColStd_HArray1OfInteger(1, nbp); val = hin; }
  else if (kod == 5) { hre = new TColStd_HArray1OfReal(1, nbp); val = hre; }
  else if (kod == 6) { hst = new Interface_HArray1OfHAsciiString(1, nbp); val = hst; }
  else { htr = new TColStd_HArray1OfTransient(1, nbp); val = htr; }
  //  Attention : si type variable, faudra changer son fusil d epaule -> htr

  for (int ip = 1; ip <= nbp; ip++) {
    const Interface_FileParameter& FP = Param(numsub, ip);
    str = FP.CValue();
    FT = FP.ParamType();
    switch (kod) {
    case 1: {
      if (FT != Interface_ParamInteger) { kod = 0; break; }
      hin->SetValue(ip, atoi(str));	break;
    }
    case 2:
    case 3: {
      if (FT != Interface_ParamEnum) { kod = 0; break; }
      if (!strcmp(str, ".F.")) hin->SetValue(ip, 0);
      else if (!strcmp(str, ".T.")) hin->SetValue(ip, 1);
      else if (!strcmp(str, ".U.")) hin->SetValue(ip, 2);
      else    kod = 0;
      break;
    }
    case 4: {
      if (FT != Interface_ParamEnum) { kod = 0; break; }
      occ::handle<StepData_SelectNamed> sn = new StepData_SelectNamed;
      sn->SetEnum(-1, str);
      htr->SetValue(ip, sn);  break;
    }
    case 5: {
      if (FT != Interface_ParamReal) { kod = 0; break; }
      hre->SetValue(ip, Interface_FileReaderData::Fastof(str));   break;
    }
    case 6: {
      if (FT != Interface_ParamText) { kod = 0; break; }
      occ::handle<TCollection_HAsciiString> txt = new TCollection_HAsciiString(str);
      cleanText(txt);
      hst->SetValue(ip, txt);
      break;
    }
    case 7: {
      occ::handle<Standard_Transient> ent = BoundEntity(FP.EntityNumber());
      htr->SetValue(ip, ent);  break;
    }
    default: break;
    }
    //    Restent les autres cas ... tout est possible. cf le type du Param
    if (kod > 0) continue;
    //    Il faut passer au transient ...
    if (htr.IsNull()) {
      htr = new TColStd_HArray1OfTransient(1, nbp);  val = htr;
      int jp;
      if (!hin.IsNull()) {
        for (jp = 1; jp < ip; jp++) {
          occ::handle<StepData_SelectInt> sin = new StepData_SelectInt;
          sin->SetInt(hin->Value(jp));
          htr->SetValue(jp, sin);
        }
      }
      if (!hre.IsNull()) {
        for (jp = 1; jp < ip; jp++) {
          occ::handle<StepData_SelectReal> sre = new StepData_SelectReal;
          sre->SetReal(hre->Value(jp));
          htr->SetValue(jp, sre);
        }
      }
      if (!hst.IsNull()) {
        for (jp = 1; jp < ip; jp++) {
          htr->SetValue(jp, hst->Value(jp));
        }
      }
    }
    //    A present, faut y aller : lire le champ et le mettre en place
    //    Ce qui suit ressemble fortement a ReadAny ...

    switch (FT) {
    case Interface_ParamMisc: break;
    case Interface_ParamInteger: {
      occ::handle<StepData_SelectInt> sin = new StepData_SelectInt;
      sin->SetInteger(atoi(str));
      htr->SetValue(ip, sin); break;
    }
    case Interface_ParamReal: {
      occ::handle<StepData_SelectReal> sre = new StepData_SelectReal;
      sre->SetReal(Interface_FileReaderData::Fastof(str));   break;
      //htr->SetValue (ip,sre); break; svv #2: unreachable
    }
    case Interface_ParamIdent: htr->SetValue(ip, BoundEntity(FP.EntityNumber()));  break;
    case Interface_ParamVoid: break;
    case Interface_ParamEnum: {
      occ::handle<StepData_SelectInt>   sin;
      occ::handle<StepData_SelectNamed> sna;
      int logic = -1;
      // PTV 16.09.2000
      // set the default value of StepData_Logical
      StepData_Logical slog = StepData_LUnknown;
      if (str[0] == '.' && str[2] == '.' && str[3] == '\0') {
        if (str[1] == 'F') { slog = StepData_LFalse;    logic = 0; }
        else if (str[1] == 'T') { slog = StepData_LTrue;     logic = 1; }
        else if (str[1] == 'U') { slog = StepData_LUnknown;  logic = 2; }
      }
      if (logic >= 0)
	{ sin = new StepData_SelectInt; sin->SetLogical(slog); htr->SetValue(ip,sin); }
      else { sna = new StepData_SelectNamed;
	     sna->SetEnum (logic,str); htr->SetValue (ip,sna);  }
      break;
    }
    case Interface_ParamLogical: break;
    case Interface_ParamText: {
      occ::handle<TCollection_HAsciiString> txt = new TCollection_HAsciiString(str);
      cleanText(txt);
      htr->SetValue(ip, txt);
      break;
    }
    case Interface_ParamSub: {
      occ::handle<Standard_Transient> sub;
      int nent = FP.EntityNumber();
      int kind = ReadSub(nent, mess, ach, descr, sub);   if (kind < 0) break;
      htr->SetValue(ip, sub);  break;
    }
    case Interface_ParamHexa: break;
    case Interface_ParamBinary: break;
    default: break;
    }
    return -1;
  }
  return 8;  // pour Any
}

//=======================================================================
//function : ReadMember
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadMember(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  occ::handle<StepData_SelectMember>& val) const
{
  occ::handle<Standard_Transient> v = val;
  occ::handle<StepData_PDescr> nuldescr;
  if (v.IsNull())
  {
    return ReadAny(num, nump, mess, ach, nuldescr, v) &&
      !(val = occ::down_cast<StepData_SelectMember>(v)).IsNull();
  }
  bool res = ReadAny(num, nump, mess, ach, nuldescr, v);
  if (v == val) return res;
  //   changement -> refus
  occ::handle<String> errmess =
    new String("Parameter n0.%d (%s) : does not match SELECT clause");
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : ReadField
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadField(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  const occ::handle<StepData_PDescr>& descr,
  StepData_Field& fild) const
{
  const Interface_FileParameter& FP = Param(num, nump);
  const char* str = FP.CValue();
  bool OK = true;
  int nent, kind;
  occ::handle<TCollection_HAsciiString> txt;
  occ::handle<Standard_Transient> sub;
  Interface_ParamType FT = FP.ParamType();
  switch (FT) {
  case Interface_ParamMisc: OK = false;  break;
  case Interface_ParamInteger: fild.SetInteger(atoi(str)); break;
  case Interface_ParamReal:
    fild.SetReal(Interface_FileReaderData::Fastof(str));   break;
  case Interface_ParamIdent:
    nent = FP.EntityNumber();
    if (nent > 0) fild.SetEntity(BoundEntity(nent));
    break;
  case Interface_ParamVoid:  break;
  case Interface_ParamText:
    txt = new TCollection_HAsciiString(str);
    cleanText(txt);
    fild.Set(txt);
    break;
  case Interface_ParamEnum:
    if (!strcmp(str, ".T.")) fild.SetLogical(StepData_LTrue);
    else if (!strcmp(str, ".F.")) fild.SetLogical(StepData_LFalse);
    else if (!strcmp(str, ".U.")) fild.SetLogical(StepData_LUnknown);
    else    fild.SetEnum(-1, str);
    break;
  case Interface_ParamLogical: OK = false;  break;
  case Interface_ParamSub:
    nent = FP.EntityNumber();
    kind = ReadSub(nent, mess, ach, descr, sub);   if (kind < 0) break;
    fild.Clear(kind);  fild.Set(sub);      break;
  case Interface_ParamHexa: OK = false;  break;
  case Interface_ParamBinary: OK = false;  break;
  default:  OK = false;  break;
  }

  if (!OK) {
    if (!strcmp(str, "*")) fild.SetDerived();
  }
  return true;
}

//=======================================================================
//function : ReadList
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadList(const int num,
  occ::handle<Interface_Check>& ach,
  const occ::handle<StepData_ESDescr>& descr,
  StepData_FieldList& list) const
{
  // controler nbs egaux
  int i, nb = list.NbFields();
  if (!CheckNbParams(num, nb, ach, descr->TypeName())) return false;
  for (i = 1; i <= nb; i++) {
    occ::handle<StepData_PDescr> pde = descr->Field(i);
    StepData_Field& fild = list.CField(i);
    ReadField(num, i, pde->Name(), ach, pde, fild);
  }
  return true;
}

//=======================================================================
//function : ReadAny
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadAny(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  const occ::handle<StepData_PDescr>& descr,
  occ::handle<Standard_Transient>& val) const
{
  const Interface_FileParameter& FP = Param(num, nump);
  const char*    str = FP.CValue();
  Interface_ParamType FT = FP.ParamType();

  //    A present, faut y aller : lire le champ et le mettre en place
  switch (FT) {
  case Interface_ParamMisc: break;
  case Interface_ParamInteger: {
    if (!val.IsNull()) {
      DeclareAndCast(StepData_SelectMember, sm, val);
      sm->SetReal(Interface_FileReaderData::Fastof(str));
      return true;
    }
    occ::handle<StepData_SelectInt> sin = new StepData_SelectInt;
    sin->SetInteger(atoi(str));
    val = sin;
    return true;
  }
  case Interface_ParamReal: {
    if (!val.IsNull()) {
      DeclareAndCast(StepData_SelectMember, sm, val);
      sm->SetReal(Interface_FileReaderData::Fastof(str));
      return true;
    }
    occ::handle<StepData_SelectReal> sre = new StepData_SelectReal;
    sre->SetReal(Interface_FileReaderData::Fastof(str));
    val = sre;
    return true;
  }
  case Interface_ParamIdent: {
    int nent = FP.EntityNumber();
    if (nent > 0) val = BoundEntity(nent);
    return (!val.IsNull());
  }
  case Interface_ParamVoid: break;
  case Interface_ParamEnum: {
    occ::handle<StepData_SelectMember> sm;
    if (!val.IsNull())  sm = GetCasted(StepData_SelectMember, val);
    occ::handle<StepData_SelectInt>   sin;
    occ::handle<StepData_SelectNamed> sna;
    int logic = -1;

    // PTV 16.09.2000
    // set the default value of StepData_Logical
    StepData_Logical slog = StepData_LUnknown;
    if (str[0] == '.' && str[2] == '.' && str[3] == '\0') {
      if (str[1] == 'F') { slog = StepData_LFalse;    logic = 0; }
      else if (str[1] == 'T') { slog = StepData_LTrue;     logic = 1; }
      else if (str[1] == 'U') { slog = StepData_LUnknown;  logic = 2; }
    }
    if (logic >= 0) {
      if (!sm.IsNull()) sm->SetLogical(slog);
      else {
        sin = new StepData_SelectInt; val = sin;
        sin->SetLogical(slog);
      }
    }
    else {
      if (!sm.IsNull()) sm->SetEnum(logic, str);
      else {
        sna = new StepData_SelectNamed;  val = sna;  // Named sans nom...
        sna->SetEnum(logic, str);
      }
    }        // -> Select general
    return true;
  }
  case Interface_ParamLogical: break;
  case Interface_ParamText: {
    occ::handle<TCollection_HAsciiString> txt = new TCollection_HAsciiString(str);
    cleanText(txt);

    // PDN May 2000: for reading SOURCE_ITEM (external references)
    if (!val.IsNull()) {
      DeclareAndCast(StepData_SelectMember, sm, val);
      sm->SetString(txt->ToCString());
      return true;
    }

    val = txt;
    return true;
  }
  case Interface_ParamSub: {
    int numsub = SubListNumber(num, nump, false);
    int nbp = NbParams(numsub);
    if (nbp == 0) return false;    // liste vide = Handle Null
    const TCollection_AsciiString& rectyp = RecordType(numsub);
    if (nbp == 1 && rectyp.ToCString()[0] != '(') {
      //  SelectNamed because Field !!!
            // skl 15.01.2003 (for members with array of real)
      DeclareAndCast(StepData_SelectArrReal, sma, val);
      if (!sma.IsNull()) {
        int numsub2 = SubListNumber(numsub, 1, false);
        int nbp2 = NbParams(numsub2);
        if (nbp2 > 1) {
          if (Param(numsub2, 1).ParamType() == Interface_ParamReal) {
            if (!sma->SetName(rectyp.ToCString())) return false;
            occ::handle<TColStd_HSequenceOfReal> aSeq = new TColStd_HSequenceOfReal;
            for (int i = 1; i <= nbp2; i++) {
              if (Param(numsub2, i).ParamType() != Interface_ParamReal) continue;
              occ::handle<Standard_Transient> asr = new StepData_SelectReal;
              if (!ReadAny(numsub2, i, mess, ach, descr, asr)) continue;
              occ::handle<StepData_SelectReal> sm1 = occ::down_cast<StepData_SelectReal>(asr);
              if (!sm1.IsNull())
                aSeq->Append(sm1->Real());
            }
            occ::handle<TColStd_HArray1OfReal> anArr = new TColStd_HArray1OfReal(1, aSeq->Length());
            for (int nr = 1; nr <= aSeq->Length(); nr++) {
              anArr->SetValue(nr, aSeq->Value(nr));
            }
            sma->SetArrReal(anArr);
            return true;
          }
        }
      }
      DeclareAndCast(StepData_SelectMember, sm, val);
      if (sm.IsNull()) {
        sm = new StepData_SelectNamed;
        val = sm;
      }
      if (!sm->SetName(rectyp.ToCString())) return false;  // loupe
      return ReadAny(numsub, 1, mess, ach, descr, val);
    }
  }
  default: break;
  }
  return false;
}

//  ....

//=======================================================================
//function : ReadXY
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadXY(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  double& X, double& Y) const
{
  occ::handle<String> errmess;  // Null si pas d erreur
  int numsub = SubListNumber(num, nump, false);
  if (numsub != 0) {
    if (NbParams(numsub) == 2) {
      const Interface_FileParameter& FPX = Param(numsub, 1);
      if (FPX.ParamType() == Interface_ParamReal)  X =
        Interface_FileReaderData::Fastof(FPX.CValue());
      else errmess = new String("Parameter n0.%d (%s) : (X,Y) X not a Real");

      const Interface_FileParameter& FPY = Param(numsub, 2);
      if (FPY.ParamType() == Interface_ParamReal)  Y =
        Interface_FileReaderData::Fastof(FPY.CValue());
      else errmess = new String("Parameter n0.%d (%s) : (X,Y) Y not a Real");

    }
    else errmess = new String("Parameter n0.%d (%s) : (X,Y) has not 2 params");
  }
  else errmess = new String("Parameter n0.%d (%s) : (X,Y) not a SubList");

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : ReadXYZ
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadXYZ(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  double& X, double& Y,
  double& Z) const
{
  occ::handle<String> errmess;  // Null si pas d erreur
  int numsub = SubListNumber(num, nump, false);
  if (numsub != 0) {
    if (NbParams(numsub) == 3) {
      const Interface_FileParameter& FPX = Param(numsub, 1);
      if (FPX.ParamType() == Interface_ParamReal)  X =
        Interface_FileReaderData::Fastof(FPX.CValue());
      else errmess = new String("Parameter n0.%d (%s) : (X,Y,Z) X not a Real");

      const Interface_FileParameter& FPY = Param(numsub, 2);
      if (FPY.ParamType() == Interface_ParamReal)  Y =
        Interface_FileReaderData::Fastof(FPY.CValue());
      else errmess = new String("Parameter n0.%d (%s) : (X,Y,Z) Y not a Real");

      const Interface_FileParameter& FPZ = Param(numsub, 3);
      if (FPZ.ParamType() == Interface_ParamReal)  Z =
        Interface_FileReaderData::Fastof(FPZ.CValue());
      else errmess = new String("Parameter n0.%d (%s) : (X,Y,Z) Z not a Real");

    }
    else errmess = new String("Parameter n0.%d (%s) : (X,Y,Z) has not 3 params");
  }
  else errmess = new String("Parameter n0.%d (%s) : (X,Y,Z) not a SubList");

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : ReadReal
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadReal(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  double& val) const
{
  occ::handle<String> errmess;  // Null si pas d erreur
  if (nump > 0 && nump <= NbParams(num)) {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamReal || FP.ParamType() == Interface_ParamInteger) 
      val = Interface_FileReaderData::Fastof(FP.CValue());
    else errmess = new String("Parameter n0.%d (%s) not a Real");
  }
  else errmess = new String("Parameter n0.%d (%s) absent");

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##

//=======================================================================
//function : ReadEntity
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadEntity(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  const occ::handle<Standard_Type>& atype,
  occ::handle<Standard_Transient>& ent) const
{
  occ::handle<String> errmess;  // Null si pas d erreur
  bool warn = false;
  if (nump > 0 && nump <= NbParams(num)) {
    const Interface_FileParameter& FP = Param(num, nump);
    int nent = FP.EntityNumber();
    if (FP.ParamType() == Interface_ParamIdent) {
      warn = (acceptvoid > 0);
      if (nent > 0) {
	occ::handle<Standard_Transient> entent = BoundEntity(nent);
        if (entent.IsNull() || !entent->IsKind(atype))
        {
          errmess = new String("Parameter n0.%d (%s) : Entity has illegal type");
          if (!entent.IsNull() && entent->IsKind(STANDARD_TYPE(StepData_UndefinedEntity)))
            ent = entent;
        }
	else ent = entent;
      }
      else errmess = new String("Parameter n0.%d (%s) : Unresolved reference");
    }
    else {
      if (acceptvoid && FP.ParamType() == Interface_ParamVoid) warn = true;
      errmess = new String("Parameter n0.%d (%s) not an Entity");
    }
  }
  else {
    warn = (acceptvoid > 0);
    errmess = new String("Parameter n0.%d (%s) absent");
  }

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  if (warn) ach->AddWarning(txtmes, errmess->ToCString());
  else ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : ReadEntity
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadEntity(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  StepData_SelectType& sel) const
{
  occ::handle<String> errmess;  // Null si pas d erreur
  bool warn = false;
  if (nump > 0 && nump <= NbParams(num)) {
    const Interface_FileParameter& FP = Param(num, nump);
    int nent = FP.EntityNumber();
    if (FP.ParamType() == Interface_ParamIdent) {
      warn = (acceptvoid > 0);
      if (nent > 0) {
	occ::handle<Standard_Transient> entent = BoundEntity(nent);
        if (!sel.Matches(entent))
        {
          errmess = new String("Parameter n0.%d (%s) : Entity has illegal type");
          //fot not supported STEP entity
          if (!entent.IsNull() && entent->IsKind(STANDARD_TYPE(StepData_UndefinedEntity)))
            sel.SetValue(entent);
        }
	else
          sel.SetValue(entent);
      }
      else
        errmess = new String("Parameter n0.%d (%s) : Unresolved reference");
    }
    else if (FP.ParamType() == Interface_ParamVoid) {
      if (acceptvoid) warn = true;
      errmess = new String("Parameter n0.%d (%s) not an Entity");
    }
    else {
      // Cas restant : on s interesse en fait au SelectMember ...
      occ::handle<Standard_Transient> sm = sel.NewMember();
      // SelectMember qui assure ce role. Peut etre specialise
      if (!ReadAny(num, nump, mess, ach, sel.Description(), sm))
        errmess = new String("Parameter n0.%d (%s) : could not be read");
      if (!sel.Matches(sm))
        errmess = new String("Parameter n0.%d (%s) : illegal parameter type");
      else
        sel.SetValue(sm);
    }
  }
  else {
    warn = (acceptvoid > 0);
    errmess = new String("Parameter n0.%d (%s) absent");
  }

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  if (warn) ach->AddWarning(txtmes, errmess->ToCString());
  else ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##

//=======================================================================
//function : ReadInteger
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadInteger(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  int& val) const
{
  occ::handle<String> errmess;  // Null si pas d erreur
  bool warn = false;
  if (nump > 0 && nump <= NbParams(num)) {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamInteger)
      val = atoi(FP.CValue());
    else if (FP.ParamType() == Interface_ParamReal)
    {
      val = static_cast<int>(std::round(Interface_FileReaderData::Fastof(FP.CValue())));
      if (acceptvoid) warn = true;
      errmess = new String("Parameter n0.%d (%s) was rounded");
    }
    if (FP.ParamType() != Interface_ParamInteger &&
        FP.ParamType() != Interface_ParamReal)
      errmess = new String("Parameter n0.%d (%s) not an Integer");
  }
  else errmess = new String("Parameter n0.%d (%s) absent");

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  if (warn) ach->AddWarning(txtmes, errmess->ToCString());
  else ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : ReadBoolean
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadBoolean(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  bool& flag) const
{
  flag = true;
  occ::handle<String> errmess;  // Null si pas d erreur
  if (nump > 0 && nump <= NbParams(num)) {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamEnum) {
      const char* txt = FP.CValue();
      if (!strcmp(txt, ".T.")) flag = true;
      else if (!strcmp(txt, ".F.")) flag = false;
      else errmess = new String("Parameter n0.%d (%s) : Incorrect Boolean Value. It was set to true");
    }
    else errmess = new String("Parameter n0.%d (%s) not a Boolean. It was set to true");
  }
  else errmess = new String("Parameter n0.%d (%s) absent.It was set to true");

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : ReadLogical
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadLogical(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  StepData_Logical& flag) const
{
  occ::handle<String> errmess;  // Null si pas d erreur
  if (nump > 0 && nump <= NbParams(num)) {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamEnum) {
      const char* txt = FP.CValue();
      if (!strcmp(txt, ".T.")) flag = StepData_LTrue;
      else if (!strcmp(txt, ".F.")) flag = StepData_LFalse;
      else if (!strcmp(txt, ".U.")) flag = StepData_LUnknown;
      else errmess = new String("Parameter n0.%d (%s) : Incorrect Logical Value");
    }
    else errmess = new String("Parameter n0.%d (%s) not a Logical");
  }
  else errmess = new String("Parameter n0.%d (%s) absent");

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : ReadString
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadString(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  occ::handle<TCollection_HAsciiString>& val) const
{
  occ::handle<String> errmess;  // Null si pas d erreur
  bool warn = false;
  if (nump > 0 && nump <= NbParams(num)) {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamText) {
      /*const char* anStr = FP.CValue();
      if(strlen(anStr) < 3)
        val = new TCollection_HAsciiString("");
      else {
        val = new TCollection_HAsciiString(FP.CValue());
        CleanText (val);
      }*/
      val = new TCollection_HAsciiString(FP.CValue());
      cleanText(val);
    } else {
      if (acceptvoid && FP.ParamType() == Interface_ParamVoid) warn = true;
      errmess = new String("Parameter n0.%d (%s) not a quoted String");
    }
  }
  else errmess = new String("Parameter n0.%d (%s) absent");

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  if (warn) ach->AddWarning(txtmes, errmess->ToCString());
  else ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : ReadEnumParam
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadEnumParam(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  const char*& text) const
{
  occ::handle<String> errmess;  // Null si pas d erreur
  bool warn = false;
  if (nump > 0 && nump <= NbParams(num)) {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamEnum) {
      text = FP.CValue();
      warn = (acceptvoid > 0);
    } else if (FP.ParamType() == Interface_ParamVoid) {
      errmess =
        new String("Parameter n0.%d (%s) : Undefined Enumeration not allowed");
      warn = (acceptvoid > 0);
    }
    else errmess = new String("Parameter n0.%d (%s) not an Enumeration");
  }
  else errmess = new String("Parameter n0.%d (%s) absent");

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  if (warn) ach->AddWarning(txtmes, errmess->ToCString());
  else ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : FailEnumValue
//purpose  : 
//=======================================================================

void  StepData_StepReaderData::FailEnumValue(const int /* num */,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach) const
{
  occ::handle<String> errmess =
    new String("Parameter n0.%d (%s) : Incorrect Enumeration Value");
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  ach->AddFail(txtmes, errmess->ToCString());
}

//=======================================================================
//function : ReadEnum
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadEnum(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  const StepData_EnumTool& enumtool,
  int& val) const
{
  //  reprendre avec ReadEnumParam ?
  occ::handle<String> errmess;  // Null si pas d erreur
  bool warn = false;
  if (nump > 0 && nump <= NbParams(num)) {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() == Interface_ParamEnum) {
      val = enumtool.Value(FP.CValue());
      if (val >= 0) return true;
      else errmess = new String("Parameter n0.%d (%s) : Incorrect Enumeration Value");
      warn = (acceptvoid > 0);
    }
    else if (FP.ParamType() == Interface_ParamVoid) {
      val = enumtool.NullValue();
      if (val < 0) errmess =
        new String("Parameter n0.%d (%s) : Undefined Enumeration not allowed");
      warn = (acceptvoid > 0);
    }
    else errmess = new String("Parameter n0.%d (%s) not an Enumeration");
  }
  else errmess = new String("Parameter n0.%d (%s) absent");

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  if (warn)
    ach->AddWarning(txtmes, errmess->ToCString());
  else
    ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : ReadTypedParam
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::ReadTypedParam(const int num,
  const int nump,
  const bool mustbetyped,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  int& numr,
  int& numrp,
  TCollection_AsciiString& typ) const
{
  occ::handle<String> errmess;  // Null si pas d erreur
  if (nump > 0 && nump <= NbParams(num)) {
    const Interface_FileParameter& FP = Param(num, nump);
    if (FP.ParamType() != Interface_ParamSub) {
      //    Pas une sous-liste : OK si admis
      numr = num;  numrp = nump;  typ.Clear();
      if (mustbetyped) {
        errmess = new String("Parameter n0.%d (%s) : single, not typed");
        sprintf(txtmes, errmess->ToCString(), nump, mess);
        ach->AddFail(txtmes, errmess->ToCString());
        return false;
      }
      return true;
    }
    numr = FP.EntityNumber();  numrp = 1;
    if (NbParams(numr) != 1) errmess =
      new String("Parameter n0.%d (%s) : SubList, not typed");
    typ = RecordType(numr);
  }
  else errmess = new String("Parameter n0.%d (%s) absent");

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//=======================================================================
//function : CheckDerived
//purpose  : 
//=======================================================================

bool StepData_StepReaderData::CheckDerived(const int num,
  const int nump,
  const char* const mess,
  occ::handle<Interface_Check>& ach,
  const bool errstat) const
{
  occ::handle<String> errmess;  // Null si pas d erreur
  bool warn = !errstat;
  if (nump > 0 && nump <= NbParams(num)) {
    if (!strcmp(Param(num, nump).CValue(), "*")) return true;
    else errmess = new String("Parameter n0.%d (%s) not Derived");
    if (acceptvoid) warn = true;
  }
  else errmess = new String("Parameter n0.%d (%s) absent");

  if (errmess.IsNull()) return true;
  sprintf(txtmes, errmess->ToCString(), nump, mess);
  if (warn) ach->AddWarning(txtmes, errmess->ToCString());
  else      ach->AddFail(txtmes, errmess->ToCString());
  return false;
}

//  #########################################################################
// ....     Methodes specifiques (demandees par FileReaderData)     .... //

//=======================================================================
//function : NbEntities
//purpose  : 
//=======================================================================

int StepData_StepReaderData::NbEntities() const  // redefined
{
  return thenbents;
}

//=======================================================================
//function : FindNextRecord
//purpose  : 
//=======================================================================

int StepData_StepReaderData::FindNextRecord
(const int num) const
{
  // retourne, sur un numero d enregistrement donne (par num), le suivant qui
  // definit une entite, ou 0 si c est fini :
  // passe le Header (nbhend premiers records) et
  // saute les enregistrements SCOPE et ENDSCOPE et les SOUS-LISTES

  if (num < 0) return 0;
  int num1 = num + 1; if (num == 0) num1 = thenbhead + 1;
  int max = NbRecords();

  while (num1 <= max) {
    if (theidents(num1) > 0) return num1;

    // SCOPE,ENDSCOPE et Sous-Liste ont un identifieur fictif: -1,-2 respectivement
    // et SUBLIST ont un negatif. Seule une vraie entite a un Ident positif
    num1++;
  }
  return 0;
}

//=======================================================================
//function : FindEntityNumber
//purpose  : 
//=======================================================================

int StepData_StepReaderData::FindEntityNumber(const int num,
  const int id) const
{
  //  Soit un "Id" : recherche dans les Parametres de type Ident de <num>,
  //  si un d eux designe #Id justement. Si oui, retourne son EntityNumber
  if (num == 0) return 0;
  int nb = NbParams(num);
  for (int i = 1; i <= nb; i++) {
    const Interface_FileParameter& FP = Param(num, i);
    if (FP.ParamType() != Interface_ParamIdent) continue;
    int ixp = atoi(&FP.CValue()[1]);
    if (ixp == id) return FP.EntityNumber();
  }
  return 0;    // ici, pas trouve
}

//  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
// ....         La fonction qui suit merite une attention speciale        ....

//  Cette methode precharge les EntityNumbers dans les Params : ils designent
//  les Entites proprement dites dans la liste lue par BoundEntity
//  Interet : adresse de meme les sous-listes (Num->no record dans le Direc)
//  resultat exploite par ParamEntity et ParamNumber

//  En l absence de SCOPE, ou si les "ident" sont strictement ordonnes, a coup
//  sur ils ne sont pas dupliques, on peut utiliser une IndexedMap en toute
//  confiance. Sinon, il faut balayer dans le fichier, mais avec les SCOPES
//  cela va beaucoup plus vite (s ils sont assez gros) : on s y retrouve.

// Pour la recherche par balayage, On opere en plusieurs etapes
// Avant toute chose, le chargement a deja fait une preparation : les idents
// (Entity, SubList) sont deja en entiers (rapidite de lecture), en particulier
// dans les EntityNumber : ainsi, on lit cet ident, on le traite, et on remet
// a la place un vrai numero de Record
//
// D abord, on passe le directory en table d entiers,  sous-listes expurgees
// en // , table inverse vers cette table, car les sous-listes peuvent par
// contre designer des objets ...

// Pour les sous-listes, on exploite leur mode de construction : elles sont
// enregistrees AVANT d etre referencees. Un tableau "subn" note donc pour
// chaque numero de sous-liste (relatif a une entite qui suit, et reference
// par elle ou une autre sous-liste qui suit egalement), son n0 de record
// REMARQUE : ceci marche aussi pour le Header, traite par l occasion

//=======================================================================
//function : SetEntityNumbers
//purpose  : 
//=======================================================================

void StepData_StepReaderData::SetEntityNumbers(const bool withmap)
{
  Message_Messenger::StreamBuffer sout = Message::SendTrace();
  //   Passe initiale : Resolution directe par Map
  //   si tout passe (pas de collision), OK. Sinon, autres passes a prevoir
  //   On resoud du meme coup les sous-listes
  int nbdirec = NbRecords();
  TColStd_Array1OfInteger subn(0, thelastn);

  bool pbmap = false;        // au moins un conflit
  int nbmap = 0;
  TColStd_IndexedMapOfInteger imap(thenbents);
  TColStd_Array1OfInteger indm(0, nbdirec);    // Index Map -> Record Number (seulement si map)

  int num; // svv Jan11 2000 : porting on DEC
  for (num = 1; num <= nbdirec; num++) {
    int ident = theidents(num);
    if (ident > 0) {      // Ident normal -> Map ?
//  Map : si Recouvrement, l inhiber. Sinon, noter index
      int indmap = imap.Add(ident);
      if (indmap <= nbmap) {
        indmap = imap.FindIndex(ident);    // plus sur
        indm(indmap) = -1;      // Map -> pb
        pbmap = true;
        //  pbmap signifie qu une autre passe sera necessaire ...
      } else {
        nbmap = indmap;
        indm(indmap) = num;      // Map ->ident
      }
    }
  }

  for (num = 1; num <= nbdirec; num++) {
    int ident = theidents(num);
    if (ident < -2) subn(-(ident + 2)) = num;  // toujours a jour ...

    int nba = NbParams(num);
    int nda = (num == 1 ? 0 : ParamFirstRank(num - 1));

    for (int na = nba; na > 0; na--) {
      //    On traite : les sous-listes (sf subn), les idents (si Map dit OK ...)
      Interface_FileParameter& FP = ChangeParameter(nda + na);
      //      Interface_FileParameter& FP = ChangeParam (num,na);
      Interface_ParamType letype = FP.ParamType();
      if (letype == Interface_ParamSub) {
        int numsub = FP.EntityNumber();
        if (numsub > thelastn) {
          Message::SendInfo()
            << "Bad Sub.N0, Record " << num << " Param " << na << ":$" << numsub << std::endl;
          continue;
        }
        FP.SetEntityNumber(subn(numsub));
      } else if (letype == Interface_ParamIdent) {
        int id = FP.EntityNumber();
        int indmap = imap.FindIndex(id);
        if (indmap > 0) {                  // la map a trouve
          int num0 = indm(indmap);
          if (num0 > 0) FP.SetEntityNumber(num0);  // ET VOILA, on a resolu
          else FP.SetEntityNumber(-id);   // CONFLIT -> faudra resoudre ...
	} else {                          // NON RESOLU, si pas pbmap, le dire
          if (pbmap) {
            FP.SetEntityNumber(-id);
            continue;            // pbmap : on se retrouvera
          }
          char failmess[100];
          //  ...  Construire le Check  ...
          sprintf(failmess,
            "Unresolved Reference, Ent.Id.#%d Param.n0 %d (Id.#%d)",
            ident, na, id);
          thecheck->AddFail(failmess, "Unresolved Reference");
          //  ...  Et sortir message un peu plus complet
          sout << "*** ERR StepReaderData *** Entite #" << ident
               << "\n    Type:" << RecordType(num)
               << "  Param.n0 " << na << ": #" << id << " Not found" << std::endl;
        }      // FIN  Mapping
      }        // FIN  Traitement Reference
    }          // FIN  Boucle Parametres
  }            // FIN  Boucle Repertoires

  if (!pbmap) {
    return;
  }
  sout << " --  2nd pass required --";

  int nbseq = thenbents + 2 * thenbscop;
  TColStd_Array1OfInteger inds(0, nbseq);   // n0 Record/Entite
  TColStd_Array1OfInteger indi(0, nbseq);   // Idents/scopes
  TColStd_Array1OfInteger indr(0, nbdirec); // inverse de nds
  occ::handle<TColStd_HArray1OfInteger> indx;    // pour EXPORT (silya)

  imap.Clear();
  bool iamap = withmap;               // (par defaut True)
  nbmap = 0;

  TColStd_SequenceOfInteger scopile;  // chainage des scopes note par pile
  int nr = 0;
  for (num = 1; num <= nbdirec; num++) {
    int ident = theidents(num);
    if (ident < -2) {             // SOUS-LISTE (cas le plus courant)
      indr(num) = nr + 1;         // recherche basee sur nr (objet qui suit)
    } else if (ident >= 0) {      // Ident normal
      nr++;  inds(nr) = num;  indi(nr) = ident; indr(num) = nr;
      if (ident > 0) {   // et non (iamap && ident > 0)
//  Map : si Recouvrement, l inhiber. Sinon, noter index
        int indmap = imap.Add(ident);
        if (indmap <= nbmap) {
          bool errorscope = false;
          indmap = imap.FindIndex(ident);    // plus sur
          pbmap = true;
          if (thenbscop == 0) errorscope = true;
          //  Numeros identiques alors quilnya pas de SCOPE ? ERREUR !
          //  (Bien sur, silya des SCOPES, on passe au travers, mais bon...)
          else {
            //  Silya des SCOPES, tachons d y voir de plus pres pour signaler un probleme
            //  Erreur si MEME groupe SCOPE
            //  ATTENTION, on recherche, non dans tous les records, mais dans les records
            //    CHAINES, cf nr et non num (pas de sous-liste, chainage scope-endscope)
            int fromscope = nr;
            int toscope = indm(indmap);
            if (toscope < 0) toscope = -toscope;
            for (;;) {
              fromscope--;    // iteration de base
              if (fromscope <= toscope) {
                errorscope = true;  // BANG, on est dessus
                break;
              }
              int idtest = indi(fromscope);
              if (idtest >= 0) continue;  // le suivant (enfin, le precedent)
              if (idtest == -1) break;     // pas meme niveau, donc c est OK
              if (idtest == -3) {
                fromscope = inds(fromscope);
                if (fromscope < toscope) break;  // on sort, pas en meme niveau
              }
            }
          }
          if (errorscope) {
            //  On est dedans : le signaler
            char ligne[80];
            sprintf(ligne, "Ident defined SEVERAL TIMES : #%d", ident);
            thecheck->AddFail(ligne, "Ident defined SEVERAL TIMES : #%d");
            sout << "StepReaderData : SetEntityNumbers, " << ligne << std::endl;
          }
          if (indm(indmap) > 0) indm(indmap) = -indm(indmap);  // Pas pour Map
      //  Cas Normal pour la Map
	} else {
	  nbmap = indmap;
	  indm(indmap) = nr;      // Map ->(indm)->inds
	}
      }
    } else if (ident == -1) {     // SCOPE
      nr ++;  inds(nr) = num;  indi(nr) = -1;    indr(num) = 0;
      scopile.Append(nr) ;
    } else if (ident == -2) {     // ENDSCOPE
      int nscop = scopile.Last() ;     // chainage SCOPE-ENDSCOPE
      scopile.Remove(scopile.Length()) ;
      nr ++; inds(nr) = nscop; indi(nr) = -3; indr(num) = 0; inds(nscop) = nr;
      if (NbParams(num) > 0) {
//  EXPORT : traitement special greffe sur celui de SCOPE (sans le perturber)
	if (indx.IsNull()) {
    indx = new TColStd_HArray1OfInteger(0, nbseq);
    for (int ixp = 0; ixp <= nbseq; ixp ++) indx->ChangeValue(ixp) = 0;
	}
  indx->ChangeValue(nr) = num;  indx->ChangeValue(nscop) = num;
      }
    } else if (ident == 0) {      // HEADER
      indr(num) = 0;
    }
  }

  //  ..    Resolution des EXPORT, silyena et silya besoin    ..
  //  Pour chaque valeur de EXPORT qui n a pas ete resolue par la MAP,
  //  determiner sa position locale par recherche en arriere depuis ENDSCOPE
  if ((!iamap || pbmap) && !indx.IsNull()) {
    for (nr = 0; nr <= nbseq; nr++) {
      if (indx->Value(nr) == 0 && indi(nr) != -3) continue;  // ENDSCOPE + EXPORT
      num = indx->Value(nr);
      int nba = NbParams(num);
      for (int na = 1; na <= nba; na++) {
        Interface_FileParameter& FP = ChangeParam(num, na);
        if (FP.ParamType() != Interface_ParamIdent) continue;
        int id = -FP.EntityNumber();
        if (id < 0) continue;    // deja resolu en tete
      /*	if (imap.Contains(id)) {            et voila
          FP.SetEntityNumber(indm(imap.FindIndex(id)));
          continue;
        }    */

        //  Recherche du Id demande : si EXPORT imbrique, deja resolu mais il faut
        //  regarder ! (inutile par contre d aller y voir : c est deja fait, car
        //  un EXPORT imbrique a ete traite AVANT celui qui imbrique)
        int n0 = nr - 1;
        if (indi(n0) == -3) n0--;         // si on suit juste un ENDSCOPE
        while (n0 > 0) {
          int irec = indi(n0);
          if (irec == id) {                // trouve
            FP.SetEntityNumber(inds(n0));
            break;
          }
          if (irec == -1) break;           // SCOPE : fin de ce SCOPE/ENDSCOPE
          if (irec == -3) {
            //  gare a EXPORT : si un EXPORT detient Id, noter son Numero deja calcule
            //  Attention : Id a lire depuis CValue  car EntityNumber deja resolu
            int nok = FindEntityNumber(indx->Value(n0), id);
            if (nok > 0) {
              FP.SetEntityNumber(nok);
              break;
            }
            n0 = inds(n0);   // ENDSCOPE ou EXPORT infructueux : le sauter
          }      // fin traitement sur un ENDSCOPE ou EXPORT
          n0--;
        }        // fin resolution d un Parametre EXPORT
      }          // fin resolution de la liste d un EXPORT
    }            // fin bouclage sur les EXPORT
  }

  //  Exploitation de la table : bouclage porte sur la table

  //  Traitement des sous-listes : se fait dans la foulee, par gestion d une pile
  //  basee sur la constitution des sous-listes
  int maxsubpil = 30;  // pile simulee avec un Array : tres fort
  occ::handle<TColStd_HArray1OfInteger> subpile =  // ... gagne de la memoire ...
    new TColStd_HArray1OfInteger(1, maxsubpil);
  int nbsubpil = 0;              // ... et tellement plus rapide !

  for (num = 1; num <= nbdirec; num++) {
    nr = indr(num);
    if (nr == 0) continue;  //    pas un objet ou une sous-liste
    int nba = NbParams(num);
    for (int na = nba; na > 0; na--) {
      //  On lit depuis la fin : cela permet de traiter les sous-listes dans la foulee
      //  Sinon, on devrait noter qu il y a eu des sous-listes et reprendre ensuite

      Interface_FileParameter& FP = ChangeParam(num, na);
      Interface_ParamType letype = FP.ParamType();
      if (letype == Interface_ParamSub) {
        //  parametre type sous-liste : numero de la sous-liste lu par depilement
        FP.SetEntityNumber(subpile->Value(nbsubpil));
        nbsubpil--;   //	subpile->Remove(nbsubpil);

      } else if (letype == Interface_ParamIdent) {
        //  parametre type ident (reference une entite) : chercher ident demande
        int id = -FP.EntityNumber();
        if (id < 0) continue;    // deja resolu en tete

      // Voila : on va chercher id dans ndi; algorithme de balayage
        int pass, sens, nok, n0, irec;	pass = sens = nok = 0;
        if (!iamap) pass = 1;                  // si map non disponible
        while (pass < 3) {
          pass++;
          //    MAP disponible
          if (pass == 1) {                     // MAP DISPONIBLE
            int indmap = imap.FindIndex(id);
            if (indmap > 0) {                  // la map a trouve
              nok = indm(indmap);
              if (nok < 0) continue;           // CONFLIT -> faut resoudre ...
              break;
            }
            else continue;
          }
          //    1re Passe : REMONTEE -> Debut fichier
          if (sens == 0 && nr > 1) {
            n0 = nr - 1;
            if (indi(n0) == -3) n0--;         // si on suit juste un ENDSCOPE
            while (n0 > 0) {
              irec = indi(n0);
              if (irec == id) {                // trouve
                nok = n0; break;
              }
              //    ENDSCOPE : Attention a EXPORT sinon sauter
              if (irec == -3) {
                if (indx.IsNull()) n0 = inds(n0);
                else {
                  //    EXPORT, il faut regarder
                  nok = FindEntityNumber(indx->Value(n0), id);
                  if (nok > 0) break;
                  n0 = inds(n0);               // ENDSCOPE : le sauter
                }
              }
              n0--;
            }
            //    2me Passe : DESCENTE -> Fin fichier
	  } else if (nr < nbseq) {             // descente -> fin fichier
            n0 = nr + 1;
            while (n0 <= nbseq) {
              irec = indi(n0);
              if (irec == id) {                // trouve
                nok = n0; break;
              }
              //    SCOPE : Attention a EXPORT sinon sauter
              if (irec == -1) {
                if (indx.IsNull()) n0 = inds(n0);
                else {
                  //    EXPORT, il faut regarder
                  nok = FindEntityNumber(indx->Value(n0), id);
                  if (nok > 0) break;
                  n0 = inds(n0);               // SCOPE : le sauter
                }
              }
              n0++;
            }
          }
          if (nok > 0) break;
          sens = 1 - sens;      // passe suivante
        }
        // ici on a nok, numero trouve
        if (nok > 0) {
          int num0 = inds(nok);
          FP.SetEntityNumber(num0);  // ET VOILA, on a resolu

                                 // pas trouve : le signaler
	} else {
          //  Alimenter le Check ...  Pour cela, determiner n0 Entite et Ident
          char failmess[100];
          int nument = 0;
          int n0ent; // svv Jan11 2000 : porting on DEC
          for (n0ent = 1; n0ent <= nr; n0ent++) {
            if (indi(n0ent) > 0) nument++;
          }
          int ident = RecordIdent(num);
          if (ident < 0) {
            for (n0ent = num + 1; n0ent <= nbdirec; n0ent++) {
              ident = RecordIdent(n0ent); if (ident > 0) break;
            }
          }
          //  ...  Construire le Check  ...
          sprintf(failmess,
            "Unresolved Reference, Ent.n0 %d (Id.#%d) Param.n0 %d (Id.#%d)",
            nument, ident, na, id);
          thecheck->AddFail(failmess, "Unresolved Reference");

          //  ...  Et sortir message un peu plus complet
          sout << "*** ERR StepReaderData *** Entite " << nument
               << ", a " << (nr * 100) / nbseq << "% de DATA : #" << ident
               << "\n    Type:" << RecordType(num)
               << "  Param.n0 " << na << ": #" << id << " Not found" << std::endl;

          FP.SetEntityNumber(0);  // -> Reference non resolue
        }
      }
    }
    //  Si ce record est lui-meme une sous-liste, empiler !
    if (inds(nr) != num) {
      if (nbsubpil >= maxsubpil) {
        maxsubpil = maxsubpil + 30;
        occ::handle<TColStd_HArray1OfInteger> newsubpil =
          new TColStd_HArray1OfInteger(1, maxsubpil);
        for (int bidpil = 1; bidpil <= maxsubpil - 30; bidpil++)
          newsubpil->SetValue(bidpil, subpile->Value(bidpil));
        subpile = newsubpil;
      }
      nbsubpil++;
      subpile->SetValue(nbsubpil, num);      // Append(num);
    }
  }
}

//  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##  ##
//  ....             Gestion du Header : Preparation, lecture             ....

//=======================================================================
//function : FindNextHeaderRecord
//purpose  : 
//=======================================================================

int StepData_StepReaderData::FindNextHeaderRecord
(const int num) const
{
  // retourne, sur un numero d enregistrement donne (par num), le suivant qui
  // definit une entite, ou 0 si c est fini :
  // Opere comme FindNextRecord mais ne balaie que le Header

  if (num < 0) return 0;
  int num1 = num + 1;
  int max = thenbhead;

  while (num1 <= max) {
    // SCOPE,ENDSCOPE et Sous-Liste ont un identifieur negatif
    // Ne retenir que les Idents positifs ou nuls (nul : pas d Ident dans Header)
    if (RecordIdent(num1) >= 0) return num1;
    num1++;
  }
  return 0;
}

//=======================================================================
//function : PrepareHeader
//purpose  : 
//=======================================================================

void StepData_StepReaderData::PrepareHeader()
{
  // Resolution des references : ne concerne que les sous-listes
  //  deja faite par SetEntityNumbers donc pas de souci a se faire

  /*
  // Algorithme repris et adapte de SetEntityNumbers
  //  Traitement des sous-listes : se fait dans la foulee, par gestion d une pile
  //  basee sur la constitution des sous-listes
    TColStd_SequenceOfInteger subpile;
    int nbsubpil = 0;     // profondeur de pile mais plus rapide ...

    for (int num = 1 ; num <= thenbhead ; num ++) {
      int nba = NbParams(num) ;
      for (int na = nba ; na > 0 ; na --) {
  ..  On lit depuis la fin : cela permet de traiter les sous-listes dans la foulee
  ..  Sinon, on devrait noter qu il y a eu des sous-listes et reprendre ensuite

        Interface_FileParameter& FP = ChangeParam(num,na);
        Interface_ParamType letype = FP.ParamType();
        if (letype == Interface_ParamSub) {
  ..  parametre type sous-liste : numero de la sous-liste lu par depilement
          FP.SetEntityNumber(subpile.Last());
  .. ..        SetParam(num,na,FP);
    subpile.Remove(nbsubpil);
          nbsubpil --;
        }
      }
  .. Si c est une sous-liste, empiler
      if (RecordIdent(num) < -2) {
        subpile.Append(num);
        nbsubpil ++;
      }
    }
  */
}

//=======================================================================
//function : GlobalCheck
//purpose  : 
//=======================================================================

const occ::handle<Interface_Check>  StepData_StepReaderData::GlobalCheck() const
{
  return thecheck;
}
