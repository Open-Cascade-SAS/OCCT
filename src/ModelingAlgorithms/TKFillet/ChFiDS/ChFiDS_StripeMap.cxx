// Created on: 1993-11-18
// Created by: Isabelle GRIGNON
// Copyright (c) 1993-1999 Matra Datavision
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

#include <ChFiDS_StripeMap.hxx>
#include <TopoDS_Vertex.hxx>

//=================================================================================================

ChFiDS_StripeMap::ChFiDS_StripeMap() = default;

//=================================================================================================

void ChFiDS_StripeMap::Add(const TopoDS_Vertex& V, const occ::handle<ChFiDS_Stripe>& F)
{
  int Index = mymap.FindIndex(V);
  if (Index == 0)
  {
    NCollection_List<occ::handle<ChFiDS_Stripe>> Empty;
    Index = mymap.Add(V, Empty);
  }
  mymap(Index).Append(F);
}

//=================================================================================================

const NCollection_List<occ::handle<ChFiDS_Stripe>>& ChFiDS_StripeMap::FindFromKey(
  const TopoDS_Vertex& V) const
{
  return mymap.FindFromKey(V);
}

//=================================================================================================

const NCollection_List<occ::handle<ChFiDS_Stripe>>& ChFiDS_StripeMap::FindFromIndex(
  const int I) const
{
  return mymap.FindFromIndex(I);
}

void ChFiDS_StripeMap::Clear()
{
  mymap.Clear();
}
