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

#include <StepData_EnumTool.hxx>
#include <TCollection_AsciiString.hxx>

StepData_EnumTool::StepData_EnumTool(const char* const e0,
                                     const char* const e1,
                                     const char* const e2,
                                     const char* const e3,
                                     const char* const e4,
                                     const char* const e5,
                                     const char* const e6,
                                     const char* const e7,
                                     const char* const e8,
                                     const char* const e9,
                                     const char* const e10,
                                     const char* const e11,
                                     const char* const e12,
                                     const char* const e13,
                                     const char* const e14,
                                     const char* const e15,
                                     const char* const e16,
                                     const char* const e17,
                                     const char* const e18,
                                     const char* const e19,
                                     const char* const e20,
                                     const char* const e21,
                                     const char* const e22,
                                     const char* const e23,
                                     const char* const e24,
                                     const char* const e25,
                                     const char* const e26,
                                     const char* const e27,
                                     const char* const e28,
                                     const char* const e29,
                                     const char* const e30,
                                     const char* const e31,
                                     const char* const e32,
                                     const char* const e33,
                                     const char* const e34,
                                     const char* const e35,
                                     const char* const e36,
                                     const char* const e37,
                                     const char* const e38,
                                     const char* const e39)
{
  AddDefinition(e0);
  AddDefinition(e1);
  AddDefinition(e2);
  AddDefinition(e3);
  AddDefinition(e4);
  AddDefinition(e5);
  AddDefinition(e6);
  AddDefinition(e7);
  AddDefinition(e8);
  AddDefinition(e9);
  AddDefinition(e10);
  AddDefinition(e11);
  AddDefinition(e12);
  AddDefinition(e13);
  AddDefinition(e14);
  AddDefinition(e15);
  AddDefinition(e16);
  AddDefinition(e17);
  AddDefinition(e18);
  AddDefinition(e19);
  AddDefinition(e20);
  AddDefinition(e21);
  AddDefinition(e22);
  AddDefinition(e23);
  AddDefinition(e24);
  AddDefinition(e25);
  AddDefinition(e26);
  AddDefinition(e27);
  AddDefinition(e28);
  AddDefinition(e29);
  AddDefinition(e30);
  AddDefinition(e31);
  AddDefinition(e32);
  AddDefinition(e33);
  AddDefinition(e34);
  AddDefinition(e35);
  AddDefinition(e36);
  AddDefinition(e37);
  AddDefinition(e38);
  AddDefinition(e39);
  theinit = thetexts.Length();
  theopt  = true;
}

void StepData_EnumTool::AddDefinition(const char* const term)
{
  char text[80];
  if (!term)
    return;
  if (term[0] == '\0')
    return;
  int n0 = 0, n1 = 0;
  for (; term[n0] != '\0'; n0++)
  {
    if (term[n0] <= 32)
    {
      if (n1 == 0)
        continue;
      if (n1 > 1 || text[0] != '$')
      {
        if (text[n1 - 1] != '.')
        {
          text[n1] = '.';
          n1++;
        }
        text[n1] = '\0';
      }
      thetexts.Append(TCollection_AsciiString(text));
      n1 = 0;
    }
    if (n1 == 0 && term[n0] != '.' && term[n0] != '$')
    {
      text[0] = '.';
      n1++;
    }
    text[n1] = term[n0];
    n1++;
  }
  if (n1 > 0 || text[0] != '$')
  {
    if (text[n1 - 1] != '.')
    {
      text[n1] = '.';
      n1++;
    }
    text[n1] = '\0';
  }
  if (text[n1 - 1] != '.')
  {
    text[n1] = '.';
    n1++;
  }
  text[n1] = '\0';
  thetexts.Append(TCollection_AsciiString(text));
}

bool StepData_EnumTool::IsSet() const
{
  return (thetexts.Length() > theinit);
}

int StepData_EnumTool::MaxValue() const
{
  return thetexts.Length() - 1;
}

void StepData_EnumTool::Optional(const bool mode)
{
  theopt = mode;
}

int StepData_EnumTool::NullValue() const
{
  return (theopt ? Value("$") : false);
}

const TCollection_AsciiString& StepData_EnumTool::Text(const int num) const
{
  if (num < 0 || num >= thetexts.Length())
    return TCollection_AsciiString::EmptyString();
  return thetexts.Value(num + 1);
}

int StepData_EnumTool::Value(const char* const txt) const
{
  int nb = thetexts.Length();
  for (int i = 1; i <= nb; i++)
  {
    if (thetexts.Value(i).IsEqual(txt))
      return i - 1;
  }
  return (-1);
}

int StepData_EnumTool::Value(const TCollection_AsciiString& txt) const
{
  int nb = thetexts.Length();
  for (int i = 1; i <= nb; i++)
  {
    if (thetexts.Value(i).IsEqual(txt))
      return i - 1;
  }
  return (-1);
}
