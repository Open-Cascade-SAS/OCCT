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


#include <gp_Pnt.hxx>
#include <Standard_GUID.hxx>
#include <Standard_Type.hxx>
#include <TDataXtd_Position.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>

IMPLEMENT_STANDARD_RTTIEXT(TDataXtd_Position,TDF_Attribute)

//=======================================================================
//function : Set (class method)
//purpose  : 
//=======================================================================
void TDataXtd_Position::Set(const TDF_Label& aLabel, const gp_Pnt& aPos) 
{
  Handle(TDataXtd_Position) pos;
  if (!aLabel.FindAttribute(TDataXtd_Position::GetID(), pos)) {
    pos = new TDataXtd_Position();
    aLabel.AddAttribute(pos);
  }
  pos->SetPosition( aPos ); 
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================
Handle(TDataXtd_Position) TDataXtd_Position::Set (const TDF_Label& L)
{
  Handle(TDataXtd_Position) POS;
  if (!L.FindAttribute (TDataXtd_Position::GetID (), POS)) {    
    POS = new TDataXtd_Position;
    L.AddAttribute(POS);
  }
  return POS;
}

//=======================================================================
//function : Get (class method)
//purpose  : 
//=======================================================================
Standard_Boolean TDataXtd_Position::Get(const TDF_Label& aLabel, gp_Pnt& aPos) 
{
  Handle(TDataXtd_Position) pos;
  if( aLabel.FindAttribute( TDataXtd_Position::GetID(), pos) ) {
    aPos = pos->GetPosition();
    return Standard_True; 
  }
  return Standard_False; 
}

//=======================================================================
//function : GetID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataXtd_Position::GetID() 
{
  static Standard_GUID TDataXtd_Position_guid("55553252-ce0c-11d1-b5d8-00a0c9064368");
  return TDataXtd_Position_guid;
}

//=======================================================================
//function : TDataXtd_Position
//purpose  : 
//=======================================================================
TDataXtd_Position::TDataXtd_Position()
  :myPosition(gp_Pnt(0.,0.,0.))
{
}

//=======================================================================
//function : GetPosition
//purpose  : 
//=======================================================================
const gp_Pnt& TDataXtd_Position::GetPosition() const
{
  return myPosition;
}

//=======================================================================
//function : Position
//purpose  : 
//=======================================================================
void TDataXtd_Position::SetPosition(const gp_Pnt& aPos) 
{
  // OCC2932 correction
  if(myPosition.X() == aPos.X() &&
     myPosition.Y() == aPos.Y() &&
     myPosition.Z() == aPos.Z())
    return;

  Backup();
  myPosition = aPos;
}

 

//=======================================================================
//function : ID
//purpose  : 
//=======================================================================
const Standard_GUID& TDataXtd_Position::ID() const
{
  return GetID();
}

//=======================================================================
//function : Restore
//purpose  : 
//=======================================================================
void TDataXtd_Position::Restore(const Handle(TDF_Attribute)& anAttribute) 
{
  myPosition = Handle(TDataXtd_Position)::DownCast(anAttribute)->GetPosition();
}

//=======================================================================
//function : NewEmpty
//purpose  : 
//=======================================================================
Handle(TDF_Attribute) TDataXtd_Position::NewEmpty() const
{
  return new TDataXtd_Position; 
}

//=======================================================================
//function : Paste
//purpose  : 
//=======================================================================
void TDataXtd_Position::Paste(const Handle(TDF_Attribute)& intoAttribute,
			     const Handle(TDF_RelocationTable)&) const
{
  Handle(TDataXtd_Position)::DownCast(intoAttribute)->SetPosition(myPosition);
}
