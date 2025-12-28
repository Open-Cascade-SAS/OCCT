// Created on: 2000-09-08
// Created by: data exchange team
// Copyright (c) 2000-2014 OPEN CASCADE SAS
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

#include <XCAFDoc_Area.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE_WITH_TYPE(XCAFDoc_Area, TDataStd_Real, "xcaf", "Area")

//=================================================================================================

XCAFDoc_Area::XCAFDoc_Area() = default;

//=================================================================================================

const Standard_GUID& XCAFDoc_Area::GetID()
{
  static Standard_GUID AreaID("efd212f2-6dfd-11d4-b9c8-0060b0ee281b");
  return AreaID;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_Area::ID() const
{
  return GetID();
}

//=================================================================================================

occ::handle<XCAFDoc_Area> XCAFDoc_Area::Set(const TDF_Label& L, const double V)
{
  occ::handle<XCAFDoc_Area> A;
  if (!L.FindAttribute(XCAFDoc_Area::GetID(), A))
  {
    A = new XCAFDoc_Area;
    L.AddAttribute(A);
  }
  A->Set(V);
  return A;
}

//=================================================================================================

void XCAFDoc_Area::Set(const double V)
{
  TDataStd_Real::Set(V);
}

//=================================================================================================

double XCAFDoc_Area::Get() const
{
  return TDataStd_Real::Get();
}

//=================================================================================================

bool XCAFDoc_Area::Get(const TDF_Label& label, double& area)
{
  occ::handle<XCAFDoc_Area> anArea;
  if (!label.FindAttribute(XCAFDoc_Area::GetID(), anArea))
    return false;

  area = anArea->Get();
  return true;
}

//=================================================================================================

Standard_OStream& XCAFDoc_Area::Dump(Standard_OStream& anOS) const
{
  anOS << "Area ";
  anOS << Get();
  return anOS;
}

//=================================================================================================

void XCAFDoc_Area::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myValue)
}
