// Created on: 1993-01-09
// Created by: CKY / Contract Toubro-Larsen (Arun MENON)
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

#include <IGESBasic_HArray1OfHArray1OfInteger.hxx>

#include <Standard_Type.hxx>

IGESBasic_HArray1OfHArray1OfInteger::IGESBasic_HArray1OfHArray1OfInteger(const int low,
                                                                         const int up)
    : thelist(low, up)
{
  occ::handle<Standard_Transient> nulo;
  thelist.Init(nulo);
}

int IGESBasic_HArray1OfHArray1OfInteger::Lower() const
{
  return thelist.Lower();
}

int IGESBasic_HArray1OfHArray1OfInteger::Upper() const
{
  return thelist.Upper();
}

int IGESBasic_HArray1OfHArray1OfInteger::Length() const
{
  return thelist.Length();
}

void IGESBasic_HArray1OfHArray1OfInteger::SetValue(const int                  num,
                                                   const occ::handle<NCollection_HArray1<int>>& val)
{
  thelist.SetValue(num, val);
}

occ::handle<NCollection_HArray1<int>> IGESBasic_HArray1OfHArray1OfInteger::Value(
  const int num) const
{
  return occ::down_cast<NCollection_HArray1<int>>(thelist.Value(num));
}
