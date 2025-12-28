// Created on: 2000-08-16
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

#include <XCAFDoc_Color.hxx>

#include <Quantity_Color.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Color, TDF_Attribute)

//=================================================================================================

XCAFDoc_Color::XCAFDoc_Color() {}

//=================================================================================================

const Standard_GUID& XCAFDoc_Color::GetID()
{
  static Standard_GUID ColorID("efd212f0-6dfd-11d4-b9c8-0060b0ee281b");
  return ColorID;
}

//=================================================================================================

occ::handle<XCAFDoc_Color> XCAFDoc_Color::Set(const TDF_Label& L, const Quantity_Color& C)
{
  occ::handle<XCAFDoc_Color> A;
  if (!L.FindAttribute(XCAFDoc_Color::GetID(), A))
  {
    A = new XCAFDoc_Color();
    L.AddAttribute(A);
  }
  A->Set(C);
  return A;
}

//=================================================================================================

occ::handle<XCAFDoc_Color> XCAFDoc_Color::Set(const TDF_Label& L, const Quantity_ColorRGBA& C)
{
  occ::handle<XCAFDoc_Color> A;
  if (!L.FindAttribute(XCAFDoc_Color::GetID(), A))
  {
    A = new XCAFDoc_Color();
    L.AddAttribute(A);
  }
  A->Set(C);
  return A;
}

//=================================================================================================

occ::handle<XCAFDoc_Color> XCAFDoc_Color::Set(const TDF_Label& L, const Quantity_NameOfColor C)
{
  occ::handle<XCAFDoc_Color> A;
  if (!L.FindAttribute(XCAFDoc_Color::GetID(), A))
  {
    A = new XCAFDoc_Color();
    L.AddAttribute(A);
  }
  A->Set(C);
  return A;
}

//=================================================================================================

occ::handle<XCAFDoc_Color> XCAFDoc_Color::Set(const TDF_Label&    L,
                                         const double R,
                                         const double G,
                                         const double B,
                                         const double alpha)
{
  occ::handle<XCAFDoc_Color> A;
  if (!L.FindAttribute(XCAFDoc_Color::GetID(), A))
  {
    A = new XCAFDoc_Color();
    L.AddAttribute(A);
  }
  A->Set(R, G, B, alpha);
  return A;
}

//=================================================================================================

void XCAFDoc_Color::Set(const Quantity_Color& C)
{
  Backup();
  myColor.SetRGB(C);
}

//=================================================================================================

void XCAFDoc_Color::Set(const Quantity_ColorRGBA& C)
{
  Backup();
  myColor = C;
}

//=================================================================================================

void XCAFDoc_Color::Set(const Quantity_NameOfColor C)
{
  Backup();
  myColor.SetRGB(C);
}

//=================================================================================================

void XCAFDoc_Color::Set(const double R,
                        const double G,
                        const double B,
                        const double alpha)
{
  Backup();
  Quantity_Color aColor;
  aColor.SetValues(R, G, B, Quantity_TOC_RGB);
  myColor.SetRGB(aColor);
  myColor.SetAlpha((float)alpha);
}

//=================================================================================================

const Quantity_Color& XCAFDoc_Color::GetColor() const
{
  return myColor.GetRGB();
}

//=================================================================================================

const Quantity_ColorRGBA& XCAFDoc_Color::GetColorRGBA() const
{
  return myColor;
}

//=================================================================================================

Quantity_NameOfColor XCAFDoc_Color::GetNOC() const
{
  return myColor.GetRGB().Name();
}

//=================================================================================================

void XCAFDoc_Color::GetRGB(double& R, double& G, double& B) const
{
  myColor.GetRGB().Values(R, G, B, Quantity_TOC_RGB);
}

//=================================================================================================

float XCAFDoc_Color::GetAlpha() const
{
  return myColor.Alpha();
}

//=================================================================================================

const Standard_GUID& XCAFDoc_Color::ID() const
{
  return GetID();
}

//=================================================================================================

void XCAFDoc_Color::Restore(const occ::handle<TDF_Attribute>& With)
{
  myColor = occ::down_cast<XCAFDoc_Color>(With)->GetColorRGBA();
}

//=================================================================================================

occ::handle<TDF_Attribute> XCAFDoc_Color::NewEmpty() const
{
  return new XCAFDoc_Color();
}

//=================================================================================================

void XCAFDoc_Color::Paste(const occ::handle<TDF_Attribute>& Into,
                          const occ::handle<TDF_RelocationTable>& /* RT */) const
{
  occ::down_cast<XCAFDoc_Color>(Into)->Set(myColor);
}

//=================================================================================================

void XCAFDoc_Color::DumpJson(Standard_OStream& theOStream, int theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myColor)
}
