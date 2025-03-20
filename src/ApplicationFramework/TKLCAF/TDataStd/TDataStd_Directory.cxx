// Created on: 1999-06-25
// Created by: Sergey RUIN
// Copyright (c) 1999-1999 Matra Datavision
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

#include <TDataStd_Directory.hxx>

#include <Standard_DomainError.hxx>
#include <Standard_Type.hxx>
#include <TDataStd.hxx>
#include <TDF_DataSet.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_TagSource.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(TDataStd_Directory, TDataStd_GenericEmpty)

//=================================================================================================

Standard_Boolean TDataStd_Directory::Find(const TDF_Label& current, Handle(TDataStd_Directory)& D)
{
  TDF_Label                  L = current;
  Handle(TDataStd_Directory) dir;
  if (L.IsNull())
    return Standard_False;

  for (;;)
  {
    if (L.FindAttribute(TDataStd_Directory::GetID(), dir))
      break;
    L = L.Father();
    if (L.IsNull())
      break;
  }

  if (!dir.IsNull())
  {
    D = dir;
    return Standard_True;
  }
  return Standard_False;
}

//=================================================================================================

const Standard_GUID& TDataStd_Directory::GetID()
{
  static Standard_GUID TDataStd_DirectoryID("2a96b61f-ec8b-11d0-bee7-080009dc3333");
  return TDataStd_DirectoryID;
}

//=================================================================================================

Handle(TDataStd_Directory) TDataStd_Directory::New(const TDF_Label& L)
{
  if (L.HasAttribute())
  {
    throw Standard_DomainError("TDataStd_Directory::New : not an empty label");
  }
  Handle(TDataStd_Directory) A = new TDataStd_Directory();
  L.AddAttribute(A);
  TDF_TagSource::Set(L);
  return A;
}

//=================================================================================================

Handle(TDataStd_Directory) TDataStd_Directory::AddDirectory(const Handle(TDataStd_Directory)& dir)
{
  TDF_Label                  newLabel = TDF_TagSource::NewChild(dir->Label());
  Handle(TDataStd_Directory) A        = TDataStd_Directory::New(newLabel);
  return A;
}

//=================================================================================================

TDF_Label TDataStd_Directory::MakeObjectLabel(const Handle(TDataStd_Directory)& dir)
{
  return TDF_TagSource::NewChild(dir->Label());
}

//=================================================================================================

TDataStd_Directory::TDataStd_Directory() {}

//=================================================================================================

const Standard_GUID& TDataStd_Directory::ID() const
{
  return GetID();
}

//=================================================================================================

// Handle(TDF_Attribute) TDataStd_Directory::NewEmpty () const
//{
//   return new TDataStd_Directory();
// }

//=================================================================================================

Standard_OStream& TDataStd_Directory::Dump(Standard_OStream& anOS) const
{
  anOS << "Directory";
  return anOS;
}
