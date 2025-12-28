// Created on: 1997-09-17
// Created by: Jean Yves LEBEY
// Copyright (c) 1997-1999 Matra Datavision
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

#include <Standard_ProgramError.hxx>
#include <TopOpeBRepDS.hxx>
#include <Standard_Integer.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <NCollection_List.hxx>
#include <NCollection_DataMap.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_TKI.hxx>

// extras define
#define MDSdmoiloi                                                                                 \
  NCollection_DataMap<int, NCollection_List<occ::handle<TopOpeBRepDS_Interference>>>
#define MDSdmiodmoiloi                                                                             \
  NCollection_DataMap<int, NCollection_List<occ::handle<TopOpeBRepDS_Interference>>>::Iterator
#define MDShaodmoiloi                                                                              \
  NCollection_HArray1<                                                                             \
    NCollection_DataMap<int, NCollection_List<occ::handle<TopOpeBRepDS_Interference>>>>

//=================================================================================================

TopOpeBRepDS_TKI::TopOpeBRepDS_TKI()
{
  Reset();
}

//=================================================================================================

void TopOpeBRepDS_TKI::Reset()
{
  int ip = (int)TopOpeBRepDS_POINT;
  int is = (int)TopOpeBRepDS_SOLID;
  if (ip > is)
  {
    throw Standard_ProgramError("TopOpeBRepDS_TKI : enumeration badly ordered");
    return;
  }
  int f   = 1;             // first index of table
  int l   = f + (is - ip); // last index of table
  mydelta = f - ip;
  // k + mydelta = i in [f,l]; TopOpeBRepDS_POINT,SOLID + mydelta = f,l
  if (myT.IsNull())
    myT = new MDShaodmoiloi(f, l);
  Clear();
  myK = TopOpeBRepDS_UNKNOWN;
  myG = 0;
}

//=================================================================================================

void TopOpeBRepDS_TKI::Clear()
{
  int i = myT->Lower(), n = myT->Upper();
  for (; i <= n; i++)
    myT->ChangeValue(i).Clear();
}

//=================================================================================================

void TopOpeBRepDS_TKI::FillOnGeometry(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L)
{
  for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(L); it.More();
       it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
    TopOpeBRepDS_Kind                             GT, ST;
    int                                           G, S;
    FDS_data(I, GT, G, ST, S);
    Add(GT, G, I);
  }
}

//=================================================================================================

void TopOpeBRepDS_TKI::FillOnSupport(
  const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& L)
{
  for (NCollection_List<occ::handle<TopOpeBRepDS_Interference>>::Iterator it(L); it.More();
       it.Next())
  {
    const occ::handle<TopOpeBRepDS_Interference>& I = it.Value();
    TopOpeBRepDS_Kind                             GT, ST;
    int                                           G, S;
    FDS_data(I, GT, G, ST, S);
    Add(ST, S, I);
  }
}

//=================================================================================================

bool TopOpeBRepDS_TKI::IsBound(const TopOpeBRepDS_Kind K, const int G) const
{
  if (!IsValidKG(K, G))
    return false;
  int  TI = KindToTableIndex(K);
  bool in = myT->Value(TI).IsBound(G);
  return in;
}

//=================================================================================================

const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& TopOpeBRepDS_TKI::Interferences(
  const TopOpeBRepDS_Kind K,
  const int               G) const
{
  bool in = IsBound(K, G);
  int  TI = KindToTableIndex(K);
  if (in)
    return myT->Value(TI).Find(G);
  return myEmptyLOI;
}

//=================================================================================================

NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& TopOpeBRepDS_TKI::ChangeInterferences(
  const TopOpeBRepDS_Kind K,
  const int               G)
{
  bool in = IsBound(K, G);
  int  TI = KindToTableIndex(K);
  if (in)
    return myT->ChangeValue(TI).ChangeFind(G);
  return myEmptyLOI;
}

//=================================================================================================

bool TopOpeBRepDS_TKI::HasInterferences(const TopOpeBRepDS_Kind K, const int G) const
{
  bool has = IsBound(K, G);
  if (has)
  {
    const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& loi = Interferences(K, G);
    int                                                             l   = loi.Extent();
    has                                                                 = (l != 0);
  }
  return has;
}

//=================================================================================================

void TopOpeBRepDS_TKI::Add(const TopOpeBRepDS_Kind K, const int G)
{
  bool ok = IsValidKG(K, G);
  if (!ok)
  {
    throw Standard_ProgramError("TopOpeBRepDS_TKI : Add K G");
    return;
  }

  bool                                                     in = IsBound(K, G);
  int                                                      TI = KindToTableIndex(K);
  NCollection_List<occ::handle<TopOpeBRepDS_Interference>> thelist;
  if (!in)
    myT->ChangeValue(TI).Bind(G, thelist);
}

//=================================================================================================

void TopOpeBRepDS_TKI::Add(const TopOpeBRepDS_Kind                       K,
                           const int                                     G,
                           const occ::handle<TopOpeBRepDS_Interference>& HI)
{
  bool ok = IsValidKG(K, G);
  if (!ok)
    throw Standard_ProgramError("TopOpeBRepDS_TKI : Add K G HI");

  Add(K, G);
  ChangeInterferences(K, G).Append(HI);
}

//=================================================================================================

void TopOpeBRepDS_TKI::DumpTKIIterator(const TCollection_AsciiString& s1,
                                       const TCollection_AsciiString& s2)
{
  std::cout << s1;
  Init();
  while (More())
  {
    TopOpeBRepDS_Kind K;
    int               G;
    Value(K, G);
    Next();
  }
  std::cout << s2;
  std::cout.flush();
}

//=================================================================================================

void TopOpeBRepDS_TKI::Init()
{
  myK = TopOpeBRepDS_UNKNOWN;
  myG = 0;
  if (myT.IsNull())
    return;
  myTI = myT->Lower();
  myK  = TableIndexToKind(myTI);
  myITM.Initialize(myT->Value(myTI));
  Find();
}

//=================================================================================================

bool TopOpeBRepDS_TKI::More() const
{
  bool b = IsValidKG(myK, myG);
  return b;
}

//=================================================================================================

void TopOpeBRepDS_TKI::Next()
{
  if (MoreITM())
  {
    NextITM();
    Find();
  }
  else if (MoreTI())
  {
    NextTI();
    if (MoreTI())
    {
      myITM.Initialize(myT->Value(myTI));
    }
    Find();
  }
}

//=================================================================================================

const NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& TopOpeBRepDS_TKI::Value(
  TopOpeBRepDS_Kind& K,
  int&               G) const
{
  if (!More())
    return myEmptyLOI;
  K = myK;
  G = myG;
  return Interferences(K, G);
}

//=================================================================================================

NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& TopOpeBRepDS_TKI::ChangeValue(
  TopOpeBRepDS_Kind& K,
  int&               G)
{
  if (!More())
    return myEmptyLOI;
  K = myK;
  G = myG;
  return ChangeInterferences(K, G);
}

//=================================================================================================

bool TopOpeBRepDS_TKI::MoreTI() const
{
  bool b = IsValidTI(myTI);
  return b;
}

//=================================================================================================

void TopOpeBRepDS_TKI::NextTI()
{
  myTI = myTI + 1;
  myK  = TableIndexToKind(myTI);
}

//=================================================================================================

bool TopOpeBRepDS_TKI::MoreITM() const
{
  bool b = myITM.More();
  return b;
}

//=================================================================================================

void TopOpeBRepDS_TKI::FindITM()
{
  bool f = false;
  while (MoreITM())
  {
    myG = myITM.Key();
    f   = HasInterferences(myK, myG);
    if (f)
      break;
    else
      myITM.Next();
  }
}

//=================================================================================================

void TopOpeBRepDS_TKI::NextITM()
{
  if (!MoreITM())
    return;
  myITM.Next();
  FindITM();
}

//=================================================================================================

void TopOpeBRepDS_TKI::Find()
{
  bool f = false;
  while (MoreTI())
  {
    while (MoreITM())
    {
      FindITM();
      f = HasInterferences(myK, myG);
      if (f)
        break;
    }
    if (f)
      break;
    else
    {
      NextTI();
      if (MoreTI())
      {
        myITM.Initialize(myT->Value(myTI));
      }
    }
  }
}

//=================================================================================================

int TopOpeBRepDS_TKI::KindToTableIndex(const TopOpeBRepDS_Kind K) const
{
  // K(Kind) + mydelta = TI(integer) = index in myT
  int TI = (int)K + mydelta;
  return TI;
}

//=================================================================================================

TopOpeBRepDS_Kind TopOpeBRepDS_TKI::TableIndexToKind(const int TI) const
{
  // K(Kind) + mydelta = TI(integer) = index in myT
  TopOpeBRepDS_Kind K = (TopOpeBRepDS_Kind)(TI - mydelta);
  return K;
}

//=================================================================================================

bool TopOpeBRepDS_TKI::IsValidTI(const int TI) const
{
  if (myT.IsNull())
    return false;
  bool nok = (TI < myT->Lower() || TI > myT->Upper());
  return !nok;
}

//=================================================================================================

bool TopOpeBRepDS_TKI::IsValidK(const TopOpeBRepDS_Kind K) const
{
  bool nok = (K < TopOpeBRepDS_POINT || K > TopOpeBRepDS_SOLID);
  return !nok;
}

//=================================================================================================

bool TopOpeBRepDS_TKI::IsValidG(const int G) const
{
  bool nok = (G <= 0);
  return !nok;
}

//=================================================================================================

bool TopOpeBRepDS_TKI::IsValidKG(const TopOpeBRepDS_Kind K, const int G) const
{
  bool nok = (!IsValidK(K) || !IsValidG(G));
  return !nok;
}
