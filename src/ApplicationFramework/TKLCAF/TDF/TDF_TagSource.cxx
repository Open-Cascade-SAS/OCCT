// Created on: 1997-08-04
// Created by: VAUTHIER Jean-Claude
// Copyright (c) 1997-1999 Matra Datavision
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

#include <TDF_TagSource.hxx>

#include <Standard_Dump.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDF_TagSource, TDF_Attribute)

//=================================================================================================

const Standard_GUID& TDF_TagSource::GetID()
{

  static Standard_GUID TDF_TagSourceID("2a96b611-ec8b-11d0-bee7-080009dc3333");
  return TDF_TagSourceID;
}

//=================================================================================================

Handle(TDF_TagSource) TDF_TagSource::Set(const TDF_Label& L)
{

  Handle(TDF_TagSource) T;
  if (!L.FindAttribute(TDF_TagSource::GetID(), T))
  {
    T = new TDF_TagSource();
    L.AddAttribute(T);
  }
  return T;
}

//=================================================================================================

TDF_Label TDF_TagSource::NewChild(const TDF_Label& L)
{
  Handle(TDF_TagSource) T;
  if (!L.FindAttribute(GetID(), T))
  {
    T = new TDF_TagSource();
    L.AddAttribute(T);
  }
  return T->NewChild();
}

//=================================================================================================

TDF_TagSource::TDF_TagSource()
    : myTag(0)
{
}

//=================================================================================================

Standard_Integer TDF_TagSource::NewTag()
{

  Backup(); // FID 02/07/98
  return ++myTag;
}

//=================================================================================================

TDF_Label TDF_TagSource::NewChild()
{
  return Label().FindChild(NewTag(), Standard_True);
}

//=================================================================================================

Standard_Integer TDF_TagSource::Get() const
{
  return myTag;
}

//=================================================================================================

void TDF_TagSource::Set(const Standard_Integer T)
{
  // OCC2932 correction
  if (myTag == T)
    return;

  Backup(); // FID 02/07/98
  myTag = T;
}

//=================================================================================================

const Standard_GUID& TDF_TagSource::ID() const
{
  return GetID();
}

//=================================================================================================

Handle(TDF_Attribute) TDF_TagSource::NewEmpty() const
{
  return new TDF_TagSource();
}

//=================================================================================================

void TDF_TagSource::Restore(const Handle(TDF_Attribute)& With)
{
  myTag = Handle(TDF_TagSource)::DownCast(With)->Get();
}

//=================================================================================================

void TDF_TagSource::Paste(const Handle(TDF_Attribute)& Into,
                          const Handle(TDF_RelocationTable)&) const
{
  Handle(TDF_TagSource)::DownCast(Into)->Set(myTag);
}

//=================================================================================================

void TDF_TagSource::DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth) const
{
  OCCT_DUMP_TRANSIENT_CLASS_BEGIN(theOStream)

  OCCT_DUMP_BASE_CLASS(theOStream, theDepth, TDF_Attribute)

  OCCT_DUMP_FIELD_VALUE_NUMERICAL(theOStream, myTag)
}
