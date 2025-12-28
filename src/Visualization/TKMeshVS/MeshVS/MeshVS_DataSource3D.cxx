// Created on: 2005-01-21
// Created by: Alexander SOLOVYOV
// Copyright (c) 2005-2014 OPEN CASCADE SAS
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

#include <MeshVS_DataSource3D.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(MeshVS_DataSource3D, MeshVS_DataSource)

//=================================================================================================

occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> MeshVS_DataSource3D::GetPrismTopology(
  const int BasePoints) const
{
  if (myPrismTopos.IsBound(BasePoints))
    return myPrismTopos.Find(BasePoints);
  else
  {
    occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> result = CreatePrismTopology(BasePoints);
    if (!result.IsNull())
      ((MeshVS_DataSource3D*)this)->myPrismTopos.Bind(BasePoints, result);
    return result;
  }
}

//=================================================================================================

occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> MeshVS_DataSource3D::GetPyramidTopology(
  const int BasePoints) const
{
  if (myPyramidTopos.IsBound(BasePoints))
    return myPyramidTopos.Find(BasePoints);
  else
  {
    occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> result = CreatePyramidTopology(BasePoints);
    if (!result.IsNull())
      ((MeshVS_DataSource3D*)this)->myPyramidTopos.Bind(BasePoints, result);
    return result;
  }
}

//=================================================================================================

occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> MeshVS_DataSource3D::CreatePrismTopology(
  const int BasePoints)
{
  occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> result;

  if (BasePoints >= 3)
  {
    result = new NCollection_HArray1<NCollection_Sequence<int>>(1, BasePoints + 2);
    int i, next;

    for (i = 0; i < BasePoints; i++)
    {
      result->ChangeValue(1).Prepend(i);
      result->ChangeValue(2).Append(i + BasePoints);

      result->ChangeValue(3 + i).Prepend(i);
      result->ChangeValue(3 + i).Prepend(i + BasePoints);
      next = (i + 1) % BasePoints;
      result->ChangeValue(3 + i).Prepend(next + BasePoints);
      result->ChangeValue(3 + i).Prepend(next);
    }
  }

  return result;
}

//=================================================================================================

occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> MeshVS_DataSource3D::CreatePyramidTopology(
  const int BasePoints)
{
  occ::handle<NCollection_HArray1<NCollection_Sequence<int>>> result;

  if (BasePoints >= 3)
  {
    result = new NCollection_HArray1<NCollection_Sequence<int>>(1, BasePoints + 1);

    for (int i = 1; i <= BasePoints; i++)
    {
      result->ChangeValue(1).Prepend(i);
      result->ChangeValue(1 + i).Append(0);
      result->ChangeValue(1 + i).Append(i);
      result->ChangeValue(1 + i).Append(i % BasePoints + 1);
    }
  }

  return result;
}
