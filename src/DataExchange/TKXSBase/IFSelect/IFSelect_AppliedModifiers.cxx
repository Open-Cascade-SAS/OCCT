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

#include <IFSelect_AppliedModifiers.hxx>
#include <IFSelect_GeneralModifier.hxx>
#include <Standard_Type.hxx>

IMPLEMENT_STANDARD_RTTIEXT(IFSelect_AppliedModifiers, Standard_Transient)

IFSelect_AppliedModifiers::IFSelect_AppliedModifiers(const int nbmax,
                                                     const int nbent)
    : thelists(nbmax + 1)
{
  thenbent  = nbent;
  theentcnt = 0;
}

bool IFSelect_AppliedModifiers::AddModif(const occ::handle<IFSelect_GeneralModifier>& modif)
{
  if (themodifs.Length() >= thelists.NbEntities())
    return false;
  themodifs.Append(modif);
  thelists.SetNumber(themodifs.Length());
  return true;
}

bool IFSelect_AppliedModifiers::AddNum(const int nument)
{
  thelists.Add(nument);
  return true;
}

int IFSelect_AppliedModifiers::Count() const
{
  return themodifs.Length();
}

bool IFSelect_AppliedModifiers::Item(const int            num,
                                                 occ::handle<IFSelect_GeneralModifier>& modif,
                                                 int&                 entcount)
{
  if (num < 1 || num > themodifs.Length())
    return false;
  modif = themodifs.Value(num);
  thelists.SetNumber(num);
  theentcnt = thelists.Length();
  entcount  = (theentcnt > 0 ? theentcnt : thenbent);
  return true;
}

int IFSelect_AppliedModifiers::ItemNum(const int nument) const
{
  return (theentcnt > 0 ? thelists.Value(nument) : nument);
}

occ::handle<NCollection_HSequence<int>> IFSelect_AppliedModifiers::ItemList() const
{
  occ::handle<NCollection_HSequence<int>> list = new NCollection_HSequence<int>();
  int                   i, nb = (theentcnt > 0 ? theentcnt : thenbent);
  for (i = 1; i <= nb; i++)
    list->Append(ItemNum(i));
  return list;
}

bool IFSelect_AppliedModifiers::IsForAll() const
{
  return (theentcnt == 0);
}
