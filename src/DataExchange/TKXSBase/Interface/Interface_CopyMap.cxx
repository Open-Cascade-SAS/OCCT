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

#include <Interface_CopyMap.hxx>
#include <Interface_InterfaceError.hxx>
#include <Interface_InterfaceModel.hxx>
#include <Standard_Transient.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Interface_CopyMap, Interface_CopyControl)

//  CopyMap : nothing more than a passive Map
Interface_CopyMap::Interface_CopyMap(const occ::handle<Interface_InterfaceModel>& amodel)
    : theres(0, amodel->NbEntities())
{
  themod = amodel;
}

void Interface_CopyMap::Clear()
{
  int                             nb = theres.Upper();
  occ::handle<Standard_Transient> bid; // Null
  for (int i = 1; i <= nb; i++)
    theres.SetValue(i, bid);
}

occ::handle<Interface_InterfaceModel> Interface_CopyMap::Model() const
{
  return themod;
}

void Interface_CopyMap::Bind(const occ::handle<Standard_Transient>& ent,
                             const occ::handle<Standard_Transient>& res)
{
  int num = themod->Number(ent);
  if (num == 0 || num > theres.Upper())
    throw Interface_InterfaceError(
      "CopyMap : Bind, Starting Entity not issued from Starting Model");
  if (!theres.Value(num).IsNull())
    throw Interface_InterfaceError("CopyMap : Bind, Starting Entity already bound");
  theres.SetValue(num, res);
}

bool Interface_CopyMap::Search(const occ::handle<Standard_Transient>& ent,
                               occ::handle<Standard_Transient>&       res) const
{
  int num = themod->Number(ent);
  if (num == 0)
    return false;
  res = theres.Value(num);
  return (!res.IsNull());
}
