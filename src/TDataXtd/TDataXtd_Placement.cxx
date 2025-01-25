// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2014 OPEN CASCADE SAS
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

#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd_Placement.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(TDataXtd_Placement, TDataStd_GenericEmpty)

//=================================================================================================

const Standard_GUID& TDataXtd_Placement::GetID()
{
  static Standard_GUID TDataXtd_PlacementID("2a96b60b-ec8b-11d0-bee7-080009dc3333");
  return TDataXtd_PlacementID;
}

//=================================================================================================

Handle(TDataXtd_Placement) TDataXtd_Placement::Set(const TDF_Label& L)
{
  Handle(TDataXtd_Placement) A;
  if (!L.FindAttribute(TDataXtd_Placement::GetID(), A))
  {
    A = new TDataXtd_Placement();
    L.AddAttribute(A);
  }
  return A;
}

//=================================================================================================

TDataXtd_Placement::TDataXtd_Placement() {}

//=================================================================================================

const Standard_GUID& TDataXtd_Placement::ID() const
{
  return GetID();
}

//=================================================================================================

Standard_OStream& TDataXtd_Placement::Dump(Standard_OStream& anOS) const
{
  anOS << "Placement";
  return anOS;
}
