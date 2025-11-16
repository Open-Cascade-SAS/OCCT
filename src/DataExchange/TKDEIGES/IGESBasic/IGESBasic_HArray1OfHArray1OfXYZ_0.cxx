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

#include <IGESBasic_HArray1OfHArray1OfXYZ.hxx>

#include <Standard_Type.hxx>

IGESBasic_HArray1OfHArray1OfXYZ::IGESBasic_HArray1OfHArray1OfXYZ(const Standard_Integer low,
                                                                 const Standard_Integer up)
    : thelist(low, up)
{
  Handle(Standard_Transient) nulo;
  thelist.Init(nulo);
}

Standard_Integer IGESBasic_HArray1OfHArray1OfXYZ::Lower() const
{
  return thelist.Lower();
}

Standard_Integer IGESBasic_HArray1OfHArray1OfXYZ::Upper() const
{
  return thelist.Upper();
}

Standard_Integer IGESBasic_HArray1OfHArray1OfXYZ::Length() const
{
  return thelist.Length();
}

void IGESBasic_HArray1OfHArray1OfXYZ::SetValue(const Standard_Integer             num,
                                               const Handle(TColgp_HArray1OfXYZ)& val)
{
  thelist.SetValue(num, val);
}

Handle(TColgp_HArray1OfXYZ) IGESBasic_HArray1OfHArray1OfXYZ::Value(const Standard_Integer num) const
{
  return Handle(TColgp_HArray1OfXYZ)::DownCast(thelist.Value(num));
}
