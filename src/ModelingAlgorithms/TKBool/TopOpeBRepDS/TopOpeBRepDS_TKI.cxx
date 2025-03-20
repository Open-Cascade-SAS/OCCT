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
#include <TopOpeBRepDS_DataMapIteratorOfDataMapOfIntegerListOfInterference.hxx>
#include <TopOpeBRepDS_EXPORT.hxx>
#include <TopOpeBRepDS_Interference.hxx>
#include <TopOpeBRepDS_TKI.hxx>

// extras define
#define MDSdmoiloi TopOpeBRepDS_DataMapOfIntegerListOfInterference
#define MDSdmiodmoiloi TopOpeBRepDS_DataMapIteratorOfDataMapOfIntegerListOfInterference
#define MDShaodmoiloi TopOpeBRepDS_HArray1OfDataMapOfIntegerListOfInterference

//=================================================================================================

TopOpeBRepDS_TKI::TopOpeBRepDS_TKI()
{
  Reset();
}

//=================================================================================================

void TopOpeBRepDS_TKI::Reset()
{
  Standard_Integer ip = (Standard_Integer)TopOpeBRepDS_POINT;
  Standard_Integer is = (Standard_Integer)TopOpeBRepDS_SOLID;
  if (ip > is)
  {
    throw Standard_ProgramError("TopOpeBRepDS_TKI : enumeration badly ordered");
    return;
  }
  Standard_Integer f = 1;             // first index of table
  Standard_Integer l = f + (is - ip); // last index of table
  mydelta            = f - ip;
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
  Standard_Integer i = myT->Lower(), n = myT->Upper();
  for (; i <= n; i++)
    myT->ChangeValue(i).Clear();
}

//=================================================================================================

void TopOpeBRepDS_TKI::FillOnGeometry(const TopOpeBRepDS_ListOfInterference& L)
{
  for (TopOpeBRepDS_ListIteratorOfListOfInterference it(L); it.More(); it.Next())
  {
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    TopOpeBRepDS_Kind                        GT, ST;
    Standard_Integer                         G, S;
    FDS_data(I, GT, G, ST, S);
    Add(GT, G, I);
  }
}

//=================================================================================================

void TopOpeBRepDS_TKI::FillOnSupport(const TopOpeBRepDS_ListOfInterference& L)
{
  for (TopOpeBRepDS_ListIteratorOfListOfInterference it(L); it.More(); it.Next())
  {
    const Handle(TopOpeBRepDS_Interference)& I = it.Value();
    TopOpeBRepDS_Kind                        GT, ST;
    Standard_Integer                         G, S;
    FDS_data(I, GT, G, ST, S);
    Add(ST, S, I);
  }
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_TKI::IsBound(const TopOpeBRepDS_Kind K,
                                           const Standard_Integer  G) const
{
  if (!IsValidKG(K, G))
    return Standard_False;
  Standard_Integer TI = KindToTableIndex(K);
  Standard_Boolean in = myT->Value(TI).IsBound(G);
  return in;
}

//=================================================================================================

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_TKI::Interferences(
  const TopOpeBRepDS_Kind K,
  const Standard_Integer  G) const
{
  Standard_Boolean in = IsBound(K, G);
  Standard_Integer TI = KindToTableIndex(K);
  if (in)
    return myT->Value(TI).Find(G);
  return myEmptyLOI;
}

//=================================================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_TKI::ChangeInterferences(const TopOpeBRepDS_Kind K,
                                                                       const Standard_Integer  G)
{
  Standard_Boolean in = IsBound(K, G);
  Standard_Integer TI = KindToTableIndex(K);
  if (in)
    return myT->ChangeValue(TI).ChangeFind(G);
  return myEmptyLOI;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_TKI::HasInterferences(const TopOpeBRepDS_Kind K,
                                                    const Standard_Integer  G) const
{
  Standard_Boolean has = IsBound(K, G);
  if (has)
  {
    const TopOpeBRepDS_ListOfInterference& loi = Interferences(K, G);
    Standard_Integer                       l   = loi.Extent();
    has                                        = (l != 0);
  }
  return has;
}

//=================================================================================================

void TopOpeBRepDS_TKI::Add(const TopOpeBRepDS_Kind K, const Standard_Integer G)
{
  Standard_Boolean ok = IsValidKG(K, G);
  if (!ok)
  {
    throw Standard_ProgramError("TopOpeBRepDS_TKI : Add K G");
    return;
  }

  Standard_Boolean                in = IsBound(K, G);
  Standard_Integer                TI = KindToTableIndex(K);
  TopOpeBRepDS_ListOfInterference thelist;
  if (!in)
    myT->ChangeValue(TI).Bind(G, thelist);
}

//=================================================================================================

void TopOpeBRepDS_TKI::Add(const TopOpeBRepDS_Kind                  K,
                           const Standard_Integer                   G,
                           const Handle(TopOpeBRepDS_Interference)& HI)
{
  Standard_Boolean ok = IsValidKG(K, G);
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
    Standard_Integer  G;
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

Standard_Boolean TopOpeBRepDS_TKI::More() const
{
  Standard_Boolean b = IsValidKG(myK, myG);
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

const TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_TKI::Value(TopOpeBRepDS_Kind& K,
                                                               Standard_Integer&  G) const
{
  if (!More())
    return myEmptyLOI;
  K = myK;
  G = myG;
  return Interferences(K, G);
}

//=================================================================================================

TopOpeBRepDS_ListOfInterference& TopOpeBRepDS_TKI::ChangeValue(TopOpeBRepDS_Kind& K,
                                                               Standard_Integer&  G)
{
  if (!More())
    return myEmptyLOI;
  K = myK;
  G = myG;
  return ChangeInterferences(K, G);
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_TKI::MoreTI() const
{
  Standard_Boolean b = IsValidTI(myTI);
  return b;
}

//=================================================================================================

void TopOpeBRepDS_TKI::NextTI()
{
  myTI = myTI + 1;
  myK  = TableIndexToKind(myTI);
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_TKI::MoreITM() const
{
  Standard_Boolean b = myITM.More();
  return b;
}

//=================================================================================================

void TopOpeBRepDS_TKI::FindITM()
{
  Standard_Boolean f = Standard_False;
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
  Standard_Boolean f = Standard_False;
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

Standard_Integer TopOpeBRepDS_TKI::KindToTableIndex(const TopOpeBRepDS_Kind K) const
{
  // K(Kind) + mydelta = TI(integer) = index in myT
  Standard_Integer TI = (Standard_Integer)K + mydelta;
  return TI;
}

//=================================================================================================

TopOpeBRepDS_Kind TopOpeBRepDS_TKI::TableIndexToKind(const Standard_Integer TI) const
{
  // K(Kind) + mydelta = TI(integer) = index in myT
  TopOpeBRepDS_Kind K = (TopOpeBRepDS_Kind)(TI - mydelta);
  return K;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_TKI::IsValidTI(const Standard_Integer TI) const
{
  if (myT.IsNull())
    return Standard_False;
  Standard_Boolean nok = (TI < myT->Lower() || TI > myT->Upper());
  return !nok;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_TKI::IsValidK(const TopOpeBRepDS_Kind K) const
{
  Standard_Boolean nok = (K < TopOpeBRepDS_POINT || K > TopOpeBRepDS_SOLID);
  return !nok;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_TKI::IsValidG(const Standard_Integer G) const
{
  Standard_Boolean nok = (G <= 0);
  return !nok;
}

//=================================================================================================

Standard_Boolean TopOpeBRepDS_TKI::IsValidKG(const TopOpeBRepDS_Kind K,
                                             const Standard_Integer  G) const
{
  Standard_Boolean nok = (!IsValidK(K) || !IsValidG(G));
  return !nok;
}
