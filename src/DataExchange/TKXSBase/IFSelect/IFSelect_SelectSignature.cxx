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

#include <IFSelect_SelectSignature.hxx>
#include <IFSelect_Signature.hxx>
#include <IFSelect_SignCounter.hxx>
#include <Interface_Graph.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SelectSignature, IFSelect_SelectExtract)

//  theexact : -1  YES   0  NO single value  > 0 NO number of values
//  signmode: 1 take if contained, 2 refuse if contained
//            3 take if equal,     4 refuse if equal
//  or numeric test, add : 16 <  24 <=  32 >  40 >=
static int multsign(const TCollection_AsciiString&                 signtext,
                    NCollection_Sequence<TCollection_AsciiString>& signlist,
                    NCollection_Sequence<int>&                     signmode)
{
  int i, nb = signtext.Length(), mode = 0;
  for (i = 1; i <= nb; i++)
  {
    char unsign = signtext.Value(i);
    if (unsign == '|' || unsign == '!' || unsign == '=')
    {
      mode = 1;
      break;
    }
    if (unsign == '<' || unsign == '>')
    {
      mode = 1;
      break;
    }
  }
  if (mode == 0)
    return mode;
  //  On va tronconner
  TCollection_AsciiString item;
  int                     imod = 1;
  for (i = 1; i <= nb; i++)
  {
    char unsign = signtext.Value(i);
    if (unsign == '|' || unsign == '!')
    {
      if (item.Length() > 0)
      {
        signlist.Append(item);
        signmode.Append(imod);
        item.Clear();
        mode++;
      }
      imod = (unsign == '|' ? 1 : 2);
    }
    else if (unsign == '<')
      imod += 16;
    else if (unsign == '>')
      imod += 32;
    else if (unsign == '=')
    {
      if (imod < 8)
        imod += 2;
      else
        imod += 8;
    }
    else
      item.AssignCat(unsign);
  }
  if (item.Length() > 0)
  {
    signlist.Append(item);
    signmode.Append(imod);
    //    mode ++;  was one at start
  }
  return mode;
}

IFSelect_SelectSignature::IFSelect_SelectSignature(const occ::handle<IFSelect_Signature>& matcher,
                                                   const TCollection_AsciiString&         signtext,
                                                   const bool                             exact)
    : thematcher(matcher),
      thesigntext(signtext),
      theexact(exact ? -1 : 0)
{
  if (!exact)
    theexact = multsign(thesigntext, thesignlist, thesignmode);
}

IFSelect_SelectSignature::IFSelect_SelectSignature(const occ::handle<IFSelect_Signature>& matcher,
                                                   const char* const                      signtext,
                                                   const bool                             exact)
    : thematcher(matcher),
      thesigntext(signtext),
      theexact(exact ? -1 : 0)
{
  if (!exact)
    theexact = multsign(thesigntext, thesignlist, thesignmode);
}

IFSelect_SelectSignature::IFSelect_SelectSignature(const occ::handle<IFSelect_SignCounter>& counter,
                                                   const char* const signtext,
                                                   const bool        exact)
    : thecounter(counter),
      thesigntext(signtext),
      theexact(exact ? -1 : 0)
{
  if (!exact)
    theexact = multsign(thesigntext, thesignlist, thesignmode);
}

occ::handle<IFSelect_Signature> IFSelect_SelectSignature::Signature() const
{
  return thematcher;
}

occ::handle<IFSelect_SignCounter> IFSelect_SelectSignature::Counter() const
{
  return thecounter;
}

bool IFSelect_SelectSignature::SortInGraph(const int,
                                           const occ::handle<Standard_Transient>& ent,
                                           const Interface_Graph&                 G) const
{
  bool                                         res;
  const char*                                  txt;
  const occ::handle<Interface_InterfaceModel>& model = G.Model();
  if (theexact <= 0)
  {
    if (!thematcher.IsNull())
      return thematcher->Matches(ent, model, thesigntext, (theexact < 0));
    txt = thecounter->ComputedSign(ent, G);
    return IFSelect_Signature::MatchValue(txt, thesigntext, (theexact < 0));
  }

  //  otherwise: list
  //  Sequence analysis: if take/don't-take alternation, the last one is right
  //   consequently, if only takes or only don't-takes, it's commutative
  //   THEREFORE recommendation: put the don't-takes at the end

  //  AT START: take = take only. don't-take = take-all-except ...
  //  So if the first is a don't-take, I start by taking everything
  int hmod = thesignmode.Value(1);
  int jmod = hmod / 8;
  int imod = hmod - (jmod * 8);
  res      = (imod == 2 || imod == 4);
  for (int i = 1; i <= theexact; i++)
  {
    const char* signtext = thesignlist.Value(i).ToCString();
    hmod                 = thesignmode.Value(i);
    jmod                 = hmod / 8;
    imod                 = hmod - (jmod * 8);
    bool quid;
    if (jmod == 0)
    {
      if (!thematcher.IsNull())
        quid = thematcher->Matches(ent, model, signtext, (imod > 2));
      else
        quid =
          IFSelect_Signature::MatchValue(thecounter->ComputedSign(ent, G), signtext, (imod > 2));
    }
    else
    {
      if (!thematcher.IsNull())
        txt = thematcher->Value(ent, model);
      else
        txt = thecounter->ComputedSign(ent, G);

      int val = atoi(txt);
      int lav = atoi(signtext);
      switch (jmod)
      {
        case 2:
          quid = (val < lav);
          break;
        case 3:
          quid = (val <= lav);
          break;
        case 4:
          quid = (val > lav);
          break;
        case 5:
          quid = (val >= lav);
          break;
        default:
          quid = false;
          break;
      }
    }
    if ((imod == 1 || imod == 3) && quid)
      res = true;
    if ((imod == 2 || imod == 4) && quid)
      res = false;
  }
  return res;
}

bool IFSelect_SelectSignature::Sort(const int,
                                    const occ::handle<Standard_Transient>& /*ent*/,
                                    const occ::handle<Interface_InterfaceModel>& /*model*/) const
{
  return true;
}

const TCollection_AsciiString& IFSelect_SelectSignature::SignatureText() const
{
  return thesigntext;
}

bool IFSelect_SelectSignature::IsExact() const
{
  return (theexact < 0);
}

TCollection_AsciiString IFSelect_SelectSignature::ExtractLabel() const
{
  TCollection_AsciiString lab;
  if (!thematcher.IsNull())
    lab.AssignCat(thematcher->Name());
  else
    lab.AssignCat(thecounter->Name());
  if (theexact < 0)
    lab.AssignCat(" matching ");
  else if (theexact == 0)
    lab.AssignCat(" containing ");
  else
    lab.AssignCat(" matching list ");
  lab.AssignCat(thesigntext);
  return lab;
}
