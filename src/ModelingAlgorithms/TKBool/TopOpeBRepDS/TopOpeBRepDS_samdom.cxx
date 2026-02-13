// Created on: 1996-03-07
// Created by: Jean Yves LEBEY
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

#include <TopOpeBRepDS_samdom.hxx>

#include <TopExp.hxx>
#include <TopoDS.hxx>
#include <TopOpeBRepDS_define.hxx>

static NCollection_IndexedDataMap<TopoDS_Shape,
                                  NCollection_List<TopoDS_Shape>,
                                  TopTools_ShapeMapHasher>* Gps1 = nullptr;
static NCollection_IndexedDataMap<TopoDS_Shape,
                                  NCollection_List<TopoDS_Shape>,
                                  TopTools_ShapeMapHasher>* Gps2 = nullptr;
// modified by NIZNHY-PKV Sun Dec 15 17:57:12 2002 f
// static occ::handle<TopOpeBRepDS_HDataStructure>      Ghds;
static occ::handle<TopOpeBRepDS_HDataStructure>* Ghds;

// modified by NIZNHY-PKV Sun Dec 15 17:57:18 2002 t

// modified by NIZNHY-PKV Sun Dec 15 17:41:43 2002 f
//=================================================================================================

void FDSSDM_Close()
{
  delete Gps1;
  Gps1 = nullptr;
  //
  delete Gps2;
  Gps2 = nullptr;
}

// modified by NIZNHY-PKV Sun Dec 15 17:56:02 2002 t
//=================================================================================================

Standard_EXPORT void FDSSDM_prepare(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  if (Gps1 == nullptr)
  {
    Gps1 = (NCollection_IndexedDataMap<
            TopoDS_Shape,
            NCollection_List<TopoDS_Shape>,
            TopTools_ShapeMapHasher>*)new NCollection_IndexedDataMap<TopoDS_Shape,
                                                                     NCollection_List<TopoDS_Shape>,
                                                                     TopTools_ShapeMapHasher>();
  }
  if (Gps2 == nullptr)
  {
    Gps2 = (NCollection_IndexedDataMap<
            TopoDS_Shape,
            NCollection_List<TopoDS_Shape>,
            TopTools_ShapeMapHasher>*)new NCollection_IndexedDataMap<TopoDS_Shape,
                                                                     NCollection_List<TopoDS_Shape>,
                                                                     TopTools_ShapeMapHasher>();
  }
  // modified by NIZNHY-PKV Sun Dec 15 17:58:28 2002 f
  // Ghds = HDS;
  void* anAddr = (void*)&HDS;
  Ghds         = (occ::handle<TopOpeBRepDS_HDataStructure>*)anAddr;
  // modified by NIZNHY-PKV Sun Dec 15 17:58:31 2002 t
  Gps1->Clear();
  Gps2->Clear();
  int i, n = HDS->NbShapes();
  for (i = 1; i <= n; i++)
  {
    const TopoDS_Shape& s   = HDS->Shape(i);
    bool                hsd = HDS->HasSameDomain(s);
    if (!hsd)
      continue;
    NCollection_List<TopoDS_Shape> thelist, thelist1;
    if (!Gps1->Contains(s))
      Gps1->Add(s, thelist);
    if (!Gps2->Contains(s))
      Gps2->Add(s, thelist1);
    NCollection_List<TopoDS_Shape>& LS1 = Gps1->ChangeFromKey(s);
    NCollection_List<TopoDS_Shape>& LS2 = Gps2->ChangeFromKey(s);
    FDSSDM_makes1s2(s, LS1, LS2);
  }
} // prepare
//=================================================================================================

Standard_EXPORT void FDSSDM_makes1s2(const TopoDS_Shape&             S,
                                     NCollection_List<TopoDS_Shape>& L1,
                                     NCollection_List<TopoDS_Shape>& L2)
// L1 = S1, complete lists L1,L2 with the shapes of the DS having same domain
{
  // modified by NIZNHY-PKV Sun Dec 15 17:59:11 2002 f
  // const occ::handle<TopOpeBRepDS_HDataStructure>& HDS = Ghds;
  const occ::handle<TopOpeBRepDS_HDataStructure>& HDS = *Ghds;
  // modified by NIZNHY-PKV Sun Dec 15 17:59:15 2002 t
  L1.Append(S);

  int i;
  int nl1 = L1.Extent(), nl2 = L2.Extent();

  while (nl1 > 0 || nl2 > 0)
  {

    NCollection_List<TopoDS_Shape>::Iterator it1(L1);
    for (i = 1; i <= nl1; i++)
    {
      const TopoDS_Shape& S1 = it1.Value();
      //                HDS->Shape(S1);
      NCollection_List<TopoDS_Shape>::Iterator itsd(HDS->SameDomain(S1));
      for (; itsd.More(); itsd.Next())
      {
        const TopoDS_Shape& S2 = itsd.Value();
        //                  HDS->Shape(S2);
        bool found = FDSSDM_contains(S2, L2);
        if (!found)
        {
          L2.Prepend(S2);
          nl2++;
        }
      }
      it1.Next();
    }
    nl1 = 0;

    NCollection_List<TopoDS_Shape>::Iterator it2(L2);
    for (i = 1; i <= nl2; i++)
    {
      const TopoDS_Shape& S2 = it2.Value();
      //      HDS->Shape(S2);
      NCollection_List<TopoDS_Shape>::Iterator itsd(HDS->SameDomain(S2));
      for (; itsd.More(); itsd.Next())
      {
        const TopoDS_Shape& S1 = itsd.Value();
        //                  HDS->Shape(S1);
        bool found = FDSSDM_contains(S1, L1);
        if (!found)
        {
          L1.Prepend(S1);
          nl1++;
        }
      }
      it2.Next();
    }

    nl2 = 0;
  }
} // makes1s2

//=================================================================================================

Standard_EXPORT void FDSSDM_s1s2makesordor(const NCollection_List<TopoDS_Shape>& LS1,
                                           const NCollection_List<TopoDS_Shape>& LS2,
                                           NCollection_List<TopoDS_Shape>&       LSO,
                                           NCollection_List<TopoDS_Shape>&       LDO)
{
  // modified by NIZNHY-PKV Sun Dec 15 17:59:37 2002 f
  // const occ::handle<TopOpeBRepDS_HDataStructure>& HDS = Ghds;
  const occ::handle<TopOpeBRepDS_HDataStructure>& HDS = *Ghds;
  // modified by NIZNHY-PKV Sun Dec 15 17:59:43 2002 t
  NCollection_List<TopoDS_Shape>::Iterator it(LS1);
  if (!it.More())
    return;
  const TopoDS_Shape& sref = it.Value();
  HDS->SameDomainReference(sref);
  TopOpeBRepDS_Config oref = HDS->SameDomainOrientation(sref);

  for (it.Initialize(LS1); it.More(); it.Next())
  {
    const TopoDS_Shape& s = it.Value();
    TopOpeBRepDS_Config o = HDS->SameDomainOrientation(s);
    //  HDS->Shape(s);
    if (o == oref && !FDSSDM_contains(s, LSO))
      LSO.Append(s);
    else if (o != oref && !FDSSDM_contains(s, LDO))
      LDO.Append(s);
  }

  for (it.Initialize(LS2); it.More(); it.Next())
  {
    const TopoDS_Shape& s = it.Value();
    TopOpeBRepDS_Config o = HDS->SameDomainOrientation(s);
    //             HDS->Shape(s);
    if (o == oref && !FDSSDM_contains(s, LSO))
      LSO.Append(s);
    else if (o != oref && !FDSSDM_contains(s, LDO))
      LDO.Append(s);
  }
} // s1s2makesordor

Standard_EXPORT bool FDSSDM_hass1s2(const TopoDS_Shape& S)
{
  bool b1 = Gps1->Contains(S);
  bool b2 = Gps2->Contains(S);
  bool b  = (b1 && b2);
  return b;
} // hass1s2

Standard_EXPORT void FDSSDM_s1s2(const TopoDS_Shape&             S,
                                 NCollection_List<TopoDS_Shape>& LS1,
                                 NCollection_List<TopoDS_Shape>& LS2)
{
  LS1.Clear();
  LS2.Clear();
  bool b = FDSSDM_hass1s2(S);
  if (!b)
  {
    FDSSDM_makes1s2(S, LS1, LS2);
    return;
  }
  const NCollection_List<TopoDS_Shape>& L1 = Gps1->FindFromKey(S);
  const NCollection_List<TopoDS_Shape>& L2 = Gps2->FindFromKey(S);
  FDSSDM_copylist(L1, LS1);
  FDSSDM_copylist(L2, LS2);
} // s1s2

Standard_EXPORT void FDSSDM_sordor(const TopoDS_Shape&             S,
                                   NCollection_List<TopoDS_Shape>& LSO,
                                   NCollection_List<TopoDS_Shape>& LDO)
{
  LSO.Clear();
  LDO.Clear();
  NCollection_List<TopoDS_Shape> LS1, LS2;
  FDSSDM_s1s2(S, LS1, LS2);
  FDSSDM_s1s2makesordor(LS1, LS2, LSO, LDO);
} // sordor

Standard_EXPORT bool FDSSDM_contains(const TopoDS_Shape& S, const NCollection_List<TopoDS_Shape>& L)
// True if S IsSame a shape of list L.
{
  for (NCollection_List<TopoDS_Shape>::Iterator it(L); it.More(); it.Next())
  {
    const TopoDS_Shape& SL     = it.Value();
    bool                issame = SL.IsSame(S);
    if (issame)
      return true;
  }
  return false;
} // contains

Standard_EXPORT void FDSSDM_copylist(const NCollection_List<TopoDS_Shape>& Lin,
                                     const int                             I1,
                                     const int                             I2,
                                     NCollection_List<TopoDS_Shape>&       Lou)
// copie des elements [i1..i2] de Lin dans Lou. 1er element de Lin = index 1
{
  NCollection_List<TopoDS_Shape>::Iterator it(Lin);
  for (int i = 1; it.More(); it.Next(), i++)
  {
    const TopoDS_Shape& EL = it.Value();
    if (i >= I1 && i <= I2)
    {
      Lou.Append(EL);
    }
  }
} // copylist

Standard_EXPORT void FDSSDM_copylist(const NCollection_List<TopoDS_Shape>& Lin,
                                     NCollection_List<TopoDS_Shape>&       Lou)
// copy de Lin dans Lou
{
  const int I1 = 1;
  const int I2 = Lin.Extent();
  FDSSDM_copylist(Lin, I1, I2, Lou);
} // copylist
