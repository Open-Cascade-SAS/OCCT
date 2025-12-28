// Created on: 1996-09-26
// Created by: Arnaud BOUZY
// Copyright (c) 1996-1999 Matra Datavision
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

#include <Resource_ConvertUnicode.hxx>
#include <Resource_Manager.hxx>
#include <Resource_Unicode.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <NCollection_UtfString.hxx>
#include <Standard_NotImplemented.hxx>
#include "Resource_CodePages.pxx"
#include "Resource_GBK.pxx"
#include "Resource_Big5.pxx"

#define isjis(c) (((c) >= 0x21 && (c) <= 0x7e))
#define iseuc(c) (((c) >= 0xa1 && (c) <= 0xfe))
#define issjis1(c) (((c) >= 0x81 && (c) <= 0x9f) || ((c) >= 0xe0 && (c) <= 0xef))

#define issjis2(c) ((c) >= 0x40 && (c) <= 0xfc && (c) != 0x7f)

#define ishankana(c) ((c) >= 0xa0 && (c) <= 0xdf)

static inline bool isshift(unsigned char c)
{
  return c >= 0x80;
}

static inline bool isshift(unsigned int c)
{
  return c >= 0x80 && c <= 0xff;
}

void Resource_Unicode::ConvertSJISToUnicode(const char* fromstr, TCollection_ExtendedString& tostr)
{
  tostr.Clear();

  unsigned char* currentstr = ((unsigned char*)fromstr);
  unsigned int   pl, ph;
  // BIG INDIAN USED HERE
  while (*currentstr != '\0')
  {
    if (issjis1(*currentstr))
    {

      ph = ((unsigned int)*currentstr);
      // Be Careful with first and second !!

      currentstr++;

      pl = ((unsigned int)*currentstr);
      currentstr++;

      Resource_sjis_to_unicode(&ph, &pl);
      char16_t                   curcar = ((char16_t)((ph << 8) | pl));
      TCollection_ExtendedString curext(curcar);
      tostr.AssignCat(curext);
    }
    else
    {
      TCollection_ExtendedString curext(((char)*currentstr));
      currentstr++;
      tostr.AssignCat(curext);
    }
  }
}

void Resource_Unicode::ConvertEUCToUnicode(const char* fromstr, TCollection_ExtendedString& tostr)
{
  tostr.Clear();

  unsigned char* currentstr = ((unsigned char*)fromstr);
  unsigned int   pl, ph;
  // BIG INDIAN USED HERE
  while (*currentstr != '\0')
  {
    if (iseuc(*currentstr))
    {

      ph = ((unsigned int)*currentstr);
      // Be Careful with first and second !!

      currentstr++;

      pl = ((unsigned int)*currentstr);
      currentstr++;

      Resource_euc_to_unicode(&ph, &pl);
      char16_t                   curcar = ((char16_t)((ph << 8) | pl));
      TCollection_ExtendedString curext(curcar);
      tostr.AssignCat(curext);
    }
    else
    {
      TCollection_ExtendedString curext(((char)*currentstr));
      currentstr++;
      tostr.AssignCat(curext);
    }
  }
}

void Resource_Unicode::ConvertGBToUnicode(const char* fromstr, TCollection_ExtendedString& tostr)
{
  tostr.Clear();

  unsigned char* currentstr = ((unsigned char*)fromstr);
  unsigned int   pl, ph;
  // BIG INDIAN USED HERE
  while (*currentstr != '\0')
  {
    if (isshift(*currentstr))
    {

      ph = ((unsigned int)*currentstr);
      // Be Careful with first and second !!

      currentstr++;

      pl = ((unsigned int)*currentstr);
      currentstr++;

      Resource_gb_to_unicode(&ph, &pl);
      char16_t                   curcar = ((char16_t)((ph << 8) | pl));
      TCollection_ExtendedString curext(curcar);
      tostr.AssignCat(curext);
    }
    else
    {
      TCollection_ExtendedString curext(((char)*currentstr));
      currentstr++;
      tostr.AssignCat(curext);
    }
  }
}

bool Resource_Unicode::ConvertGBKToUnicode(const char* fromstr, TCollection_ExtendedString& tostr)
{
  tostr.Clear();

  unsigned char* currentch = ((unsigned char*)fromstr);
  unsigned int   gb1 = 0x00, gb2 = 0x00, gb3 = 0x00;

  while (*currentch != '\0')
  {
    if (gb3 != 0x00)
    {
      if (!(*currentch >= 0x30 && *currentch <= 0x39))
      {
        TCollection_ExtendedString curext3(((char)*currentch));
        TCollection_ExtendedString curext2(((char)gb3));
        TCollection_ExtendedString curext1(((char)gb2));
        tostr.Insert(0, curext3);
        tostr.Insert(0, curext2);
        tostr.Insert(0, curext1);
        return false;
      }

      unsigned int codepnt = ((gb1 - 0x81) * (10 * 126 * 10)) + ((gb2 - 0x30) * (10 * 126))
                             + ((gb3 - 0x81) * 10) + *currentch - 0x30;
      if (codepnt < 23940)
      {
        unsigned short             uni    = gbkuni[codepnt];
        char16_t                   curcar = ((char16_t)uni);
        TCollection_ExtendedString curext(curcar);
        tostr.AssignCat(curext);
        currentch++;
        continue;
      }

      return false;
    }
    else if (gb2 != 0x00)
    {
      if (*currentch >= 0x81 && *currentch <= 0xFE)
      {
        gb3 = (unsigned int)(*currentch);
        currentch++;
        continue;
      }
      TCollection_ExtendedString curext2(((char)*currentch));
      TCollection_ExtendedString curext1(((char)gb2));
      tostr.Insert(0, curext2);
      tostr.Insert(0, curext1);
      return false;
    }
    else if (gb1 != 0x00)
    {
      if (*currentch >= 0x30 && *currentch <= 0x39)
      {
        gb2 = (unsigned int)(*currentch);
        currentch++;
        continue;
      }

      unsigned int lead    = gb1;
      unsigned int pointer = 0;
      gb1                  = 0x00;
      unsigned int offset  = *currentch < 0x7F ? 0x40 : 0x41;

      if ((*currentch >= 0x40 && *currentch <= 0x7E) || (*currentch >= 0x80 && *currentch <= 0xFE))
      {
        pointer = (lead - 0x81) * 190 + (*currentch - offset);

        if (pointer < 23940)
        {
          unsigned short             uni    = gbkuni[pointer];
          char16_t                   curcar = ((char16_t)uni);
          TCollection_ExtendedString curext(curcar);
          tostr.AssignCat(curext);
          currentch++;
          continue;
        }
      }
      if (*currentch <= 0x7F)
      {
        // ASCII symbol
        TCollection_ExtendedString curext(((char)*currentch));
        currentch++;
        tostr.Insert(0, curext);
        continue;
      }
      return false;
    }
    else
    {
      if (*currentch <= 0x7F)
      {
        // ASCII symbol
        TCollection_ExtendedString curext(((char)*currentch));
        currentch++;
        tostr.AssignCat(curext);
      }
      else if (*currentch == 0x80)
      {
        // Special symbol
        char16_t                   curcar = ((char16_t)((0x20 << 8) | 0xAC));
        TCollection_ExtendedString curext(curcar);
        tostr.AssignCat(curext);
        currentch++;
      }
      else if (*currentch >= 0x81 && *currentch <= 0xFE)
      {
        // Chinese symbol
        gb1 = (unsigned int)(*currentch);
        currentch++;
      }
      else
        return false;
    }
  }
  return true;
}

bool Resource_Unicode::ConvertBig5ToUnicode(const char* fromstr, TCollection_ExtendedString& tostr)
{
  tostr.Clear();

  unsigned char* currentch = ((unsigned char*)fromstr);
  unsigned int   big5lead  = 0x00;

  while (*currentch != '\0')
  {
    if (big5lead != 0x00)
    {
      unsigned int lead    = big5lead;
      unsigned int pointer = 0;
      big5lead             = 0x00;
      unsigned int offset  = *currentch < 0x7F ? 0x40 : 0x62;

      if ((*currentch >= 0x40 && *currentch <= 0x7E) || (*currentch >= 0xA1 && *currentch <= 0xFE))
      {
        pointer = (lead - 0x81) * 157 + (*currentch - offset);

        int aLength = tostr.Length();
        switch (pointer)
        {
          case 1133: {
            tostr.Insert(aLength + 1, (char16_t)0x00CA);
            tostr.Insert(aLength + 2, (char16_t)0x0304);
            currentch++;
            continue;
          }
          case 1135: {
            tostr.Insert(aLength + 1, (char16_t)0x00CA);
            tostr.Insert(aLength + 2, (char16_t)0x030C);
            currentch++;
            continue;
          }
          case 1164: {
            tostr.Insert(aLength + 1, (char16_t)0x00EA);
            tostr.Insert(aLength + 2, (char16_t)0x0304);
            currentch++;
            continue;
          }
          case 1166: {
            tostr.Insert(aLength + 1, (char16_t)0x00EA);
            tostr.Insert(aLength + 2, (char16_t)0x030C);
            currentch++;
            continue;
          }
          default: {
            if (pointer < 19782)
            {
              unsigned int uni = big5uni[pointer];
              if (uni <= 0xFFFF)
              {
                char16_t curcar = ((char16_t)uni);
                tostr.Insert(aLength + 1, curcar);
              }
              else
              {
                char32_t                        aChar32[] = {uni};
                NCollection_UtfString<char32_t> aStr32(aChar32);
                NCollection_UtfString<char16_t> aStr16 = aStr32.ToUtf16();

                if (aStr16.Size() != 4)
                  return false; // not a surrogate pair
                const char16_t* aChar16 = aStr16.ToCString();
                tostr.Insert(aLength + 1, (char16_t)(*aChar16));
                aChar16++;
                tostr.Insert(aLength + 2, (char16_t)(*aChar16));
              }
              currentch++;
              continue;
            }
          }
        }
      }
      if (*currentch <= 0x7F)
      {
        // ASCII symbol
        TCollection_ExtendedString curext(((char)*currentch));
        currentch++;
        tostr.Insert(0, curext);
        continue;
      }
      return false;
    }
    else
    {
      if (*currentch <= 0x7F)
      {
        // ASCII symbol
        TCollection_ExtendedString curext(((char)*currentch));
        currentch++;
        tostr.AssignCat(curext);
      }
      else if (*currentch >= 0x81 && *currentch <= 0xFE)
      {
        // Chinese symbol
        big5lead = (unsigned int)(*currentch);
        currentch++;
      }
      else
        return false;
    }
  }
  return true;
}

bool Resource_Unicode::ConvertUnicodeToSJIS(const TCollection_ExtendedString& fromstr,
                                            Standard_PCharacter&              tostr,
                                            const int                         maxsize)
{
  int          nbtrans  = 0;
  int          nbext    = 1;
  bool         finished = false;
  char16_t     curcar;
  unsigned int pl, ph;
  // BIG INDIAN USED HERE

  while (!finished)
  {
    if (nbext > fromstr.Length())
    {
      finished       = true;
      tostr[nbtrans] = '\0';
    }
    else
    {
      curcar = fromstr.Value(nbext);
      nbext++;
      ph = (((unsigned int)curcar) >> 8) & 0xFF;
      pl = ((unsigned int)curcar) & 0xFF;
      Resource_unicode_to_sjis(&ph, &pl);
      if (issjis1(ph))
      {
        if (nbtrans < (maxsize - 3))
        {
          tostr[nbtrans] = ((char)ph);
          nbtrans++;
          tostr[nbtrans] = ((char)pl);
          nbtrans++;
        }
        else
        {
          tostr[nbtrans] = '\0';
          return false;
        }
      }
      else
      {
        tostr[nbtrans] = ((char)pl);
        nbtrans++;
      }
      if (nbtrans >= (maxsize - 1))
      {
        tostr[maxsize - 1] = '\0';
        return false;
      }
    }
  }
  return true;
}

bool Resource_Unicode::ConvertUnicodeToEUC(const TCollection_ExtendedString& fromstr,
                                           Standard_PCharacter&              tostr,
                                           const int                         maxsize)
{
  int          nbtrans  = 0;
  int          nbext    = 1;
  bool         finished = false;
  char16_t     curcar;
  unsigned int pl, ph;
  // BIG INDIAN USED HERE

  while (!finished)
  {
    if (nbext > fromstr.Length())
    {
      finished       = true;
      tostr[nbtrans] = '\0';
    }
    else
    {
      curcar = fromstr.Value(nbext);
      nbext++;
      ph = (((unsigned int)curcar) >> 8) & 0xFF;
      pl = ((unsigned int)curcar) & 0xFF;
      Resource_unicode_to_euc(&ph, &pl);
      if (iseuc(ph))
      {
        if (nbtrans < (maxsize - 3))
        {
          tostr[nbtrans] = ((char)ph);
          nbtrans++;
          tostr[nbtrans] = ((char)pl);
          nbtrans++;
        }
        else
        {
          tostr[nbtrans - 1] = '\0';
          return false;
        }
      }
      else
      {
        tostr[nbtrans] = ((char)pl);
        nbtrans++;
      }
      if (nbtrans >= (maxsize - 1))
      {
        tostr[maxsize - 1] = '\0';
        return false;
      }
    }
  }
  return true;
}

bool Resource_Unicode::ConvertUnicodeToGB(const TCollection_ExtendedString& fromstr,
                                          Standard_PCharacter&              tostr,
                                          const int                         maxsize)
{
  int          nbtrans  = 0;
  int          nbext    = 1;
  bool         finished = false;
  char16_t     curcar;
  unsigned int pl, ph;
  // BIG INDIAN USED HERE

  while (!finished)
  {
    if (nbext > fromstr.Length())
    {
      finished       = true;
      tostr[nbtrans] = '\0';
    }
    else
    {
      curcar = fromstr.Value(nbext);
      nbext++;
      ph = (((unsigned int)curcar) >> 8) & 0xFF;
      pl = ((unsigned int)curcar) & 0xFF;
      Resource_unicode_to_gb(&ph, &pl);
      if (isshift(ph))
      {
        if (nbtrans < (maxsize - 3))
        {
          tostr[nbtrans] = ((char)ph);
          nbtrans++;
          tostr[nbtrans] = ((char)pl);
          nbtrans++;
        }
        else
        {
          tostr[nbtrans - 1] = '\0';
          return false;
        }
      }
      else
      {
        tostr[nbtrans] = ((char)curcar) & 0xFF;
        nbtrans++;
      }
      if (nbtrans >= (maxsize - 1))
      {
        tostr[maxsize - 1] = '\0';
        return false;
      }
    }
  }
  return true;
}

bool Resource_Unicode::ConvertUnicodeToANSI(const TCollection_ExtendedString& fromstr,
                                            Standard_PCharacter&              tostr,
                                            const int                         maxsize)
{
  int          nbtrans  = 0;
  int          nbext    = 1;
  bool         finished = false;
  char16_t     curcar;
  unsigned int pl, ph;
  // BIG INDIAN USED HERE

  while (!finished)
  {
    if (nbext > fromstr.Length())
    {
      finished       = true;
      tostr[nbtrans] = '\0';
    }
    else
    {
      curcar = fromstr.Value(nbext);
      nbext++;
      ph = ((unsigned int)curcar) >> 8;
      pl = ((unsigned int)curcar) & 0xFF;
      if (ph == 0)
      {
        tostr[nbtrans] = ((char)pl);
      }
      else
      {
        tostr[nbtrans] = ' ';
      }
      nbtrans++;
    }
    if (nbtrans >= (maxsize - 1))
    {
      tostr[maxsize - 1] = '\0';
      return false;
    }
  }
  return true;
}

static bool AlreadyRead = false;

static Resource_FormatType& Resource_Current_Format()
{
  static Resource_FormatType theformat = Resource_ANSI;
  if (!AlreadyRead)
  {
    AlreadyRead                       = true;
    occ::handle<Resource_Manager> mgr = new Resource_Manager("CharSet");
    if (mgr->Find("FormatType"))
    {
      TCollection_AsciiString form = mgr->Value("FormatType");
      if (form.IsEqual("SJIS"))
      {
        theformat = Resource_SJIS;
      }
      else if (form.IsEqual("EUC"))
      {
        theformat = Resource_EUC;
      }
      else if (form.IsEqual("GB"))
      {
        theformat = Resource_GB;
      }
      else
      {
        theformat = Resource_ANSI;
      }
    }
    else
    {
      theformat = Resource_ANSI;
    }
  }
  return theformat;
}

void Resource_Unicode::SetFormat(const Resource_FormatType typecode)
{
  AlreadyRead               = true;
  Resource_Current_Format() = typecode;
}

Resource_FormatType Resource_Unicode::GetFormat()
{
  return Resource_Current_Format();
}

void Resource_Unicode::ReadFormat()
{
  AlreadyRead = false;
  Resource_Unicode::GetFormat();
}

void Resource_Unicode::ConvertFormatToUnicode(const Resource_FormatType   theFormat,
                                              const char*                 theFromStr,
                                              TCollection_ExtendedString& theToStr)
{
  switch (theFormat)
  {
    case Resource_FormatType_SJIS: {
      ConvertSJISToUnicode(theFromStr, theToStr);
      break;
    }
    case Resource_FormatType_EUC: {
      ConvertEUCToUnicode(theFromStr, theToStr);
      break;
    }
    case Resource_FormatType_GB: {
      ConvertGBToUnicode(theFromStr, theToStr);
      break;
    }
    case Resource_FormatType_ANSI: {
      theToStr = TCollection_ExtendedString(theFromStr, false);
      break;
    }
    case Resource_FormatType_CP1250:
    case Resource_FormatType_CP1251:
    case Resource_FormatType_CP1252:
    case Resource_FormatType_CP1253:
    case Resource_FormatType_CP1254:
    case Resource_FormatType_CP1255:
    case Resource_FormatType_CP1256:
    case Resource_FormatType_CP1257:
    case Resource_FormatType_CP1258:
    case Resource_FormatType_iso8859_1:
    case Resource_FormatType_iso8859_2:
    case Resource_FormatType_iso8859_3:
    case Resource_FormatType_iso8859_4:
    case Resource_FormatType_iso8859_5:
    case Resource_FormatType_iso8859_6:
    case Resource_FormatType_iso8859_7:
    case Resource_FormatType_iso8859_8:
    case Resource_FormatType_iso8859_9:
    case Resource_FormatType_CP850: {
      const int       aCodePageIndex = (int)theFormat - (int)Resource_FormatType_CP1250;
      const char16_t* aCodePage      = THE_CODEPAGES_ANSI[aCodePageIndex];
      theToStr.Clear();
      for (const char* anInputPntr = theFromStr; *anInputPntr != '\0'; ++anInputPntr)
      {
        unsigned char anInputChar = (unsigned char)(*anInputPntr);
        char16_t aRes = (anInputChar & 0x80) != 0 ? aCodePage[(0x7f & anInputChar)] : anInputChar;
        if (aRes == 0)
        {
          aRes = '?';
        }
        theToStr.AssignCat(aRes);
      }
      break;
    }
    case Resource_FormatType_Big5: {
      ConvertBig5ToUnicode(theFromStr, theToStr);
      break;
    }
    case Resource_FormatType_GBK: {
      ConvertGBKToUnicode(theFromStr, theToStr);
      break;
    }
    case Resource_FormatType_UTF8: {
      theToStr = TCollection_ExtendedString(theFromStr, true);
      break;
    }
    case Resource_FormatType_SystemLocale: {
      NCollection_UtfString<char16_t> aString;
      aString.FromLocale(theFromStr);
      theToStr = TCollection_ExtendedString(aString.ToCString());
      break;
    }
  }
}

bool Resource_Unicode::ConvertUnicodeToFormat(const Resource_FormatType         theFormat,
                                              const TCollection_ExtendedString& theFromStr,
                                              Standard_PCharacter&              theToStr,
                                              const int                         theMaxSize)
{
  switch (theFormat)
  {
    case Resource_FormatType_SJIS: {
      return ConvertUnicodeToSJIS(theFromStr, theToStr, theMaxSize);
    }
    case Resource_FormatType_EUC: {
      return ConvertUnicodeToEUC(theFromStr, theToStr, theMaxSize);
    }
    case Resource_FormatType_GB: {
      return ConvertUnicodeToGB(theFromStr, theToStr, theMaxSize);
    }
    case Resource_FormatType_ANSI: {
      return ConvertUnicodeToANSI(theFromStr, theToStr, theMaxSize);
    }
    case Resource_FormatType_CP1250:
    case Resource_FormatType_CP1251:
    case Resource_FormatType_CP1252:
    case Resource_FormatType_CP1253:
    case Resource_FormatType_CP1254:
    case Resource_FormatType_CP1255:
    case Resource_FormatType_CP1256:
    case Resource_FormatType_CP1257:
    case Resource_FormatType_CP1258:
    case Resource_FormatType_iso8859_1:
    case Resource_FormatType_iso8859_2:
    case Resource_FormatType_iso8859_3:
    case Resource_FormatType_iso8859_4:
    case Resource_FormatType_iso8859_5:
    case Resource_FormatType_iso8859_6:
    case Resource_FormatType_iso8859_7:
    case Resource_FormatType_iso8859_8:
    case Resource_FormatType_iso8859_9:
    case Resource_FormatType_CP850: {
      if (theMaxSize < theFromStr.Length())
      {
        return false;
      }
      const int       aCodePageIndex = (int)theFormat - (int)Resource_FormatType_CP1250;
      const char16_t* aCodePage      = THE_CODEPAGES_ANSI[aCodePageIndex];
      for (int aToCharInd = 0; aToCharInd < theMaxSize - 1; ++aToCharInd)
      {
        bool     isFind    = false;
        char16_t aFromChar = theFromStr.Value(aToCharInd + 1);
        if (aFromChar == 0)
        {
          // zero value should be handled explicitly to avoid false conversion by
          // selected code page that may have unused values (encoded as zero)
          theToStr[aToCharInd] = '\0';
        }
        else
        {
          // find the character in the code page
          for (unsigned char anIndCP = 0; aFromChar != 0 && anIndCP < 128; ++anIndCP)
          {
            if (aCodePage[anIndCP] == aFromChar)
            {
              theToStr[aToCharInd] = anIndCP | 0x80;
              isFind               = true;
            }
          }
          // if character is not found, put '?'
          if (!isFind)
          {
            theToStr[aToCharInd] = '?';
          }
        }
      }
      theToStr[theMaxSize - 1] = '\0';
      return true;
    }
    case Resource_FormatType_UTF8: {
      if (theMaxSize < theFromStr.LengthOfCString())
      {
        return false;
      }
      theFromStr.ToUTF8CString(theToStr);
      return true;
    }
    case Resource_FormatType_SystemLocale: {
      const NCollection_UtfString<char16_t> aString(theFromStr.ToExtString());
      return aString.ToLocale(theToStr, theMaxSize);
    }
    case Resource_FormatType_GBK:
    case Resource_FormatType_Big5: {
      throw Standard_NotImplemented("Resource_Unicode::ConvertUnicodeToFormat - convert from GBK "
                                    "and Big5 to Unocode is not implemented");
    }
  }
  return false;
}
