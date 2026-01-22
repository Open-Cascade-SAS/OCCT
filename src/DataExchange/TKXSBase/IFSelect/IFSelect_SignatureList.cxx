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

#include <IFSelect_SignatureList.hxx>
#include <Interface_InterfaceModel.hxx>
#include <MoniTool_Macros.hxx>
#include <Interface_MSG.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_HAsciiString.hxx>
#include <NCollection_Sequence.hxx>
#include <NCollection_HSequence.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_SignatureList, Standard_Transient)

IFSelect_SignatureList::IFSelect_SignatureList(const bool withlist)
{
  thesignonly = false;
  thelistat   = withlist;
  thenbnuls   = 0;
  SetName("...");
}

void IFSelect_SignatureList::SetList(const bool withlist)
{
  thelistat = withlist;
}

bool& IFSelect_SignatureList::ModeSignOnly()
{
  return thesignonly;
}

void IFSelect_SignatureList::Clear()
{
  thelastval.Clear();
  thenbnuls = 0;
  thedicount.Clear();
  thediclist.Clear();
}

void IFSelect_SignatureList::Add(const occ::handle<Standard_Transient>& ent, const char* const sign)
{
  if (thesignonly)
  {
    thelastval.Clear();
    thelastval.AssignCat(sign);
    return;
  }

  if (sign[0] == '\0')
  {
    thenbnuls++;
    return;
  }

  (*thedicount.Bound(sign, 0))++;

  if (thelistat)
  {
    occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> alist;
    if (thediclist.Contains(sign))
      alist = occ::down_cast<NCollection_HSequence<occ::handle<Standard_Transient>>>(
        thediclist.FindFromKey(sign));
    else
    {
      alist = new NCollection_HSequence<occ::handle<Standard_Transient>>();
      thediclist.Add(sign, alist);
    }
    alist->Append(ent);
  }
}

const char* IFSelect_SignatureList::LastValue() const
{
  return thelastval.ToCString();
}

void IFSelect_SignatureList::Init(
  const char* const                                               name,
  const NCollection_IndexedDataMap<TCollection_AsciiString, int>& theCount,
  const NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>& list,
  const int nbnuls)
{
  thelastval.Clear();
  thename    = new TCollection_HAsciiString(name);
  thedicount = theCount;
  thediclist = list;
  thenbnuls  = nbnuls;
  if (thediclist.IsEmpty())
    thelistat = false;
}

occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> IFSelect_SignatureList::
  List(const char* const root) const
{
  occ::handle<NCollection_HSequence<occ::handle<TCollection_HAsciiString>>> list =
    new NCollection_HSequence<occ::handle<TCollection_HAsciiString>>();
  NCollection_IndexedDataMap<TCollection_AsciiString, int>::Iterator iter(thedicount);
  for (; iter.More(); iter.Next())
  {
    if (!iter.Key().StartsWith(root))
      continue;

    occ::handle<TCollection_HAsciiString> sign = new TCollection_HAsciiString(iter.Key());
    list->Append(sign);
  }
  return list;
}

bool IFSelect_SignatureList::HasEntities() const
{
  return thelistat;
}

int IFSelect_SignatureList::NbNulls() const
{
  return thenbnuls;
}

int IFSelect_SignatureList::NbTimes(const char* const sign) const
{
  int nb = 0;
  thedicount.FindFromKey(sign, nb);
  return nb;
}

occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> IFSelect_SignatureList::
  Entities(const char* const sign) const
{
  occ::handle<NCollection_HSequence<occ::handle<Standard_Transient>>> list;
  occ::handle<Standard_Transient>                                     aTList;
  if (!thelistat)
    return list;
  if (thediclist.FindFromKey(sign, aTList))
    list = occ::down_cast<NCollection_HSequence<occ::handle<Standard_Transient>>>(aTList);
  else
    list = new NCollection_HSequence<occ::handle<Standard_Transient>>();
  return list;
}

void IFSelect_SignatureList::SetName(const char* const name)
{
  thename = new TCollection_HAsciiString(name);
}

const char* IFSelect_SignatureList::Name() const
{
  return thename->ToCString();
}

void IFSelect_SignatureList::PrintCount(Standard_OStream& S) const
{
  int                                                                nbtot = 0, nbsign = 0;
  NCollection_IndexedDataMap<TCollection_AsciiString, int>::Iterator iter(thedicount);
  S << " Count	" << thename->ToCString() << "\n -----	-----------" << std::endl;
  for (; iter.More(); iter.Next())
  {
    int val = iter.Value();
    S << Interface_MSG::Blanks(val, 6) << val << "	" << iter.Key() << std::endl;
    nbtot += val;
    nbsign++;
  }
  if (thenbnuls > 0)
    S << thename->ToCString() << " Nul : " << thenbnuls << std::endl;
  S << "    Nb Total:" << nbtot << "  for " << nbsign << " items" << std::endl;
}

void IFSelect_SignatureList::PrintList(Standard_OStream&                            S,
                                       const occ::handle<Interface_InterfaceModel>& model,
                                       const IFSelect_PrintCount                    mod) const
{
  if (mod == IFSelect_ItemsByEntity)
    return;
  if (mod == IFSelect_CountByItem)
  {
    PrintCount(S);
    return;
  }
  if (mod == IFSelect_CountSummary)
  {
    PrintSum(S);
    return;
  }
  if (!HasEntities())
  {
    S << " SignatureList " << Name() << " : PrintList, list not available" << std::endl;
    PrintCount(S);
    return;
  }
  int nbtot = 0, nbsign = 0;
  NCollection_IndexedDataMap<TCollection_AsciiString, occ::handle<Standard_Transient>>::Iterator
    iter(thediclist);
  for (; iter.More(); iter.Next())
  {
    DeclareAndCast(NCollection_HSequence<occ::handle<Standard_Transient>>, list, iter.Value());
    S << Name() << " : " << iter.Key() << std::endl;
    if (list.IsNull())
    {
      S << "  - (empty list)" << std::endl;
      continue;
    }
    int nb = list->Length();
    S << "  - Nb: " << nb << " : ";
    int nc = nb;
    if (nb > 5 && mod == IFSelect_ShortByItem)
      nc = 5;
    for (int i = 1; i <= nc; i++)
    {
      if (list->Value(i).IsNull())
      {
        S << "  0";
        if (mod == IFSelect_EntitiesByItem)
          S << ":(Global)";
        continue;
      }
      int num = model->Number(list->Value(i));
      if (num == IFSelect_ShortByItem)
      {
        S << "  ??";
        continue;
      }
      S << "  " << num;
      if (mod == IFSelect_EntitiesByItem)
      {
        S << ":";
        model->PrintLabel(list->Value(i), S);
      }
    }
    if (nc < nb)
      S << "  .. etc";
    S << std::endl;
    nbtot += nb;
    nbsign++;
  }
  S << " Nb Total:" << nbtot << "  for " << nbsign << " items" << std::endl;
}

void IFSelect_SignatureList::PrintSum(Standard_OStream& S) const
{
  NCollection_IndexedDataMap<TCollection_AsciiString, int>::Iterator iter(thedicount);
  S << " Summary " << thename->ToCString() << "\n -----	-----------" << std::endl;
  int nbtot = 0, nbsign = 0, maxent = 0, nbval = 0, nbve = 0, minval = 0, maxval = 0, totval = 0;
  for (; iter.More(); iter.Next())
  {
    int nbent = iter.Value();
    nbtot += nbent;
    nbsign++;
    if (nbent > maxent)
      maxent = nbent;
    const TCollection_AsciiString& name = iter.Key();
    //    if (!name.IsIntegerValue()) continue;  not very reliable
    int  ic, nc = name.Length();
    bool iaint = true;
    for (ic = 1; ic <= nc; ic++)
    {
      char unc = name.Value(ic);
      if (ic == 1 && (unc == ' ' || unc == '+' || unc == '-'))
        continue;
      if (unc >= '0' && unc <= '9')
        continue;
      iaint = false;
      break;
    }
    if (!iaint)
      continue;
    int val = name.IntegerValue();
    if (nbval == 0)
    {
      minval = maxval = val;
    }
    if (minval > val)
      minval = val;
    if (maxval < val)
      maxval = val;
    nbval++;
    nbve += nbent;
    totval += (val * nbent);
  }
  S << "    Nb Total:" << nbtot << "  for " << nbsign << " items" << std::endl;
  S << "    Highest count of entities : " << maxent << " on one item" << std::endl;
  if (nbval > 0)
  {
    S << "    Summary on Integer Values" << std::endl;
    S << "    Nb Integer Items : " << nbval << std::endl;
    S << "    For Nb Entities  : " << nbve << std::endl;
    S << "    Cumulated Values : " << totval << std::endl;
    S << "    Maximum Value    : " << maxval << std::endl;
    int avg1, avg2;
    avg1 = totval / nbve;
    avg2 = ((totval - (avg1 * nbve)) * 10) / nbve;
    S << "    Average Value    : " << avg1 << " " << avg2 << "/10" << std::endl;
    S << "    Minimum Value    : " << minval << std::endl;
  }
}
