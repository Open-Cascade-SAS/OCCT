// Created on: 2009-04-06
// Created by: Sergey ZARITCHNY
// Copyright (c) 2009-2012 OPEN CASCADE SAS
//
// The content of this file is subject to the Open CASCADE Technology Public
// License Version 6.5 (the "License"). You may not use the content of this file
// except in compliance with the License. Please obtain a copy of the License
// at http://www.opencascade.org and read it completely before using this file.
//
// The Initial Developer of the Original Code is Open CASCADE S.A.S., having its
// main offices at: 1, place des Freres Montgolfier, 78280 Guyancourt, France.
//
// The Original Code and all software distributed under the License is
// distributed on an "AS IS" basis, without warranty of any kind, and the
// Initial Developer hereby disclaims all such warranties, including without
// limitation, any warranties of merchantability, fitness for a particular
// purpose or non-infringement. Please see the License for the specific terms
// and conditions governing the rights and limitations under the License.



#include <TDataXtd_Position.ixx>
#include <TDF_Label.hxx>

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
