// File:        TDataStd_Tick.cxx
// Created:     May 29 11:40:00 2007
// Author:      Vlad Romashko
//  	    	<vladislav.romashko@opencascade.com>
// Copyright:   Open CASCADE

#include <TDataStd_Tick.ixx>

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
void TDataStd_Tick::Restore (const Handle(TDF_Attribute)& other) 
{
  // There are no fields in this attribute.
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataStd_Tick::Paste(const Handle(TDF_Attribute)& Into,
			  const Handle(TDF_RelocationTable)& RT) const
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
