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

StepData_EnumTool::StepData_EnumTool(const char* e0,
                                     const char* e1,
                                     const char* e2,
                                     const char* e3,
                                     const char* e4,
                                     const char* e5,
                                     const char* e6,
                                     const char* e7,
                                     const char* e8,
                                     const char* e9,
                                     const char* e10,
                                     const char* e11,
                                     const char* e12,
                                     const char* e13,
                                     const char* e14,
                                     const char* e15,
                                     const char* e16,
                                     const char* e17,
                                     const char* e18,
                                     const char* e19,
                                     const char* e20,
                                     const char* e21,
                                     const char* e22,
                                     const char* e23,
                                     const char* e24,
                                     const char* e25,
                                     const char* e26,
                                     const char* e27,
                                     const char* e28,
                                     const char* e29,
                                     const char* e30,
                                     const char* e31,
                                     const char* e32,
                                     const char* e33,
                                     const char* e34,
                                     const char* e35,
                                     const char* e36,
                                     const char* e37,
                                     const char* e38,
                                     const char* e39)
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

void StepData_EnumTool::AddDefinition(const char* term)
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

int StepData_EnumTool::Value(const char* txt) const
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
