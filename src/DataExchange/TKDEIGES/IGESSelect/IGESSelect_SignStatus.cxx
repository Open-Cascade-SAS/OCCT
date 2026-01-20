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

#include <IGESData_IGESEntity.hxx>
#include <IGESSelect_SignStatus.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>
#include <TCollection_AsciiString.hxx>

#include <stdio.h>
IMPLEMENT_STANDARD_RTTIEXT(IGESSelect_SignStatus, IFSelect_Signature)

static char theval[10];

IGESSelect_SignStatus::IGESSelect_SignStatus()
    : IFSelect_Signature("D.E. Status")
{
}

const char* IGESSelect_SignStatus::Value(
  const occ::handle<Standard_Transient>& ent,
  const occ::handle<Interface_InterfaceModel>& /*model*/) const
{
  occ::handle<IGESData_IGESEntity> igesent = occ::down_cast<IGESData_IGESEntity>(ent);
  if (igesent.IsNull())
    return "";
  int i, j, k, l;
  i = igesent->BlankStatus();
  j = igesent->SubordinateStatus();
  k = igesent->UseFlag();
  l = igesent->HierarchyStatus();
  Sprintf(theval, "%d,%d,%d,%d", i, j, k, l);
  return theval;
}

bool IGESSelect_SignStatus::Matches(const occ::handle<Standard_Transient>& ent,
                                                const occ::handle<Interface_InterfaceModel>& /*model*/,
                                                const TCollection_AsciiString& text,
                                                const bool         exact) const
{
  occ::handle<IGESData_IGESEntity> igesent = occ::down_cast<IGESData_IGESEntity>(ent);
  if (igesent.IsNull())
    return false;
  int i, j, k, l;
  i = igesent->BlankStatus();
  j = igesent->SubordinateStatus();
  k = igesent->UseFlag();
  l = igesent->HierarchyStatus();
  int n, nb = text.Length();
  if (nb > 9)
    nb = 9;
  for (n = 1; n <= nb; n++)
    theval[n - 1] = text.Value(n);
  theval[nb] = '\0';

  int vir = 0, val = 0;
  for (n = 0; n < nb; n++)
  {
    char car = theval[n];
    if (car == ',')
    {
      vir++;
      continue;
    }
    val = int(car - 48);
    if (car == 'V' && vir == 0)
      val = 0;
    if (car == 'B' && vir == 0)
      val = 1;
    if (car == 'I' && vir == 1)
      val = 0;
    if (car == 'P' && vir == 1)
      val = 1;
    if (car == 'L' && vir == 1)
      val = 2;
    if (car == 'D' && vir == 1)
      val = 3;
    //    Matches ?
    if (vir == 0)
    { // Blank
      if (i == val && !exact)
        return true;
      if (i != val && exact)
        return false;
    }
    if (vir == 1)
    { // Subord.
      if (j == val && !exact)
        return true;
      if (j != val && exact)
        return false;
    }
    if (vir == 2)
    { // UseFlag
      if (k == val && !exact)
        return true;
      if (k != val && exact)
        return false;
    }
    if (vir == 3)
    { // Hierarchy
      if (l == val && !exact)
        return true;
      if (l != val && exact)
        return false;
    }
  }
  return exact; // a bit of reflection to arrive
}
