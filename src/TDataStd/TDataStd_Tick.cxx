// Created on: 2007-05-29
// Created by: Vlad Romashko
// Copyright (c) 2007-2014 OPEN CASCADE SAS
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

#include <TDataStd_Tick.hxx>

#include <TDF_Label.hxx>
#include <Standard_GUID.hxx>

IMPLEMENT_DERIVED_ATTRIBUTE(TDataStd_Tick,TDataStd_GenericEmpty)

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_Tick::GetID () 
{
  static Standard_GUID TDataStd_TickID("40DC60CD-30B9-41be-B002-4169EFB34EA5");
  return TDataStd_TickID; 
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataStd_Tick) TDataStd_Tick::Set (const TDF_Label& L)
{
  Handle(TDataStd_Tick) A;
  if (!L.FindAttribute(TDataStd_Tick::GetID (), A)) 
  {
    A = new TDataStd_Tick();
    L.AddAttribute(A);
  }
  return A;
}

//=======================================================================
//function : TDataStd_Tick
//purpose  : 
//=======================================================================
TDataStd_Tick::TDataStd_Tick () 
{
}

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataStd_Tick::ID () const 
{ 
  return GetID(); 
}

//=======================================================================
//function : Dump
//purpose  : 
//=======================================================================
Standard_OStream& TDataStd_Tick::Dump (Standard_OStream& anOS) const
{ 
  anOS << "Tick";
  return anOS;
}
