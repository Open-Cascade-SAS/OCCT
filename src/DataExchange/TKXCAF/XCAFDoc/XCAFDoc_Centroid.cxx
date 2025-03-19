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

#include <XCAFDoc_Centroid.hxx>

#include <gp_Pnt.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(XCAFDoc_Centroid, TDF_Attribute)

//=================================================================================================

XCAFDoc_Centroid::XCAFDoc_Centroid() {}

//=================================================================================================

const Standard_GUID& XCAFDoc_Centroid::GetID()
{
  static Standard_GUID CentroidID("efd212f3-6dfd-11d4-b9c8-0060b0ee281b");
  return CentroidID;
}

//=================================================================================================

Handle(XCAFDoc_Centroid) XCAFDoc_Centroid::Set(const TDF_Label& L, const gp_Pnt& pnt)
{
  Handle(XCAFDoc_Centroid) A;
  if (!L.FindAttribute(XCAFDoc_Centroid::GetID(), A))
  {
    A = new XCAFDoc_Centroid();
    L.AddAttribute(A);
  }
  A->Set(pnt);
  return A;
}

//=================================================================================================

void XCAFDoc_Centroid::Set(const gp_Pnt& pnt)
{
  Backup();
  myCentroid = pnt;
}

//=================================================================================================

gp_Pnt XCAFDoc_Centroid::Get() const
{
  return myCentroid;
}

//=================================================================================================

Standard_Boolean XCAFDoc_Centroid::Get(const TDF_Label& label, gp_Pnt& pnt)
{
  Handle(XCAFDoc_Centroid) aCentroid;
  if (!label.FindAttribute(XCAFDoc_Centroid::GetID(), aCentroid))
    return Standard_False;

  pnt = aCentroid->Get();
  return Standard_True;
}

//=================================================================================================

const Standard_GUID& XCAFDoc_Centroid::ID() const
{
  return GetID();
}

//=================================================================================================

void XCAFDoc_Centroid::Restore(const Handle(TDF_Attribute)& With)
{
  myCentroid = Handle(XCAFDoc_Centroid)::DownCast(With)->Get();
}

//=================================================================================================

Handle(TDF_Attribute) XCAFDoc_Centroid::NewEmpty() const
{
  return new XCAFDoc_Centroid();
}

//=================================================================================================

void XCAFDoc_Centroid::Paste(const Handle(TDF_Attribute)& Into,
                             const Handle(TDF_RelocationTable)& /* RT */) const
{
  Handle(XCAFDoc_Centroid)::DownCast(Into)->Set(myCentroid);
}

//=================================================================================================

Standard_OStream& XCAFDoc_Centroid::Dump(Standard_OStream& anOS) const
{
  anOS << "Centroid ( ";
  anOS << myCentroid.X() << ",";
  anOS << myCentroid.Y() << ",";
  anOS << myCentroid.Z() << ")";
  return anOS;
}

//=================================================================================================

void XCAFDoc_Centroid::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUES_DUMPED(theOStream, theDepth, &myCentroid)
}
