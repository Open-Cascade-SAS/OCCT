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


#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataStd_Tick.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataStd_Tick,TDF_Attribute)

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
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataStd_Tick::NewEmpty() const
{ 
  return new TDataStd_Tick(); 
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataStd_Tick::Restore (const Handle(TDF_Attribute)& ) 
{
  // There are no fields in this attribute.
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_Tick::Paste(const Handle(TDF_Attribute)& ,
			  const Handle(TDF_RelocationTable)& ) const
{
  // There are no fields in this attribute.
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
