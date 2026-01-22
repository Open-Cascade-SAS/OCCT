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

#include <Interface_MSG.hxx>
#include <NCollection_DataMap.hxx>
#include <OSD_Process.hxx>
#include <Quantity_Date.hxx>
#include <Standard_DomainError.hxx>
#include <Standard_Macro.hxx>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

#include <cstdio>
static NCollection_DataMap<TCollection_AsciiString, occ::handle<TCollection_HAsciiString>> thedic;
static NCollection_DataMap<TCollection_AsciiString, int>                                   thelist;
static occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>>           thedup;
static bool theprint = true;
static bool therec   = false;
static bool therun   = false;
static bool theraise = false;

static char blank[] =
  "                                                                            ";
static int maxblank = (int)strlen(blank);

Interface_MSG::Interface_MSG(const char* const key)
    : thekey(key),
      theval(nullptr)
{
}

Interface_MSG::Interface_MSG(const char* const key, const int i1)
    : thekey(key),
      theval(nullptr)
{
  char mess[300];
  Sprintf(mess, Interface_MSG::Translated(thekey), i1);
  theval = new char[strlen(mess) + 1];
  strcpy(theval, mess);
}

Interface_MSG::Interface_MSG(const char* const key, const int i1, const int i2)
    : thekey(key),
      theval(nullptr)
{
  char mess[300];
  Sprintf(mess, Interface_MSG::Translated(thekey), i1, i2);
  theval = new char[strlen(mess) + 1];
  strcpy(theval, mess);
}

Interface_MSG::Interface_MSG(const char* const key, const double r1, const int intervals)
    : thekey(key),
      theval(nullptr)
{
  char mess[300];
  Sprintf(mess,
          Interface_MSG::Translated(thekey),
          (intervals < 0 ? r1 : Interface_MSG::Intervalled(r1, intervals)));
  theval = new char[strlen(mess) + 1];
  strcpy(theval, mess);
}

Interface_MSG::Interface_MSG(const char* const key, const char* const str)
    : thekey(key),
      theval(nullptr)
{
  char mess[300];
  Sprintf(mess, Interface_MSG::Translated(thekey), str);
  theval = new char[strlen(mess) + 1];
  strcpy(theval, mess);
}

Interface_MSG::Interface_MSG(const char* const key, const int val, const char* const str)
    : thekey(key),
      theval(nullptr)
{
  char mess[300];
  Sprintf(mess, Interface_MSG::Translated(thekey), val, str);
  theval = new char[strlen(mess) + 1];
  strcpy(theval, mess);
}

const char* Interface_MSG::Value() const
{
  return (theval ? theval : Interface_MSG::Translated(thekey));
}

void Interface_MSG::Destroy()
{
  if (theval)
  {
    delete[] theval;
    theval = nullptr;
  }
}

Interface_MSG::operator const char*() const
{
  return Value();
}

//  ###########    Lecture Ecriture Fichier    ##########

int Interface_MSG::Read(Standard_IStream& S)
{
  int  i, nb = 0;
  char buf[200], key[200];
  buf[0] = '\0';
  while (S.getline(buf, 200))
  {
    if (buf[0] == '@' && buf[1] == '@')
      continue;
    if (buf[0] == '\0')
      continue;
    if (buf[0] == '@')
    {
      nb++;
      for (i = 1; i <= 199; i++)
      {
        key[i - 1] = buf[i];
        if (buf[i] == '\0')
          break;
      }
    }
    else
      Record(key, buf);
    buf[0] = '\0';
  }
  return nb;
}

int Interface_MSG::Read(const char* const file)
{
  std::ifstream S(file);
  if (!S)
    return -1;
  return Read(S);
}

int Interface_MSG::Write(Standard_OStream& S, const char* const rootkey)
{
  int nb = 0;
  if (thedic.IsEmpty())
    return nb;
  if (rootkey[0] != '\0')
    S << "@@ ROOT:" << rootkey << std::endl;
  NCollection_DataMap<TCollection_AsciiString, occ::handle<TCollection_HAsciiString>>::Iterator
    iter(thedic);
  for (; iter.More(); iter.Next())
  {
    if (!iter.Key().StartsWith(rootkey))
      continue;
    S << "@" << iter.Key() << "\n";
    const occ::handle<TCollection_HAsciiString>& str = iter.Value();
    if (str.IsNull())
      continue;
    nb++;
    S << str->ToCString() << "\n";
  }
  S << std::flush;
  return nb;
}

//  ###########   EXPLOITATION   ##########

bool Interface_MSG::IsKey(const char* const key)
{
  return (key[0] == '^');
}

const char* Interface_MSG::Translated(const char* const key)
{
  if (!therun)
    return key;
  if (!thedic.IsEmpty())
  {
    occ::handle<TCollection_HAsciiString> str;
    if (thedic.Find(key, str))
      return str->ToCString();
  }
  if (theprint)
    std::cout << " **  Interface_MSG:Translate ?? " << key << "  **" << std::endl;
  if (therec)
  {
    (*thelist.TryBound(key, 0))++;
  }
  if (theraise)
    throw Standard_DomainError("Interface_MSG : Translate");
  return key;
}

void Interface_MSG::Record(const char* const key, const char* const item)
{
  occ::handle<TCollection_HAsciiString> dup;
  occ::handle<TCollection_HAsciiString> str = new TCollection_HAsciiString(item);
  if (thedic.Bind(key, str))
    return; // newly added, skip the rest
  if (theprint)
    std::cout << " **  Interface_MSG:Record ?? " << key << " ** " << item << "  **" << std::endl;
  if (therec)
  {
    if (thedup.IsNull())
      thedup = new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
    dup = new TCollection_HAsciiString(key);
    thedup->Append(dup);
    dup = new TCollection_HAsciiString(item);
    thedup->Append(dup);
  }
  if (theraise)
    throw Standard_DomainError("Interface_MSG : Record");
}

void Interface_MSG::SetTrace(const bool toprint, const bool torecord)
{
  theprint = toprint;
  therec   = torecord;
}

void Interface_MSG::SetMode(const bool running, const bool raising)
{
  therun   = running;
  theraise = raising;
}

void Interface_MSG::PrintTrace(Standard_OStream& S)
{
  occ::handle<TCollection_HAsciiString> dup;
  int                                   i, nb = 0;
  if (!thedup.IsNull())
    nb = thedup->Length() / 2;
  for (i = 1; i <= nb; i++)
  {
    dup = thedup->Value(2 * i - 1);
    S << "** DUP:" << dup->ToCString();
    dup = thedup->Value(2 * i);
    S << " ** " << dup->ToCString() << std::endl;
  }

  if (thelist.IsEmpty())
    return;
  NCollection_DataMap<TCollection_AsciiString, int>::Iterator iter(thelist);
  for (; iter.More(); iter.Next())
  {
    S << "** MSG(NB=" << iter.Value() << "): " << iter.Key() << std::endl;
  }
}

//  ###########    FLOATING POINT ROUNDING    ############

double Interface_MSG::Intervalled(const double val, const int order, const bool upper)
{
  double vl = (val > 0. ? val : -val);
  double bl = 1., bu = 1.;
  if (vl >= 1.)
  {
    bu = 10.;
    for (int i = 0; i < 200; i++)
    {
      if (vl < bu)
        break;
      bl = bu;
      bu *= 10.;
    }
  }
  else
  {
    bl = 0.1;
    for (int i = 0; i < 200; i++)
    {
      if (vl >= bl)
        break;
      bu = bl;
      bl /= 10.;
    }
    if (vl == 0.)
      return 0.;
  }

  double rst = vl / bl;
  if (order <= 1)
    rst = (upper ? 10. : 1.);
  else if (order == 2)
  {
    if (rst <= 3.)
      rst = (upper ? 3. : 1.);
    else
      rst = (upper ? 10. : 3.);
  }
  else if (order == 3)
  {
    if (rst <= 2.)
      rst = (upper ? 2. : 1.);
    else if (rst <= 5.)
      rst = (upper ? 5. : 2.);
    else
      rst = (upper ? 10. : 5.);
  }
  else if (order == 4)
  {
    if (rst <= 2.)
      rst = (upper ? 2. : 1.);
    else if (rst <= 3.)
      rst = (upper ? 3. : 2.);
    else if (rst <= 6.)
      rst = (upper ? 6. : 3.);
    else
      rst = (upper ? 10. : 6.);
  }
  else if (order <= 6)
  {
    if (rst <= 1.5)
      rst = (upper ? 1.5 : 1.);
    else if (rst <= 2.)
      rst = (upper ? 2. : 1.5);
    else if (rst <= 3.)
      rst = (upper ? 3. : 2.);
    else if (rst <= 5.)
      rst = (upper ? 5. : 3.);
    else if (rst <= 7.)
      rst = (upper ? 7. : 5.);
    else
      rst = (upper ? 10. : 7.);
  }
  else
  { // only makes sense up to 10 ...
    if (rst <= 1.2)
      rst = (upper ? 1.2 : 1.);
    else if (rst <= 1.5)
      rst = (upper ? 1.5 : 1.2);
    else if (rst <= 2.)
      rst = (upper ? 2. : 1.5);
    else if (rst <= 2.5)
      rst = (upper ? 2.5 : 2.);
    else if (rst <= 3.)
      rst = (upper ? 3. : 2.5);
    else if (rst <= 4.)
      rst = (upper ? 4. : 3.);
    else if (rst <= 5.)
      rst = (upper ? 5. : 4.);
    else if (rst <= 6.)
      rst = (upper ? 6. : 5.);
    else if (rst <= 8.)
      rst = (upper ? 8. : 6.);
    else
      rst = (upper ? 10. : 8.);
  }
  return ((val < 0.) ? -(bl * rst) : (bl * rst));
}

//  ###########    DATES    ############

void Interface_MSG::TDate(const char* const text,
                          const int         yy,
                          const int         mm,
                          const int         dd,
                          const int         hh,
                          const int         mn,
                          const int         ss,
                          const char* const format)
{
  //  null values : at the beginning (with at least one non-null, the last one)
  //  -> completed with current values (system date)
  //  all null we leave

  // svv #2 int y1 , m1 , d1 , h1 , n1 , s1;
  int y2 = yy, m2 = mm, d2 = dd, h2 = hh, n2 = mn, s2 = ss;
  if (yy == 0 && ss != 0)
  {
    //  completion
    OSD_Process   pourdate;
    Quantity_Date ladate = pourdate.SystemDate();
    if (yy == 0)
    {
      y2 = ladate.Year();
      if (mm == 0)
      {
        m2 = ladate.Month();
        if (dd == 0)
        {
          d2 = ladate.Day();
          if (hh == 0)
          {
            h2 = ladate.Hour();
            if (mn == 0)
            {
              n2 = ladate.Minute();
              s2 = ladate.Second();
            }
          }
        }
      }
    }
  }
  char* pText = (char*)text;
  if (!format || format[0] == '\0')
    Sprintf(pText, "%4.4d-%2.2d-%2.2d:%2.2d-%2.2d-%2.2d", y2, m2, d2, h2, n2, s2);
  else if ((format[0] == 'c' || format[0] == 'C') && format[1] == ':')
    Sprintf(pText, &format[2], y2, m2, d2, h2, n2, s2);
}

bool Interface_MSG::NDate(const char* const text,
                          int&              yy,
                          int&              mm,
                          int&              dd,
                          int&              hh,
                          int&              mn,
                          int&              ss)
{
  int i, num = 1;
  for (i = 0; text[i] != '\0'; i++)
  {
    char val = text[i];
    if (val >= 48 && val <= 57)
    {
      if ((num & 1) == 0)
        num++;
      if (num == 1)
        yy = yy * 10 + (val - 48);
      if (num == 3)
        mm = mm * 10 + (val - 48);
      if (num == 5)
        dd = dd * 10 + (val - 48);
      if (num == 7)
        hh = hh * 10 + (val - 48);
      if (num == 9)
        mn = mn * 10 + (val - 48);
      if (num == 11)
        ss = ss * 10 + (val - 48);
    }
    else if ((num & 1) != 0)
      num++;
  }
  return (num > 0);
}

int Interface_MSG::CDate(const char* const text1, const char* const text2)
{
  int i1 = 0, i2 = 0, i3 = 0, i4 = 0, i5 = 0, i6 = 0, j1 = 0, j2 = 0, j3 = 0, j4 = 0, j5 = 0,
      j6 = 0;
  if (!NDate(text1, i1, i2, i3, i4, i5, i6))
    return 0;
  if (!NDate(text2, j1, j2, j3, j4, j5, j6))
    return 0;
  if (i1 < j1)
    return -1;
  if (i1 > j1)
    return 1;
  if (i2 < j2)
    return -1;
  if (i2 > j2)
    return 1;
  if (i3 < j3)
    return -1;
  if (i3 > j3)
    return 1;
  if (i4 < j4)
    return -1;
  if (i4 > j4)
    return 1;
  if (i5 < j5)
    return -1;
  if (i5 > j5)
    return 1;
  if (i6 < j6)
    return -1;
  if (i6 > j6)
    return 1;
  return 0;
}

const char* Interface_MSG::Blanks(const int val, const int max)
{
  int count;
  if (val < 0)
    return Interface_MSG::Blanks(-val, max - 1);
  if (val < 10)
    count = 9;
  else if (val < 100)
    count = 8;
  else if (val < 1000)
    count = 7;
  else if (val < 10000)
    count = 6;
  else if (val < 100000)
    count = 5;
  else if (val < 1000000)
    count = 4;
  else if (val < 10000000)
    count = 3;
  else if (val < 100000000)
    count = 2;
  else if (val < 1000000000)
    count = 1;
  else
    count = 0;
  count = count + max - 10;
  if (count < 0)
    count = 0;
  return &blank[maxblank - count];
}

const char* Interface_MSG::Blanks(const char* const val, const int max)
{
  int lng = (int)strlen(val);
  if (lng > maxblank || lng > max)
    return "";
  return &blank[maxblank - max + lng];
}

const char* Interface_MSG::Blanks(const int count)
{
  if (count <= 0)
    return "";
  if (count >= maxblank)
    return blank;
  return &blank[maxblank - count];
}

void Interface_MSG::Print(Standard_OStream& S, const char* const val, const int max, const int just)
{
  if (max > maxblank)
  {
    Print(S, val, maxblank, just);
    return;
  }
  int lng = (int)strlen(val);
  if (lng > max)
  {
    S << val;
    return;
  }
  int m1 = (max - lng) / 2;
  int m2 = max - lng - m1;
  if (just < 0)
    S << val << &blank[maxblank - m1 - m2];
  else if (just == 0)
    S << &blank[maxblank - m1] << val << &blank[maxblank - m2];
  else
    S << &blank[maxblank - m1 - m2] << val;
}
