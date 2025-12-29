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

#include <IFSelect_Signature.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <cstdio>
IMPLEMENT_STANDARD_RTTIEXT(IFSelect_Signature, Interface_SignType)

// unused
// static const char* nulsign = "";
static char intval[20];

IFSelect_Signature::IFSelect_Signature(const char* name)
    : thename(name)
{
  thecasi[0] = thecasi[1] = thecasi[2] = 0;
}

void IFSelect_Signature::SetIntCase(const bool hasmin,
                                    const int  valmin,
                                    const bool hasmax,
                                    const int  valmax)
{
  thecasi[0] = 1;
  if (hasmin)
  {
    thecasi[0] += 2;
    thecasi[1] = valmin;
  }
  if (hasmax)
  {
    thecasi[0] += 4;
    thecasi[2] = valmax;
  }
}

bool IFSelect_Signature::IsIntCase(bool& hasmin, int& valmin, bool& hasmax, int& valmax) const
{
  hasmin = hasmax = false;
  valmin = valmax = 0;
  if (!thecasi[0])
    return false;
  if (thecasi[0] & 2)
  {
    hasmin = true;
    valmin = thecasi[1];
  }
  if (thecasi[0] & 4)
  {
    hasmax = true;
    valmax = thecasi[2];
  }
  return true;
}

void IFSelect_Signature::AddCase(const char* acase)
{
  if (thecasl.IsNull())
    thecasl = new NCollection_HSequence<TCollection_AsciiString>();
  TCollection_AsciiString scase(acase);
  thecasl->Append(scase);
}

occ::handle<NCollection_HSequence<TCollection_AsciiString>> IFSelect_Signature::CaseList() const
{
  return thecasl;
}

const char* IFSelect_Signature::Name() const
{
  return thename.ToCString();
}

TCollection_AsciiString IFSelect_Signature::Label() const
{
  TCollection_AsciiString label("Signature : ");
  label.AssignCat(thename);
  return label;
}

bool IFSelect_Signature::Matches(const occ::handle<Standard_Transient>&       ent,
                                 const occ::handle<Interface_InterfaceModel>& model,
                                 const TCollection_AsciiString&               text,
                                 const bool                                   exact) const

{
  return IFSelect_Signature::MatchValue(Value(ent, model), text, exact);
}

bool IFSelect_Signature::MatchValue(const char*                    val,
                                    const TCollection_AsciiString& text,
                                    const bool                     exact)
{
  if (exact)
    return text.IsEqual(val);
  // NB: no regexp
  char cardeb = text.Value(1);
  int  ln, lnt, i, j;
  ln  = text.Length();
  lnt = (int)(strlen(val) - ln);
  for (i = 0; i <= lnt; i++)
  {
    if (val[i] == cardeb)
    {
      //    a candidate
      bool res = true;
      for (j = 1; j < ln; j++)
      {
        if (val[i + j] != text.Value(j + 1))
        {
          res = false;
          break;
        }
      }
      if (res)
        return res;
    }
  }
  return false;
}

const char* IFSelect_Signature::IntValue(const int val)
{
  switch (val)
  {
    case 0:
      return "0";
    case 1:
      return "1";
    case 2:
      return "2";
    case 3:
      return "3";
    case 4:
      return "4";
    case 5:
      return "5";
    case 6:
      return "6";
    case 7:
      return "7";
    case 8:
      return "8";
    case 9:
      return "9";
    default:
      break;
  }
  Sprintf(intval, "%d", val);
  return intval;
}
