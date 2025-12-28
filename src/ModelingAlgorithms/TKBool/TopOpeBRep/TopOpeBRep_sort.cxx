// Created on: 1998-06-04
// Created by: Jean Yves LEBEY
// Copyright (c) 1998-1999 Matra Datavision
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

#include <TopOpeBRepDS_TKI.hxx>
#include <cstdlib>
#include <TopOpeBRep_define.hxx>
#include <TopOpeBRepDS_CurvePointInterference.hxx>

typedef NCollection_List<occ::handle<TopOpeBRepDS_Interference>>* BOA_t;

static int compll(const void* v1, const void* v2)
{
  BOA_t l1 = *(BOA_t*)v1;
  BOA_t l2 = *(BOA_t*)v2;
  if (l1->Extent() == 0)
    return (0);
  else if (l2->Extent() == 0)
    return (0);

  occ::handle<TopOpeBRepDS_CurvePointInterference> i1 =
    occ::down_cast<TopOpeBRepDS_CurvePointInterference>(l1->First());
  if (i1.IsNull())
    return 0;
  occ::handle<TopOpeBRepDS_CurvePointInterference> i2 =
    occ::down_cast<TopOpeBRepDS_CurvePointInterference>(l2->First());
  if (i2.IsNull())
    return 0;
  double p1 = i1->Parameter();
  double p2 = i2->Parameter();
  if (p1 < p2)
    return (-1);
  else if (p1 > p2)
    return (1);
  return 0;
}

static void BREP_sortonparameter2(NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LOI)
{
  TopOpeBRepDS_TKI tki;
  tki.FillOnGeometry(LOI);
  int ng = 0;
  for (tki.Init(); tki.More(); tki.Next())
    ng++;
  size_t            sng = (size_t)ng;
  size_t            sad = sizeof(BOA_t);
  BOA_t*            T   = (BOA_t*)Standard::Allocate(sng * sad);
  TopOpeBRepDS_Kind K;
  int               G, j = 0;
  for (tki.Init(); tki.More(); tki.Next(), j++)
    T[j] = &(tki.ChangeValue(K, G));
  qsort(T, sng, sad, compll);
  LOI.Clear();
  for (j = 0; j < ng; j++)
  {
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& l = *T[j];
    LOI.Append(l);
  }
  Standard::Free(T);
}

Standard_EXPORT void BREP_sortonparameter(const occ::handle<TopOpeBRepDS_HDataStructure>& HDS)
{
  TopOpeBRepDS_DataStructure& BDS = HDS->ChangeDS();
  int                         i = 1, n = HDS->NbShapes();
  for (; i <= n; i++)
  {
    const TopoDS_Shape& s = HDS->Shape(i);
    if (s.ShapeType() != TopAbs_EDGE)
      continue;
    NCollection_List<occ::handle<TopOpeBRepDS_Interference>>& LOI = BDS.ChangeShapeInterferences(s);
    BREP_sortonparameter2(LOI);
  }
}
